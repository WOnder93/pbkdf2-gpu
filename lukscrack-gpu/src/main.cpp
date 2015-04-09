/*
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "passworddata.h"
#include "listpasswordgenerator.h"
#include "lukscrack.h"

#include "libcommandline/commandlineparser.h"
#include "libcommandline/argumenthandlers.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <set>

using namespace libcommandline;
using namespace libpbkdf2::compute::opencl;
using namespace lukscrack;

struct Arguments
{
    bool showHelp = false;
    bool listDevices = false;

    std::string action = "crack";
    std::string pwgen = "list";
    std::string pwgenOpts = "-";

    std::set<size_t> deviceIndices = { 0 };
    std::string openclDataDir = "data";
    size_t batchSize = 2048;
    size_t threads = 1;

    bool noNewline = false;

    size_t keyslot = 0;
    std::string headerFile;
};

typedef unsigned long long u_type;

static CommandLineParser<Arguments> buildCmdLineParser()
{
    static const auto positional = PositionalArgumentHandler<Arguments>(
                [] (Arguments &state, const std::string &arg) { state.headerFile = arg; },
            "HEADERFILE", "the file containing the LUKS header (may be a block device)");

    std::vector<const CommandLineOption<Arguments>*> options {
        new FlagOption<Arguments>(
            [] (Arguments &state) { state.listDevices = true; },
            "list-devices", 'l', "list all available devices and exit"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &action) { state.action = action; },
            "action", 'a', "the action to perform", "crack", "crack|benchmark"),
        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &pwgenSpec) {
                size_t delim = pwgenSpec.find(':');
                if (delim == std::string::npos) {
                    state.pwgen.assign(pwgenSpec);
                    state.pwgenOpts.clear();
                } else {
                    state.pwgen.assign(pwgenSpec.begin(), pwgenSpec.begin() + delim);
                    state.pwgenOpts.assign(pwgenSpec.begin() + delim + 1, pwgenSpec.end());
                }
            }, "pwgen", 'p', "the password generator to use", "list:-", "list:PWLISTFILE"),

        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type index) {
                state.deviceIndices.insert((size_t)index);
            }), "device", 'd', "use device with index INDEX", "0", "INDEX"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &folder) { state.openclDataDir = folder; },
            "opencl-data-dir", '\0', "the data directory for the 'opencl' mode", "data", "DIR"),

        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.batchSize = (size_t)num;
            }), "batch-size", 'b', "the number of tasks per batch", "2048", "N"),

        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.threads = (size_t)num;
            }), "threads", 't', "the number of threads to use for CPU-side computation "
                "(0 = auto)", "1", "N"),

        new FlagOption<Arguments>(
            [] (Arguments &state) { state.showHelp = true; },
            "no-newline", 'n', "do not print newline after the password"),

        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.keyslot = (size_t)num;
            }), "keyslot", 'k', "the keyslot to crack (0-8, 0 will use first active slot)", "0", "N"),

        new FlagOption<Arguments>(
            [] (Arguments &state) { state.showHelp = true; },
            "help", '?', "show this help and exit")
    };

    return CommandLineParser<Arguments>(
        "A tool for cracking LUKS partition passwords using GPU.\n"
                "\n"
                "When run in the 'crack' mode and a valid password is found, "
                "prints it to stdout and exits with 0. When no valid password "
                "is found, exits with 1.",
        positional, options);
}

int main(int, const char * const *argv)
{
    std::string progname = argv[0];
    CommandLineParser<Arguments> parser = buildCmdLineParser();

    Arguments args;
    int ret = parser.parseArguments(args, argv);
    if (ret != 0) {
        return ret;
    }
    if (args.showHelp) {
        parser.printHelp(argv);
        return 0;
    }

    GlobalContext global(args.openclDataDir);
    if (args.listDevices) {
        size_t i = 0;
        for (auto &device : global.getAllDevices()) {
            std::cout << "Device #" << i << ": " << device.getInfo() << std::endl;
            i++;
        }
        return 0;
    }

    if (args.headerFile.size() == 0) {
        std::cerr << progname << ": no header file specified" << std::endl;
        return 1;
    }

    std::ifstream header(args.headerFile);
    if (header.fail()) {
        std::cerr << progname << ": unable to open header file: "
                  << args.headerFile << std::endl;
        return 1;
    }

    if (args.keyslot > 8) {
        std::cerr << progname << ": invalid keyslot number (must be 0-8): "
                  << args.keyslot << std::endl;
        return 1;
    }

    PasswordData pwData;
    try {
        pwData.readFromLuksHeader(header, args.keyslot);
    } catch (const InvalidLuksHeaderException &ex) {
        std::cerr << progname << ": " << ex.what() << std::endl;
        return 1;
    }

    std::unique_ptr<PasswordGenerator> pwGen;
    if (args.pwgen == "list") {
        try {
            auto pwGenPtr = new ListPasswordGenerator(args.pwgenOpts);
            pwGen = std::move(std::unique_ptr<PasswordGenerator>(pwGenPtr));
        } catch (const std::ios_base::failure &ex) {
            std::cerr << progname << ": pwgen initialization failed: "
                      << ex.what() << std::endl;
            return 1;
        }
    } else {
        std::cerr << progname << ": invalid password generator: "
                  << args.pwgen << std::endl;
        return 1;
    }

    auto &allDevices = global.getAllDevices();
    std::vector<Device> devices;
    for (auto i : args.deviceIndices) {
        if (i >= allDevices.size()) {
            std::cerr << progname << ": device index out of range: "
                      << i << std::endl;
            return 1;
        }
        devices.push_back(allDevices[i]);
    }

    LuksCrack crack(&global, devices, &pwData, pwGen.get(),
                    args.threads, args.batchSize);
    if (args.action == "crack") {
        crack.runCracking();

        if (!crack.wasPasswordFound()) {
            return 1;
        }

        std::cout << crack.getPassword();
        if (!args.noNewline) {
            std::cout << std::endl;
        }
    } else {
        std::cerr << progname << ": invalid action: " << args.action << std::endl;
        return 1;
    }
    return 0;
}


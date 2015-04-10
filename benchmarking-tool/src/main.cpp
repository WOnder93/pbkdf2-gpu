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

#include "benchmark.h"

#include "libpbkdf2-compute-opencl/types.h"
#include "libpbkdf2-compute-cpu/types.h"
#include "libcommandline/commandlineparser.h"
#include "libcommandline/argumenthandlers.h"

#include <iostream>

using namespace libpbkdf2::compute;
using namespace libcommandline;

struct Arguments
{
    bool showHelp = false;
    bool listDevices = false;

    std::string mode = "opencl";

    std::size_t deviceIndex = 0;

    std::string outputType = "iters-per-sec";
    std::string outputMode = "verbose";

    std::string hashSpec = "sha1";
    std::string salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
    std::size_t iterations = 4096;
    std::size_t dkLength = 20;
    std::size_t batchSize = 2048;
    std::size_t sampleCount = 10;

    std::string openclDataDir = "data";
};

typedef unsigned long long u_type;

static CommandLineParser<Arguments> buildCmdLineParser()
{
    static const auto positional = PositionalArgumentHandler<Arguments>([] (Arguments &, const std::string &) {});

    std::vector<const CommandLineOption<Arguments>*> options {
        new FlagOption<Arguments>(
            [] (Arguments &state) { state.listDevices = true; },
            "list-devices", 'l', "list all available devices and exit"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &mode) { state.mode = mode; },
            "mode", 'm', "the mode in which to run ('opencl' for OpenCL, 'cpu' for CPU)", "opencl", "MODE"),

        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type index) {
                state.deviceIndex = (std::size_t)index;
            }), "device", 'd', "use device with index INDEX", "0", "INDEX"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &type) { state.outputType = type; },
            "output-type", 'o', "what to output (iters-per-sec|ns|ns-per-1M-iters)", "iters-per-sec", "TYPE"),
        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &mode) { state.outputMode = mode; },
            "output-mode", '\0', "output mode (verbose|raw|mean|mean-and-mdev)", "verbose", "MODE"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &hashSpec) { state.hashSpec = hashSpec; },
            "hash-spec", 'h', "the hash spec to use", "sha1", "SPEC"),
        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &salt) { state.salt = salt; },
            "salt", '\0', "the salt", "saltSALTsaltSALTsaltSALTsaltSALTsalt", "SALT"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.iterations = (std::size_t)num;
            }), "iterations", 'i', "the number of PBKDF2 iterations", "4096", "N"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.dkLength = (std::size_t)num;
            }), "dk-length", 'k', "the length of the derived key", "20", "BYTES"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.batchSize = (std::size_t)num;
            }), "batch-size", 'b', "the number of tasks per batch", "2048", "N"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.sampleCount = (std::size_t)num;
            }), "samples", 's', "the number of batches to run and measure", "10", "N"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &folder) { state.openclDataDir = folder; },
            "opencl-data-dir", '\0', "the data directory for the 'opencl' mode", "data", "DIR"),

        new FlagOption<Arguments>(
            [] (Arguments &state) { state.showHelp = true; },
            "help", '?', "show this help and exit")
    };

    return CommandLineParser<Arguments>(
        "A tool for benchmarking the libpbkdf2-compute-* libraries.",
        positional, options);
}

template<class Mode>
int doStuff(std::string progname,
            const typename Mode::TGlobalContext *globalCtx,
            const Arguments &args)
{
    if (args.outputMode == "verbose") {
        std::cout << "Running benchmark in '" << args.mode
                  << "' mode..." << std::endl;
    }

    if (args.listDevices) {
        std::size_t i = 0;
        for (auto &device : globalCtx->getAllDevices()) {
            std::cout << "Device #" << i << ": " << device.getInfo() << std::endl;
            i++;
        }
        return 0;
    }

    auto &devices = globalCtx->getAllDevices();
    if (args.deviceIndex >= devices.size()) {
        std::cerr << progname << ": device index out of range: "
                  << args.deviceIndex << std::endl;
        return 1;
    }

    auto &device = devices[args.deviceIndex];
    if (args.outputMode == "verbose") {
        std::cout << "Using device #" << args.deviceIndex << ": "
                  << device.getInfo() << std::endl;
    }

    auto stats = runBenchmark<Mode>(
                globalCtx, device, args.outputMode == "verbose", args.hashSpec,
                args.salt.data(), args.salt.size(), args.iterations,
                args.dkLength, args.batchSize, args.sampleCount);

    if (args.outputMode == "verbose") {
        std::cout << "Mean computation time: "
                  << RunTimeStatistics::repr((nanosecs)stats.getNanoseconds().getMean())
                  << std::endl;
        std::cout << "Mean deviation: "
                  << RunTimeStatistics::repr((nanosecs)stats.getNanoseconds().getMeanDeviation())
                  << " (" << stats.getNanoseconds().getMeanDeviationPerMean() * 100.0 << "%)"
                  << std::endl;

        std::cout << "Mean computation time (per 1M iterations): "
                  << RunTimeStatistics::repr((nanosecs)stats.getNsPer1MIterations().getMean())
                  << std::endl;
        std::cout << "Mean deviation (per 1M iterations): "
                  << RunTimeStatistics::repr((nanosecs)stats.getNsPer1MIterations().getMeanDeviation())
                  << std::endl;

        std::cout << "Mean iterations per second: "
                  << stats.getIterationsPerSecond().getMean()
                  << std::endl;
        return 0;
    }

    const DataSet *dataSet;
    if (args.outputType == "iters-per-sec") {
        dataSet = &stats.getIterationsPerSecond();
    } else if (args.outputType == "ns") {
        dataSet = &stats.getNanoseconds();
    } else if (args.outputType == "ns-per-1M-iters") {
        dataSet = &stats.getNsPer1MIterations();
    } else {
        std::cerr << progname << ": invalid output type: '"
                  << args.outputType << "'" << std::endl;
        return 1;
    }

    if (args.outputMode == "raw") {
        for (auto sample : dataSet->getSamples()) {
            std::cout << sample << std::endl;
        }
    } else if (args.outputMode == "mean") {
        std::cout << dataSet->getMean() << std::endl;
    } else if (args.outputMode == "mean-and-mdev") {
        std::cout << dataSet->getMean() << std::endl;
        std::cout << dataSet->getMeanDeviation() << std::endl;
    } else {
        std::cerr << progname << ": invalid output mode: '"
                  << args.outputMode << "'" << std::endl;
        return 1;
    }
    return 0;
}

int main(int, const char * const *argv)
{
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

    if (args.mode == "opencl") {
        opencl::GlobalContext global(args.openclDataDir);

        return doStuff<opencl::Types>(argv[0], &global, args);
    } else if (args.mode == "cpu") {
        cpu::GlobalContext global(nullptr);

        return doStuff<cpu::Types>(argv[0], &global, args);
    } else {
        std::cerr << argv[0] << ": invalid mode: " << args.mode << std::endl;
        return 1;
    }
    return 0;
}

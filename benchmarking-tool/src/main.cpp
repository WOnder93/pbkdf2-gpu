#include "benchmark.h"

#include "libpbkdf2-compute-opencl/types.h"
#include "libpbkdf2-compute-cpu/types.h"
#include "libcommandline/commandlineparser.h"
#include "libcommandline/argumenthandlers.h"

#include <iostream>

using namespace libpbkdf2::compute;
using namespace libcommandline;

struct Arguments {
    bool showHelp = false;
    bool listDevices = false;

    std::string mode = "opencl";

    size_t deviceIndex = 0;

    std::string hashSpec = "sha1";
    std::string salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
    size_t iterations = 4096;
    size_t dkLength = 20;
    size_t batchSize = 2048;
    size_t sampleCount = 10;

    std::string openclDataDir = "data";
};

typedef unsigned long long u_type;

static CommandLineParser<Arguments> buildCmdLineParser() {
    static const auto positional = PositionalArgumentHandler<Arguments>([] (Arguments &, const std::string &) {});

    std::vector<const CommandLineOption<Arguments>*> options = {
        new FlagOption<Arguments>(
            [] (Arguments &state) { state.listDevices = true; },
            "list-devices", 'l', "list all available devices and exit"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &mode) { state.mode = mode; },
            "mode", 'm', "the mode in which to run ('opencl' for OpenCL, 'cpu' for CPU)", "opencl", "MODE"),

        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type index) {
                state.deviceIndex = (size_t)index;
            }), "device", 'd', "use device with index INDEX", "0", "INDEX"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &hashSpec) { state.hashSpec = hashSpec; },
            "hash-spec", 'h', "the hash spec to use", "sha1", "SPEC"),
        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &salt) { state.salt = salt; },
            "salt", '\0', "the salt", "saltSALTsaltSALTsaltSALTsaltSALTsalt", "SALT"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.iterations = (size_t)num;
            }), "iterations", 'i', "the number of PBKDF2 iterations", "4096", "N"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.dkLength = (size_t)num;
            }), "dk-length", 'k', "the length of the derived key", "20", "BYTES"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.batchSize = (size_t)num;
            }), "batch-size", 'b', "the number of tasks per batch", "2048", "N"),
        new ArgumentOption<Arguments>(
            makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) {
                state.sampleCount = (size_t)num;
            }), "samples", 's', "the number of batches to run and measure", "10", "N"),

        new ArgumentOption<Arguments>(
            [] (Arguments &state, const std::string &folder) { state.openclDataDir = folder; },
            "opencl-data-dir", '\0', "the data directory for the 'opencl' mode", "data", "DIR"),

        new FlagOption<Arguments>(
            [] (Arguments &state) { state.showHelp = true; },
            "help", 'h', "show this help and exit")
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
    if (args.listDevices) {
        size_t i = 0;
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
    std::cout << "Using device #" << args.deviceIndex << ": "
              << device.getInfo() << std::endl;

    runBenchmark<Mode>(
                globalCtx, device, args.hashSpec,
                args.salt.data(), args.salt.size(), args.iterations,
                args.dkLength, args.batchSize, args.sampleCount);
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
        std::cout << "Running benchmark in OpenCL mode..." << std::endl;

        opencl::GlobalContext global(args.openclDataDir);

        doStuff<opencl::Types>(argv[0], &global, args);
    } else if (args.mode == "cpu") {
        std::cout << "Running benchmark in CPU mode..." << std::endl;

        cpu::GlobalContext global(nullptr);

        doStuff<cpu::Types>(argv[0], &global, args);
    } else {
        std::cerr << argv[0] << ": invalid mode: " << args.mode << std::endl;
    }
    return 0;
}

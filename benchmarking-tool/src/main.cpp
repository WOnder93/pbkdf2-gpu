#include "benchmark.h"

#include "libpbkdf2-compute-opencl/types.h"
#include "libpbkdf2-compute-cpu/types.h"
#include "libcommandline/commandlineparser.h"
#include "libcommandline/argumenthandlers.h"

#include <iostream>

using namespace libpbkdf2::compute;
using namespace libcommandline;

struct Arguments {
    std::string mode = "opencl";
    std::string hashSpec = "sha1";
    std::string salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
    size_t iterations = 4096;
    size_t dkLength = 20;
    size_t batchSize = 2048;
    size_t sampleCount = 10;

    bool showHelp = false;
};

typedef unsigned long long u_type;

static CommandLineParser<Arguments> buildCmdLineParser() {
    return CommandLineParser<Arguments>(
        "A tool for benchmarking the libpbkdf2-compute-* libraries.",
        PositionalArgumentHandler<Arguments>("", "", [] (Arguments &, const std::string &) {}),
        {
            new ArgumentOption<Arguments>(
                "mode", 'm', "the mode in which to run ('opencl' for OpenCL, 'cpu' for CPU)", "opencl",
                [] (Arguments &state, const std::string &salt) { state.mode = salt; }),
            new ArgumentOption<Arguments>(
                "hash-spec", 'h', "the hash spec to use", "sha1",
                [] (Arguments &state, const std::string &salt) { state.hashSpec = salt; }),
            new ArgumentOption<Arguments>(
                "salt", '\0', "the salt", "saltSALTsaltSALTsaltSALTsaltSALTsalt",
                [] (Arguments &state, const std::string &salt) { state.salt = salt; }),
            new ArgumentOption<Arguments>(
                "iterations", 'i', "the number of PBKDF2 iterations", "4096",
                makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) { state.iterations = (size_t)num; })),
            new ArgumentOption<Arguments>(
                "dk-length", 'd', "the length of the derived key", "20",
                makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) { state.dkLength = (size_t)num; })),
            new ArgumentOption<Arguments>(
                "batch-size", 'b', "the number of tasks per batch", "2048",
                makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) { state.batchSize = (size_t)num; })),
            new ArgumentOption<Arguments>(
                "samples", 's', "the number of batches to run and measure", "10",
                makeNumericHandler<Arguments, u_type>([] (Arguments &state, u_type num) { state.sampleCount = (size_t)num; })),
            new FlagOption<Arguments>(
                "help", 'h', "show this help and exit",
                [] (Arguments &state) { state.showHelp = true; }),
        });
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
        std::cout << "Benchmarking OpenCL..." << std::endl;

        opencl::GlobalContext global("data");

        auto &devices = global.getAvailableDevices();
        if (devices.size() == 0) {
            std::cerr << argv[0] << ": no devices found" << std::endl;
            return 1;
        }
        auto &device = devices[0];
        std::cout << "Using device #0:" << std::endl;
        std::cout << device.getInfo() << std::endl;

        runBenchmark<opencl::Types>(&global, device, args.hashSpec,
                args.salt.data(), args.salt.size(), args.iterations,
                args.dkLength, args.batchSize, args.sampleCount);
    } else if (args.mode == "cpu") {
        std::cout << "Benchmarking CPU..." << std::endl;

        cpu::GlobalContext global(nullptr);

        auto &devices = global.getAvailableDevices();
        if (devices.size() == 0) {
            std::cerr << argv[0] << ": no devices found" << std::endl;
            return 1;
        }
        auto &device = devices[0];
        std::cout << "Using device #0:" << std::endl;
        std::cout << device.getInfo() << std::endl;

        runBenchmark<cpu::Types>(&global, device, args.hashSpec,
                 args.salt.data(), args.salt.size(), args.iterations,
                 args.dkLength, args.batchSize, args.sampleCount);
    } else {
        std::cerr << argv[0] << ": invalid mode: " << args.mode << std::endl;
    }
    return 0;
}

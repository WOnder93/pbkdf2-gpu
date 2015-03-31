#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "runtimestatistics.h"

#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <iostream>

#define PASSWORD_LENGTH 64

/**
 * @brief A dummy password generator.
 */
class DummyPasswordGenerator
{
private:
    std::mt19937 gen;
    std::string currentPw;

public:
    inline DummyPasswordGenerator()
        : gen(std::chrono::system_clock::now().time_since_epoch().count())
    {
    }

    void operator()(const char *&password, size_t &passwordSize) {
        currentPw.resize(PASSWORD_LENGTH);
        for (size_t i = 0; i < PASSWORD_LENGTH; i++) {
            currentPw[i] = (unsigned char)gen();
        }
        password = currentPw.data();
        passwordSize = currentPw.size();
    }
};

template<class Types>
RunTimeStatistics runBenchmark(
        const typename Types::TGlobalContext *globalCtx,
        const typename Types::TDevice &dev,
        bool beVerbose,
        const std::string &hashSpec,
        const void *salt, size_t saltLength,
        size_t iterationCount, size_t dkLength,
        size_t batchSize, size_t sampleCount)
{
    std::vector<typename Types::TDevice> devices(&dev, &dev + 1);

    typename Types::THashFunctionContext hfContext(globalCtx, devices, hashSpec);
    typename Types::TComputeContext computeContext(&hfContext, salt, saltLength, dkLength, iterationCount);
    typename Types::TDeviceContext deviceContext(&computeContext, dev);
    typename Types::TProcessingUnit unit(&deviceContext, batchSize);

    typedef std::chrono::steady_clock clock_type;

    if (beVerbose) {
        std::cout << "Starting computation..." << std::endl;
    }

    DummyPasswordGenerator pwgen;
    RunTimeStatistics compTimeStats(iterationCount, batchSize);
    for (size_t i = 0; i < sampleCount; i++) {
        if (beVerbose) {
            std::cout << "  Sample " << i << "..." << std::endl;
        }

        clock_type::time_point checkpt0 = clock_type::now();

        unit.writePasswords(pwgen);

        clock_type::time_point checkpt1 = clock_type::now();

        unit.beginProcessing();
        unit.endProcessing();

        clock_type::time_point checkpt2 = clock_type::now();

        unit.readDerivedKeys([batchSize] (typename Types::TProcessingUnit::KeyIterator &it) {
            for (size_t i = 0; i < batchSize; i++) {
                it.nextKey();
            }
        });

        clock_type::time_point checkpt3 = clock_type::now();

        if (beVerbose) {
            clock_type::duration wrTime = checkpt1 - checkpt0;
            auto wrTimeNs = toNanoseconds(wrTime);
            std::cout << "    Writing took     " << RunTimeStatistics::repr(wrTimeNs) << std::endl;
        }

        clock_type::duration compTime = checkpt2 - checkpt1;
        auto compTimeNs = toNanoseconds(compTime);
        if (beVerbose) {
            std::cout << "    Computation took " << RunTimeStatistics::repr(compTimeNs) << std::endl;
        }
        compTimeStats.addSample(compTimeNs);

        if (beVerbose) {
            clock_type::duration rdTime = checkpt3 - checkpt2;
            auto rdTimeNs = toNanoseconds(rdTime);
            std::cout << "    Reading took     " << RunTimeStatistics::repr(rdTimeNs) << std::endl;
        }
    }
    compTimeStats.close();

    return compTimeStats;
}

#endif // BENCHMARK_H

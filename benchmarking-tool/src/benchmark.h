#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <numeric>
#include <iostream>
#include <cstdint>

#define PASSWORD_LENGTH 64
#define NS_PER_SECOND 1000000000

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

/**
 * @brief A class for storing and computing run time statistics.
 *
 * Nanoseconds are used as a universal unit so that it is easy
 * to convert time from other units.
 */
class RunTimeStatistics
{
public:
    typedef uintmax_t nanosecs;

    template<class duration>
    static nanosecs toNanoseconds(const duration &d) {
        return d.count() * (NS_PER_SECOND * duration::period::num / duration::period::den);
    }

    static double toMinutes(nanosecs ns)
    {
        return (double)ns / ((nanosecs)60 * 1000 * 1000 * 1000);
    }

    static double toSeconds(nanosecs ns)
    {
        return (double)ns / ((nanosecs)1000 * 1000 * 1000);
    }

    static double toMilliSeconds(nanosecs ns)
    {
        return (double)ns / ((nanosecs)1000 * 1000);
    }

    static double toMicroSeconds(nanosecs ns)
    {
        return (double)ns / (nanosecs)1000;
    }

    static std::string repr(nanosecs ns)
    {
        if (ns < (nanosecs)1000) {
            return std::to_string(ns) + " ns";
        }
        if (ns < (nanosecs)1000 * 1000) {
            return std::to_string(toMicroSeconds(ns)) + " us";
        }
        if (ns < (nanosecs)1000 * 1000 * 1000) {
            return std::to_string(toMilliSeconds(ns)) + " ms";
        }
        if (ns < (nanosecs)60 * 1000 * 1000 * 1000) {
            return std::to_string(toSeconds(ns)) + " s";
        }
        return std::to_string(toMinutes(ns)) + " min";
    }

private:
    nanosecs iter;

    std::vector<nanosecs> samples;
    nanosecs sum;
    nanosecs mean;
    nanosecs devSum;
    nanosecs devMean;

public:
    inline nanosecs getMean() const { return mean; }
    inline nanosecs getMeanDeviation() const { return devMean; }

    inline double getMeanDeviationPerMean() const { return (double)devMean / mean; }

    inline nanosecs getMeanPerIteration() const {
        return mean / iter;
    }
    inline nanosecs getMeanDeviationPerIteration() const {
        return devMean / iter;
    }

    inline nanosecs getMeanPer1MIterations() const {
        return (mean * 1000 * 1000) / iter;
    }
    inline nanosecs getMeanDeviationPer1MIterations() const {
        return (devMean * 1000 * 1000) / iter;
    }

    inline uintmax_t getIterationsPerSecond() const {
        return ((uintmax_t)iter * 1000 * 1000 * 1000) / mean;
    }

    inline RunTimeStatistics(size_t iterationCount, size_t batchSize)
        : iter(iterationCount * batchSize), samples(), sum(0) { }

    void addSample(nanosecs sample)
    {
        sum += sample;
        samples.push_back(sample);
    }

    void close()
    {
        mean = sum / samples.size();

        devSum = std::accumulate(
            samples.begin(), samples.end(), (nanosecs)0,
            [=](nanosecs s, nanosecs x) { return s + abs(x - mean); });
        devMean = devSum / samples.size();
    }
};

template<typename Types>
void runBenchmark(
        const typename Types::TGlobalContext *globalCtx,
        const typename Types::TDevice &dev,
        const std::string &hashSpec,
        const void *salt, size_t saltLength,
        size_t iterationCount, size_t dkLength,
        size_t batchSize, size_t sampleCount)
{
    std::vector<typename Types::TDevice> devices(&dev, &dev + 1);

    typename Types::THashFunctionContext hfContext(globalCtx, devices, hashSpec);
    typename Types::TComputeContext computeContext(&hfContext, salt, saltLength, dkLength, iterationCount);
    typename Types::TDeviceContext deviceContext(&computeContext, 0);
    typename Types::TProcessingUnit unit(&deviceContext, batchSize);

    typedef std::chrono::steady_clock clock_type;

    std::cout << "Starting computation..." << std::endl;

    DummyPasswordGenerator pwgen;
    RunTimeStatistics wrTimeStats(1, batchSize);
    RunTimeStatistics compTimeStats(iterationCount, batchSize);
    RunTimeStatistics rdTimeStats(1, batchSize);
    for (size_t i = 0; i < sampleCount; i++) {
        std::cout << "  Sample " << i << "..." << std::endl;

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

        clock_type::duration wrTime = checkpt1 - checkpt0;
        auto wrTimeNs = RunTimeStatistics::toNanoseconds(wrTime);
        std::cout << "    Writing took     " << RunTimeStatistics::repr(wrTimeNs) << std::endl;
        wrTimeStats.addSample(wrTimeNs);

        clock_type::duration compTime = checkpt2 - checkpt1;
        auto compTimeNs = RunTimeStatistics::toNanoseconds(compTime);
        std::cout << "    Computation took " << RunTimeStatistics::repr(compTimeNs) << std::endl;
        compTimeStats.addSample(compTimeNs);

        clock_type::duration rdTime = checkpt3 - checkpt2;
        auto rdTimeNs = RunTimeStatistics::toNanoseconds(rdTime);
        std::cout << "    Reading took     " << RunTimeStatistics::repr(rdTimeNs) << std::endl;
        rdTimeStats.addSample(wrTimeNs);
    }
    compTimeStats.close();

    std::cout << "Average computation time: "
              << RunTimeStatistics::repr(compTimeStats.getMean())
              << std::endl;
    std::cout << "Average deviation: "
              << RunTimeStatistics::repr(compTimeStats.getMeanDeviation())
              << " (" << compTimeStats.getMeanDeviationPerMean() * 100.0 << "%)"
              << std::endl;

    std::cout << "Average computation time (per 1M iterations): "
              << RunTimeStatistics::repr(compTimeStats.getMeanPer1MIterations())
              << std::endl;
    std::cout << "Average deviation (per 1M iterations): "
              << RunTimeStatistics::repr(compTimeStats.getMeanDeviationPer1MIterations())
              << std::endl;

    std::cout << "Average iterations per second: "
              << compTimeStats.getIterationsPerSecond()
              << std::endl;
}

#endif // BENCHMARK_H

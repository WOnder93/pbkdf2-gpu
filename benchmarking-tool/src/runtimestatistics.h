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

#ifndef RUNTIMESTATISTICS_H
#define RUNTIMESTATISTICS_H

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <numeric>

typedef uintmax_t nanosecs;

template<class duration>
static nanosecs toNanoseconds(const duration &d)
{
    return d.count() * ((nanosecs)1000 * 1000 * 1000 * duration::period::num / duration::period::den);
}

class DataSet
{
private:
    std::vector<uintmax_t> samples;
    uintmax_t sum;
    uintmax_t mean;
    uintmax_t devSum;
    uintmax_t devMean;

public:
    inline const std::vector<uintmax_t> &getSamples() const { return samples; }
    inline uintmax_t getMean() const { return mean; }
    inline uintmax_t getMeanDeviation() const { return devMean; }

    inline double getMeanDeviationPerMean() const { return (double)devMean / mean; }

    inline DataSet() : samples(), sum(0) { }

    void addSample(uintmax_t sample)
    {
        sum += sample;
        samples.push_back(sample);
    }

    void close()
    {
        mean = sum / samples.size();

        devSum = std::accumulate(
            samples.begin(), samples.end(), (uintmax_t)0,
            [=](uintmax_t s, uintmax_t x)
        {
            auto dev = (intmax_t)(x - mean);
            return s + (dev >= 0 ? dev : -dev);
        });
        devMean = devSum / samples.size();
    }
};

class RunTimeStatistics
{
private:
    uintmax_t iter;

    DataSet itersPerSec;
    DataSet ns;
    DataSet nsPer1MIters;

public:
    inline const DataSet &getIterationsPerSecond() const { return itersPerSec; }
    inline const DataSet &getNanoseconds() const { return ns; }
    inline const DataSet &getNsPer1MIterations() const { return nsPer1MIters; }

    inline RunTimeStatistics(size_t iterationCount, size_t batchSize)
        : iter(iterationCount * batchSize),
          itersPerSec(), ns(), nsPer1MIters()
    {
    }

    void addSample(nanosecs sample)
    {
        ns.addSample((uintmax_t)sample);
        nsPer1MIters.addSample(((uintmax_t)sample * 1000 * 1000) / iter);
        itersPerSec.addSample((iter * 1000 * 1000 * 1000) / (uintmax_t)sample);
    }

    void close()
    {
        ns.close();
        nsPer1MIters.close();
        itersPerSec.close();
    }

    inline static double toMinutes(nanosecs ns)
    {
        return (double)ns / ((nanosecs)60 * 1000 * 1000 * 1000);
    }

    inline static double toSeconds(nanosecs ns)
    {
        return (double)ns / ((nanosecs)1000 * 1000 * 1000);
    }

    inline static double toMilliSeconds(nanosecs ns)
    {
        return (double)ns / ((nanosecs)1000 * 1000);
    }

    inline static double toMicroSeconds(nanosecs ns)
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
};

#endif // RUNTIMESTATISTICS_H

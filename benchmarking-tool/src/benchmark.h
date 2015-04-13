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

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "libpbkdf2-gpu-common/logging.h"

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

    void operator()(const char *&password, std::size_t &passwordSize) {
        currentPw.resize(PASSWORD_LENGTH);
        for (std::size_t i = 0; i < PASSWORD_LENGTH; i++) {
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
        const void *salt, std::size_t saltLength,
        std::size_t iterationCount, std::size_t dkLength,
        std::size_t batchSize, std::size_t sampleCount)
{
    std::vector<typename Types::TDevice> devices(&dev, &dev + 1);

    pbkdf2_gpu::common::RootLogger logger(nullptr);
    typename Types::THashFunctionContext hfContext(globalCtx, devices, hashSpec);
    typename Types::TComputeContext computeContext(&hfContext, salt, saltLength, dkLength, iterationCount);
    typename Types::TDeviceContext deviceContext(&computeContext, dev);
    typename Types::TProcessingUnit unit(&deviceContext, batchSize, &logger);

    typedef std::chrono::steady_clock clock_type;

    if (beVerbose) {
        std::cout << "Starting computation..." << std::endl;
    }

    DummyPasswordGenerator pwgen;
    RunTimeStatistics compTimeStats(iterationCount, batchSize);
    for (std::size_t i = 0; i < sampleCount; i++) {
        if (beVerbose) {
            std::cout << "  Sample " << i << "..." << std::endl;
        }

        clock_type::time_point checkpt0 = clock_type::now();
        {
            auto passwords = unit.openPasswords();
            typename Types::TProcessingUnit::Passwords::Writer writer(passwords);
            for (std::size_t i = 0; i < batchSize; i++) {
                const char *pw;
                std::size_t pwLength;
                pwgen(pw, pwLength);
                writer.setPassword(pw, pwLength);

                writer.moveForward(1);
            }
        }
        clock_type::time_point checkpt1 = clock_type::now();

        unit.beginProcessing();
        unit.endProcessing();

        clock_type::time_point checkpt2 = clock_type::now();
        {
            auto keys = unit.openDerivedKeys();
            typename Types::TProcessingUnit::DerivedKeys::Reader reader(keys);
            for (std::size_t i = 0; i < batchSize; i++) {
                reader.moveForward(1);
            }
        }
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

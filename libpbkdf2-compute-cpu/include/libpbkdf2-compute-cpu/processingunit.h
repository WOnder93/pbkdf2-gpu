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

#ifndef LIBPBKDF2_COMPUTE_CPU_PROCESSINGUNIT_H
#define LIBPBKDF2_COMPUTE_CPU_PROCESSINGUNIT_H

#include "devicecontext.h"

#include "libpbkdf2-gpu-common/logging.h"

#include <vector>
#include <memory>
#include <functional>
#include <future>

namespace libpbkdf2 {
namespace compute {
namespace cpu {

using namespace pbkdf2_gpu::common;

class ProcessingUnit
{
private:
    const DeviceContext *context;
    std::size_t batchSize;
    Logger *logger;

    std::vector<std::string> passwordBuffer;
    std::unique_ptr<unsigned char[]> dkBuffer;

    std::future<void> taskFuture;

public:
    class PasswordWriter
    {
    private:
        std::vector<std::string>::iterator it;

    public:
        PasswordWriter(ProcessingUnit &parent, std::size_t index = 0);

        void moveForward(std::size_t offset);
        void moveBackwards(std::size_t offset);

        void setPassword(const void *pw, std::size_t pwSize) const;
    };

    class DerivedKeyReader
    {
    private:
        std::size_t dkLength;
        const unsigned char *src;

    public:
        DerivedKeyReader(ProcessingUnit &parent, std::size_t index = 0);

        void moveForward(std::size_t offset);
        void moveBackwards(std::size_t offset);

        const void *getDerivedKey() const;
    };

    std::size_t getBatchSize() const { return batchSize; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    ProcessingUnit() { }

    ProcessingUnit(const ProcessingUnit &) = delete;
    ProcessingUnit &operator=(const ProcessingUnit &) = delete;

    ProcessingUnit(ProcessingUnit &&) = default;
    ProcessingUnit &operator=(ProcessingUnit &&) = default;

    ProcessingUnit(const DeviceContext *context,
                   std::size_t batchSize, Logger *logger);

    void beginProcessing();
    void endProcessing();
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_PROCESSINGUNIT_H

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

#ifndef LIBPBKDF2_COMPUTE_OPENCL_PROCESSINGUNIT_H
#define LIBPBKDF2_COMPUTE_OPENCL_PROCESSINGUNIT_H

#include "devicecontext.h"

#include <functional>
#include <iterator>
#include <memory>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class ProcessingUnit
{
private:
    const DeviceContext *context;
    std::size_t batchSize;

    cl::CommandQueue cmdQueue;

    cl::Buffer inputBuffer;
    cl::Buffer outputBuffer;
    cl::Buffer debugBuffer;

    cl::Kernel kernel;

    std::size_t inputSize;
    std::size_t outputSize;

    std::size_t outputBlocks;

    std::unique_ptr<cl_uint[]> inputDataBuffer;
    std::unique_ptr<cl_uint[]> outputDataBuffer;

    cl::Event event;

public:
    class Passwords
    {
    private:
        const ProcessingUnit *parent;
        void *hostBuffer;

    public:
        class Writer
        {
        private:
            cl_uint *dest;

            std::size_t count;
            std::size_t inputSize;
            const HashAlgorithm *hashAlg;
            std::unique_ptr<cl_uint[]> buffer;

        public:
            Writer(const Passwords &parent, std::size_t index = 0);

            void moveForward(std::size_t offset);
            void moveBackwards(std::size_t offset);

            void setPassword(const void *pw, std::size_t pwSize) const;
        };

        Passwords(const ProcessingUnit *parent);
        ~Passwords();
    };

    class DerivedKeys
    {
    private:
        const ProcessingUnit *parent;
        void *hostBuffer;

    public:
        class Reader
        {
        private:
            const cl_uint *src;

            std::size_t count;
            std::size_t outputSize;
            std::size_t outputBlockCount;
            std::size_t outputBlockSize;
            std::unique_ptr<cl_uint[]> buffer;

        public:
            Reader(const DerivedKeys &parent, std::size_t index = 0);

            void moveForward(std::size_t offset);
            void moveBackwards(std::size_t offset);

            const void *getDerivedKey() const;
        };

        DerivedKeys(const ProcessingUnit *parent);
        ~DerivedKeys();
    };

    inline std::size_t getBatchSize() const { return batchSize; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline ProcessingUnit() { }

    ProcessingUnit(const ProcessingUnit &) = delete;
    ProcessingUnit &operator=(const ProcessingUnit &) = delete;

    ProcessingUnit(ProcessingUnit &&) = default;
    ProcessingUnit &operator=(ProcessingUnit &&) = default;

    ProcessingUnit(const DeviceContext *context, std::size_t batchSize);

    inline Passwords openPasswords() { return Passwords(this); }
    inline DerivedKeys openDerivedKeys() { return DerivedKeys(this); }

    void beginProcessing();
    void endProcessing();
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_PROCESSINGUNIT_H

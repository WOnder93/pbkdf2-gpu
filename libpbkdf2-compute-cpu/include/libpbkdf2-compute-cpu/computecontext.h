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

#ifndef LIBPBKDF2_COMPUTE_CPU_COMPUTECONTEXT_H
#define LIBPBKDF2_COMPUTE_CPU_COMPUTECONTEXT_H

#include "hashfunctioncontext.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class ComputeContext
{
private:
    const HashFunctionContext *parentContext;

    const void *saltData;
    std::size_t saltLength;
    std::size_t dkLength;
    std::size_t iterationCount;

public:
    inline const HashFunctionContext *getParentContext() const { return parentContext; }

    inline const void *getSaltData() const { return saltData; }
    inline std::size_t getSaltLength() const { return saltLength; }
    inline std::size_t getDerivedKeyLength() const { return dkLength; }
    inline std::size_t getIterationCount() const { return iterationCount; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline ComputeContext() { }

    ComputeContext(const ComputeContext &) = delete;
    ComputeContext &operator=(const ComputeContext &) = delete;

    ComputeContext(ComputeContext &&) = default;
    ComputeContext &operator=(ComputeContext &&) = default;

    inline ComputeContext(const HashFunctionContext *parentContext,
                   const void *salt, std::size_t saltLength,
                   std::size_t dkLength, std::size_t iterationCount)
        : parentContext(parentContext), saltData(salt), saltLength(saltLength),
          dkLength(dkLength), iterationCount(iterationCount)
    {
    }
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_COMPUTECONTEXT_H

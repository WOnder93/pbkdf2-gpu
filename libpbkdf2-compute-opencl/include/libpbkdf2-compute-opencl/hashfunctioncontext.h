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

#ifndef LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONCONTEXT_H
#define LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONCONTEXT_H

#include <string>

#include "libhashspec-hashalgorithm/hashalgorithm.h"

#include "opencl.h"
#include "globalcontext.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

using namespace libhashspec::hashalgorithm;

class HashFunctionContext
{
private:
    const GlobalContext *globalContext;

    std::vector<cl::Device> devices;
    cl::Context context;

    std::string hashSpec;
    const HashAlgorithm *hashAlg;

public:
    inline const GlobalContext *getGlobalContext() const { return globalContext; }

    inline const std::vector<cl::Device> &getDevices() const { return devices; }
    inline const cl::Context &getContext() const { return context; }

    inline const std::string &getHashSpec() const { return hashSpec; }
    inline const HashAlgorithm *getHashAlgorithm() const { return hashAlg; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline HashFunctionContext() { }

    HashFunctionContext(const HashFunctionContext &) = delete;
    HashFunctionContext &operator=(const HashFunctionContext &) = delete;

    HashFunctionContext(HashFunctionContext &&) = default;
    HashFunctionContext &operator=(HashFunctionContext &&) = default;

    HashFunctionContext(
            const GlobalContext *globalContext,
            const std::vector<Device> &devices,
            const std::string &hashSpec);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONCONTEXT_H

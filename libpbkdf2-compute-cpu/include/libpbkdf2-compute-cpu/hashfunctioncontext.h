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

#ifndef LIBPBKDF2_COMPUTE_CPU_HASHFUNCTIONCONTEXT_H
#define LIBPBKDF2_COMPUTE_CPU_HASHFUNCTIONCONTEXT_H

#include "globalcontext.h"

#include <openssl/evp.h>

#include <vector>
#include <string>

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class HashFunctionContext
{
private:
    const GlobalContext *globalContext;
    const ::EVP_MD *digest;

public:
    const GlobalContext *getGlobalContext() const { return globalContext; }
    const ::EVP_MD *getDigest() const { return digest; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    HashFunctionContext() { }

    HashFunctionContext(const HashFunctionContext &) = delete;
    HashFunctionContext &operator=(const HashFunctionContext &) = delete;

    HashFunctionContext(HashFunctionContext &&) = default;
    HashFunctionContext &operator=(HashFunctionContext &&) = default;

    HashFunctionContext(
            const GlobalContext *globalContext,
            const std::vector<Device> &devices,
            const std::string &hashSpec);
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_HASHFUNCTIONCONTEXT_H

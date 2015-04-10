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

#ifndef LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H
#define LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H

#include <openssl/evp.h>

#include <string>
#include <stdexcept>

namespace libhashspec {
namespace hashalgorithm {

class HashException : std::runtime_error
{
public:
    HashException(const std::string &message)
        : std::runtime_error(message)
    {
    }
};

class HashAlgorithm
{
private:
    const ::EVP_MD *digest;

    inline HashAlgorithm(const ::EVP_MD *digest)
        : digest(digest)
    {
    }

public:
    class Context
    {
    private:
        ::EVP_MD_CTX ctx;

    public:
        Context(const HashAlgorithm &hashAlg);
        ~Context();

        void update(const void *data, std::size_t size);
        void digest(void *dest);
    };

    std::size_t getInputBlockLength() const;
    std::size_t getOutputBlockLength() const;

    inline void computeDigest(const void *data, std::size_t size, void *dest) const
    {
        Context ctx(*this);
        ctx.update(data, size);
        ctx.digest(dest);
    }

    static const HashAlgorithm &getAlgorithm(const std::string &hashSpec);
};


} // namespace hashalgorithm
} // namespace libhashspec

#endif // LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H

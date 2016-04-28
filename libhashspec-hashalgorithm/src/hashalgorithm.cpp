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

#include "hashalgorithm.h"

#include "libhashspec-openssl/digestlookup.h"

#include <unordered_map>

namespace libhashspec {
namespace hashalgorithm {

using namespace libhashspec::openssl;

HashAlgorithm::Context::Context(const HashAlgorithm &hashAlg)
{
    EVP_MD_CTX_init(&ctx);
    EVP_DigestInit(&ctx, hashAlg.digest);
}

HashAlgorithm::Context::~Context()
{
    EVP_MD_CTX_cleanup(&ctx);
}

void HashAlgorithm::Context::update(const void *data, std::size_t size)
{
    EVP_DigestUpdate(&ctx, data, size);
}

void HashAlgorithm::Context::digest(void *dest)
{
    /* the digest size will be guarenteed to match the output block length: */
    EVP_DigestFinal(&ctx, (unsigned char *)dest, NULL);
}

std::size_t HashAlgorithm::getInputBlockLength() const
{
    return (std::size_t)EVP_MD_block_size(digest);
}

std::size_t HashAlgorithm::getOutputBlockLength() const
{
    return (std::size_t)EVP_MD_size(digest);
}

const HashAlgorithm &HashAlgorithm::getAlgorithm(const std::string &hashSpec)
{
    static const std::unordered_map<std::string, HashAlgorithm> algorithms = {
        { "ripemd160", HashAlgorithm(DigestLookup::getDigest("ripemd160")) },
        { "sha1",      HashAlgorithm(DigestLookup::getDigest("sha1")) },
        { "sha256",    HashAlgorithm(DigestLookup::getDigest("sha256")) },
        { "sha512",    HashAlgorithm(DigestLookup::getDigest("sha512")) },
        { "whirlpool", HashAlgorithm(DigestLookup::getDigest("whirlpool")) },
    };

    return algorithms.at(hashSpec);
}

} // namespace hashalgorithm
} // namespace libhashspec

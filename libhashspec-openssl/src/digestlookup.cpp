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

#include "digestlookup.h"

#include <unordered_map>

namespace libhashspec {
namespace openssl {

static const std::unordered_map<std::string, const EVP_MD *> digests = {
    { "ripemd160", EVP_ripemd160() },
    { "sha1",      EVP_sha1() },
    { "sha256",    EVP_sha256() },
    { "sha512",    EVP_sha512() },
    { "whirlpool", EVP_whirlpool() },
};

const EVP_MD *DigestLookup::getDigest(std::string hashSpec)
{
    return digests.at(hashSpec);
}

} // namespace openssl
} // namespace libhashspec

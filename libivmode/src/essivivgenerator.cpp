/*
 * Based on lib/crypto_backend/crypto_storage.c from Cryptsetup
 * <https://gitlab.com/cryptsetup/cryptsetup>
 *
 * Copyright (C) 2014, Milan Broz
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

#include "essivivgenerator.h"

#include <cstring>

namespace libivmode {

EssivIVGenerator::EssivIVGenerator(const std::string &hashSpec)
{
    hashAlgorithm = &HashAlgorithm::getAlgorithm(hashSpec);
}

EssivIVGenerator::Context::Context(
        std::size_t ivLength,
        std::unique_ptr<unsigned char[]> &&hashedKey,
        const CipherAlgorithm &cipherAlg)
    : ivLength(ivLength), hashedKey(std::move(hashedKey))
{
    ctx = CipherAlgorithm::EncryptionContex(&cipherAlg, hashedKey.get());
}

void EssivIVGenerator::Context::generateIV(
        std::size_t sector, void *dest) const
{
    unsigned char *cursor = (unsigned char *)dest;
    cursor[0] = sector         & 0xff;
    cursor[1] = (sector >>  8) & 0xff;
    cursor[2] = (sector >> 16) & 0xff;
    cursor[3] = (sector >> 24) & 0xff;
    cursor[4] = (sector >> 32) & 0xff;
    cursor[5] = (sector >> 40) & 0xff;
    cursor[6] = (sector >> 48) & 0xff;
    cursor[7] = (sector >> 56) & 0xff;
    std::memset(cursor + 8, 0, ivLength - 8);

    ctx.encrypt(dest, ivLength, nullptr, dest);
}

std::shared_ptr<const IVGenerator::Context> EssivIVGenerator::createContext(
        std::size_t ivLength, const std::string &cipherName,
        const void *key, std::size_t keyLength) const
{
    std::size_t hashLength = hashAlgorithm->getOutputBlockLength();
    auto buffer = std::unique_ptr<unsigned char[]>(new unsigned char[hashLength]);
    hashAlgorithm->computeDigest(key, keyLength, buffer.get());

    auto &cipherAlg = CipherAlgorithm::getAlgorithm(cipherName, hashLength, "ecb");
    if (cipherAlg.getBlockLength() != ivLength) {
        throw std::logic_error(
                    "EssivIVGenerator: The IV length "
                    "must match the cipher block length!");
    }

    return std::make_shared<Context>(ivLength, std::move(buffer), cipherAlg);
}

} // namespace libivmode

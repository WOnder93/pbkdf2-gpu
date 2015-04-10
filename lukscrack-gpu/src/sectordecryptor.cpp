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

#include "sectordecryptor.h"

#define LUKS_SECTOR_SIZE 512

namespace lukscrack {

SectorDecryptor::SectorDecryptor(
        const std::string &cipherName, std::size_t keySize,
        const std::string &cipherMode,
        const std::string &ivmode)
    : cipherName(cipherName)
{
    cipherAlg = &CipherAlgorithm::getAlgorithm(cipherName, keySize, cipherMode);
    ivgen = IVGenerator::getGenerator(ivmode);
}

void SectorDecryptor::decryptSectors(
        const void *key, const void *data, void *dest,
        std::size_t startSector, std::size_t sectorCount) const
{
    std::size_t ivLength = cipherAlg->getIVLength();
    CipherAlgorithm::DecryptionContex decrCtx(cipherAlg, key);
    auto ivgenCtx = ivgen->createContext(
                cipherAlg->getIVLength(), cipherName,
                key, cipherAlg->getKeySize());

    std::unique_ptr<unsigned char[]> ivBuffer(nullptr);
    if (ivgenCtx.get() != nullptr) {
        ivBuffer = std::unique_ptr<unsigned char[]> {
            new unsigned char[ivLength]
        };
    }
    auto iv = ivBuffer.get();
    auto inputCursor = (const unsigned char *)data;
    auto outputCursor = (unsigned char *)dest;
    for (std::size_t i = 0; i < sectorCount; i++) {
        if (ivgenCtx.get() != nullptr) {
            ivgenCtx->generateIV(startSector + i, iv);
        }
        decrCtx.decrypt(inputCursor, LUKS_SECTOR_SIZE, iv, outputCursor);

        inputCursor += LUKS_SECTOR_SIZE;
        outputCursor += LUKS_SECTOR_SIZE;
    }
}

} // namespace lukscrack

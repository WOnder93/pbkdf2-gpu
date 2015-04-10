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

#ifndef LUKSCRACK_SECTORDECRYPTOR_H
#define LUKSCRACK_SECTORDECRYPTOR_H

#include "libivmode/ivgenerator.h"
#include "libcipherspec-cipheralgorithm/cipheralgorithm.h"

namespace lukscrack {

using namespace libivmode;
using namespace libcipherspec::cipheralgorithm;

class SectorDecryptor
{
private:
    const CipherAlgorithm *cipherAlg;
    std::string cipherName;
    std::shared_ptr<const IVGenerator> ivgen;

public:
    inline SectorDecryptor() { }

    SectorDecryptor(
            const std::string &cipherName, std::size_t keySize,
            const std::string &cipherMode,
            const std::string &ivmode);

    void decryptSectors(
            const void *key, const void *data, void *dest,
            std::size_t startSector, std::size_t sectorCount) const;
};

} // namespace lukscrack

#endif // LUKSCRACK_SECTORDECRYPTOR_H

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

#include "crackingcontext.h"

namespace lukscrack {
namespace gpu {

CrackingContext::CrackingContext(const GlobalContext *global, const std::vector<Device> &devices, const PasswordData *passwordData)
    : passwordData(passwordData)
{
    auto &hashSpec = passwordData->getHashSpec();
    auto &cipherName = passwordData->getCipherName();
    auto &mode = passwordData->getCipherMode();
    size_t keySize = passwordData->getKeySize();
    size_t afStripes = passwordData->getKeyslotStripes();

    size_t delim = mode.find('-');
    std::string cipherMode, ivmode;
    if (delim == std::string::npos) {
        cipherMode.assign(mode);
    } else {
        cipherMode.assign(mode.begin(), mode.begin() + delim);
        ivmode.assign(mode.begin() + delim + 1, mode.end());
    }

    decryptor = SectorDecryptor(cipherName, keySize, cipherMode, ivmode);
    afMerger = AFMerger(keySize, afStripes, hashSpec);

    hfContext = HashFunctionContext(global, devices, passwordData->getHashSpec());

    keyslotContext = ComputeContext(
                &hfContext,
                passwordData->getKeyslotSalt(), PasswordData::SALT_LENGTH,
                passwordData->getKeySize(), passwordData->getKeyslotIter());
    mkDigestContext = ComputeContext(
                &hfContext,
                passwordData->getMasterKeyDigestSalt(), PasswordData::SALT_LENGTH,
                PasswordData::MASTER_KEY_DIGEST_LENGTH, passwordData->getMasterKeyDigestIter());
}

} // namespace gpu
} // namespace lukscrack

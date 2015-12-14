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

#ifndef LUKSCRACK_GPU_CRACKINGCONTEXT_H
#define LUKSCRACK_GPU_CRACKINGCONTEXT_H

// TODO: make generic
#include "libpbkdf2-compute-opencl/computecontext.h"

#include "passworddata.h"
#include "sectordecryptor.h"
#include "afmerger.h"

namespace lukscrack {
namespace gpu {

using namespace libpbkdf2::compute::opencl;

class CrackingContext
{
private:
    const PasswordData *passwordData;

    HashFunctionContext hfContext;

    ComputeContext keyslotContext;
    ComputeContext mkDigestContext;

    SectorDecryptor decryptor;
    AFMerger afMerger;

public:
    const PasswordData *getPasswordData() const { return passwordData; }

    const ComputeContext &getKeyslotContext() const { return keyslotContext; }
    const ComputeContext &getMKDigestContext() const { return mkDigestContext; }

    const SectorDecryptor &getSectorDecryptor() const { return decryptor; }
    const AFMerger &getAFMerger() const { return afMerger; }

    CrackingContext(
            const GlobalContext *global,
            const std::vector<Device> &devices,
            const PasswordData *passwordData);
};

} // namespace gpu
} // namespace lukscrack

#endif // LUKSCRACK_GPU_CRACKINGCONTEXT_H

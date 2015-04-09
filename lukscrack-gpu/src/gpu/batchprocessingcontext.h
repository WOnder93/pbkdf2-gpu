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

#ifndef LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H
#define LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H

#include "libpbkdf2-compute-opencl/processingunit.h"

#include "passwordgenerator.h"

#include "crackingcontext.h"
#include "threadpool.h"

#include <vector>

namespace lukscrack {
namespace gpu {

using namespace libpbkdf2::compute::opencl;

class BatchProcessingContext
{
private:
    const CrackingContext *parentContext;
    ThreadPool *threadPool;

    size_t batchSize;

    DeviceContext keyslotContext;
    DeviceContext mkDigestContext;

    ProcessingUnit keyslotUnit;
    ProcessingUnit mkDigestUnit;

    std::vector<std::string> passwords;
    std::unique_ptr<unsigned char[]> keyMaterialDecryptedBuffers;
    std::unique_ptr<unsigned char[]> masterKeyBuffers;

public:
    inline const std::vector<std::string> &getCurrentPasswords() const { return passwords; }

    inline BatchProcessingContext() { }

    BatchProcessingContext(
            const CrackingContext *parentContext, const Device &device,
            ThreadPool *threadPool, size_t batchSize);

    bool initializePasswords(lukscrack::PasswordGenerator &generator);

    inline void beginKeyslotUnit() { keyslotUnit.beginProcessing(); }
    inline void endKeyslotUnit() { keyslotUnit.endProcessing(); }

    void decryptMasterKey();

    inline void beginMKDigestUnit() { mkDigestUnit.beginProcessing(); }
    inline void endMKDigestUnit() { mkDigestUnit.endProcessing(); }

    /**
     * @brief processResults
     * @return the index of the valid password, or -1 if no password is valid
     */
    ssize_t processResults();
};

} // namespace gpu
} // namespace lukscrack

#endif // LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H

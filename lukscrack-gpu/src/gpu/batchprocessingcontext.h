#ifndef LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H
#define LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H

// TODO: make generic
#include "libpbkdf2-compute-opencl/processingunit.h"

#include "cipheralgorithm.h"
#include "passwordgenerator.h"

#include "crackingcontext.h"

#include <vector>

namespace lukscrack {
namespace gpu {

using namespace libpbkdf2::compute::opencl;

class BatchProcessingContext
{
private:
    const CrackingContext *parentContext;

    size_t batchSize;

    DeviceContext keyslotContext;
    DeviceContext mkDigestContext;

    ProcessingUnit keyslotUnit;
    ProcessingUnit mkDigestUnit;

    std::shared_ptr<const CipherAlgorithm> cipherAlg;

    std::vector<std::string> passwords;

public:
    BatchProcessingContext(
            const CrackingContext *parentContext,
            size_t deviceIndex, size_t batchSize);

    void initializePasswords(lukscrack::PasswordGenerator &generator);
    void decryptMasterKey();
    /**
     * @brief processResults
     * @return the index of the valid password, or -1 if no password is valid
     */
    ssize_t processResults();
};

} // namespace gpu
} // namespace lukscrack

#endif // LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H

#ifndef LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H
#define LUKSCRACK_GPU_BATCHPROCESSINGCONTEXT_H

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
    std::vector<char> keyMaterialDecryptedBuffer;
    std::vector<char> masterKeyBuffer;

public:
    inline const std::vector<std::string> &getCurrentPasswords() const { return passwords; }

    inline BatchProcessingContext() { }

    BatchProcessingContext(
            const CrackingContext *parentContext,
            const Device &device, size_t batchSize);

    void initializePasswords(lukscrack::PasswordGenerator &generator);

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

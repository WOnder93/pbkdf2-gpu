#include "batchprocessingcontext.h"

#include "crypto/af.h"

#include <memory>
#include <cstring>

namespace lukscrack {
namespace gpu {

BatchProcessingContext::BatchProcessingContext(
        const CrackingContext *parentContext,
        const Device &deviceIndex, size_t batchSize)
    : parentContext(parentContext), batchSize(batchSize),
      passwords(batchSize)
{
    const PasswordData *passwordData = parentContext->getPasswordData();
    cipherAlg = CipherAlgorithm::get(passwordData->getCipherName(), passwordData->getCipherMode(),
                                     passwordData->getKeySize()); // TODO handle missing

    keyslotContext = DeviceContext(&parentContext->getKeyslotContext(), deviceIndex);
    mkDigestContext = DeviceContext(&parentContext->getMKDigestContext(), deviceIndex);

    keyslotUnit = ProcessingUnit(&keyslotContext, batchSize);
    mkDigestUnit = ProcessingUnit(&mkDigestContext, batchSize);

    size_t keySize = passwordData->getKeySize();
    size_t afStripes = passwordData->getKeyslotStripes();
    size_t keyMaterialSize = afStripes * keySize;
    keyMaterialDecryptedBuffer = std::vector<char>(keyMaterialSize);
    masterKeyBuffer = std::vector<char>(keySize);
}

void BatchProcessingContext::initializePasswords(lukscrack::PasswordGenerator &generator)
{
    size_t i = 0;
    keyslotUnit.writePasswords([&] (const char *&outPwData, size_t &outPwLength) {
        const char *pwData;
        size_t pwLength;
        if (!generator.nextPassword(pwData, pwLength)) {
            pwData = nullptr;
            pwLength = 0;
        }
        passwords[i].assign(pwData, pwData + pwLength);
        outPwData = pwData;
        outPwLength = pwLength;
        ++i;
    });
}

void BatchProcessingContext::decryptMasterKey()
{
    auto passwordData = parentContext->getPasswordData();

    const char *keyMaterial = passwordData->getKeyMaterial();
    size_t keySize = passwordData->getKeySize();
    size_t afStripes = passwordData->getKeyslotStripes();
    size_t keyMaterialSize = afStripes * keySize;

    char *keyMaterialDecrypted = keyMaterialDecryptedBuffer.data();
    char *masterKey = masterKeyBuffer.data();
    keyslotUnit.readDerivedKeys([=](ProcessingUnit::KeyIterator &iter)
    {
        mkDigestUnit.writePasswords([=, &iter](const char *&password, size_t &passwordSize)
        {
            const void *key = iter.nextKey();
            cipherAlg->decrypt(key, keyMaterial, keyMaterialSize, keyMaterialDecrypted);
            crypto::AF_merge(keyMaterialDecrypted, masterKey, keySize,
                             afStripes, passwordData->getHashSpec().c_str());

            password = masterKey;
            passwordSize = keySize;
        });
    });
}

ssize_t BatchProcessingContext::processResults()
{
    auto passwordData = parentContext->getPasswordData();
    const char *mkDigest = passwordData->getMasterKeyDigest();

    ssize_t match = -1;
    mkDigestUnit.readDerivedKeys([=, &match](ProcessingUnit::KeyIterator &iter)
    {
        for (size_t i = 0; i < batchSize; i++) {
            auto key = iter.nextKey();
            if (std::memcmp(key, mkDigest, PasswordData::MASTER_KEY_DIGEST_SIZE) == 0) {
                match = i;
                break;
            }
        }
    });
    return match;
}

} // namespace gpu
} // namespace lukscrack

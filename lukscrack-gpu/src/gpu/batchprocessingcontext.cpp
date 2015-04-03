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

    keyslotContext = DeviceContext(&parentContext->getKeyslotContext(), deviceIndex);
    mkDigestContext = DeviceContext(&parentContext->getMKDigestContext(), deviceIndex);

    keyslotUnit = ProcessingUnit(&keyslotContext, batchSize);
    mkDigestUnit = ProcessingUnit(&mkDigestContext, batchSize);

    size_t keySize = passwordData->getKeySize();
    size_t keyMaterialSectors = passwordData->getKeyMaterialSectors();
    size_t keyMaterialSize = keyMaterialSectors * PasswordData::SECTOR_SIZE;
    keyMaterialDecryptedBuffer = std::vector<unsigned char>(keyMaterialSize);
    masterKeyBuffer = std::vector<unsigned char>(keySize);
}

bool BatchProcessingContext::initializePasswords(lukscrack::PasswordGenerator &generator)
{
    size_t i = 0;
    bool passwordsLeft = true;
    keyslotUnit.writePasswords([&] (const char *&outPwData, size_t &outPwLength) {
        const char *pwData;
        size_t pwLength;
        if (!generator.nextPassword(pwData, pwLength)) {
            pwData = nullptr;
            pwLength = 0;
            passwordsLeft = false;
        }
        passwords[i].assign(pwData, pwData + pwLength);
        outPwData = pwData;
        outPwLength = pwLength;
        ++i;
    });
    return passwordsLeft;
}

void BatchProcessingContext::decryptMasterKey()
{
    auto passwordData = parentContext->getPasswordData();

    const unsigned char *keyMaterial = passwordData->getKeyMaterial();

    size_t keySize = passwordData->getKeySize();
    size_t afStripes = passwordData->getKeyslotStripes();

    size_t keyMaterialSectors = passwordData->getKeyMaterialSectors();

    unsigned char *keyMaterialDecrypted = keyMaterialDecryptedBuffer.data();
    unsigned char *masterKey = masterKeyBuffer.data();

    auto decryptor = &parentContext->getSectorDecryptor();
    keyslotUnit.readDerivedKeys([=](ProcessingUnit::KeyIterator &iter)
    {
        mkDigestUnit.writePasswords([=, &iter](const char *&password, size_t &passwordSize)
        {
            const void *key = iter.nextKey();

            decryptor->decryptSectors(
                        key, keyMaterial, keyMaterialDecrypted,
                        0, keyMaterialSectors);

            crypto::AF_merge((char *)keyMaterialDecrypted, (char *)masterKey, keySize,
                             afStripes, passwordData->getHashSpec().c_str());

            password = (char *)masterKey;
            passwordSize = keySize;
        });
    });
}

ssize_t BatchProcessingContext::processResults()
{
    auto passwordData = parentContext->getPasswordData();
    const unsigned char *mkDigest = passwordData->getMasterKeyDigest();

    ssize_t match = -1;
    mkDigestUnit.readDerivedKeys([=, &match](ProcessingUnit::KeyIterator &iter)
    {
        for (size_t i = 0; i < batchSize; i++) {
            auto key = iter.nextKey();
            if (std::memcmp(key, mkDigest, PasswordData::MASTER_KEY_DIGEST_LENGTH) == 0) {
                match = i;
                break;
            }
        }
    });
    return match;
}

} // namespace gpu
} // namespace lukscrack

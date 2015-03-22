#include "batchprocessingcontext.h"

#include "crypto/af.h"

#include <memory>
#include <cstring>

namespace lukscrack {
namespace gpu {

BatchProcessingContext::BatchProcessingContext(const CrackingContext *parentContext, size_t deviceIndex, size_t batchSize)
    : parentContext(parentContext), batchSize(batchSize)
{
    const PasswordData *passwordData = parentContext->getPasswordData();
    cipherAlg = CipherAlgorithm::get(passwordData->getCipherName(), passwordData->getCipherMode(),
                                     passwordData->getKeySize()); // TODO handle missing

    keyslotContext = DeviceContext(&parentContext->getKeyslotContext(), deviceIndex);
    mkDigestContext = DeviceContext(&parentContext->getMKDigestContext(), deviceIndex);

    keyslotUnit = ProcessingUnit(&keyslotContext, batchSize);
    mkDigestUnit = ProcessingUnit(&mkDigestContext, batchSize);

    passwords.resize(batchSize);
}

void BatchProcessingContext::initializePasswords(lukscrack::PasswordGenerator &generator)
{
    size_t i = 0;
    keyslotUnit.writePasswords([&] (const char *&password, size_t &passwordSize) {
        if (!generator.nextPassword()) {
            passwords[i] = std::string();
            password = nullptr;
            passwordSize = 0;
        } else {
            auto &pw = generator.getCurrentPassword();
            passwords[i] = pw;
            password = pw.data();
            passwordSize = pw.size();
        }
        ++i;
    });
    keyslotUnit.beginProcessing();
}

void BatchProcessingContext::decryptMasterKey()
{
    keyslotUnit.endProcessing();

    auto passwordData = parentContext->getPasswordData();

    const char *keyMaterial = passwordData->getKeyMaterial();
    size_t keySize = passwordData->getKeySize();
    size_t afStripes = passwordData->getKeyslotStripes();
    size_t keyMaterialSize = afStripes * keySize;

    /* buffers for decrypted material: */
    char *keyMaterialDecrypted = new char[keyMaterialSize];
    if (keyMaterialDecrypted == nullptr) {
        throw std::bad_alloc();
    }
    /* to ensure memory will be freed if an exception gets thrown: */
    std::unique_ptr<char[]> keyMaterialDecryptedOwner(keyMaterialDecrypted);

    char *masterKey = new char[keySize];
    if (masterKey == nullptr) {
        throw std::bad_alloc();
    }
    std::unique_ptr<char[]> masterKeyOwner(masterKey);

    keyslotUnit.readDerivedKeys(
                [=](ProcessingUnit::KeyIterator &iter) {
        mkDigestUnit.writePasswords(
                    [=, &iter](const char *&password, size_t &passwordSize) {
            const void *key = iter.nextKey();
            cipherAlg->decrypt(key, keyMaterial, keyMaterialSize, keyMaterialDecrypted);
            crypto::AF_merge(keyMaterialDecrypted, masterKey, keySize,
                             afStripes, passwordData->getHashSpec().c_str());

            password = masterKey;
            passwordSize = keySize;
        });
    });
    mkDigestUnit.beginProcessing();
}

ssize_t BatchProcessingContext::processResults()
{
    mkDigestUnit.endProcessing();

    auto passwordData = parentContext->getPasswordData();
    const char *mkDigest = passwordData->getMasterKeyDigest();

    ssize_t match = -1;
    mkDigestUnit.readDerivedKeys(
                [=, &match](ProcessingUnit::KeyIterator &iter) {
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

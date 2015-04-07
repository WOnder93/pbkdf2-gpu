#include "batchprocessingcontext.h"

#include "crypto/af.h"

#include <memory>
#include <cstring>

namespace lukscrack {
namespace gpu {

BatchProcessingContext::BatchProcessingContext(
        const CrackingContext *parentContext, const Device &deviceIndex,
        ThreadPool *threadPool, size_t batchSize)
    : parentContext(parentContext), threadPool(threadPool),
      batchSize(batchSize), passwords(batchSize)
{
    const PasswordData *passwordData = parentContext->getPasswordData();

    keyslotContext = DeviceContext(&parentContext->getKeyslotContext(), deviceIndex);
    mkDigestContext = DeviceContext(&parentContext->getMKDigestContext(), deviceIndex);

    keyslotUnit = ProcessingUnit(&keyslotContext, batchSize);
    mkDigestUnit = ProcessingUnit(&mkDigestContext, batchSize);

    size_t taskCount = threadPool->getSize();
    if (taskCount <= 1) {
        taskCount = 1;
    }
    size_t keySize = passwordData->getKeySize();
    size_t keyMaterialSectors = passwordData->getKeyMaterialSectors();
    size_t keyMaterialSize = keyMaterialSectors * PasswordData::SECTOR_SIZE;
    keyMaterialDecryptedBuffers = std::unique_ptr<unsigned char[]>(
                new unsigned char[taskCount * keyMaterialSize]);
    masterKeyBuffers = std::unique_ptr<unsigned char[]>(
                new unsigned char[taskCount * keySize]);
}

bool BatchProcessingContext::initializePasswords(lukscrack::PasswordGenerator &generator)
{
    bool passwordsLeft = true;
    auto passwords = keyslotUnit.openPasswords();
    ProcessingUnit::Passwords::Writer writer(passwords);
    for (size_t i = 0; i < batchSize; i++) {
        const char *pwData;
        size_t pwLength;
        if (!generator.nextPassword(pwData, pwLength)) {
            pwData = nullptr;
            pwLength = 0;
            passwordsLeft = false;
        }
        this->passwords[i].assign(pwData, pwData + pwLength);
        writer.setPassword(pwData, pwLength);
        writer.moveForward(1);
    }
    return passwordsLeft;
}

void BatchProcessingContext::decryptMasterKey()
{
    auto passwordData = parentContext->getPasswordData();

    const unsigned char *keyMaterial = passwordData->getKeyMaterial();

    size_t keySize = passwordData->getKeySize();
    size_t afStripes = passwordData->getKeyslotStripes();

    size_t keyMaterialSectors = passwordData->getKeyMaterialSectors();
    size_t keyMaterialSize = keyMaterialSectors * PasswordData::SECTOR_SIZE;

    unsigned char *keyMaterialDecrypted = keyMaterialDecryptedBuffers.get();
    unsigned char *masterKey = masterKeyBuffers.get();

    auto decryptor = &parentContext->getSectorDecryptor();

    auto keys = keyslotUnit.openDerivedKeys();
    auto passwords = mkDigestUnit.openPasswords();

    if (threadPool->getSize() > 1) {
        /* run key material decryption in parallel: */
        size_t taskCount = threadPool->getSize();
        std::vector<std::function<void()>> tasks(taskCount);

        size_t div = batchSize / taskCount;
        size_t mod = batchSize % taskCount;
        size_t start = 0;
        for (size_t k = 0; k < taskCount; k++) {
            size_t end = start + div;
            if (k < mod) {
                end += 1;
            }

            tasks[k] = [=, &keys, &passwords] () {

                ProcessingUnit::DerivedKeys::Reader reader(keys, start);
                ProcessingUnit::Passwords::Writer writer(passwords, start);
                for (size_t i = start; i < end; i++) {
                    const void *key = reader.getDerivedKey();

                    decryptor->decryptSectors(
                                key, keyMaterial, keyMaterialDecrypted,
                                0, keyMaterialSectors);

                    crypto::AF_merge((char *)keyMaterialDecrypted,
                                     (char *)masterKey, keySize,
                                     afStripes, passwordData->getHashSpec().c_str());

                    writer.setPassword((char *)masterKey, keySize);

                    reader.moveForward(1);
                    writer.moveForward(1);
                }
            };

            start = end;
            keyMaterialDecrypted += keyMaterialSize;
            masterKey += keySize;
        }

        auto futures = threadPool->enqueueTasks(tasks);
        for (auto &future : futures) {
            future.wait();
            /* if the task raised an exception, throw it here: */
            future.get();
        }
    } else {
        ProcessingUnit::DerivedKeys::Reader reader(keys);
        ProcessingUnit::Passwords::Writer writer(passwords);
        for (size_t i = 0; i < batchSize; i++) {
            const void *key = reader.getDerivedKey();

            decryptor->decryptSectors(
                        key, keyMaterial, keyMaterialDecrypted,
                        0, keyMaterialSectors);

            crypto::AF_merge((char *)keyMaterialDecrypted,
                             (char *)masterKey, keySize,
                             afStripes, passwordData->getHashSpec().c_str());

            writer.setPassword((char *)masterKey, keySize);

            reader.moveForward(1);
            writer.moveForward(1);
        }
    }
}

ssize_t BatchProcessingContext::processResults()
{
    auto passwordData = parentContext->getPasswordData();
    const unsigned char *mkDigest = passwordData->getMasterKeyDigest();

    auto keys = mkDigestUnit.openDerivedKeys();
    ProcessingUnit::DerivedKeys::Reader reader(keys);
    for (size_t i = 0; i < batchSize; i++) {
        auto key = reader.getDerivedKey();
        if (std::memcmp(key, mkDigest, PasswordData::MASTER_KEY_DIGEST_LENGTH) == 0) {
            return i;
        }

        reader.moveForward(1);
    }
    return -1;
}

} // namespace gpu
} // namespace lukscrack

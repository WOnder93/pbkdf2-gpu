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

#include "batchprocessingcontext.h"

#include <memory>
#include <cstring>

namespace lukscrack {
namespace gpu {

BatchProcessingContext::BatchProcessingContext(
        const CrackingContext *parentContext,
        const std::string &name, const Device &deviceIndex,
        ThreadPool *threadPool, std::size_t batchSize,
        Logger *logger)
    : parentContext(parentContext), threadPool(threadPool),
      batchSize(batchSize),
      keyslotUnitLogger(logger, name + "-keyslot:  "),
      mkDigestUnitLogger(logger, name + "-mkdigest: "),
      passwords(batchSize)
{
    const PasswordData *passwordData = parentContext->getPasswordData();

    keyslotContext = DeviceContext(&parentContext->getKeyslotContext(), deviceIndex);
    mkDigestContext = DeviceContext(&parentContext->getMKDigestContext(), deviceIndex);

    keyslotUnit = ProcessingUnit(&keyslotContext, batchSize, &keyslotUnitLogger);
    mkDigestUnit = ProcessingUnit(&mkDigestContext, batchSize, &mkDigestUnitLogger);

    std::size_t taskCount = threadPool->getSize();
    if (taskCount <= 1) {
        taskCount = 1;
    }
    std::size_t keySize = passwordData->getKeySize();
    std::size_t keyMaterialSectors = passwordData->getKeyMaterialSectors();
    std::size_t keyMaterialSize = keyMaterialSectors * PasswordData::SECTOR_SIZE;
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
    for (std::size_t i = 0; i < batchSize; i++) {
        const char *pwData;
        std::size_t pwLength;
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

    std::size_t keySize = passwordData->getKeySize();

    std::size_t keyMaterialSectors = passwordData->getKeyMaterialSectors();
    std::size_t keyMaterialSize = keyMaterialSectors * PasswordData::SECTOR_SIZE;

    unsigned char *keyMaterialDecrypted = keyMaterialDecryptedBuffers.get();
    unsigned char *masterKey = masterKeyBuffers.get();

    auto merger = &parentContext->getAFMerger();
    auto decryptor = &parentContext->getSectorDecryptor();

    auto keys = keyslotUnit.openDerivedKeys();
    auto passwords = mkDigestUnit.openPasswords();

    if (threadPool->getSize() > 1) {
        /* run key material decryption in parallel: */
        std::size_t taskCount = threadPool->getSize();
        std::vector<std::function<void()>> tasks(taskCount);

        std::size_t div = batchSize / taskCount;
        std::size_t mod = batchSize % taskCount;
        std::size_t start = 0;
        for (std::size_t k = 0; k < taskCount; k++) {
            std::size_t end = start + div;
            if (k < mod) {
                end += 1;
            }

            tasks[k] = [=, &keys, &passwords] () {
                ProcessingUnit::DerivedKeys::Reader reader(keys, start);
                ProcessingUnit::Passwords::Writer writer(passwords, start);
                AFMerger::Context mergerCtx(merger);
                for (std::size_t i = start; i < end; i++) {
                    const void *key = reader.getDerivedKey();

                    decryptor->decryptSectors(
                                key, keyMaterial, keyMaterialDecrypted,
                                0, keyMaterialSectors);

                    mergerCtx.merge(keyMaterialDecrypted, masterKey);

                    writer.setPassword(masterKey, keySize);

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
        AFMerger::Context mergerCtx(merger);
        for (std::size_t i = 0; i < batchSize; i++) {
            const void *key = reader.getDerivedKey();

            decryptor->decryptSectors(
                        key, keyMaterial, keyMaterialDecrypted,
                        0, keyMaterialSectors);

            mergerCtx.merge(keyMaterialDecrypted, masterKey);

            writer.setPassword(masterKey, keySize);

            reader.moveForward(1);
            writer.moveForward(1);
        }
    }
}

bool BatchProcessingContext::processResults(std::size_t &matchIndex)
{
    auto passwordData = parentContext->getPasswordData();
    const unsigned char *mkDigest = passwordData->getMasterKeyDigest();

    auto keys = mkDigestUnit.openDerivedKeys();
    ProcessingUnit::DerivedKeys::Reader reader(keys);
    for (std::size_t i = 0; i < batchSize; i++) {
        auto key = reader.getDerivedKey();
        if (std::memcmp(key, mkDigest, PasswordData::MASTER_KEY_DIGEST_LENGTH) == 0) {
            matchIndex = i;
            return true;
        }

        reader.moveForward(1);
    }
    return false;
}

} // namespace gpu
} // namespace lukscrack

#include "sectordecryptor.h"

#define LUKS_SECTOR_SIZE 512

namespace lukscrack {

SectorDecryptor::SectorDecryptor(
        const std::string &cipherName, size_t keySize,
        const std::string &cipherMode,
        const std::string &ivmode)
    : cipherName(cipherName)
{
    cipherAlg = &CipherAlgorithm::getAlgorithm(cipherName, keySize, cipherMode);
    ivgen = IVGenerator::getGenerator(ivmode);
}

void SectorDecryptor::decryptSectors(
        const void *key, const void *data, void *dest,
        size_t startSector, size_t sectorCount) const
{
    size_t ivLength = cipherAlg->getIVLength();
    CipherAlgorithm::DecryptionContex decrCtx(cipherAlg, key);
    auto ivgenCtx = ivgen->createContext(
                cipherAlg->getIVLength(), cipherName,
                key, cipherAlg->getKeySize());

    std::unique_ptr<unsigned char[]> ivBuffer(nullptr);
    if (ivgenCtx.get() != nullptr) {
        ivBuffer = std::unique_ptr<unsigned char[]> {
            new unsigned char[ivLength]
        };
    }
    auto iv = ivBuffer.get();
    auto inputCursor = (const unsigned char *)data;
    auto outputCursor = (unsigned char *)dest;
    for (size_t i = 0; i < sectorCount; i++) {
        if (ivgenCtx.get() != nullptr) {
            ivgenCtx->generateIV(startSector + i, ivBuffer.get());
        }
        decrCtx.decrypt(inputCursor, LUKS_SECTOR_SIZE, iv, outputCursor);

        inputCursor += LUKS_SECTOR_SIZE;
        outputCursor += LUKS_SECTOR_SIZE;
    }
}

} // namespace lukscrack

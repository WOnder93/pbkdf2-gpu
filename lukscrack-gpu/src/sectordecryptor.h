#ifndef LUKSCRACK_SECTORDECRYPTOR_H
#define LUKSCRACK_SECTORDECRYPTOR_H

#include "libivmode/ivgenerator.h"
#include "libcipherspec-cipheralgorithm/cipheralgorithm.h"

namespace lukscrack {

using namespace libivmode;
using namespace libcipherspec::cipheralgorithm;

class SectorDecryptor
{
private:
    const CipherAlgorithm *cipherAlg;
    std::string cipherName;
    std::shared_ptr<const IVGenerator> ivgen;

public:
    inline SectorDecryptor() { }

    SectorDecryptor(
            const std::string &cipherName, size_t keySize,
            const std::string &cipherMode,
            const std::string &ivmode);

    void decryptSectors(
            const void *key, const void *data, void *dest,
            size_t startSector, size_t sectorCount) const;
};

} // namespace lukscrack

#endif // LUKSCRACK_SECTORDECRYPTOR_H

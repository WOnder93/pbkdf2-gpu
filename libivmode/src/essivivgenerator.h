#ifndef LIBIVMODE_ESSIVIVGENERATOR_H
#define LIBIVMODE_ESSIVIVGENERATOR_H

#include "libhashspec-hashalgorithm/hashalgorithm.h"
#include "libcipherspec-cipheralgorithm/cipheralgorithm.h"

#include "ivgenerator.h"

namespace libivmode {

using namespace libhashspec::hashalgorithm;
using namespace libcipherspec::cipheralgorithm;

class EssivIVGenerator : public IVGenerator
{
private:
    class Context : public IVGenerator::Context
    {
    private:
        size_t ivLength;
        std::unique_ptr<unsigned char[]> hashedKey;

        CipherAlgorithm::EncryptionContex ctx;

    public:
        Context(size_t ivLength,
                std::unique_ptr<unsigned char[]> &&hashedKey,
                const CipherAlgorithm &cipherAlg);

        void generateIV(size_t sector, void *dest) const override;
    };

    const HashAlgorithm *hashAlgorithm;

public:
    EssivIVGenerator(const std::string &hashSpec);

    std::shared_ptr<const IVGenerator::Context> createContext(
            size_t ivLength, const std::string &cipherName,
            const void *key, size_t keyLength) const override;
};

} // namespace libivmode

#endif // LIBIVMODE_ESSIVIVGENERATOR_H

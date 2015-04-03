#include "essivivgenerator.h"

#include <cstring>

namespace libivmode {

EssivIVGenerator::EssivIVGenerator(const std::string &hashSpec)
{
    hashAlgorithm = &HashAlgorithm::getAlgorithm(hashSpec);
}

EssivIVGenerator::Context::Context(
        size_t ivLength,
        std::unique_ptr<unsigned char[]> &&hashedKey,
        const CipherAlgorithm &cipherAlg)
    : ivLength(ivLength), hashedKey(std::move(hashedKey))
{
    ctx = CipherAlgorithm::EncryptionContex(&cipherAlg, hashedKey.get());
}

void EssivIVGenerator::Context::generateIV(
        size_t sector, void *dest) const
{
    unsigned char *cursor = (unsigned char *)dest;
    cursor[0] = sector         & 0xff;
    cursor[1] = (sector >>  8) & 0xff;
    cursor[2] = (sector >> 16) & 0xff;
    cursor[3] = (sector >> 24) & 0xff;
    cursor[4] = (sector >> 32) & 0xff;
    cursor[5] = (sector >> 40) & 0xff;
    cursor[6] = (sector >> 48) & 0xff;
    cursor[7] = (sector >> 56) & 0xff;
    std::memset(cursor + 8, 0, ivLength - 8);

    ctx.encrypt(dest, ivLength, nullptr, dest);
}

std::shared_ptr<const IVGenerator::Context> EssivIVGenerator::createContext(
        size_t ivLength, const std::string &cipherName,
        const void *key, size_t keyLength) const
{
    size_t hashLength = hashAlgorithm->getOutputBlockLength();
    auto buffer = std::unique_ptr<unsigned char[]>(new unsigned char[hashLength]);
    hashAlgorithm->computeDigest(key, keyLength, buffer.get());

    auto &cipherAlg = CipherAlgorithm::getAlgorithm(cipherName, hashLength, "ecb");
    if (cipherAlg.getBlockLength() != ivLength) {
        throw std::logic_error(
                    "EssivIVGenerator: The IV length "
                    "must match the cipher block length!");
    }

    return std::make_shared<Context>(ivLength, std::move(buffer), cipherAlg);
}

} // namespace libivmode

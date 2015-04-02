#ifndef LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H
#define LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H

#include <openssl/evp.h>

#include <string>

namespace libhashspec {
namespace hashalgorithm {

class HashAlgorithm
{
private:
    const ::EVP_MD *digest;
    size_t inputBlockLength;
    size_t outputBlockLength;

    inline HashAlgorithm(const ::EVP_MD *digest, size_t inputBlockLength, size_t outputBlockLength)
        : digest(digest), inputBlockLength(inputBlockLength), outputBlockLength(outputBlockLength)
    {
    }

public:
    inline size_t getInputBlockLength() const { return inputBlockLength; }
    inline size_t getOutputBlockLength() const { return outputBlockLength; }

    void computeDigest(const void *data, size_t size, void *dest) const;

    static const HashAlgorithm &getAlgorithm(const std::string &hashSpec);
};


} // namespace hashalgorithm
} // namespace libhashspec
#endif // LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H

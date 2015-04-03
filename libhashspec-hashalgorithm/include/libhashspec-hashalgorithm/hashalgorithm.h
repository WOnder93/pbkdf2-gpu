#ifndef LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H
#define LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H

#include <openssl/evp.h>

#include <string>
#include <stdexcept>

namespace libhashspec {
namespace hashalgorithm {

class HashException : std::runtime_error
{
public:
    HashException(const std::string &message)
        : std::runtime_error(message)
    {
    }
};

class HashAlgorithm
{
private:
    const ::EVP_MD *digest;

    inline HashAlgorithm(const ::EVP_MD *digest)
        : digest(digest)
    {
    }

public:
    class Context
    {
    private:
        ::EVP_MD_CTX ctx;

    public:
        Context(const HashAlgorithm &hashAlg);
        ~Context();

        void update(const void *data, size_t size);
        void digest(void *dest);
    };

    size_t getInputBlockLength() const;
    size_t getOutputBlockLength() const;

    inline void computeDigest(const void *data, size_t size, void *dest) const
    {
        Context ctx(*this);
        ctx.update(data, size);
        ctx.digest(dest);
    }

    static const HashAlgorithm &getAlgorithm(const std::string &hashSpec);
};


} // namespace hashalgorithm
} // namespace libhashspec

#endif // LIBHASHSPEC_HASHALGORITHM_HASHALGORITHM_H

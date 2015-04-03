#include "hashalgorithm.h"

#include "libhashspec-openssl/digestlookup.h"

#include <unordered_map>

namespace libhashspec {
namespace hashalgorithm {

using namespace libhashspec::openssl;

HashAlgorithm::Context::Context(const HashAlgorithm &hashAlg)
{
    EVP_MD_CTX_init(&ctx);
    EVP_DigestInit(&ctx, hashAlg.digest);
}

HashAlgorithm::Context::~Context()
{
    EVP_MD_CTX_cleanup(&ctx);
}

void HashAlgorithm::Context::update(const void *data, size_t size)
{
    EVP_DigestUpdate(&ctx, data, size);
}

void HashAlgorithm::Context::digest(void *dest)
{
    /* the digest size will be guarenteed to match the output block length: */
    EVP_DigestFinal(&ctx, (unsigned char *)dest, NULL);
}

size_t HashAlgorithm::getInputBlockLength() const
{
    return (size_t)EVP_MD_block_size(digest);
}

size_t HashAlgorithm::getOutputBlockLength() const
{
    return (size_t)EVP_MD_size(digest);
}

const HashAlgorithm &HashAlgorithm::getAlgorithm(const std::string &hashSpec)
{
    static const std::unordered_map<std::string, HashAlgorithm> algorithms = {
        { "ripemd160", HashAlgorithm(DigestLookup::getDigest("ripemd160")) },
        { "sha1",      HashAlgorithm(DigestLookup::getDigest("sha1")) },
        { "sha256",    HashAlgorithm(DigestLookup::getDigest("sha256")) },
        { "sha512",    HashAlgorithm(DigestLookup::getDigest("sha512")) },
    };

    return algorithms.at(hashSpec);
}

} // namespace hashalgorithm
} // namespace libhashspec

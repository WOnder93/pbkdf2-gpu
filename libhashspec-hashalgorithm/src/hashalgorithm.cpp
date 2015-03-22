#include "hashalgorithm.h"

#include "libhashspec-openssl/digestlookup.h"

#include <unordered_map>

namespace libhashspec {
namespace hashalgorithm {

using namespace libhashspec::openssl;

void HashAlgorithm::computeDigest(const void *data, size_t size, void *dest) const
{
    EVP_MD_CTX ctx;
    EVP_DigestInit(&ctx, digest);
    EVP_DigestUpdate(&ctx, data, size);
    /* the digest size will be guarenteed to match the output bock length: */
    EVP_DigestFinal(&ctx, (unsigned char *)dest, NULL);
}

const HashAlgorithm &HashAlgorithm::getAlgorithm(std::string hashSpec)
{
    static const std::unordered_map<std::string, HashAlgorithm> algorithms = {
        { "ripemd160",  HashAlgorithm(DigestLookup::getDigest("ripemd160"),  64, 20) },
        { "sha1",       HashAlgorithm(DigestLookup::getDigest("sha1"),       64, 20) },
        { "sha256",     HashAlgorithm(DigestLookup::getDigest("sha256"),     64, 32) },
        { "sha512",     HashAlgorithm(DigestLookup::getDigest("sha512"),    128, 64) },
    };

    return algorithms.at(hashSpec);
}

} // namespace hashalgorithm
} // namespace libhashspec

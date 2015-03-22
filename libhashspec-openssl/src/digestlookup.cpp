#include "digestlookup.h"

#include <unordered_map>

namespace libhashspec {
namespace openssl {

static const std::unordered_map<std::string, const EVP_MD *> digests = {
    { "ripemd160", EVP_ripemd160() },
    { "sha1",      EVP_sha1() },
    { "sha256",    EVP_sha256() },
    { "sha512",    EVP_sha512() },
};

const EVP_MD *DigestLookup::getDigest(std::string hashSpec)
{
    return digests.at(hashSpec);
}

} // namespace openssl
} // namespace libhashspec

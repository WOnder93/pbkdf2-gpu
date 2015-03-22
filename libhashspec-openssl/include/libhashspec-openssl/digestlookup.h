#ifndef LIBHASHSPEC_OPENSSL_DIGESTLOOKUP_H
#define LIBHASHSPEC_OPENSSL_DIGESTLOOKUP_H

#include <openssl/evp.h>

#include <string>

namespace libhashspec {
namespace openssl {

namespace DigestLookup
{
    const ::EVP_MD *getDigest(std::string hashSpec);
}

} // namespace openssl
} // namespace libhashspec

#endif // LIBHASHSPEC_OPENSSL_DIGESTLOOKUP_H

#include "hashfunctioncontext.h"

#include "libhashspec-openssl/digestlookup.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

using namespace libhashspec::openssl;

HashFunctionContext::HashFunctionContext(
        const void *,
        const std::vector<std::nullptr_t> &,
        const std::string &hashSpec)
    : digest(DigestLookup::getDigest(hashSpec))
{
}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

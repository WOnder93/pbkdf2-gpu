#include "hashfunctioncontext.h"

#include "libhashspec-openssl/digestlookup.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

using namespace libhashspec::openssl;

HashFunctionContext::HashFunctionContext(const GlobalContext *globalContext,
        const std::vector<Device> &,
        const std::string &hashSpec)
    : globalContext(globalContext), digest(DigestLookup::getDigest(hashSpec))
{
}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

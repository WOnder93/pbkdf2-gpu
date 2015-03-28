#ifndef LIBPBKDF2_COMPUTE_CPU_HASHFUNCTIONCONTEXT_H
#define LIBPBKDF2_COMPUTE_CPU_HASHFUNCTIONCONTEXT_H

#include "globalcontext.h"

#include <openssl/evp.h>

#include <vector>
#include <string>

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class HashFunctionContext
{
private:
    const GlobalContext *globalContext;
    const ::EVP_MD *digest;

public:
    inline const GlobalContext *getGlobalContext() const { return globalContext; }
    inline const ::EVP_MD *getDigest() const { return digest; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline HashFunctionContext() { }

    HashFunctionContext(const HashFunctionContext &) = delete;
    HashFunctionContext &operator=(const HashFunctionContext &) = delete;

    HashFunctionContext(HashFunctionContext &&) = default;
    HashFunctionContext &operator=(HashFunctionContext &&) = default;

    HashFunctionContext(
            const GlobalContext *globalContext,
            const std::vector<Device> &devices,
            const std::string &hashSpec);
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_HASHFUNCTIONCONTEXT_H

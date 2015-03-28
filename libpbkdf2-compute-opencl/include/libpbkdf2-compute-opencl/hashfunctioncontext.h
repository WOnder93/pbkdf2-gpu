#ifndef LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONCONTEXT_H
#define LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONCONTEXT_H

#include <string>

#include "libhashspec-hashalgorithm/hashalgorithm.h"

#include "opencl.h"
#include "globalcontext.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

using namespace libhashspec::hashalgorithm;

class HashFunctionContext
{
private:
    const GlobalContext *globalContext;

    std::vector<cl::Device> devices;
    cl::Context context;

    std::string hashSpec;
    const HashAlgorithm *hashAlg;

public:
    inline const GlobalContext *getGlobalContext() const { return globalContext; }

    inline const std::vector<cl::Device> &getDevices() const { return devices; }
    inline const cl::Context &getContext() const { return context; }

    inline const std::string &getHashSpec() const { return hashSpec; }
    inline const HashAlgorithm *getHashAlgorithm() const { return hashAlg; }

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

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONCONTEXT_H

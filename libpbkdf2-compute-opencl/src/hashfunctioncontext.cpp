#include "hashfunctioncontext.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

HashFunctionContext::HashFunctionContext(
        const GlobalContext *globalContext,
        const std::vector<cl::Device> &devices,
        const std::string &hashSpec)
    : globalContext(globalContext),
      devices(devices), context(devices),
      hashSpec(hashSpec), hashAlg(&HashAlgorithm::getAlgorithm(hashSpec))
{
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

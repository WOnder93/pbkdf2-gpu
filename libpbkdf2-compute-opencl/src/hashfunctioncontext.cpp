#include "hashfunctioncontext.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

HashFunctionContext::HashFunctionContext(
        const GlobalContext *globalContext,
        const std::vector<Device> &devices,
        const std::string &hashSpec)
    : globalContext(globalContext), devices(),
      hashSpec(hashSpec), hashAlg(&HashAlgorithm::getAlgorithm(hashSpec))
{
    for (const Device &device : devices) {
        this->devices.push_back(device.getCLDevice());
    }
    context = cl::Context(this->devices);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

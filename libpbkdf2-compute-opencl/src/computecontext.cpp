#include "computecontext.h"

#include "kernelloader.h"

#include "alignment.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

ComputeContext::ComputeContext(const HashFunctionContext *parentContext,
                               const void *salt, size_t saltLength,
                               size_t dkLength, size_t iterationCount)
    : parentContext(parentContext), saltData(salt), saltLength(saltLength),
      dkLength(dkLength), iterationCount(iterationCount)
{
    // load and build the kernel program:
    // TODO: check error:
    std::string path = KernelLoader::findPBKDF2Program(parentContext->getGlobalContext()->getDataDirectory(), parentContext->getHashSpec());
    prog = KernelLoader::loadPBKDF2Program(parentContext->getContext(), path, saltLength);

    saltBufferSize = ALIGN(4, saltLength);
    saltBuffer = cl::Buffer(parentContext->getContext(), CL_MEM_READ_ONLY, saltBufferSize);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#include "devicecontext.h"

#include <cstring>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

DeviceContext::DeviceContext(const ComputeContext *parentContext, size_t deviceIndex)
    : parentContext(parentContext)
{
    auto hfContext = parentContext->getParentContext();
    device = hfContext->getDevices().at(deviceIndex);
    queue = cl::CommandQueue(hfContext->getContext(), device);

    void *hostSaltBuffer = queue.enqueueMapBuffer(
                parentContext->getSaltBuffer(), true, CL_MAP_WRITE,
                0, parentContext->getSaltBufferSize());

    std::memcpy(hostSaltBuffer, parentContext->getSaltData(), parentContext->getSaltLength());
    std::memset((char *)hostSaltBuffer + parentContext->getSaltLength(), 0, parentContext->getSaltBufferSize() - parentContext->getSaltLength());

    queue.enqueueUnmapMemObject(parentContext->getSaltBuffer(), hostSaltBuffer);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

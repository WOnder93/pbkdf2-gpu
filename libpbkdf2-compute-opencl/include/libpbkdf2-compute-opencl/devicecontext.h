#ifndef LIBPBKDF2_COMPUTE_OPENCL_DEVICECONTEXT_H
#define LIBPBKDF2_COMPUTE_OPENCL_DEVICECONTEXT_H

#include "computecontext.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class DeviceContext
{
private:
    const ComputeContext *parentContext;

    cl::Device device;
    cl::CommandQueue queue;

public:
    inline const ComputeContext *getParentContext() const { return parentContext; }
    inline const cl::Device &getDevice() const { return device; }
    inline const cl::CommandQueue &getCommandQueue() const { return queue; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline DeviceContext() { }

    DeviceContext(const DeviceContext &) = delete;
    DeviceContext &operator=(const DeviceContext &) = delete;

    DeviceContext(DeviceContext &&) = default;
    DeviceContext &operator=(DeviceContext &&) = default;

    DeviceContext(const ComputeContext *parentContext, size_t deviceIndex);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_DEVICECONTEXT_H

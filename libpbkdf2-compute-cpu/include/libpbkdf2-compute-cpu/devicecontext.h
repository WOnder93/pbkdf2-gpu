#ifndef LIBPBKDF2_COMPUTE_CPU_DEVICECONTEXT_H
#define LIBPBKDF2_COMPUTE_CPU_DEVICECONTEXT_H

#include "computecontext.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class DeviceContext
{
private:
    const ComputeContext *parentContext;
    Device device;

public:
    inline const ComputeContext *getParentContext() const { return parentContext; }
    inline GlobalContext::CoreContext &getCoreContext() const
    {
        return parentContext->getParentContext()->
                getGlobalContext()->getCoreContext(device);
    }

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

    inline DeviceContext(const ComputeContext *parentContext, const Device &device)
        : parentContext(parentContext), device(device) { }
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_DEVICECONTEXT_H

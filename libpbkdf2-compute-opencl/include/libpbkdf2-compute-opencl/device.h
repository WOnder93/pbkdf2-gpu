#ifndef LIBPBKDF2_COMPUTE_OPENCL_DEVICE_H
#define LIBPBKDF2_COMPUTE_OPENCL_DEVICE_H

#include "opencl.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class Device
{
private:
    cl::Device device;

public:
    std::string getInfo() const;

    const cl::Device &getCLDevice() const { return device; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline Device() { }

    inline Device(const cl::Device &device)
        : device(device)
    {
    }
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_DEVICE_H

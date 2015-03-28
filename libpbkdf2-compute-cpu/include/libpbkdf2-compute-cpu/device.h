#ifndef LIBPBKDF2_COMPUTE_CPU_DEVICE_H
#define LIBPBKDF2_COMPUTE_CPU_DEVICE_H

#include <string>

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class Device
{
private:
    unsigned coreIndex;

public:
    inline std::string getInfo() const
    {
        return "Virtual CPU core #" + std::to_string(coreIndex);
    }

    inline unsigned getCoreIndex() const { return coreIndex; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline Device() { }

    Device(unsigned coreIndex) : coreIndex(coreIndex) { }
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_DEVICE_H

#ifndef LIBPBKDF2_COMPUTE_OPENCL_GLOBALCONTEXT_H
#define LIBPBKDF2_COMPUTE_OPENCL_GLOBALCONTEXT_H

#include "device.h"

#include <string>
#include <vector>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class GlobalContext
{
private:
    std::string dataDirectory;
    std::vector<Device> devices;

public:
    inline const std::vector<Device> &getAllDevices() const { return devices; }

    inline const std::string &getDataDirectory() const { return dataDirectory; }

    GlobalContext(const std::string &dataDirectory);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_GLOBALCONTEXT_H

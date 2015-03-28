#include "globalcontext.h"

#include "opencl.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

GlobalContext::GlobalContext(const std::string &dataDirectory)
    : dataDirectory(dataDirectory)
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::vector<cl::Device> clDevices;
    for (cl::Platform platform : platforms) {
        platform.getDevices(CL_DEVICE_TYPE_ALL, &clDevices);
        devices.insert(devices.end(), clDevices.begin(), clDevices.end());
    }
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

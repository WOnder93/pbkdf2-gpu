#ifndef LIBPBKDF2_COMPUTE_OPENCL_KERNELLOADER_H
#define LIBPBKDF2_COMPUTE_OPENCL_KERNELLOADER_H

#include "cl.h"

#include <string>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

namespace KernelLoader
{
    inline static std::string findProgram(
            const std::string &dataFolder, const std::string &kernelName)
    {
        return dataFolder + "/opencl/kernels/" + kernelName + ".cl";
    }

    inline static std::string findPBKDF2Program(
            const std::string &dataFolder, const std::string &hashSpec)
    {
        return findProgram(dataFolder, "pbkdf2_" + hashSpec);
    }

    cl::Program loadProgram(const cl::Context &context,
                            const std::string &sourcePath,
                            const std::string &buildOptions = std::string());

    cl::Program loadPBKDF2Program(const cl::Context &context,
                                  const std::string &sourcePath,
                                  size_t saltLength,
                                  bool log = false, bool debug = false, bool tests = false);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_KERNELLOADER_H

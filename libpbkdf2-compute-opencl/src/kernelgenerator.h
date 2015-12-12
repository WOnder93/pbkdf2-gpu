#ifndef LIBPBKDF2_COMPUTE_OPENCL_KERNELGENERATOR_H
#define LIBPBKDF2_COMPUTE_OPENCL_KERNELGENERATOR_H

#include "hashfunctionhelper.h"

#include <ostream>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class KernelGenerator
{
public:
    static void generateKernel(std::ostream &out, std::size_t saltLength,
                               const HashFunctionHelper &hfHelper);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_KERNELGENERATOR_H

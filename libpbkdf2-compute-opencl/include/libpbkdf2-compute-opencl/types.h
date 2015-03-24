#ifndef LIBPBKDF2_COMPUTE_OPENCL_TYPES_H
#define LIBPBKDF2_COMPUTE_OPENCL_TYPES_H

#include "opencl.h"

#include "processingunit.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class Types
{
public:
    typedef cl::Device          TDevice;
    typedef GlobalContext       TGlobalContext;
    typedef HashFunctionContext THashFunctionContext;
    typedef ComputeContext      TComputeContext;
    typedef DeviceContext       TDeviceContext;
    typedef ProcessingUnit      TProcessingUnit;
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_TYPES_H

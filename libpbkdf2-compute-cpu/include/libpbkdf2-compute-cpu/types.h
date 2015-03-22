#ifndef LIBPBKDF2_COMPUTE_CPU_TYPES_H
#define LIBPBKDF2_COMPUTE_CPU_TYPES_H

#include "processingunit.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class Types
{
public:
    typedef std::nullptr_t      TDevice;
    typedef void                TGlobalContext;
    typedef HashFunctionContext THashFunctionContext;
    typedef ComputeContext      TComputeContext;
    typedef DeviceContext       TDeviceContext;
    typedef ProcessingUnit      TProcessingUnit;
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_TYPES_H

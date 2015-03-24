#ifndef OPENCL_H
#define OPENCL_H

/* Use only OpenCL 1.1 APIs (because of NVIDIA...): */
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
#undef CL_VERSION_1_2

/* Throw exceptions on errors: */
#define __CL_ENABLE_EXCEPTIONS
/* Include local version of <CL/cl.hpp>
 * because not all platforms ship with it: */
#include "cl.hpp"

#endif // OPENCL_H

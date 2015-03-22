#ifndef CL_H
#define CL_H

/* Use only OpenCL 1.1 APIs (because of NVIDIA...): */
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
#undef CL_VERSION_1_2

/* Throw exceptions on errors: */
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#endif // CL_H

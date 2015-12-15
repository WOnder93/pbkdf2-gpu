#ifndef LIBPBKDF2_COMPUTE_OPENCL_SHA512HASHFUNCTIONHELPER_H
#define LIBPBKDF2_COMPUTE_OPENCL_SHA512HASHFUNCTIONHELPER_H

#include "sha2hashfunctionhelper.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class Sha512HashFunctionHelper : public Sha2HashFunctionHelper
{
private:
    Sha512HashFunctionHelper();

public:
    static const Sha512HashFunctionHelper INSTANCE;
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_SHA512HASHFUNCTIONHELPER_H

#ifndef LIBPBKDF2_COMPUTE_OPENCL_UINTHASHFUNCTIONHELPER_H
#define LIBPBKDF2_COMPUTE_OPENCL_UINTHASHFUNCTIONHELPER_H

#include "hashfunctionhelper.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class UIntHashFunctionHelper : public HashFunctionHelper
{
public:
    UIntHashFunctionHelper(
            bool isLittleEndian,
            std::size_t iblockWords, std::size_t oblockWords,
            std::size_t mlWords, const char * const *initState);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_UINTHASHFUNCTIONHELPER_H

#include "uinthashfunctionhelper.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

UIntHashFunctionHelper::UIntHashFunctionHelper(
        bool isLittleEndian,
        std::size_t iblockWords, std::size_t oblockWords,
        std::size_t mlWords, const char * const *initState)
    : HashFunctionHelper(
          isLittleEndian,
          "uint", 4, iblockWords, oblockWords, mlWords, initState)
{
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2


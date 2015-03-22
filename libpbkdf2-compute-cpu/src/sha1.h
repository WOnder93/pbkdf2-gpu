#ifndef LIBPBKDF2_COMPUTE_CPU_SHA1_H
#define LIBPBKDF2_COMPUTE_CPU_SHA1_H

#include <cstdint>

#define SHA1_BLOCK_LENGTH       64
#define SHA1_DIGEST_LENGTH      20

namespace libpbkdf2 {
namespace compute {
namespace cpu {

namespace impl
{
    typedef struct {
        uint32_t   state[5];
        uint64_t   count;
        unsigned char   buffer[SHA1_BLOCK_LENGTH];
    } SHA1_CTX;

    void SHA1Init(SHA1_CTX * context);
    void SHA1Transform(uint32_t state[5], const unsigned char buffer[SHA1_BLOCK_LENGTH]);
    void SHA1Update(SHA1_CTX *context, const unsigned char *data, unsigned int len);
    void SHA1Final(unsigned char digest[SHA1_DIGEST_LENGTH], SHA1_CTX *context);
}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_SHA1_H

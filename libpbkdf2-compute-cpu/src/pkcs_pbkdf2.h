#ifndef LIBPBKDF2_COMPUTE_CPU_PKCS_PBKDF2_H
#define LIBPBKDF2_COMPUTE_CPU_PKCS_PBKDF2_H

#include <cstdlib>
#include <cstdint>

namespace libpbkdf2 {
namespace compute {
namespace cpu {

namespace impl
{
    int pkcs5_pbkdf2(const char *pass, size_t pass_len, const uint8_t *salt,
        size_t salt_len, uint8_t *key, size_t key_len, unsigned int rounds);
}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_PKCS_PBKDF2_H

#include <cstring>
#include <cstdint>
#include <cstdlib>

#include "pkcs_pbkdf2.h"

#include "sha1.h"

#define bzero(s, n)	std::memset((s), 0, (n))
#define bcopy(a, b, c)  std::memcpy(b, a, c)

#define MINIMUM(a,b) (((a) < (b)) ? (a) : (b))

namespace libpbkdf2 {
namespace compute {
namespace cpu {

namespace impl {
/*
 * HMAC-SHA-1 (from RFC 2202).
 */
static inline void
hmac_sha1(const uint8_t *text, size_t text_len, const uint8_t *key,
    size_t key_len, uint8_t digest[SHA1_DIGEST_LENGTH])
{
    SHA1_CTX ctx;
    uint8_t k_pad[SHA1_BLOCK_LENGTH];
    uint8_t tk[SHA1_DIGEST_LENGTH];
    int i;

    if (key_len > SHA1_BLOCK_LENGTH) {
        SHA1Init(&ctx);
        SHA1Update(&ctx, key, key_len);
        SHA1Final(tk, &ctx);

        key = tk;
        key_len = SHA1_DIGEST_LENGTH;
    }

    bzero(k_pad, sizeof k_pad);
    bcopy(key, k_pad, key_len);
    for (i = 0; i < SHA1_BLOCK_LENGTH; i++)
        k_pad[i] ^= 0x36;

    SHA1Init(&ctx);
    SHA1Update(&ctx, k_pad, SHA1_BLOCK_LENGTH);
    SHA1Update(&ctx, text, text_len);
    SHA1Final(digest, &ctx);

    bzero(k_pad, sizeof k_pad);
    bcopy(key, k_pad, key_len);
    for (i = 0; i < SHA1_BLOCK_LENGTH; i++)
        k_pad[i] ^= 0x5c;

    SHA1Init(&ctx);
    SHA1Update(&ctx, k_pad, SHA1_BLOCK_LENGTH);
    SHA1Update(&ctx, digest, SHA1_DIGEST_LENGTH);
    SHA1Final(digest, &ctx);
}

/*
 * Password-Based Key Derivation Function 2 (PKCS #5 v2.0).
 * Code based on IEEE Std 802.11-2007, Annex H.4.2.
 */
int
pkcs5_pbkdf2(const char *pass, size_t pass_len, const uint8_t *salt,
    size_t salt_len, uint8_t *key, size_t key_len, unsigned int rounds)
{
    uint8_t *asalt, obuf[SHA1_DIGEST_LENGTH];
    uint8_t d1[SHA1_DIGEST_LENGTH], d2[SHA1_DIGEST_LENGTH];
    unsigned int i, j;
    unsigned int count;
    size_t r;

    if (rounds < 1 || key_len == 0)
        return -1;
    if (salt_len == 0 || salt_len > SIZE_MAX - 4)
        return -1;
    if ((asalt = (uint8_t *)malloc(salt_len + 4)) == NULL)
        return -1;

    std::memcpy(asalt, salt, salt_len);

    for (count = 1; key_len > 0; count++) {
        asalt[salt_len + 0] = (count >> 24) & 0xff;
        asalt[salt_len + 1] = (count >> 16) & 0xff;
        asalt[salt_len + 2] = (count >> 8) & 0xff;
        asalt[salt_len + 3] = count & 0xff;

        hmac_sha1(asalt, salt_len + 4, (uint8_t *)pass, pass_len, d1);
        std::memcpy(obuf, d1, sizeof(obuf));

        for (i = 1; i < rounds; i++) {
            hmac_sha1(d1, sizeof(d1), (uint8_t *)pass, pass_len, d2);
            std::memcpy(d1, d2, sizeof(d1));
            for (j = 0; j < sizeof(obuf); j++)
                obuf[j] ^= d1[j];
        }

        r = MINIMUM(key_len, SHA1_DIGEST_LENGTH);
        std::memcpy(key, obuf, r);
        key += r;
        key_len -= r;
    };
    free(asalt);

    return 0;
}

}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

/*
 * (Modified from) crypto backend implementation
 *
 * Copyright (C) 2010-2012, Red Hat, Inc. All rights reserved.
 * Copyright (C) 2010-2014, Milan Broz
 * Copyright (C) 2015, Ondrej Mosnacek
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef LUKSCRACK_CRYPTO_CRYPTO_BACKEND_H
#define LUKSCRACK_CRYPTO_CRYPTO_BACKEND_H

#include <cstdlib>

namespace lukscrack {
namespace crypto {

struct crypt_hash;
struct crypt_cipher;

int crypt_backend_init();
void crypt_backend_cleanup();

/* HASH */
int crypt_hash_size(const char *name);
int crypt_hash_init(struct crypt_hash **ctx, const char *name);
int crypt_hash_write(struct crypt_hash *ctx, const char *buffer, size_t length);
int crypt_hash_final(struct crypt_hash *ctx, char *buffer, size_t length);
int crypt_hash_destroy(struct crypt_hash *ctx);

/* ciphers */
int crypt_cipher_blocksize(const char *name);
int crypt_cipher_init(struct crypt_cipher **ctx, const char *name,
                    const char *mode, const void *buffer, size_t length);
int crypt_cipher_destroy(struct crypt_cipher *ctx);
int crypt_cipher_encrypt(struct crypt_cipher *ctx,
                         const char *in, char *out, size_t length,
                         const char *iv, size_t iv_length);
int crypt_cipher_decrypt(struct crypt_cipher *ctx,
                         const char *in, char *out, size_t length,
                         const char *iv, size_t iv_length);

/* Memzero helper (memset on stack can be optimized out) */
static inline void crypt_backend_memzero(void *s, size_t n)
{
        volatile char *p = (volatile char *)s;
        while(n--) *p++ = 0;
}

} // namespace crypto
} // namespace lukscrack

#endif // LUKSCRACK_CRYPTO_CRYPTO_BACKEND_H

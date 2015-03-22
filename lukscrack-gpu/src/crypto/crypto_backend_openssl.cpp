/*
 * (Modified from) OPENSSL crypto backend implementation
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
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 *
 * You must obey the GNU Lesser General Public License in all respects
 * for all of the code used other than OpenSSL.
 */

#include "crypto_backend.h"

#include "errno.h"

#include <cstring>
#include <openssl/evp.h>

namespace lukscrack {
namespace crypto {

static int crypto_backend_initialised = 0;

struct crypt_hash {
    EVP_MD_CTX md;
    const EVP_MD *hash_id;
    int hash_len;
};

int crypt_backend_init()
{
    if (crypto_backend_initialised)
        return 0;

    OpenSSL_add_all_algorithms();

    crypto_backend_initialised = 1;
    return 0;
}

void crypt_backend_cleanup()
{
    if (!crypto_backend_initialised)
        return;

    EVP_cleanup();
}

/* HASH */
int crypt_hash_size(const char *name)
{
    const EVP_MD *hash_id = EVP_get_digestbyname(name);

    if (!hash_id)
        return -EINVAL;

    return EVP_MD_size(hash_id);
}

int crypt_hash_init(struct crypt_hash **ctx, const char *name)
{
    struct crypt_hash *h;

    h = (struct crypt_hash *)malloc(sizeof(*h));
    if (!h)
        return -ENOMEM;

    h->hash_id = EVP_get_digestbyname(name);
    if (!h->hash_id) {
        free(h);
        return -EINVAL;
    }

    if (EVP_DigestInit(&h->md, h->hash_id) != 1) {
        free(h);
        return -EINVAL;
    }

    h->hash_len = EVP_MD_size(h->hash_id);
    *ctx = h;
    return 0;
}

static int crypt_hash_restart(struct crypt_hash *ctx)
{
    if (EVP_DigestInit(&ctx->md, ctx->hash_id) != 1)
        return -EINVAL;

    return 0;
}

int crypt_hash_write(struct crypt_hash *ctx, const char *buffer, size_t length)
{
    if (EVP_DigestUpdate(&ctx->md, buffer, length) != 1)
        return -EINVAL;

    return 0;
}

int crypt_hash_final(struct crypt_hash *ctx, char *buffer, size_t length)
{
    unsigned char tmp[EVP_MAX_MD_SIZE];
    unsigned int tmp_len = 0;

    if (length > (size_t)ctx->hash_len)
        return -EINVAL;

    if (EVP_DigestFinal_ex(&ctx->md, tmp, &tmp_len) != 1)
        return -EINVAL;

    memcpy(buffer, tmp, length);
    crypt_backend_memzero(tmp, sizeof(tmp));

    if (tmp_len < length)
        return -EINVAL;

    if (crypt_hash_restart(ctx))
        return -EINVAL;

    return 0;
}

int crypt_hash_destroy(struct crypt_hash *ctx)
{
    EVP_MD_CTX_cleanup(&ctx->md);
    memset(ctx, 0, sizeof(*ctx));
    free(ctx);
    return 0;
}

} // namespace crypto
} // namespace lukscrack

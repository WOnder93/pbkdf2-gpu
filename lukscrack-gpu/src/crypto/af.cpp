/*
 * (Modified from) AFsplitter - Anti forensic information splitter
 *
 * Copyright (C) 2004, Clemens Fruhwirth <clemens@endorphin.org>
 * Copyright (C) 2009-2012, Red Hat, Inc. All rights reserved.
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * AFsplitter diffuses information over a large stripe of data,
 * therefor supporting secure data destruction.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "af.h"

#include "errno.h"
#include "crypto_backend.h"

#include <cstdlib>
#include <cstring>
#include <cstdint>

using namespace std;

namespace lukscrack {
namespace crypto {

static void XORblock(const char *src1, const char *src2, char *dst, size_t n)
{
        size_t j;

        for(j = 0; j < n; ++j)
                dst[j] = src1[j] ^ src2[j];
}

static int hash_buf(const char *src, char *dst, uint32_t iv,
                    size_t len, const char *hash_name)
{
        struct crypt_hash *hd = NULL;
        char iv_char[4];
        int r;

        iv_char[3] = iv & 0xFF; iv >>= 8;
        iv_char[2] = iv & 0xFF; iv >>= 8;
        iv_char[1] = iv & 0xFF; iv >>= 8;
        iv_char[0] = iv & 0xFF;

        if (crypt_hash_init(&hd, hash_name))
                return -EINVAL;

        if ((r = crypt_hash_write(hd, iv_char, sizeof(uint32_t))))
                goto out;

        if ((r = crypt_hash_write(hd, src, len)))
                goto out;

        r = crypt_hash_final(hd, dst, len);
out:
        crypt_hash_destroy(hd);
        return r;
}

/* diffuse: Information spreading over the whole dataset with
 * the help of hash function.
 */

static int diffuse(char *src, char *dst, size_t size, const char *hash_name)
{
        int hash_size = crypt_hash_size(hash_name);
        unsigned int digest_size;
        unsigned int i, blocks, padding;

        if (hash_size <= 0)
                return 1;
        digest_size = hash_size;

        blocks = size / digest_size;
        padding = size % digest_size;

        for (i = 0; i < blocks; i++)
                if(hash_buf(src + digest_size * i,
                            dst + digest_size * i,
                            i, (size_t)digest_size, hash_name))
                        return 1;

        if(padding)
                if(hash_buf(src + digest_size * i,
                            dst + digest_size * i,
                            i, (size_t)padding, hash_name))
                        return 1;

        return 0;
}

int AF_merge(char *src, char *dst, size_t blocksize,
                 unsigned int blocknumbers, const char *hash)
{
        unsigned int i;
        char *bufblock;
        int r = -EINVAL;

        if((bufblock = (char *)calloc(blocksize, 1)) == NULL)
                return -ENOMEM;

        memset(bufblock,0,blocksize);
        for(i=0; i<blocknumbers-1; i++) {
                XORblock(src+(blocksize*i),bufblock,bufblock,blocksize);
                if(diffuse(bufblock, bufblock, blocksize, hash))
                        goto out;
        }
        XORblock(src + blocksize * i, bufblock, dst, blocksize);
        r = 0;
out:
        free(bufblock);
        return r;
}

} // namespace crypto
} // namespace lukscrack

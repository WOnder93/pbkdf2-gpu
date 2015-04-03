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

#include "libhashspec-hashalgorithm/hashalgorithm.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cstdint>

namespace lukscrack {
namespace crypto {

using namespace libhashspec::hashalgorithm;

// TODO: refactor this Frankenstein...

static inline void XORblock(const char *src1, const char *src2, char *dst, size_t n)
{
    for(size_t j = 0; j < n; ++j)
        dst[j] = src1[j] ^ src2[j];
}

static int hash_buf(const char *src, char *dst, uint32_t iv,
                    size_t len, const HashAlgorithm &hashAlg)
{
    char iv_char[4];
    iv_char[3] = iv & 0xFF; iv >>= 8;
    iv_char[2] = iv & 0xFF; iv >>= 8;
    iv_char[1] = iv & 0xFF; iv >>= 8;
    iv_char[0] = iv & 0xFF;

    try {
        HashAlgorithm::Context ctx(hashAlg);
        ctx.update(iv_char, 4);
        ctx.update(src, len);
        ctx.digest(dst);
    } catch(const HashException &) {
        return -EINVAL;
    }
    return 0;
}

/* diffuse: Information spreading over the whole dataset with
 * the help of hash function.
 */

static int diffuse(char *src, char *dst, size_t size, const HashAlgorithm &hashAlg)
{
    size_t digest_size;
    size_t i, blocks, padding;

    digest_size = hashAlg.getOutputBlockLength();

    blocks = size / digest_size;
    padding = size % digest_size;

    for (i = 0; i < blocks; i++)
        if (hash_buf(src + digest_size * i,
                     dst + digest_size * i,
                     i, digest_size, hashAlg))
            return 1;

    if (padding)
        if (hash_buf(src + digest_size * i,
                     dst + digest_size * i,
                     i, padding, hashAlg))
            return 1;

    return 0;
}

int AF_merge(char *src, char *dst, size_t blocksize,
             size_t blocknumbers, const std::string &hashSpec)
{
    size_t i;
    char *bufblock;
    int r = -EINVAL;

    auto &hashAlg = HashAlgorithm::getAlgorithm(hashSpec);

    if ((bufblock = (char *)std::calloc(blocksize, 1)) == NULL)
        return -ENOMEM;

    std::memset(bufblock, 0, blocksize);
    for(i = 0; i < blocknumbers - 1; i++) {
        XORblock(src + blocksize * i, bufblock, bufblock, blocksize);
        if (diffuse(bufblock, bufblock, blocksize, hashAlg))
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

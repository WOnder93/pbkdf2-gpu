/*
 * Based on AFsplitter (lib/luks1/af.c) from Cryptsetup
 * <https://gitlab.com/cryptsetup/cryptsetup>
 *
 * Copyright (C) 2004, Clemens Fruhwirth <clemens@endorphin.org>
 * Copyright (C) 2009-2012, Red Hat, Inc. All rights reserved.
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "afmerger.h"

#include "libpbkdf2-gpu-common/alignment.h"
#include "libpbkdf2-gpu-common/endianness.h"

#include <cstring>
#include <cstdint>

namespace lukscrack {

using namespace pbkdf2_gpu::common;

AFMerger::Context::Context(const AFMerger *parent)
    : parent(parent),
      buffer(new unsigned char[ALIGN(parent->digestSize, parent->blockSize)])
{
}

AFMerger::AFMerger(std::size_t blockSize, std::size_t blockCount,
                   const std::string &hashSpec)
    : blockSize(blockSize), blockCount(blockCount)
{
    hashAlg = &HashAlgorithm::getAlgorithm(hashSpec);

    digestSize = hashAlg->getOutputBlockLength();
    subblockCount = blockSize / digestSize;
    subblockPadding = blockSize % digestSize;
}

static void hash_buf(const void *src, void *dst, std::size_t iv,
                     std::size_t len, const HashAlgorithm *hashAlg)
{
    unsigned char iv_char[4];
    Endianness::write32BE(iv_char, (std::uint_fast32_t)iv);

    HashAlgorithm::Context ctx(*hashAlg);
    ctx.update(iv_char, 4);
    ctx.update(src, len);
    ctx.digest(dst);
}

static const unsigned char *diffuse(
        const unsigned char *src, unsigned char *dst,
        std::size_t subblockCount, std::size_t subblockPadding,
        std::size_t digestSize, const HashAlgorithm *hashAlg)
{
    std::size_t k;
    for (k = 0; k < subblockCount; k++) {
        hash_buf(src, dst, k, digestSize, hashAlg);
        src += digestSize;
        dst += digestSize;
    }
    if (subblockPadding != 0) {
        hash_buf(src, dst, k, subblockPadding, hashAlg);
        src += subblockPadding;
    }
    return src;
}

static void xor_buf(const unsigned char *src1,
                    const unsigned char *src2,
                    unsigned char *dst, std::size_t bytes)
{
    for (std::size_t i = 0; i < bytes; i++) {
        dst[i] = src1[i] ^ src2[i];
    }
}

void AFMerger::merge(const void *src, void *dst, unsigned char *buffer) const
{
    if (blockCount == 1) {
        std::memcpy(dst, src, blockSize);
        return;
    }

    std::size_t digestSize = this->digestSize;
    std::size_t subblockCount = this->subblockCount;
    std::size_t subblockPadding = this->subblockPadding;
    auto hashAlg = this->hashAlg;
    auto srcCursor = (const unsigned char *)src;

    /* directly hash first block: */
    srcCursor = diffuse(srcCursor, buffer, subblockCount, subblockPadding,
                        digestSize, hashAlg);

    for (std::size_t i = 0; i < blockCount - 2; i++) {
        /* XOR: */
        xor_buf(srcCursor, buffer, buffer, blockSize);
        srcCursor += blockSize;

        /* diffuse: */
        diffuse(buffer, buffer, subblockCount, subblockPadding,
                digestSize, hashAlg);
    }
    /* final XOR: */
    xor_buf(srcCursor, buffer, (unsigned char *)dst, blockSize);
}

} // namespace lukscrack

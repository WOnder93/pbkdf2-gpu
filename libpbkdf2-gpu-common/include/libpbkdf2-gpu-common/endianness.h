/*
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

#ifndef PBKDF2_GPU_COMMON_ENDIANNESS_H
#define PBKDF2_GPU_COMMON_ENDIANNESS_H

#include <cstdint>

namespace pbkdf2_gpu {
namespace common {

class Endianness
{
public:
    /* This is sort of ugly and inefficient, but OTOH it's simple
     * and portable: */

    static std::uint_fast16_t read16LE(const void *src)
    {
        auto cursor = (const unsigned char *)src + 2 - 1;
        std::uint_fast16_t res = 0;
        res |= (std::uint_fast16_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast16_t)*cursor;
        return res;
    }

    static std::uint_fast16_t read16BE(const void *src)
    {
        auto cursor = (const unsigned char *)src;
        std::uint_fast16_t res = 0;
        res |= (std::uint_fast16_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast16_t)*cursor;
        return res;
    }

    static std::uint_fast32_t read32LE(const void *src)
    {
        auto cursor = (const unsigned char *)src + 4 - 1;
        std::uint_fast32_t res = 0;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast32_t)*cursor;
        return res;
    }

    static std::uint_fast32_t read32BE(const void *src)
    {
        auto cursor = (const unsigned char *)src;
        std::uint_fast32_t res = 0;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast32_t)*cursor;
        return res;
    }

    static std::uint_fast64_t read64LE(const void *src)
    {
        auto cursor = (const unsigned char *)src + 8 - 1;
        std::uint_fast64_t res = 0;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast64_t)*cursor;
        return res;
    }

    static std::uint_fast64_t read64BE(const void *src)
    {
        auto cursor = (const unsigned char *)src;
        std::uint_fast64_t res = 0;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast64_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast64_t)*cursor;
        return res;
    }

    static void write16LE(void *dst, std::uint_fast16_t i)
    {
        auto cursor = (unsigned char *)dst;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i;
    }

    static void write16BE(void *dst, std::uint_fast16_t i)
    {
        auto cursor = (unsigned char *)dst + 2 - 1;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i;
    }

    static void write32LE(void *dst, std::uint_fast32_t i)
    {
        auto cursor = (unsigned char *)dst;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i;
    }

    static void write32BE(void *dst, std::uint_fast32_t i)
    {
        auto cursor = (unsigned char *)dst + 4 - 1;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i;
    }

    static void write64LE(void *dst, std::uint_fast64_t i)
    {
        auto cursor = (unsigned char *)dst;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i;
    }

    static void write64BE(void *dst, std::uint_fast64_t i)
    {
        auto cursor = (unsigned char *)dst + 8 - 1;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i;
    }
};

} // namespace common
} // namespace pbkdf2_gpu

#endif // PBKDF2_GPU_COMMON_ENDIANNESS_H

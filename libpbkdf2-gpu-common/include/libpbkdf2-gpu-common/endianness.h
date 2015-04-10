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

    inline static std::uint_fast16_t read16LE(const void *src)
    {
        auto cursor = (const unsigned char *)src + 2 - 1;
        std::uint_fast16_t res = 0;
        res |= (std::uint_fast16_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast16_t)*cursor;
        return res;
    }

    inline static std::uint_fast16_t read16BE(const void *src)
    {
        auto cursor = (const unsigned char *)src;
        std::uint_fast16_t res = 0;
        res |= (std::uint_fast16_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast16_t)*cursor;
        return res;
    }

    inline static std::uint_fast32_t read32LE(const void *src)
    {
        auto cursor = (const unsigned char *)src + 4 - 1;
        std::uint_fast32_t res = 0;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; --cursor;
        res |= (std::uint_fast32_t)*cursor;
        return res;
    }

    inline static std::uint_fast32_t read32BE(const void *src)
    {
        auto cursor = (const unsigned char *)src;
        std::uint_fast32_t res = 0;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast32_t)*cursor; res <<= 8; ++cursor;
        res |= (std::uint_fast32_t)*cursor;
        return res;
    }

    inline static std::uint_fast64_t read64LE(const void *src)
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

    inline static std::uint_fast64_t read64BE(const void *src)
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

    inline static void write16LE(void *dst, std::uint_fast16_t i)
    {
        auto cursor = (unsigned char *)dst;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i;
    }

    inline static void write16BE(void *dst, std::uint_fast16_t i)
    {
        auto cursor = (unsigned char *)dst + 2 - 1;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i;
    }

    inline static void write32LE(void *dst, std::uint_fast32_t i)
    {
        auto cursor = (unsigned char *)dst;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i; i >>= 8; ++cursor;
        *cursor = (unsigned char)i;
    }

    inline static void write32BE(void *dst, std::uint_fast32_t i)
    {
        auto cursor = (unsigned char *)dst + 4 - 1;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i; i >>= 8; --cursor;
        *cursor = (unsigned char)i;
    }

    inline static void write64LE(void *dst, std::uint_fast64_t i)
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

    inline static void write64BE(void *dst, std::uint_fast64_t i)
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

/*
 * PBKDF2-HMAC-SHA1 OpenCL kernel
 *
 * Copyright (c) 2012, Sayantan Datta <std2048 at gmail dot com>
 * Copyright (c) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Based on software published under the following license:
 *
 *     This software is Copyright (c) 2012 Sayantan Datta <std2048 at gmail dot com>
 *     and it is hereby released to the general public under the following terms:
 *     Redistribution and use in source and binary forms, with or without modification, are permitted.
 *     Based on S3nf implementation http://openwall.info/wiki/john/MSCash2
 *     Modified to support salts upto 19 characters. Bug in orginal code allowed only upto 8 characters.
 */

#ifdef cl_nv_pragma_unroll
#define NVIDIA
#endif /* cl_nv_pragma_unroll */

/* TODO: make optional: */
#define DEBUG_LOG

#define LENGTH_UINT(len) (len / sizeof(uint))

/* Tests depend on logging functions: */
#ifdef TESTS

#ifndef DEBUG_LOG
#define DEBUG_LOG
#endif /* DEBUG_LOG */

#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
#endif /* ENABLE_LOGGING */

#endif /* TESTS */

/* Logging functions: */
#ifdef DEBUG_LOG
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

typedef struct output_stream {
    __global char *buffer;
    uint pos;
} output_stream_t;

inline void stream_init(__private output_stream_t *stream, __global char *buffer)
{
    stream->buffer = buffer;
    stream->pos = 0;
}

inline void stream_close(__private output_stream_t *stream)
{
    stream->buffer[stream->pos++] = '\0';
}

inline void dump_l(__private output_stream_t *stream, __constant char *str)
{
    while (*str != '\0') {
        stream->buffer[stream->pos++] = *str;
        ++str;
    }
}

inline void dump_s(__private output_stream_t *stream, __private char *str)
{
    while (*str != '\0') {
        stream->buffer[stream->pos++] = *str;
        ++str;
    }
}

inline void dump_ui(__private output_stream_t *stream, uint i)
{
    uint rev = 0;
    while (i != 0) {
        uint d = i / 10;
        rev *= 10;
        rev += i % 10;
        i = d;
    }
    if (rev == 0) {
        stream->buffer[stream->pos++] = '0';
    } else {
        do {
            uint d = rev / 10;
            uint r = rev % 10;
            stream->buffer[stream->pos++] = '0' + r;
            rev = d;
        } while (rev != 0);
    }
}

inline void dump_uix(__private output_stream_t *stream, uint n)
{
    for (uint i = 0; i < 8; i++) {
        uint r = (n >> (32 - ((i + 1) << 2))) & 0xf;
        stream->buffer[stream->pos++] = r < 0xa ? '0' + r : 'a' + (r - 0xa);
    }
}

inline void dump_uix_a(__private output_stream_t *stream, __private uint *array, uint length) {
    dump_l(stream, "[");
    for (uint i = 0; i < length; i++) {
        if (i > 0) { dump_l(stream, " "); }
        dump_uix(stream, array[i]);
    }
    dump_l(stream, "]");
}
#endif /* DEBUG_LOG */

/* SHA1 macros: */
#define SHA1_INIT_A                 0x67452301
#define SHA1_INIT_B                 0xEFCDAB89
#define SHA1_INIT_C                 0x98BADCFE
#define SHA1_INIT_D                 0x10325476
#define SHA1_INIT_E                 0xC3D2E1F0

#define SHA1_UNROLL_IBLOCK(block) \
    block[0x0], block[0x1], block[0x2], block[0x3], \
    block[0x4], block[0x5], block[0x6], block[0x7], \
    block[0x8], block[0x9], block[0xA], block[0xB], \
    block[0xC], block[0xD], block[0xE], block[0xF]

#define SHA1_UNROLL_OBLOCK(block) \
    block[0x0], \
    block[0x1], \
    block[0x2], \
    block[0x3], \
    block[0x4]

#define SHA1_UNROLL_INITSTATE \
    SHA1_INIT_A, \
    SHA1_INIT_B, \
    SHA1_INIT_C, \
    SHA1_INIT_D, \
    SHA1_INIT_E

#define S1(x) rotate((x), (uint)1)
#define S5(x) rotate((x), (uint)5)
#define S30(x) rotate((x), (uint)30)

#define SHA1_STEP(a, b, c, d, e, f, k, x) \
do { \
    e += S5(a) + f(b,c,d) + k + x; b = S30(b); \
} while(0)

#define SHA1_NEXT_WORDS(buffer, W0, W1, W2, W3, W4, W5, W6, W7, W8, W9, WA, WB, WC, WD, WE, WF) \
do { \
    buffer[0x0] = S1(WD          ^ W8          ^ W2          ^ W0); \
    buffer[0x1] = S1(WE          ^ W9          ^ W3          ^ W1); \
    buffer[0x2] = S1(WF          ^ WA          ^ W4          ^ W2); \
    buffer[0x3] = S1(buffer[0x0] ^ WB          ^ W5          ^ W3); \
    buffer[0x4] = S1(buffer[0x1] ^ WC          ^ W6          ^ W4); \
    buffer[0x5] = S1(buffer[0x2] ^ WD          ^ W7          ^ W5); \
    buffer[0x6] = S1(buffer[0x3] ^ WE          ^ W8          ^ W6); \
    buffer[0x7] = S1(buffer[0x4] ^ WF          ^ W9          ^ W7); \
    buffer[0x8] = S1(buffer[0x5] ^ buffer[0x0] ^ WA          ^ W8); \
    buffer[0x9] = S1(buffer[0x6] ^ buffer[0x1] ^ WB          ^ W9); \
    buffer[0xA] = S1(buffer[0x7] ^ buffer[0x2] ^ WC          ^ WA); \
    buffer[0xB] = S1(buffer[0x8] ^ buffer[0x3] ^ WD          ^ WB); \
    buffer[0xC] = S1(buffer[0x9] ^ buffer[0x4] ^ WE          ^ WC); \
    buffer[0xD] = S1(buffer[0xA] ^ buffer[0x5] ^ WF          ^ WD); \
    buffer[0xE] = S1(buffer[0xB] ^ buffer[0x6] ^ buffer[0x0] ^ WE); \
    buffer[0xF] = S1(buffer[0xC] ^ buffer[0x7] ^ buffer[0x1] ^ WF); \
} while(0)

#ifndef NVIDIA
#define SHA1_F0(x,y,z) bitselect(z, y, x)
#else
#define SHA1_F0(x,y,z) (z ^ (x & (y ^ z)))
#endif /* NVIDIA */

#define SHA1_F1(x,y,z) (x ^ y ^ z)
#define SHA1_F2(x,y,z) bitselect(y, x, y ^ z)
#define SHA1_F3(x,y,z) (x ^ y ^ z)

#define SHA1_K0 0x5A827999
#define SHA1_K1 0x6ED9EBA1
#define SHA1_K2 0x8F1BBCDC
#define SHA1_K3 0xCA62C1D6

#define SHA1(A, B, C, D, E, buffer, W0, W1, W2, W3, W4, W5, W6, W7, W8, W9, WA, WB, WC, WD, WE, WF) \
do { \
    SHA1_STEP(A, B, C, D, E, SHA1_F0, SHA1_K0, W0); \
    SHA1_STEP(E, A, B, C, D, SHA1_F0, SHA1_K0, W1); \
    SHA1_STEP(D, E, A, B, C, SHA1_F0, SHA1_K0, W2); \
    SHA1_STEP(C, D, E, A, B, SHA1_F0, SHA1_K0, W3); \
    SHA1_STEP(B, C, D, E, A, SHA1_F0, SHA1_K0, W4); \
    SHA1_STEP(A, B, C, D, E, SHA1_F0, SHA1_K0, W5); \
    SHA1_STEP(E, A, B, C, D, SHA1_F0, SHA1_K0, W6); \
    SHA1_STEP(D, E, A, B, C, SHA1_F0, SHA1_K0, W7); \
    SHA1_STEP(C, D, E, A, B, SHA1_F0, SHA1_K0, W8); \
    SHA1_STEP(B, C, D, E, A, SHA1_F0, SHA1_K0, W9); \
    SHA1_STEP(A, B, C, D, E, SHA1_F0, SHA1_K0, WA); \
    SHA1_STEP(E, A, B, C, D, SHA1_F0, SHA1_K0, WB); \
    SHA1_STEP(D, E, A, B, C, SHA1_F0, SHA1_K0, WC); \
    SHA1_STEP(C, D, E, A, B, SHA1_F0, SHA1_K0, WD); \
    SHA1_STEP(B, C, D, E, A, SHA1_F0, SHA1_K0, WE); \
    SHA1_STEP(A, B, C, D, E, SHA1_F0, SHA1_K0, WF); \
    SHA1_NEXT_WORDS(buffer, \
        W0, W1, W2, W3, W4, W5, W6, W7, W8, \
        W9, WA, WB, WC, WD, WE, WF); \
    SHA1_STEP(E, A, B, C, D, SHA1_F0, SHA1_K0, buffer[0x0]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F0, SHA1_K0, buffer[0x1]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F0, SHA1_K0, buffer[0x2]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F0, SHA1_K0, buffer[0x3]); \
\
    SHA1_STEP(A, B, C, D, E, SHA1_F1, SHA1_K1, buffer[0x4]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F1, SHA1_K1, buffer[0x5]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F1, SHA1_K1, buffer[0x6]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F1, SHA1_K1, buffer[0x7]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F1, SHA1_K1, buffer[0x8]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F1, SHA1_K1, buffer[0x9]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F1, SHA1_K1, buffer[0xA]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F1, SHA1_K1, buffer[0xB]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F1, SHA1_K1, buffer[0xC]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F1, SHA1_K1, buffer[0xD]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F1, SHA1_K1, buffer[0xE]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F1, SHA1_K1, buffer[0xF]); \
    SHA1_NEXT_WORDS(buffer, \
        buffer[0x0], buffer[0x1], buffer[0x2], buffer[0x3], \
        buffer[0x4], buffer[0x5], buffer[0x6], buffer[0x7], \
        buffer[0x8], buffer[0x9], buffer[0xA], buffer[0xB], \
        buffer[0xC], buffer[0xD], buffer[0xE], buffer[0xF]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F1, SHA1_K1, buffer[0x0]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F1, SHA1_K1, buffer[0x1]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F1, SHA1_K1, buffer[0x2]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F1, SHA1_K1, buffer[0x3]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F1, SHA1_K1, buffer[0x4]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F1, SHA1_K1, buffer[0x5]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F1, SHA1_K1, buffer[0x6]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F1, SHA1_K1, buffer[0x7]); \
\
    SHA1_STEP(A, B, C, D, E, SHA1_F2, SHA1_K2, buffer[0x8]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F2, SHA1_K2, buffer[0x9]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F2, SHA1_K2, buffer[0xA]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F2, SHA1_K2, buffer[0xB]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F2, SHA1_K2, buffer[0xC]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F2, SHA1_K2, buffer[0xD]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F2, SHA1_K2, buffer[0xE]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F2, SHA1_K2, buffer[0xF]); \
    SHA1_NEXT_WORDS(buffer, \
        buffer[0x0], buffer[0x1], buffer[0x2], buffer[0x3], \
        buffer[0x4], buffer[0x5], buffer[0x6], buffer[0x7], \
        buffer[0x8], buffer[0x9], buffer[0xA], buffer[0xB], \
        buffer[0xC], buffer[0xD], buffer[0xE], buffer[0xF]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F2, SHA1_K2, buffer[0x0]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F2, SHA1_K2, buffer[0x1]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F2, SHA1_K2, buffer[0x2]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F2, SHA1_K2, buffer[0x3]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F2, SHA1_K2, buffer[0x4]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F2, SHA1_K2, buffer[0x5]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F2, SHA1_K2, buffer[0x6]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F2, SHA1_K2, buffer[0x7]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F2, SHA1_K2, buffer[0x8]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F2, SHA1_K2, buffer[0x9]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F2, SHA1_K2, buffer[0xA]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F2, SHA1_K2, buffer[0xB]); \
\
    SHA1_STEP(A, B, C, D, E, SHA1_F3, SHA1_K3, buffer[0xC]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F3, SHA1_K3, buffer[0xD]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F3, SHA1_K3, buffer[0xE]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F3, SHA1_K3, buffer[0xF]); \
    SHA1_NEXT_WORDS(buffer, \
        buffer[0x0], buffer[0x1], buffer[0x2], buffer[0x3], \
        buffer[0x4], buffer[0x5], buffer[0x6], buffer[0x7], \
        buffer[0x8], buffer[0x9], buffer[0xA], buffer[0xB], \
        buffer[0xC], buffer[0xD], buffer[0xE], buffer[0xF]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F3, SHA1_K3, buffer[0x0]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F3, SHA1_K3, buffer[0x1]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F3, SHA1_K3, buffer[0x2]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F3, SHA1_K3, buffer[0x3]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F3, SHA1_K3, buffer[0x4]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F3, SHA1_K3, buffer[0x5]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F3, SHA1_K3, buffer[0x6]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F3, SHA1_K3, buffer[0x7]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F3, SHA1_K3, buffer[0x8]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F3, SHA1_K3, buffer[0x9]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F3, SHA1_K3, buffer[0xA]); \
    SHA1_STEP(A, B, C, D, E, SHA1_F3, SHA1_K3, buffer[0xB]); \
    SHA1_STEP(E, A, B, C, D, SHA1_F3, SHA1_K3, buffer[0xC]); \
    SHA1_STEP(D, E, A, B, C, SHA1_F3, SHA1_K3, buffer[0xD]); \
    SHA1_STEP(C, D, E, A, B, SHA1_F3, SHA1_K3, buffer[0xE]); \
    SHA1_STEP(B, C, D, E, A, SHA1_F3, SHA1_K3, buffer[0xF]); \
} while(0)

/* SHA1 block sizes: */
#define INPUT_BLOCK_LENGTH 64
#define OUTPUT_BLOCK_LENGTH 20

inline void sha1_update_block(
    uint prev_A, uint prev_B, uint prev_C, uint prev_D, uint prev_E,
    __private uint *block, __private uint *state)
{
    state[0] = prev_A;
    state[1] = prev_B;
    state[2] = prev_C;
    state[3] = prev_D;
    state[4] = prev_E;

    SHA1(
        state[0], state[1], state[2], state[3], state[4],
        block,
        block[0x0], block[0x1], block[0x2], block[0x3],
        block[0x4], block[0x5], block[0x6], block[0x7],
        block[0x8], block[0x9], block[0xA], block[0xB],
        block[0xC], block[0xD], block[0xE], block[0xF]
    );

    state[0] += prev_A;
    state[1] += prev_B;
    state[2] += prev_C;
    state[3] += prev_D;
    state[4] += prev_E;
}

inline void sha1_digest_digest(
    uint prev_A, uint prev_B, uint prev_C, uint prev_D, uint prev_E,
    __private uint *buffer, __private uint *state)
{
    state[0] = prev_A;
    state[1] = prev_B;
    state[2] = prev_C;
    state[3] = prev_D;
    state[4] = prev_E;

    SHA1(
        state[0], state[1], state[2], state[3], state[4],
        buffer,
        buffer[0], buffer[1], buffer[2], buffer[3], buffer[4],
        0x80000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x000002A0
    );

    state[0] += prev_A;
    state[1] += prev_B;
    state[2] += prev_C;
    state[3] += prev_D;
    state[4] += prev_E;
}

#ifdef DEBUG_LOG
inline void dump_sha1(__private output_stream_t *out, __private uint *hash)
{
    dump_l(out, "SHA1");
    dump_uix_a(out, hash, LENGTH_UINT(OUTPUT_BLOCK_LENGTH));
}
#endif /* DEBUG_LOG */

/* PBKDF2 functions: */
#ifndef SALT_LENGTH
#error "SALT_LENGTH not defined!"
#endif /* SALT_LENGTH */

#define SWITCH_ENDIANNESS(v) (\
    (((uint)(v) & 0xff) << 24) | \
    (((uint)(v) <<  8) & 0x00ff0000) | \
    (((uint)(v) >>  8) & 0x0000ff00) | \
    (((uint)(v) >> 24) & 0x000000ff))

inline void pbkdf2_init(
    __private output_stream_t *dbg,
    __constant uint *salt, uint dk_block_index,
    const __private uint *istate,
    __private uint *state)
{
    const __private uint *prev_state = istate;

    /* Process contigous blocks of salt: */
    {
        uint buffer[LENGTH_UINT(INPUT_BLOCK_LENGTH)];
        for (uint i = 0; i < SALT_LENGTH / INPUT_BLOCK_LENGTH; i++) {
            for (uint k = 0; k < LENGTH_UINT(INPUT_BLOCK_LENGTH); k++) {
                buffer[k] = SWITCH_ENDIANNESS(salt[k]);
            }
            sha1_update_block(SHA1_UNROLL_OBLOCK(prev_state), buffer, state);
            salt += LENGTH_UINT(INPUT_BLOCK_LENGTH);
            prev_state = state;
        }
    }

    /* Prepare and process the last (possibly partial) block of salt with the padding: */
    /* (may be one or two blocks) */
#define TAIL_BLOCKS ((SALT_LENGTH % INPUT_BLOCK_LENGTH) / (INPUT_BLOCK_LENGTH - 4 - 8) + 1)
    {
        uint tail[TAIL_BLOCKS * LENGTH_UINT(INPUT_BLOCK_LENGTH)];
        {
            uint i = 0;
            /* Process the rest of the salt: */
            /* (except the last (SALT_LENGTH % 4) bytes) */
            for (; i < LENGTH_UINT(SALT_LENGTH % INPUT_BLOCK_LENGTH); i++) {
                tail[i] = SWITCH_ENDIANNESS(salt[i]);
            }

            /* Append DK block index: */
            if ((SALT_LENGTH % 4) == 0) {
                tail[i] = dk_block_index + 1;
                i++;
                tail[i] = 0x80000000;
                i++;
            } else {
                tail[i] = SWITCH_ENDIANNESS(salt[i]) | ((dk_block_index + 1) >> ((SALT_LENGTH % 4) << 3));
                i++;
                tail[i] = ((dk_block_index + 1) << (32 - ((SALT_LENGTH % 4) << 3))) | ((uint)0x80000000 >> ((SALT_LENGTH % 4) << 3));
                i++;
            }

            /* Pad with zeroes: */
            for (; i < TAIL_BLOCKS * LENGTH_UINT(INPUT_BLOCK_LENGTH) - 1; i++) {
                tail[i] = 0x00000000;
            }

            /* Put SHA1 message length at the end of the last block: */
            tail[TAIL_BLOCKS * LENGTH_UINT(INPUT_BLOCK_LENGTH) - 1] = (INPUT_BLOCK_LENGTH + SALT_LENGTH + 4) * 8;
        }

#ifdef ENABLE_LOGGING
    dump_l(dbg, "Tail: ");
    dump_uix_a(dbg, tail, TAIL_BLOCKS * LENGTH_UINT(INPUT_BLOCK_LENGTH));
    dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

        /* Process the prepared blocks: */
        __private uint *input = tail;
        for (uint i = 0; i < TAIL_BLOCKS; i++) {
            sha1_update_block(SHA1_UNROLL_OBLOCK(prev_state), input, state);
            input += LENGTH_UINT(INPUT_BLOCK_LENGTH);
            prev_state = state;
        }
        /* From now on 'prev_state' definitely refers to 'state' */
        /* which contains the half-done HMAC of iteration 0.     */
    }
#undef TAIL_BLOCKS

#ifdef ENABLE_LOGGING
    dump_l(dbg, "Iteration 0 semi-digest: ");
    dump_sha1(dbg, state);
    dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */
}

inline void pbkdf2_iter(
    __private output_stream_t *dbg,
    const __private uint *istate, const __private uint *ostate,
    uint iterations,
    __private uint *dk, __private uint *state)
{
    uint buffer[LENGTH_UINT(INPUT_BLOCK_LENGTH)];

    /* Complete the HMAC of the first iteration: */
    sha1_digest_digest(SHA1_UNROLL_OBLOCK(ostate), state, buffer);

#ifdef ENABLE_LOGGING
    dump_l(dbg, "Iteration 0 digest: ");
    dump_sha1(dbg, buffer);
    dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

    /* Copy the result of iteration 0 to the final result buffer: */
    for (uint i = 0; i < LENGTH_UINT(OUTPUT_BLOCK_LENGTH); i++) {
        dk[i] = buffer[i];
    }

    /* Perform the remaining iterations: */
    for (uint i = 1; i < iterations; i++) {
        sha1_digest_digest(SHA1_UNROLL_OBLOCK(istate), buffer, state);

#ifdef ENABLE_LOGGING
        dump_l(dbg, "Iteration ");
        dump_ui(dbg, i);
        dump_l(dbg, " semi-digest: ");
        dump_sha1(dbg, state);
        dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

        sha1_digest_digest(SHA1_UNROLL_OBLOCK(ostate), state, buffer);

#ifdef ENABLE_LOGGING
        dump_l(dbg, "Iteration ");
        dump_ui(dbg, i);
        dump_l(dbg, " digest: ");
        dump_sha1(dbg, buffer);
        dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

        /* XOR the result of this iteration into the final result: */
        for (uint i = 0; i < LENGTH_UINT(OUTPUT_BLOCK_LENGTH); i++) {
            dk[i] ^= buffer[i];
        }

#ifdef ENABLE_LOGGING
        dump_l(dbg, "Iteration ");
        dump_ui(dbg, i);
        dump_l(dbg, " digest XOR'd: ");
        dump_sha1(dbg, dk);
        dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */
    }
}

/* Main kernel: */
__kernel
void pbkdf2_kernel(
    const __global uint *input,
    __global uint *output,
    __constant uint *salt,
    uint dk_blocks,
    uint iterations,
    uint batchSize,
    __global char *debug_buffer)
{
#ifdef DEBUG_LOG
    output_stream_t dbg;
    stream_init(&dbg, debug_buffer);
#endif /* DEBUG_LOG */

    uint input_block_index = (uint)get_global_id(0);
    uint input_pos = input_block_index;

    uint ipad_state[LENGTH_UINT(OUTPUT_BLOCK_LENGTH)];
    uint opad_state[LENGTH_UINT(OUTPUT_BLOCK_LENGTH)];
    {
        uint ipad[LENGTH_UINT(INPUT_BLOCK_LENGTH)];
        uint opad[LENGTH_UINT(INPUT_BLOCK_LENGTH)];

        for (uint row = 0; row < LENGTH_UINT(INPUT_BLOCK_LENGTH); row++) {
            uint in = SWITCH_ENDIANNESS(input[input_pos + row * batchSize]);

#ifdef ENABLE_LOGGING
            dump_uix(&dbg, in);
            dump_l(&dbg, " ");
#endif /* ENABLE_LOGGING */

            ipad[row] = in ^ 0x36363636;
            opad[row] = in ^ 0x5C5C5C5C;
        }
#ifdef ENABLE_LOGGING
        dump_l(&dbg, "\n");
#endif /* ENABLE_LOGGING */

        sha1_update_block(SHA1_UNROLL_INITSTATE, ipad, ipad_state);
        sha1_update_block(SHA1_UNROLL_INITSTATE, opad, opad_state);
    }

    uint dk_block_index = (uint)get_global_id(1);

    uint buffer[LENGTH_UINT(INPUT_BLOCK_LENGTH)];
    pbkdf2_init(&dbg, salt, dk_block_index, ipad_state, buffer);

    uint dk[LENGTH_UINT(OUTPUT_BLOCK_LENGTH)];
    pbkdf2_iter(&dbg, ipad_state, opad_state, iterations, dk, buffer);

    uint output_block_index = input_block_index * dk_blocks + dk_block_index;
    uint output_pos = output_block_index;

    for (uint i = 0; i < LENGTH_UINT(OUTPUT_BLOCK_LENGTH); i++) {
        output[output_pos + i * batchSize * dk_blocks] = SWITCH_ENDIANNESS(dk[i]);
    }

#ifdef DEBUG_LOG
    stream_close(&dbg);
#endif /* DEBUG_LOG */
}

/* Testing functions & testing kernel: */
#ifdef TESTS
inline bool sha1_compare(__private uint *hash1, __private uint *hash2)
{
    for (uint i = 0; i < LENGTH_UINT(OUTPUT_BLOCK_LENGTH); i++) {
        if (hash1[i] != hash2[i]) {
            return false;
        }
    }
    return true;
}

inline void test_sha1(__private output_stream_t *out, __constant char *name,
    __private uint *block, __private uint *hash)
{
    uint state[LENGTH_UINT(OUTPUT_BLOCK_LENGTH)];
    sha1_update_block(SHA1_UNROLL_INITSTATE, block, state);
    if (!sha1_compare(hash, state)) {
        dump_l(out, "SHA1 test '");
        dump_l(out, name);
        dump_l(out, "' failed! ");
        dump_sha1(out, state);
        dump_l(out, " should be ");
        dump_sha1(out, hash);
        dump_l(out, "\n");
    }
}

__kernel
void run_tests(__global char *debug_buffer)
{
    output_stream_t out;
    out.buffer = debug_buffer;
    out.pos = 0;

    {
        if (SWITCH_ENDIANNESS(0x12345678) != 0x78563412) {
            dump_l(&out, "SWAP_ENDIANNESS(0x12345678) should be 0x78563412\n");
        }
        if (SWITCH_ENDIANNESS(0x78563412) != 0x12345678) {
            dump_l(&out, "SWAP_ENDIANNESS(0x78563412) should be 0x12345678\n");
        }
    }
    {
        uint tv_i[] = {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
        };
        uint tv_o[] = {
            0xda39a3ee, 0x5e6b4b0d, 0x3255bfef, 0x95601890, 0xafd80709,
        };
        test_sha1(&out, "", tv_i, tv_o);
    }
    {
        uint tv_i[] = {
            0x61626380, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000018,
        };
        uint tv_o[] = {
            0xa9993e36, 0x4706816a, 0xba3e2571, 0x7850c26c, 0x9cd0d89d,
        };
        test_sha1(&out, "abc", tv_i, tv_o);
    }

    stream_close(&out);
}
#endif /* TESTS */

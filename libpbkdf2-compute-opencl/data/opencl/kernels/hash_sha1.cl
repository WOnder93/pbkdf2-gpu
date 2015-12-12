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

#ifdef HASH_NAME
#error "Hash function already defined!"
#endif

#include "sha_common.cl"
#include "hash_word_uint.cl"

#define HASH_NAME "SHA-1"

#define HASH_IBLOCK_WORDS 16
#define HASH_OBLOCK_WORDS  5

#define HASH_ML_WORDS 2

#define SHA1_INIT_A  0x67452301
#define SHA1_INIT_B  0xEFCDAB89
#define SHA1_INIT_C  0x98BADCFE
#define SHA1_INIT_D  0x10325476
#define SHA1_INIT_E  0xC3D2E1F0

#define HASH_IBLOCK_UNROLL(block) \
    block[0x0], block[0x1], block[0x2], block[0x3], \
    block[0x4], block[0x5], block[0x6], block[0x7], \
    block[0x8], block[0x9], block[0xA], block[0xB], \
    block[0xC], block[0xD], block[0xE], block[0xF]

#define HASH_OBLOCK_UNROLL(block) \
    block[0x0], \
    block[0x1], \
    block[0x2], \
    block[0x3], \
    block[0x4]

#define HASH_INITSTATE_UNROLL \
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

#define SHA1_F0(x, y, z) SHA_F0(x, y, z)
#define SHA1_F1(x, y, z) (x ^ y ^ z)
#define SHA1_F2(x, y, z) SHA_F1(x, y, z)
#define SHA1_F3(x, y, z) (x ^ y ^ z)

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

inline void hash_update_block(
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

inline void hash_digest_digest(
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

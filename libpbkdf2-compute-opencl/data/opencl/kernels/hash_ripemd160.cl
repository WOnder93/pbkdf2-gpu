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

#define HASH_NAME "RIPEMD-160"
#define HASH_LITTLE_ENDIAN

#define HASH_IBLOCK_WORDS 16
#define HASH_OBLOCK_WORDS  5

#define HASH_ML_WORDS 2

#define RIPEMD160_INIT_A  0x67452301
#define RIPEMD160_INIT_B  0xEFCDAB89
#define RIPEMD160_INIT_C  0x98BADCFE
#define RIPEMD160_INIT_D  0x10325476
#define RIPEMD160_INIT_E  0xC3D2E1F0

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
    RIPEMD160_INIT_A, \
    RIPEMD160_INIT_B, \
    RIPEMD160_INIT_C, \
    RIPEMD160_INIT_D, \
    RIPEMD160_INIT_E

#define ROL(x, n) rotate((x), (uint)(x))

#define RIPEMD160_STEP(a, b, c, d, e, f, k, x, s) \
do { \
    a += ROL(a + f(b,c,d) + x + k, s) + e; c = ROL(c, 10); \
} while(0)

#define RIPEMD160_F0(x, y, z) (x ^ y ^ z)
#define RIPEMD160_F1(x, y, z) SHA_F0(x, y, z)
#define RIPEMD160_F2(x, y, z) ((x | ~y) ^ z)
#define RIPEMD160_F3(x, y, z) RIPEMD160_F1(z, x, y)
#define RIPEMD160_F4(x, y, z) RIPEMD160_F2(y, z, x)

#define RIPEMD160_K0 0x00000000
#define RIPEMD160_K1 0x5A827999
#define RIPEMD160_K2 0x6ED9EBA1
#define RIPEMD160_K3 0x8F1BBCDC
#define RIPEMD160_K4 0xA953FD4E
#define RIPEMD160_K5 0x50A28BE6
#define RIPEMD160_K6 0x5C4DD124
#define RIPEMD160_K7 0x6D703EF3
#define RIPEMD160_K8 0x7A6D76E9
#define RIPEMD160_K9 0x00000000

#define RIPEMD160(pA, pB, pC, pD, pE, A, B, C, D, E, buffer, W0, W1, W2, W3, W4, W5, W6, W7, W8, W9, WA, WB, WC, WD, WE, WF) \
do { \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F0, RIPEMD160_K0, W0, 11); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F0, RIPEMD160_K0, W1, 14); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F0, RIPEMD160_K0, W2, 15); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F0, RIPEMD160_K0, W3, 12); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F0, RIPEMD160_K0, W4,  5); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F0, RIPEMD160_K0, W5,  8); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F0, RIPEMD160_K0, W6,  7); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F0, RIPEMD160_K0, W7,  9); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F0, RIPEMD160_K0, W8, 11); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F0, RIPEMD160_K0, W9, 13); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F0, RIPEMD160_K0, WA, 14); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F0, RIPEMD160_K0, WB, 15); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F0, RIPEMD160_K0, WC,  6); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F0, RIPEMD160_K0, WD,  7); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F0, RIPEMD160_K0, WE,  9); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F0, RIPEMD160_K0, WF,  8); \
\
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F1, RIPEMD160_K1, W7,  7); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F1, RIPEMD160_K1, W4,  6); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F1, RIPEMD160_K1, WD,  8); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F1, RIPEMD160_K1, W1, 13); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F1, RIPEMD160_K1, WA, 11); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F1, RIPEMD160_K1, W6,  9); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F1, RIPEMD160_K1, WF,  7); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F1, RIPEMD160_K1, W3, 15); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F1, RIPEMD160_K1, WC,  7); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F1, RIPEMD160_K1, W0, 12); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F1, RIPEMD160_K1, W9, 15); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F1, RIPEMD160_K1, W5,  9); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F1, RIPEMD160_K1, W2, 11); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F1, RIPEMD160_K1, WE,  7); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F1, RIPEMD160_K1, WB, 13); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F1, RIPEMD160_K1, W8, 12); \
\
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F2, RIPEMD160_K2, W3, 11); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F2, RIPEMD160_K2, WA, 13); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F2, RIPEMD160_K2, WE,  6); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F2, RIPEMD160_K2, W4,  7); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F2, RIPEMD160_K2, W9, 14); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F2, RIPEMD160_K2, WF,  9); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F2, RIPEMD160_K2, W8, 13); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F2, RIPEMD160_K2, W1, 15); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F2, RIPEMD160_K2, W2, 14); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F2, RIPEMD160_K2, W7,  8); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F2, RIPEMD160_K2, W0, 13); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F2, RIPEMD160_K2, W6,  6); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F2, RIPEMD160_K2, WD,  5); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F2, RIPEMD160_K2, WB, 12); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F2, RIPEMD160_K2, W5,  7); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F2, RIPEMD160_K2, WC,  5); \
\
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F3, RIPEMD160_K3, W1, 11); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F3, RIPEMD160_K3, W9, 12); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F3, RIPEMD160_K3, WB, 14); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F3, RIPEMD160_K3, WA, 15); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F3, RIPEMD160_K3, W0, 14); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F3, RIPEMD160_K3, W8, 15); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F3, RIPEMD160_K3, WC,  9); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F3, RIPEMD160_K3, W4,  8); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F3, RIPEMD160_K3, WD,  9); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F3, RIPEMD160_K3, W3, 14); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F3, RIPEMD160_K3, W7,  5); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F3, RIPEMD160_K3, WF,  6); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F3, RIPEMD160_K3, WE,  8); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F3, RIPEMD160_K3, W5,  6); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F3, RIPEMD160_K3, W6,  5); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F3, RIPEMD160_K3, W2, 12); \
\
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F4, RIPEMD160_K4, W4,  9); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F4, RIPEMD160_K4, W0, 15); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F4, RIPEMD160_K4, W5,  5); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F4, RIPEMD160_K4, W9, 11); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F4, RIPEMD160_K4, W7,  6); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F4, RIPEMD160_K4, WC,  8); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F4, RIPEMD160_K4, W2, 13); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F4, RIPEMD160_K4, WA, 12); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F4, RIPEMD160_K4, WE,  5); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F4, RIPEMD160_K4, W1, 12); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F4, RIPEMD160_K4, W3, 13); \
    RIPEMD160_STEP(A, B, C, D, E, RIPEMD160_F4, RIPEMD160_K4, W8, 14); \
    RIPEMD160_STEP(E, A, B, C, D, RIPEMD160_F4, RIPEMD160_K4, WB, 11); \
    RIPEMD160_STEP(D, E, A, B, C, RIPEMD160_F4, RIPEMD160_K4, W6,  8); \
    RIPEMD160_STEP(C, D, E, A, B, RIPEMD160_F4, RIPEMD160_K4, WF,  5); \
    RIPEMD160_STEP(B, C, D, E, A, RIPEMD160_F4, RIPEMD160_K4, WD,  6); \
\
    hash_word_t a = pA; \
    hash_word_t b = pB; \
    hash_word_t c = pC; \
    hash_word_t d = pD; \
    hash_word_t e = pE; \
\
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F4, RIPEMD160_K5, W5,  8); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F4, RIPEMD160_K5, WE,  9); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F4, RIPEMD160_K5, W7,  9); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F4, RIPEMD160_K5, W0, 11); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F4, RIPEMD160_K5, W9, 13); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F4, RIPEMD160_K5, W2, 15); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F4, RIPEMD160_K5, WB, 15); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F4, RIPEMD160_K5, W4,  5); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F4, RIPEMD160_K5, WD,  7); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F4, RIPEMD160_K5, W6,  7); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F4, RIPEMD160_K5, WF,  8); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F4, RIPEMD160_K5, W8, 11); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F4, RIPEMD160_K5, W1, 14); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F4, RIPEMD160_K5, WA, 14); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F4, RIPEMD160_K5, W3, 12); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F4, RIPEMD160_K5, WC,  6); \
\
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F3, RIPEMD160_K6, W6,  9); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F3, RIPEMD160_K6, WB, 13); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F3, RIPEMD160_K6, W3, 15); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F3, RIPEMD160_K6, W7,  7); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F3, RIPEMD160_K6, W0, 12); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F3, RIPEMD160_K6, WD,  8); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F3, RIPEMD160_K6, W5,  9); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F3, RIPEMD160_K6, WA, 11); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F3, RIPEMD160_K6, WE,  7); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F3, RIPEMD160_K6, WF,  7); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F3, RIPEMD160_K6, W8, 12); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F3, RIPEMD160_K6, WC,  7); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F3, RIPEMD160_K6, W4,  6); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F3, RIPEMD160_K6, W9, 15); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F3, RIPEMD160_K6, W1, 13); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F3, RIPEMD160_K6, W2, 11); \
\
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F2, RIPEMD160_K7, WF,  9); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F2, RIPEMD160_K7, W5,  7); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F2, RIPEMD160_K7, W1, 15); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F2, RIPEMD160_K7, W3, 11); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F2, RIPEMD160_K7, W7,  8); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F2, RIPEMD160_K7, WE,  6); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F2, RIPEMD160_K7, W6,  6); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F2, RIPEMD160_K7, W9, 14); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F2, RIPEMD160_K7, WB, 12); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F2, RIPEMD160_K7, W8, 13); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F2, RIPEMD160_K7, WC,  5); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F2, RIPEMD160_K7, W2, 14); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F2, RIPEMD160_K7, WA, 13); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F2, RIPEMD160_K7, W0, 13); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F2, RIPEMD160_K7, W4,  7); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F2, RIPEMD160_K7, WD,  5); \
\
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F1, RIPEMD160_K8, W8, 15); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F1, RIPEMD160_K8, W6,  5); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F1, RIPEMD160_K8, W4,  8); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F1, RIPEMD160_K8, W1, 11); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F1, RIPEMD160_K8, W3, 14); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F1, RIPEMD160_K8, WB, 14); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F1, RIPEMD160_K8, WF,  6); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F1, RIPEMD160_K8, W0, 14); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F1, RIPEMD160_K8, W5,  6); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F1, RIPEMD160_K8, WC,  9); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F1, RIPEMD160_K8, W2, 12); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F1, RIPEMD160_K8, WD,  9); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F1, RIPEMD160_K8, W9, 12); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F1, RIPEMD160_K8, W7,  5); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F1, RIPEMD160_K8, WA, 15); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F1, RIPEMD160_K8, WE,  8); \
\
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F0, RIPEMD160_K9, WC,  8); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F0, RIPEMD160_K9, WF,  5); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F0, RIPEMD160_K9, WA, 12); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F0, RIPEMD160_K9, W4,  9); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F0, RIPEMD160_K9, W1, 12); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F0, RIPEMD160_K9, W5,  5); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F0, RIPEMD160_K9, W8, 14); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F0, RIPEMD160_K9, W7,  6); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F0, RIPEMD160_K9, W6,  8); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F0, RIPEMD160_K9, W2, 13); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F0, RIPEMD160_K9, WD,  6); \
    RIPEMD160_STEP(a, b, c, d, e, RIPEMD160_F0, RIPEMD160_K9, WE,  5); \
    RIPEMD160_STEP(e, a, b, c, d, RIPEMD160_F0, RIPEMD160_K9, W0, 15); \
    RIPEMD160_STEP(d, e, a, b, c, RIPEMD160_F0, RIPEMD160_K9, W3, 13); \
    RIPEMD160_STEP(c, d, e, a, b, RIPEMD160_F0, RIPEMD160_K9, W9, 11); \
    RIPEMD160_STEP(b, c, d, e, a, RIPEMD160_F0, RIPEMD160_K9, WB, 11); \
\
    c += B; \
    B = D + e; \
    D = A + b; \
    A = C + d; \
    C = E + a; \
    E =     c; \
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

    RIPEMD160(
        prev_A, prev_B, prev_C, prev_D, prev_E,
        state[0], state[1], state[2], state[3], state[4],
        block,
        block[0x0], block[0x1], block[0x2], block[0x3],
        block[0x4], block[0x5], block[0x6], block[0x7],
        block[0x8], block[0x9], block[0xA], block[0xB],
        block[0xC], block[0xD], block[0xE], block[0xF]
    );

    state[0] += prev_B;
    state[1] += prev_C;
    state[2] += prev_D;
    state[3] += prev_E;
    state[4] += prev_A;
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

    RIPEMD160(
        prev_A, prev_B, prev_C, prev_D, prev_E,
        state[0], state[1], state[2], state[3], state[4],
        buffer,
        buffer[0], buffer[1], buffer[2], buffer[3], buffer[4],
        0x80000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x000002A0, 0x00000000
    );

    state[0] += prev_B;
    state[1] += prev_C;
    state[2] += prev_D;
    state[3] += prev_E;
    state[4] += prev_A;
}

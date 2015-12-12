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

#define HASH_NAME "SHA256"

#define HASH_IBLOCK_WORDS 16
#define HASH_OBLOCK_WORDS  8

#define HASH_ML_WORDS 2

#define SHA256_INIT_A  0x6a09e667
#define SHA256_INIT_B  0xbb67ae85
#define SHA256_INIT_C  0x3c6ef372
#define SHA256_INIT_D  0xa54ff53a
#define SHA256_INIT_E  0x510e527f
#define SHA256_INIT_F  0x9b05688c
#define SHA256_INIT_G  0x1f83d9ab
#define SHA256_INIT_H  0x5be0cd19

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
    block[0x4], \
    block[0x5], \
    block[0x6], \
    block[0x7]

#define HASH_INITSTATE_UNROLL \
    SHA256_INIT_A, \
    SHA256_INIT_B, \
    SHA256_INIT_C, \
    SHA256_INIT_D, \
    SHA256_INIT_E, \
    SHA256_INIT_F, \
    SHA256_INIT_G, \
    SHA256_INIT_H

#define ROR(x, n) rotate((uint)(x), 32 - (uint)(n))

#define SHA256_F0(x, y, z) SHA_F0(x, y, z)
#define SHA256_F1(x, y, z) SHA_F1(x, y, z)

#define SHA256_STEP(a, b, c, d, e, f, g, h, k, x) \
do { \
    h += ROR(e, 6) ^ ROR(e, 11) ^ ROR(e, 25); \
    h += SHA256_F0(e, f, g); \
    h += k; \
    h += x; \
    d += h; \
    h += ROR(a, 2) ^ ROR(a, 13) ^ ROR(a, 22); \
    h += SHA256_F1(a, b, c); \
} while(0)

#define SHA256_NEXT_WORDS(buffer, W0, W1, W2, W3, W4, W5, W6, W7, W8, W9, WA, WB, WC, WD, WE, WF) \
do { \
    buffer[0x0] = W0 + (ROR(W1,          7) ^ ROR(W1,          18) ^ (W1          >> 3)) + W9          + (ROR(WE,          17) ^ ROR(WE,          19) ^ (WE          >> 10)); \
    buffer[0x1] = W1 + (ROR(W2,          7) ^ ROR(W2,          18) ^ (W2          >> 3)) + WA          + (ROR(WF,          17) ^ ROR(WF,          19) ^ (WF          >> 10)); \
    buffer[0x2] = W2 + (ROR(W3,          7) ^ ROR(W3,          18) ^ (W3          >> 3)) + WB          + (ROR(buffer[0x0], 17) ^ ROR(buffer[0x0], 19) ^ (buffer[0x0] >> 10)); \
    buffer[0x3] = W3 + (ROR(W4,          7) ^ ROR(W4,          18) ^ (W4          >> 3)) + WC          + (ROR(buffer[0x1], 17) ^ ROR(buffer[0x1], 19) ^ (buffer[0x1] >> 10)); \
    buffer[0x4] = W4 + (ROR(W5,          7) ^ ROR(W5,          18) ^ (W5          >> 3)) + WD          + (ROR(buffer[0x2], 17) ^ ROR(buffer[0x2], 19) ^ (buffer[0x2] >> 10)); \
    buffer[0x5] = W5 + (ROR(W6,          7) ^ ROR(W6,          18) ^ (W6          >> 3)) + WE          + (ROR(buffer[0x3], 17) ^ ROR(buffer[0x3], 19) ^ (buffer[0x3] >> 10)); \
    buffer[0x6] = W6 + (ROR(W7,          7) ^ ROR(W7,          18) ^ (W7          >> 3)) + WF          + (ROR(buffer[0x4], 17) ^ ROR(buffer[0x4], 19) ^ (buffer[0x4] >> 10)); \
    buffer[0x7] = W7 + (ROR(W8,          7) ^ ROR(W8,          18) ^ (W8          >> 3)) + buffer[0x0] + (ROR(buffer[0x5], 17) ^ ROR(buffer[0x5], 19) ^ (buffer[0x5] >> 10)); \
    buffer[0x8] = W8 + (ROR(W9,          7) ^ ROR(W9,          18) ^ (W9          >> 3)) + buffer[0x1] + (ROR(buffer[0x6], 17) ^ ROR(buffer[0x6], 19) ^ (buffer[0x6] >> 10)); \
    buffer[0x9] = W9 + (ROR(WA,          7) ^ ROR(WA,          18) ^ (WA          >> 3)) + buffer[0x2] + (ROR(buffer[0x7], 17) ^ ROR(buffer[0x7], 19) ^ (buffer[0x7] >> 10)); \
    buffer[0xA] = WA + (ROR(WB,          7) ^ ROR(WB,          18) ^ (WB          >> 3)) + buffer[0x3] + (ROR(buffer[0x8], 17) ^ ROR(buffer[0x8], 19) ^ (buffer[0x8] >> 10)); \
    buffer[0xB] = WB + (ROR(WC,          7) ^ ROR(WC,          18) ^ (WC          >> 3)) + buffer[0x4] + (ROR(buffer[0x9], 17) ^ ROR(buffer[0x9], 19) ^ (buffer[0x9] >> 10)); \
    buffer[0xC] = WC + (ROR(WD,          7) ^ ROR(WD,          18) ^ (WD          >> 3)) + buffer[0x5] + (ROR(buffer[0xA], 17) ^ ROR(buffer[0xA], 19) ^ (buffer[0xA] >> 10)); \
    buffer[0xD] = WD + (ROR(WE,          7) ^ ROR(WE,          18) ^ (WE          >> 3)) + buffer[0x6] + (ROR(buffer[0xB], 17) ^ ROR(buffer[0xB], 19) ^ (buffer[0xB] >> 10)); \
    buffer[0xE] = WE + (ROR(WF,          7) ^ ROR(WF,          18) ^ (WF          >> 3)) + buffer[0x7] + (ROR(buffer[0xC], 17) ^ ROR(buffer[0xC], 19) ^ (buffer[0xC] >> 10)); \
    buffer[0xF] = WF + (ROR(buffer[0x0], 7) ^ ROR(buffer[0x0], 18) ^ (buffer[0x0] >> 3)) + buffer[0x8] + (ROR(buffer[0xD], 17) ^ ROR(buffer[0xD], 19) ^ (buffer[0xD] >> 10)); \
} while(0)

#define SHA256_K00 0x428a2f98
#define SHA256_K01 0x71374491
#define SHA256_K02 0xb5c0fbcf
#define SHA256_K03 0xe9b5dba5
#define SHA256_K04 0x3956c25b
#define SHA256_K05 0x59f111f1
#define SHA256_K06 0x923f82a4
#define SHA256_K07 0xab1c5ed5
#define SHA256_K08 0xd807aa98
#define SHA256_K09 0x12835b01
#define SHA256_K0A 0x243185be
#define SHA256_K0B 0x550c7dc3
#define SHA256_K0C 0x72be5d74
#define SHA256_K0D 0x80deb1fe
#define SHA256_K0E 0x9bdc06a7
#define SHA256_K0F 0xc19bf174
#define SHA256_K10 0xe49b69c1
#define SHA256_K11 0xefbe4786
#define SHA256_K12 0x0fc19dc6
#define SHA256_K13 0x240ca1cc
#define SHA256_K14 0x2de92c6f
#define SHA256_K15 0x4a7484aa
#define SHA256_K16 0x5cb0a9dc
#define SHA256_K17 0x76f988da
#define SHA256_K18 0x983e5152
#define SHA256_K19 0xa831c66d
#define SHA256_K1A 0xb00327c8
#define SHA256_K1B 0xbf597fc7
#define SHA256_K1C 0xc6e00bf3
#define SHA256_K1D 0xd5a79147
#define SHA256_K1E 0x06ca6351
#define SHA256_K1F 0x14292967
#define SHA256_K20 0x27b70a85
#define SHA256_K21 0x2e1b2138
#define SHA256_K22 0x4d2c6dfc
#define SHA256_K23 0x53380d13
#define SHA256_K24 0x650a7354
#define SHA256_K25 0x766a0abb
#define SHA256_K26 0x81c2c92e
#define SHA256_K27 0x92722c85
#define SHA256_K28 0xa2bfe8a1
#define SHA256_K29 0xa81a664b
#define SHA256_K2A 0xc24b8b70
#define SHA256_K2B 0xc76c51a3
#define SHA256_K2C 0xd192e819
#define SHA256_K2D 0xd6990624
#define SHA256_K2E 0xf40e3585
#define SHA256_K2F 0x106aa070
#define SHA256_K30 0x19a4c116
#define SHA256_K31 0x1e376c08
#define SHA256_K32 0x2748774c
#define SHA256_K33 0x34b0bcb5
#define SHA256_K34 0x391c0cb3
#define SHA256_K35 0x4ed8aa4a
#define SHA256_K36 0x5b9cca4f
#define SHA256_K37 0x682e6ff3
#define SHA256_K38 0x748f82ee
#define SHA256_K39 0x78a5636f
#define SHA256_K3A 0x84c87814
#define SHA256_K3B 0x8cc70208
#define SHA256_K3C 0x90befffa
#define SHA256_K3D 0xa4506ceb
#define SHA256_K3E 0xbef9a3f7
#define SHA256_K3F 0xc67178f2

#define SHA256(A, B, C, D, E, F, G, H, buffer, W0, W1, W2, W3, W4, W5, W6, W7, W8, W9, WA, WB, WC, WD, WE, WF) \
do { \
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K00, W0); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K01, W1); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K02, W2); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K03, W3); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K04, W4); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K05, W5); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K06, W6); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K07, W7); \
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K08, W8); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K09, W9); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K0A, WA); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K0B, WB); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K0C, WC); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K0D, WD); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K0E, WE); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K0F, WF); \
\
    SHA256_NEXT_WORDS(buffer, \
        W0, W1, W2, W3, W4, W5, W6, W7, \
        W8, W9, WA, WB, WC, WD, WE, WF); \
\
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K10, buffer[0x0]); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K11, buffer[0x1]); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K12, buffer[0x2]); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K13, buffer[0x3]); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K14, buffer[0x4]); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K15, buffer[0x5]); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K16, buffer[0x6]); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K17, buffer[0x7]); \
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K18, buffer[0x8]); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K19, buffer[0x9]); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K1A, buffer[0xA]); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K1B, buffer[0xB]); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K1C, buffer[0xC]); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K1D, buffer[0xD]); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K1E, buffer[0xE]); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K1F, buffer[0xF]); \
\
    SHA256_NEXT_WORDS(buffer, \
        buffer[0x0], buffer[0x1], buffer[0x2], buffer[0x3], \
        buffer[0x4], buffer[0x5], buffer[0x6], buffer[0x7], \
        buffer[0x8], buffer[0x9], buffer[0xA], buffer[0xB], \
        buffer[0xC], buffer[0xD], buffer[0xE], buffer[0xF]); \
\
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K20, buffer[0x0]); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K21, buffer[0x1]); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K22, buffer[0x2]); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K23, buffer[0x3]); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K24, buffer[0x4]); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K25, buffer[0x5]); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K26, buffer[0x6]); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K27, buffer[0x7]); \
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K28, buffer[0x8]); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K29, buffer[0x9]); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K2A, buffer[0xA]); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K2B, buffer[0xB]); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K2C, buffer[0xC]); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K2D, buffer[0xD]); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K2E, buffer[0xE]); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K2F, buffer[0xF]); \
\
    SHA256_NEXT_WORDS(buffer, \
        buffer[0x0], buffer[0x1], buffer[0x2], buffer[0x3], \
        buffer[0x4], buffer[0x5], buffer[0x6], buffer[0x7], \
        buffer[0x8], buffer[0x9], buffer[0xA], buffer[0xB], \
        buffer[0xC], buffer[0xD], buffer[0xE], buffer[0xF]); \
\
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K30, buffer[0x0]); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K31, buffer[0x1]); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K32, buffer[0x2]); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K33, buffer[0x3]); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K34, buffer[0x4]); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K35, buffer[0x5]); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K36, buffer[0x6]); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K37, buffer[0x7]); \
    SHA256_STEP(A, B, C, D, E, F, G, H, SHA256_K38, buffer[0x8]); \
    SHA256_STEP(H, A, B, C, D, E, F, G, SHA256_K39, buffer[0x9]); \
    SHA256_STEP(G, H, A, B, C, D, E, F, SHA256_K3A, buffer[0xA]); \
    SHA256_STEP(F, G, H, A, B, C, D, E, SHA256_K3B, buffer[0xB]); \
    SHA256_STEP(E, F, G, H, A, B, C, D, SHA256_K3C, buffer[0xC]); \
    SHA256_STEP(D, E, F, G, H, A, B, C, SHA256_K3D, buffer[0xD]); \
    SHA256_STEP(C, D, E, F, G, H, A, B, SHA256_K3E, buffer[0xE]); \
    SHA256_STEP(B, C, D, E, F, G, H, A, SHA256_K3F, buffer[0xF]); \
} while(0)

inline void hash_update_block(
    uint prev_A, uint prev_B, uint prev_C, uint prev_D,
    uint prev_E, uint prev_F, uint prev_G, uint prev_H,
    __private uint *block, __private uint *state)
{
    state[0] = prev_A;
    state[1] = prev_B;
    state[2] = prev_C;
    state[3] = prev_D;
    state[4] = prev_E;
    state[5] = prev_F;
    state[6] = prev_G;
    state[7] = prev_H;

    SHA256(
        state[0], state[1], state[2], state[3],
        state[4], state[5], state[6], state[7],
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
    state[5] += prev_F;
    state[6] += prev_G;
    state[7] += prev_H;
}

inline void hash_digest_digest(
    uint prev_A, uint prev_B, uint prev_C, uint prev_D,
    uint prev_E, uint prev_F, uint prev_G, uint prev_H,
    __private uint *buffer, __private uint *state)
{
    state[0] = prev_A;
    state[1] = prev_B;
    state[2] = prev_C;
    state[3] = prev_D;
    state[4] = prev_E;
    state[5] = prev_F;
    state[6] = prev_G;
    state[7] = prev_H;

    SHA256(
        state[0], state[1], state[2], state[3],
        state[4], state[5], state[6], state[7],
        buffer,
        buffer[0], buffer[1], buffer[2], buffer[3],
        buffer[4], buffer[5], buffer[6], buffer[7],
        0x80000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000300
    );

    state[0] += prev_A;
    state[1] += prev_B;
    state[2] += prev_C;
    state[3] += prev_D;
    state[4] += prev_E;
    state[5] += prev_F;
    state[6] += prev_G;
    state[7] += prev_H;
}

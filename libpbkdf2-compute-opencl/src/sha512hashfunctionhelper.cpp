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

#include "sha512hashfunctionhelper.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

static const char * const INIT_STATE[] = {
    "(ulong)0x6a09e667f3bcc908",
    "(ulong)0xbb67ae8584caa73b",
    "(ulong)0x3c6ef372fe94f82b",
    "(ulong)0xa54ff53a5f1d36f1",
    "(ulong)0x510e527fade682d1",
    "(ulong)0x9b05688c2b3e6c1f",
    "(ulong)0x1f83d9abfb41bd6b",
    "(ulong)0x5be0cd19137e2179",
};

static const char * const KS[] = {
    "(ulong)0x428a2f98d728ae22", "(ulong)0x7137449123ef65cd",
    "(ulong)0xb5c0fbcfec4d3b2f", "(ulong)0xe9b5dba58189dbbc",
    "(ulong)0x3956c25bf348b538", "(ulong)0x59f111f1b605d019",
    "(ulong)0x923f82a4af194f9b", "(ulong)0xab1c5ed5da6d8118",
    "(ulong)0xd807aa98a3030242", "(ulong)0x12835b0145706fbe",
    "(ulong)0x243185be4ee4b28c", "(ulong)0x550c7dc3d5ffb4e2",
    "(ulong)0x72be5d74f27b896f", "(ulong)0x80deb1fe3b1696b1",
    "(ulong)0x9bdc06a725c71235", "(ulong)0xc19bf174cf692694",
    "(ulong)0xe49b69c19ef14ad2", "(ulong)0xefbe4786384f25e3",
    "(ulong)0x0fc19dc68b8cd5b5", "(ulong)0x240ca1cc77ac9c65",
    "(ulong)0x2de92c6f592b0275", "(ulong)0x4a7484aa6ea6e483",
    "(ulong)0x5cb0a9dcbd41fbd4", "(ulong)0x76f988da831153b5",
    "(ulong)0x983e5152ee66dfab", "(ulong)0xa831c66d2db43210",
    "(ulong)0xb00327c898fb213f", "(ulong)0xbf597fc7beef0ee4",
    "(ulong)0xc6e00bf33da88fc2", "(ulong)0xd5a79147930aa725",
    "(ulong)0x06ca6351e003826f", "(ulong)0x142929670a0e6e70",
    "(ulong)0x27b70a8546d22ffc", "(ulong)0x2e1b21385c26c926",
    "(ulong)0x4d2c6dfc5ac42aed", "(ulong)0x53380d139d95b3df",
    "(ulong)0x650a73548baf63de", "(ulong)0x766a0abb3c77b2a8",
    "(ulong)0x81c2c92e47edaee6", "(ulong)0x92722c851482353b",
    "(ulong)0xa2bfe8a14cf10364", "(ulong)0xa81a664bbc423001",
    "(ulong)0xc24b8b70d0f89791", "(ulong)0xc76c51a30654be30",
    "(ulong)0xd192e819d6ef5218", "(ulong)0xd69906245565a910",
    "(ulong)0xf40e35855771202a", "(ulong)0x106aa07032bbd1b8",
    "(ulong)0x19a4c116b8d2d0c8", "(ulong)0x1e376c085141ab53",
    "(ulong)0x2748774cdf8eeb99", "(ulong)0x34b0bcb5e19b48a8",
    "(ulong)0x391c0cb3c5c95a63", "(ulong)0x4ed8aa4ae3418acb",
    "(ulong)0x5b9cca4f7763e373", "(ulong)0x682e6ff3d6b2b8a3",
    "(ulong)0x748f82ee5defb2fc", "(ulong)0x78a5636f43172f60",
    "(ulong)0x84c87814a1f0ab72", "(ulong)0x8cc702081a6439ec",
    "(ulong)0x90befffa23631e28", "(ulong)0xa4506cebde82bde9",
    "(ulong)0xbef9a3f7b2c67915", "(ulong)0xc67178f2e372532b",
    "(ulong)0xca273eceea26619c", "(ulong)0xd186b8c721c0c207",
    "(ulong)0xeada7dd6cde0eb1e", "(ulong)0xf57d4f7fee6ed178",
    "(ulong)0x06f067aa72176fba", "(ulong)0x0a637dc5a2c898a6",
    "(ulong)0x113f9804bef90dae", "(ulong)0x1b710b35131c471b",
    "(ulong)0x28db77f523047d84", "(ulong)0x32caab7b40c72493",
    "(ulong)0x3c9ebe0a15c9bebc", "(ulong)0x431d67c49c100d4c",
    "(ulong)0x4cc5d4becb3e42b6", "(ulong)0x597f299cfc657e2a",
    "(ulong)0x5fcb6fab3ad6faec", "(ulong)0x6c44198c4a475817",
};

static const Shifts SHIFTS {
    28, 34, 39,
    14, 18, 41,
     1,  8,  7,
    19, 61,  6,
};

const Sha512HashFunctionHelper Sha512HashFunctionHelper::INSTANCE;

Sha512HashFunctionHelper::Sha512HashFunctionHelper()
    : Sha2HashFunctionHelper("ulong", 8, INIT_STATE, 80, KS, SHIFTS)
{
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2


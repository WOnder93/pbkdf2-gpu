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

#include "sha256hashfunctionhelper.h"

#define IBLOCK_WORDS 16
#define OBLOCK_WORDS 8
#define ML_WORDS 2

#define ITERATIONS 64

namespace libpbkdf2 {
namespace compute {
namespace opencl {

static const char * const INIT_STATE[] = {
    "0x6A09E667",
    "0xBB67AE85",
    "0x3C6EF372",
    "0xA54FF53A",
    "0x510E527F",
    "0x9B05688C",
    "0x1F83D9AB",
    "0x5BE0CD19",
};

static const char * const KS[] = {
    "0x428a2f98", "0x71374491", "0xb5c0fbcf", "0xe9b5dba5",
    "0x3956c25b", "0x59f111f1", "0x923f82a4", "0xab1c5ed5",
    "0xd807aa98", "0x12835b01", "0x243185be", "0x550c7dc3",
    "0x72be5d74", "0x80deb1fe", "0x9bdc06a7", "0xc19bf174",
    "0xe49b69c1", "0xefbe4786", "0x0fc19dc6", "0x240ca1cc",
    "0x2de92c6f", "0x4a7484aa", "0x5cb0a9dc", "0x76f988da",
    "0x983e5152", "0xa831c66d", "0xb00327c8", "0xbf597fc7",
    "0xc6e00bf3", "0xd5a79147", "0x06ca6351", "0x14292967",
    "0x27b70a85", "0x2e1b2138", "0x4d2c6dfc", "0x53380d13",
    "0x650a7354", "0x766a0abb", "0x81c2c92e", "0x92722c85",
    "0xa2bfe8a1", "0xa81a664b", "0xc24b8b70", "0xc76c51a3",
    "0xd192e819", "0xd6990624", "0xf40e3585", "0x106aa070",
    "0x19a4c116", "0x1e376c08", "0x2748774c", "0x34b0bcb5",
    "0x391c0cb3", "0x4ed8aa4a", "0x5b9cca4f", "0x682e6ff3",
    "0x748f82ee", "0x78a5636f", "0x84c87814", "0x8cc70208",
    "0x90befffa", "0xa4506ceb", "0xbef9a3f7", "0xc67178f2",
};

const Sha256HashFunctionHelper Sha256HashFunctionHelper::INSTANCE;

Sha256HashFunctionHelper::Sha256HashFunctionHelper()
    : UIntHashFunctionHelper(
          false, IBLOCK_WORDS, OBLOCK_WORDS, ML_WORDS, INIT_STATE)
{
}

void Sha256HashFunctionHelper::writeDefinitions(OpenCLWriter &out) const
{
    out << "#ifdef cl_nv_pragma_unroll" << std::endl;
    out << "#define NVIDIA" << std::endl;
    out << "#endif /* cl_nv_pragma_unroll */" << std::endl;
    out << std::endl;
    out << "#ifndef NVIDIA" << std::endl;
    out << "#define SHA_F0(x,y,z) bitselect(z, y, x)" << std::endl;
    out << "#else" << std::endl;
    out << "#define SHA_F0(x,y,z) (z ^ (x & (y ^ z)))" << std::endl;
    out << "#endif /* NVIDIA */" << std::endl;
    out << std::endl;
    out << "#define SHA_F1(x,y,z) bitselect(y, x, y ^ z)" << std::endl;
    out << std::endl;
}

void Sha256HashFunctionHelper::writeUpdate(
        OpenCLWriter &writer,
        const std::vector<std::string> &prevState,
        const std::vector<std::string> &state,
        const std::vector<std::string> &inputBlock,
        bool swap) const
{
    for (std::size_t i = 0; i < OBLOCK_WORDS; i++) {
        writer.beginAssignment(state[i]);
        writer << prevState[i];
        writer.endAssignment();
    }
    writer.writeEmptyLine();

    const std::vector<std::string> &dest = swap ? prevState : state;
    const std::vector<std::string> &aux  = swap ? state : prevState;

    for (std::size_t iter = 0; iter < ITERATIONS; iter++) {
        std::size_t state_a = (ITERATIONS - iter) % OBLOCK_WORDS;

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + ("
               << "rotate(" << dest[(state_a + 4) % OBLOCK_WORDS]
               << ", 32 - (uint)6) ^ "
               << "rotate(" << dest[(state_a + 4) % OBLOCK_WORDS]
               << ", 32 - (uint)11) ^ "
               << "rotate(" << dest[(state_a + 4) % OBLOCK_WORDS]
               << ", 32 - (uint)25))";
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + "
               << "SHA_F0(" << dest[(state_a + 4) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 5) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 6) % OBLOCK_WORDS] << ")";
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + " << KS[iter];
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + "
               << inputBlock[iter % IBLOCK_WORDS];
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 3) % OBLOCK_WORDS]);
        writer << dest[(state_a + 3) % OBLOCK_WORDS] << " + "
               << dest[(state_a + 7) % OBLOCK_WORDS];
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + ("
               << "rotate(" << dest[(state_a + 0) % OBLOCK_WORDS]
               << ", 32 - (uint)2) ^ "
               << "rotate(" << dest[(state_a + 0) % OBLOCK_WORDS]
               << ", 32 - (uint)13) ^ "
               << "rotate(" << dest[(state_a + 0) % OBLOCK_WORDS]
               << ", 32 - (uint)22))";
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + "
               << "SHA_F1(" << dest[(state_a + 0) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 1) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 2) % OBLOCK_WORDS] << ")";
        writer.endAssignment();

        if (iter % IBLOCK_WORDS == IBLOCK_WORDS - 1) {
            writer.writeEmptyLine();
            for (std::size_t i = 0; i < IBLOCK_WORDS; i++) {
                writer.beginAssignment(inputBlock[i]);
                writer << inputBlock[i] << " + ("
                       << "rotate("
                       << inputBlock[(i +  1) % IBLOCK_WORDS]
                       << ", 32 - (uint)7) ^ "
                       << "rotate("
                       << inputBlock[(i +  1) % IBLOCK_WORDS]
                       << ", 32 - (uint)18) ^ "
                       << "(" << inputBlock[(i +  1) % IBLOCK_WORDS]
                       << " >> 3))";
                writer.endAssignment();

                writer.beginAssignment(inputBlock[i]);
                writer << inputBlock[i] << " + "
                       << inputBlock[(i +  9) % IBLOCK_WORDS] << " + ("
                       << "rotate("
                       << inputBlock[(i +  14) % IBLOCK_WORDS]
                       << ", 32 - (uint)17) ^ "
                       << "rotate("
                       << inputBlock[(i +  14) % IBLOCK_WORDS]
                       << ", 32 - (uint)19) ^ "
                       << "(" << inputBlock[(i +  14) % IBLOCK_WORDS]
                       << " >> 10))";
                writer.endAssignment();

            }
        }
        writer.writeEmptyLine();
    }

    for (std::size_t i = 0; i < OBLOCK_WORDS; i++) {
        writer.beginAssignment(dest[i]);
        writer << dest[i] << " + " << aux[i];
        writer.endAssignment();
    }
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2


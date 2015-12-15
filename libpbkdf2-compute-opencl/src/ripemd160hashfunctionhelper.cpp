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

#include "ripemd160hashfunctionhelper.h"

#define IBLOCK_WORDS 16
#define OBLOCK_WORDS 5
#define ML_WORDS 2

#define ITERATIONS 160

namespace libpbkdf2 {
namespace compute {
namespace opencl {

static const char * const INIT_STATE[] = {
    "0x67452301",
    "0xEFCDAB89",
    "0x98BADCFE",
    "0x10325476",
    "0xC3D2E1F0",
};

static const char * const FS[] = {
    "RIPEMD160_F0",
    "RIPEMD160_F1",
    "RIPEMD160_F2",
    "RIPEMD160_F3",
    "RIPEMD160_F4",
};

static const char * const KS[] = {
    "0x00000000",
    "0x5A827999",
    "0x6ED9EBA1",
    "0x8F1BBCDC",
    "0xA953FD4E",
    "0x50A28BE6",
    "0x5C4DD124",
    "0x6D703EF3",
    "0x7A6D76E9",
    "0x00000000",
};

static const std::size_t INDICES[] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xA,
    0xB,
    0xC,
    0xD,
    0xE,
    0xF,
    0x7,
    0x4,
    0xD,
    0x1,
    0xA,
    0x6,
    0xF,
    0x3,
    0xC,
    0x0,
    0x9,
    0x5,
    0x2,
    0xE,
    0xB,
    0x8,
    0x3,
    0xA,
    0xE,
    0x4,
    0x9,
    0xF,
    0x8,
    0x1,
    0x2,
    0x7,
    0x0,
    0x6,
    0xD,
    0xB,
    0x5,
    0xC,
    0x1,
    0x9,
    0xB,
    0xA,
    0x0,
    0x8,
    0xC,
    0x4,
    0xD,
    0x3,
    0x7,
    0xF,
    0xE,
    0x5,
    0x6,
    0x2,
    0x4,
    0x0,
    0x5,
    0x9,
    0x7,
    0xC,
    0x2,
    0xA,
    0xE,
    0x1,
    0x3,
    0x8,
    0xB,
    0x6,
    0xF,
    0xD,
    0x5,
    0xE,
    0x7,
    0x0,
    0x9,
    0x2,
    0xB,
    0x4,
    0xD,
    0x6,
    0xF,
    0x8,
    0x1,
    0xA,
    0x3,
    0xC,
    0x6,
    0xB,
    0x3,
    0x7,
    0x0,
    0xD,
    0x5,
    0xA,
    0xE,
    0xF,
    0x8,
    0xC,
    0x4,
    0x9,
    0x1,
    0x2,
    0xF,
    0x5,
    0x1,
    0x3,
    0x7,
    0xE,
    0x6,
    0x9,
    0xB,
    0x8,
    0xC,
    0x2,
    0xA,
    0x0,
    0x4,
    0xD,
    0x8,
    0x6,
    0x4,
    0x1,
    0x3,
    0xB,
    0xF,
    0x0,
    0x5,
    0xC,
    0x2,
    0xD,
    0x9,
    0x7,
    0xA,
    0xE,
    0xC,
    0xF,
    0xA,
    0x4,
    0x1,
    0x5,
    0x8,
    0x7,
    0x6,
    0x2,
    0xD,
    0xE,
    0x0,
    0x3,
    0x9,
    0xB,
};

static const char * const SHIFTS[] = {
    "11",
    "14",
    "15",
    "12",
    " 5",
    " 8",
    " 7",
    " 9",
    "11",
    "13",
    "14",
    "15",
    " 6",
    " 7",
    " 9",
    " 8",
    " 7",
    " 6",
    " 8",
    "13",
    "11",
    " 9",
    " 7",
    "15",
    " 7",
    "12",
    "15",
    " 9",
    "11",
    " 7",
    "13",
    "12",
    "11",
    "13",
    " 6",
    " 7",
    "14",
    " 9",
    "13",
    "15",
    "14",
    " 8",
    "13",
    " 6",
    " 5",
    "12",
    " 7",
    " 5",
    "11",
    "12",
    "14",
    "15",
    "14",
    "15",
    " 9",
    " 8",
    " 9",
    "14",
    " 5",
    " 6",
    " 8",
    " 6",
    " 5",
    "12",
    " 9",
    "15",
    " 5",
    "11",
    " 6",
    " 8",
    "13",
    "12",
    " 5",
    "12",
    "13",
    "14",
    "11",
    " 8",
    " 5",
    " 6",
    " 8",
    " 9",
    " 9",
    "11",
    "13",
    "15",
    "15",
    " 5",
    " 7",
    " 7",
    " 8",
    "11",
    "14",
    "14",
    "12",
    " 6",
    " 9",
    "13",
    "15",
    " 7",
    "12",
    " 8",
    " 9",
    "11",
    " 7",
    " 7",
    "12",
    " 7",
    " 6",
    "15",
    "13",
    "11",
    " 9",
    " 7",
    "15",
    "11",
    " 8",
    " 6",
    " 6",
    "14",
    "12",
    "13",
    " 5",
    "14",
    "13",
    "13",
    " 7",
    " 5",
    "15",
    " 5",
    " 8",
    "11",
    "14",
    "14",
    " 6",
    "14",
    " 6",
    " 9",
    "12",
    " 9",
    "12",
    " 5",
    "15",
    " 8",
    " 8",
    " 5",
    "12",
    " 9",
    "12",
    " 5",
    "14",
    " 6",
    " 8",
    "13",
    " 6",
    " 5",
    "15",
    "13",
    "11",
    "11",
};

const RipeMd160HashFunctionHelper RipeMd160HashFunctionHelper::INSTANCE;

RipeMd160HashFunctionHelper::RipeMd160HashFunctionHelper()
    : HashFunctionHelper(
          true, "uint", 4, IBLOCK_WORDS, OBLOCK_WORDS, ML_WORDS, INIT_STATE)
{
}

void RipeMd160HashFunctionHelper::writeDefinitions(OpenCLWriter &out) const
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
    out << "#define RIPEMD160_F0(x, y, z) (x ^ y ^ z)" << std::endl;
    out << "#define RIPEMD160_F1(x, y, z) SHA_F0(x, y, z)" << std::endl;
    out << "#define RIPEMD160_F2(x, y, z) ((x | ~y) ^ z)" << std::endl;
    out << "#define RIPEMD160_F3(x, y, z) RIPEMD160_F1(z, x, y)" << std::endl;
    out << "#define RIPEMD160_F4(x, y, z) RIPEMD160_F2(y, z, x)" << std::endl;
    out << std::endl;
}

void RipeMd160HashFunctionHelper::writeUpdate(
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

    writer.beginBlock();

    std::vector<std::string> aux2 {
        "ripemd160_aux_0",
        "ripemd160_aux_1",
        "ripemd160_aux_2",
        "ripemd160_aux_3",
        "ripemd160_aux_4",
    };

    for (std::size_t i = 0; i < OBLOCK_WORDS; i++) {
        writer.writeDeclaration("uint", aux2[i]);
    }
    writer.writeEmptyLine();

    for (std::size_t i = 0; i < OBLOCK_WORDS; i++) {
        writer.beginAssignment(aux2[i]);
        writer << prevState[i];
        writer.endAssignment();
    }
    writer.writeEmptyLine();

    const std::vector<std::string> *current = &dest;
    for (std::size_t iter = 0; iter < ITERATIONS; iter++) {
        std::size_t state_a = (ITERATIONS - iter) % OBLOCK_WORDS;
        std::size_t deci = iter / (ITERATIONS / 10);
        const char *K = KS[deci];
        const char *F = FS[deci < 5 ? deci : 10 - deci - 1];

        writer.beginAssignment((*current)[(state_a + 0) % OBLOCK_WORDS]);
        writer << "rotate(" << (*current)[(state_a + 0) % OBLOCK_WORDS] << " + "
               << F << "(" << (*current)[(state_a + 1) % OBLOCK_WORDS]
               << ", " << (*current)[(state_a + 2) % OBLOCK_WORDS]
               << ", " << (*current)[(state_a + 3) % OBLOCK_WORDS]
               << ") + " << inputBlock[INDICES[iter]]
               << " + " << K
               << ", (uint)" << SHIFTS[iter]
               << ") + " << (*current)[(state_a + 4) % OBLOCK_WORDS];
        writer.endAssignment();

        writer.beginAssignment((*current)[(state_a + 2) % OBLOCK_WORDS]);
        writer << "rotate(" << (*current)[(state_a + 2) % OBLOCK_WORDS]
               << ", (uint)10)";
        writer.endAssignment();

        if (iter == ITERATIONS / 2 - 1) {
            current = &aux2;
        }
        writer.writeEmptyLine();
    }

    writer.beginAssignment(aux2[2]);
    writer << aux2[2] << " + " << dest[1];
    writer.endAssignment();

    writer.beginAssignment(dest[1]);
    writer << dest[3] << " + " << aux2[4];
    writer.endAssignment();

    writer.beginAssignment(dest[3]);
    writer << dest[0] << " + " << aux2[1];
    writer.endAssignment();

    writer.beginAssignment(dest[0]);
    writer << dest[2] << " + " << aux2[3];
    writer.endAssignment();

    writer.beginAssignment(dest[2]);
    writer << dest[4] << " + " << aux2[0];
    writer.endAssignment();

    writer.beginAssignment(dest[4]);
    writer << aux2[2];
    writer.endAssignment();
    writer.writeEmptyLine();

    for (std::size_t i = 0; i < OBLOCK_WORDS; i++) {
        writer.beginAssignment(dest[i]);
        writer << dest[i] << " + " << aux[(i + 1) % OBLOCK_WORDS];
        writer.endAssignment();
    }
    writer.endBlock();
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2


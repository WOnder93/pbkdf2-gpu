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

#include "sha2hashfunctionhelper.h"

#define IBLOCK_WORDS 16
#define OBLOCK_WORDS 8
#define ML_WORDS 2

namespace libpbkdf2 {
namespace compute {
namespace opencl {

static void writeRotateRight(
        OpenCLWriter &out,
        const std::string &operand, std::size_t n,
        const char *wordType, std::size_t wordBytes)
{
    out << "rotate(" << operand << ", "
        << 8 * wordBytes << " - (" << wordType << ")" << n << ")";
}

Sha2HashFunctionHelper::Sha2HashFunctionHelper(
        const char *wordClType, std::size_t wordBytes,
        const char * const *initState, std::size_t iterations,
        const char * const *roundConstants,
        const Shifts &shifts)
    : HashFunctionHelper(
          false, wordClType, wordBytes,
          IBLOCK_WORDS, OBLOCK_WORDS, ML_WORDS, initState),
      iterations(iterations), roundConstants(roundConstants), shifts(shifts)
{
}

void Sha2HashFunctionHelper::writeDefinitions(OpenCLWriter &out) const
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

void Sha2HashFunctionHelper::writeUpdate(
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

    for (std::size_t iter = 0; iter < iterations; iter++) {
        std::size_t state_a = (iterations - iter) % OBLOCK_WORDS;

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + (";
        writeRotateRight(writer, dest[(state_a + 4) % OBLOCK_WORDS],
                shifts.S1_rot1, getWordClType(), getWordBytes());
        writer << " ^ ";
        writeRotateRight(writer, dest[(state_a + 4) % OBLOCK_WORDS],
                shifts.S1_rot2, getWordClType(), getWordBytes());
        writer << " ^ ";
        writeRotateRight(writer, dest[(state_a + 4) % OBLOCK_WORDS],
                shifts.S1_rot3, getWordClType(), getWordBytes());
        writer << ")";
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + "
               << "SHA_F0(" << dest[(state_a + 4) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 5) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 6) % OBLOCK_WORDS] << ")";
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 7) % OBLOCK_WORDS]);
        writer << dest[(state_a + 7) % OBLOCK_WORDS]
               << " + " << roundConstants[iter];
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
        writer << dest[(state_a + 7) % OBLOCK_WORDS] << " + (";
        writeRotateRight(writer, dest[(state_a + 0) % OBLOCK_WORDS],
                shifts.S0_rot1, getWordClType(), getWordBytes());
        writer << " ^ ";
        writeRotateRight(writer, dest[(state_a + 0) % OBLOCK_WORDS],
                shifts.S0_rot2, getWordClType(), getWordBytes());
        writer << " ^ ";
        writeRotateRight(writer, dest[(state_a + 0) % OBLOCK_WORDS],
                shifts.S0_rot3, getWordClType(), getWordBytes());
        writer << ")";
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
                writer << inputBlock[i] << " + (";
                writeRotateRight(writer, inputBlock[(i +  1) % IBLOCK_WORDS],
                        shifts.s0_rot1, getWordClType(), getWordBytes());
                writer << " ^ ";
                writeRotateRight(writer, inputBlock[(i +  1) % IBLOCK_WORDS],
                        shifts.s0_rot2, getWordClType(), getWordBytes());
                writer << " ^ (" << inputBlock[(i +  1) % IBLOCK_WORDS]
                       << " >> " <<  shifts.s0_shift << "))";
                writer.endAssignment();

                writer.beginAssignment(inputBlock[i]);
                writer << inputBlock[i] << " + "
                       << inputBlock[(i +  9) % IBLOCK_WORDS] << " + (";
                writeRotateRight(writer, inputBlock[(i +  14) % IBLOCK_WORDS],
                        shifts.s1_rot1, getWordClType(), getWordBytes());
                writer << " ^ ";
                writeRotateRight(writer, inputBlock[(i +  14) % IBLOCK_WORDS],
                        shifts.s1_rot2, getWordClType(), getWordBytes());
                writer << " ^ (" << inputBlock[(i +  14) % IBLOCK_WORDS]
                       << " >> " <<  shifts.s1_shift << "))";
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


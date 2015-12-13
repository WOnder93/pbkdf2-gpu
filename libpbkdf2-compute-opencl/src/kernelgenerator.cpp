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

#include "kernelgenerator.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

static std::vector<std::string> declareArray(
        OpenCLWriter &writer,
        const char *type, const char *varPrefix,
        std::size_t size)
{
    std::vector<std::string> res;
    res.reserve(size);
    for (std::size_t i = 0; i < size; i++) {
        std::string var = varPrefix + std::to_string(i);

        writer.writeDeclaration(type, var);

        res.emplace_back(std::move(var));
    }
    writer.writeEmptyLine();
    return res;
}

static void writeUpdateWithDigest(
        OpenCLWriter &writer,
        const HashFunctionHelper &hfHelper,
        const std::vector<std::string> &prevState,
        const std::vector<std::string> &input,
        const std::vector<std::string> &extra,
        const std::vector<std::string> &dest)
{
    std::size_t hfIBlockWords = hfHelper.getInputBlockWords();
    std::size_t hfOBlockWords = hfHelper.getOutputBlockWords();
    std::size_t hfMlWords = hfHelper.getMessageLengthWords();
    std::size_t hfWordBytes = hfHelper.getWordBytes();
    bool hfLE = hfHelper.isLittleEndian();

    bool twoBlocks = hfIBlockWords - hfOBlockWords <= hfMlWords;

    for (std::size_t i = 0; i < hfIBlockWords - hfOBlockWords; i++) {
        writer.beginAssignment(extra[i]);
        if (i == 0) {
            if (hfLE) {
                writer << "(hash_word_t)0x80";
            } else {
                writer << "HASH_WORD_SWITCH_ENDIANNESS((hash_word_t)0x80)";
            }
        } else {
            writer << "0";
        }
        writer.endAssignment();
    }
    if (!twoBlocks) {
        writer.beginAssignment(extra[hfIBlockWords - hfOBlockWords - 1]);
        writer << "(hash_word_t)"
               << ((hfIBlockWords + hfOBlockWords) * hfWordBytes * 8);
        writer.endAssignment();
    }
    writer.writeEmptyLine();

    std::vector<std::string> buffer;
    buffer.reserve(hfIBlockWords);
    buffer.insert(buffer.end(), input.begin(),
                  input.begin() + hfOBlockWords);
    buffer.insert(buffer.end(), extra.begin(),
                  extra.begin() + hfIBlockWords - hfOBlockWords);

    hfHelper.writeUpdate(writer, prevState, buffer, dest, buffer);

    if (!twoBlocks) {
        return;
    }

    for (std::size_t i = 0; i < hfIBlockWords; i++) {
        writer.beginAssignment(extra[i]);
        if (i == 0 && hfOBlockWords == hfIBlockWords) {
            if (hfLE) {
                writer << "(hash_word_t)0x80";
            } else {
                writer << "HASH_WORD_SWITCH_ENDIANNESS((hash_word_t)0x80)";
            }
        } else {
            writer << "0";
        }
        writer.endAssignment();
    }
    writer.beginAssignment(extra[hfIBlockWords - 1]);
    writer << "(hash_word_t)"
           << ((hfIBlockWords + hfOBlockWords) * hfWordBytes * 8);
    writer.endAssignment();
    writer.writeEmptyLine();

    hfHelper.writeUpdate(writer, dest, extra, input, extra, true);
}

void KernelGenerator::generateKernel(std::ostream &out, std::size_t saltBytes,
                                     const HashFunctionHelper &hfHelper)
{
    bool hfLE = hfHelper.isLittleEndian();
    std::size_t hfWordBytes = (int)hfHelper.getWordBytes();
    std::size_t hfIBlockWords = hfHelper.getInputBlockWords();
    std::size_t hfOBlockWords = hfHelper.getOutputBlockWords();
    std::size_t hfIBlockBytes = hfIBlockWords * hfWordBytes;
    std::size_t hfMlWords = hfHelper.getMessageLengthWords();
    std::size_t hfMlBytes = hfMlWords * hfWordBytes;

    OpenCLWriter writer { out };
    hfHelper.writeDefinitions(writer);

    writer << "#ifndef __OPENCL_VERSION__" << std::endl;
    writer << "/* C compatibility For dumb IDEs: */" << std::endl;
    writer << "#ifndef __kernel" << std::endl;
    writer << "#define __kernel" << std::endl;
    writer << "#endif" << std::endl;
    writer << "#ifndef __global" << std::endl;
    writer << "#define __global" << std::endl;
    writer << "#endif" << std::endl;
    writer << "#ifndef __constant" << std::endl;
    writer << "#define __constant const" << std::endl;
    writer << "#endif" << std::endl;
    writer << "#endif /* __OPENCL_VERSION__ */" << std::endl;
    writer << std::endl;

    /* define the hash word type: */
    writer << "typedef " << hfHelper.getWordClType() << " hash_word_t;" << std::endl;
    writer << std::endl;

    /* define the endianness switch macro: */
    writer << "#define HASH_WORD_SWITCH_ENDIANNESS(w) (0";
    for (std::size_t i = 0; i < hfWordBytes; i++) {
        writer << " | ";

        int offset = hfWordBytes - 1 - 2 * (int)i;
        if (offset > 0) {
            writer << "((w & (0xFF << " << i * 8 << ")) << "
                   << offset * 8 << ")";
        } else if (offset < 0) {
            writer << "((w & (0xFF << " << i * 8 << ")) >> "
                   << -offset * 8 << ")";
        } else {
            writer << "(w & (0xFF << " << i * 8 << "))";
        }
    }
    writer << ")" << std::endl;
    writer << std::endl;

    /* define automatic endianness resolving macros: */
    writer << "#ifndef __ENDIAN_LITTLE__" << std::endl;
    if (hfLE) {
        writer << "#define HASH_WORD_FROM_DEV(w) HASH_WORD_SWITCH_ENDIANNESS(w)" << std::endl;
        writer << "#define HASH_WORD_TO_DEV(w)   HASH_WORD_SWITCH_ENDIANNESS(w)" << std::endl;
    } else {
        writer << "#define HASH_WORD_FROM_DEV(w) (w)" << std::endl;
        writer << "#define HASH_WORD_TO_DEV(w)   (w)" << std::endl;
    }
    writer << "#else" << std::endl;
    if (!hfLE) {
        writer << "#define HASH_WORD_FROM_DEV(w) HASH_WORD_SWITCH_ENDIANNESS(w)" << std::endl;
        writer << "#define HASH_WORD_TO_DEV(w)   HASH_WORD_SWITCH_ENDIANNESS(w)" << std::endl;
    } else {
        writer << "#define HASH_WORD_FROM_DEV(w) (w)" << std::endl;
        writer << "#define HASH_WORD_TO_DEV(w)   (w)" << std::endl;
    }
    writer << "#endif /* __ENDIAN_LITTLE__ */" << std::endl;
    writer << std::endl;

    writer << "/* Main kernel: */" << std::endl;
    writer << "__kernel" << std::endl;
    writer << "void pbkdf2_kernel(" << std::endl;
    writer << "    const __global hash_word_t *input," << std::endl;
    writer << "    __global hash_word_t *output," << std::endl;
    writer << "    __constant hash_word_t *salt," << std::endl;
    writer << "    uint dk_blocks," << std::endl;
    writer << "    uint iterations," << std::endl;
    writer << "    uint batchSize," << std::endl;
    writer << "    __global char *debug_buffer)" << std::endl;
    writer.beginBlock();

    writer.writeDeclaration("uint", "input_block_index");
    writer.writeDeclaration("uint", "input_pos");
    writer.beginAssignment("input_block_index");
    writer << "(uint)get_global_id(0)";
    writer.endAssignment();
    writer.beginAssignment("input_pos");
    writer << "input_block_index";
    writer.endAssignment();
    writer.writeEmptyLine();

    auto ipadState = declareArray(writer, "hash_word_t", "ipad_state_",
                                  hfOBlockWords);
    auto opadState = declareArray(writer, "hash_word_t", "opad_state_",
                                  hfOBlockWords);

    auto ipad = declareArray(writer, "hash_word_t", "ipad_",
                             hfIBlockWords);
    auto opad = declareArray(writer, "hash_word_t", "opad_",
                             hfIBlockWords);

    writer.writeDeclaration("hash_word_t", "in");
    writer.writeEmptyLine();

    for (std::size_t i = 0; i < hfIBlockWords; i++) {
        writer.beginAssignment("in");
        writer << "HASH_WORD_FROM_DEV(input[input_pos + "
               << i << " * batchSize])";
        writer.endAssignment();

        writer.beginAssignment(ipad[i]);
        writer << "in ^ 0x";
        for (std::size_t k = 0; k < hfWordBytes; k++) {
            writer << "36";
        }
        writer.endAssignment();

        writer.beginAssignment(opad[i]);
        writer << "in ^ 0x";
        for (std::size_t k = 0; k < hfWordBytes; k++) {
            writer << "5C";
        }
        writer.endAssignment();

        writer.writeEmptyLine();
    }

    hfHelper.writeUpdate(writer, hfHelper.getInitState(), ipad, ipadState, ipad);
    hfHelper.writeUpdate(writer, hfHelper.getInitState(), opad, opadState, opad);

    writer.writeDeclaration("uint", "dk_block_index");
    writer.beginAssignment("dk_block_index");
    writer << "(uint)get_global_id(1)";
    writer.endAssignment();
    writer.writeEmptyLine();

    auto state1 = declareArray(writer, "hash_word_t", "state1_",
                               hfOBlockWords);
    auto state2 = declareArray(writer, "hash_word_t", "state2_",
                               hfOBlockWords);
    auto buffer = declareArray(writer, "hash_word_t", "buffer_",
                               hfIBlockWords);
    for (std::size_t i = 0; i < hfOBlockWords; i++) {
        writer.beginAssignment(state1[i]);
        writer << ipadState[i];
        writer.endAssignment();
    }
    writer.writeEmptyLine();

    std::size_t saltBlocks = saltBytes / hfIBlockBytes;
    std::size_t saltRemainderBytes = saltBytes % hfIBlockBytes;
    std::size_t saltRemainderWords = saltRemainderBytes / hfWordBytes;

    writer.beginLoop("i", "0", std::to_string(saltBlocks));
    for (std::size_t i = 0; i < hfIBlockWords; i++) {
        writer.beginAssignment(buffer[i]);
        writer << "HASH_WORD_FROM_DEV(salt[i * " << hfIBlockWords
               << " + " << i << "])";
        writer.endAssignment();
    }
    writer.writeEmptyLine();

    hfHelper.writeUpdate(writer, state1, buffer, state2, buffer, true);
    writer.endBlock();
    writer.writeEmptyLine();

    writer.writeDeclaration("uint", "block_num");
    writer.beginAssignment("block_num");
    writer << "dk_block_index + 1";
    writer.endAssignment();
    writer.writeEmptyLine();

    std::size_t tailBlocks =
            saltRemainderBytes / (hfIBlockBytes - 4 - hfMlBytes) + 1;
    auto tail = declareArray(writer, "hash_word_t", "tail_",
                             hfIBlockWords * tailBlocks);

    std::size_t tailIndex = 0;
    while (tailIndex < saltRemainderWords) {
        writer.beginAssignment(tail[tailIndex]);
        writer << "HASH_WORD_FROM_DEV(salt["
               << (saltBlocks * hfIBlockWords + tailIndex)
               << "])";
        writer.endAssignment();
        tailIndex++;
    }
    std::size_t b = 0;
    if (saltRemainderBytes % hfWordBytes != 0) {
        writer.beginAssignment(tail[tailIndex]);
        writer << "HASH_WORD_FROM_DEV(salt["
               << (saltBlocks * hfIBlockWords + saltRemainderWords)
               << "])";
        writer.endAssignment();

        b += saltRemainderBytes % hfWordBytes;
    } else {
        writer.beginAssignment(tail[tailIndex]);
        writer << "0";
        writer.endAssignment();
    }

    for (std::size_t i = 0; i < 4; i++) {
        std::size_t shift = hfLE ? b : (hfWordBytes - b - 1);
        writer.beginAssignment(tail[tailIndex]);
        writer << tail[tailIndex]
               << " | ((hash_word_t)((block_num >> ("
               << (3 - i) << " * 8)) & 0xFF) << (" << shift << " * 8))";
        writer.endAssignment();

        if (++b == hfWordBytes) {
            tailIndex++;
            b = 0;

            writer.beginAssignment(tail[tailIndex]);
            writer << "0";
            writer.endAssignment();
        }
    }

    writer.beginAssignment(tail[tailIndex]);
    writer << tail[tailIndex] << " | (0x80 << (8 * "
           << (hfLE ? b : (hfWordBytes - b - 1)) << "))";
    writer.endAssignment();
    tailIndex++;

    while (tailIndex < tailBlocks * hfIBlockWords - 1) {
        writer.beginAssignment(tail[tailIndex]);
        writer << "0";
        writer.endAssignment();
        tailIndex++;
    }
    writer.beginAssignment(tail[tailIndex]);
    writer << "(hash_word_t)"
           << std::to_string((hfIBlockBytes + saltBytes + 4) * 8);
    writer.endAssignment();
    writer.writeEmptyLine();

    auto tailBlock = tail.begin();
    for (std::size_t i = 0; i < tailBlocks; i++) {
        std::vector<std::string> block { tailBlock, tailBlock + hfIBlockWords };

        hfHelper.writeUpdate(writer, state1, block, state2, block, true);

        tailBlock += hfIBlockWords;
    }
    writer.writeEmptyLine();

    auto dk = declareArray(writer, "hash_word_t", "dk_",
                                    hfOBlockWords);
    auto extraBuffer = declareArray(writer, "hash_word_t", "extra_",
                                    hfIBlockWords);

    writeUpdateWithDigest(writer, hfHelper, opadState, state1,
                          extraBuffer, state2);
    writer.writeEmptyLine();

    for (std::size_t i = 0; i < hfOBlockWords; i++) {
        writer.beginAssignment(dk[i]);
        writer << state2[i];
        writer.endAssignment();
    }
    writer.writeEmptyLine();

    writer.beginLoop("iteration", "1", "iterations");
    writeUpdateWithDigest(writer, hfHelper, ipadState, state2,
                          extraBuffer, state1);
    writer.writeEmptyLine();

    writeUpdateWithDigest(writer, hfHelper, opadState, state1,
                          extraBuffer, state2);
    writer.writeEmptyLine();

    for (std::size_t i = 0; i < hfOBlockWords; i++) {
        writer.beginAssignment(dk[i]);
        writer << dk[i] << " ^ " << state2[i];
        writer.endAssignment();
    }
    writer.endBlock();
    writer.writeEmptyLine();

    writer.writeDeclaration("uint", "output_block_index");
    writer.writeDeclaration("uint", "output_pos");
    writer.beginAssignment("output_block_index");
    writer << "input_block_index * dk_blocks + dk_block_index";
    writer.endAssignment();
    writer.beginAssignment("output_pos");
    writer << "output_block_index";
    writer.endAssignment();
    writer.writeEmptyLine();

    for (std::size_t i = 0; i < hfOBlockWords; i++) {
        writer.beginArrayAssignment("output");
        writer << "output_pos + " << i << " * batchSize * dk_blocks";
        writer.endArrayAssignment();
        writer << "HASH_WORD_TO_DEV(" << dk[i] << ")";
        writer.endAssignment();
    }

    writer.endBlock();
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2


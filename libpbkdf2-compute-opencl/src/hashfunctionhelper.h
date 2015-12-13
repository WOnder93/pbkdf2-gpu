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

#ifndef LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONHELPER_H
#define LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONHELPER_H

#include "openclwriter.h"

#include <string>
#include <vector>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class HashFunctionHelper
{
private:
    bool littleEndian;

    std::string wordClType;
    std::size_t wordBytes;

    std::size_t iblockWords;
    std::size_t oblockWords;

    std::size_t mlWords;

    std::vector<std::string> initState;

protected:
    HashFunctionHelper(
            bool littleEndian,
            const char *wordClType, std::size_t wordBytes,
            std::size_t iblockWords, std::size_t oblockWords,
            std::size_t mlWords, const char * const *initState);

public:
    bool isLittleEndian() const { return littleEndian; }

    const std::string &getWordClType() const { return wordClType; }
    std::size_t getWordBytes() const { return wordBytes; }

    std::size_t getInputBlockWords() const { return iblockWords; }
    std::size_t getOutputBlockWords() const { return oblockWords; }

    std::size_t getMessageLengthWords() const { return mlWords; }

    const std::vector<std::string> &getInitState() const { return initState; }

    virtual void writeDefinitions(OpenCLWriter &out) const = 0;

    virtual void writeUpdate(
            OpenCLWriter &out,
            const std::vector<std::string> &prevState,
            const std::vector<std::string> &state,
            const std::vector<std::string> &inputBlock,
            bool swap = false) const = 0;

    static const HashFunctionHelper &getHelper(const std::string &hashSpec);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONHELPER_H

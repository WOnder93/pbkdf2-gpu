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

#include "hashfunctionhelper.h"

#include "sha1hashfunctionhelper.h"
#include "sha256hashfunctionhelper.h"
#include "ripemd160hashfunctionhelper.h"

#include <unordered_map>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

HashFunctionHelper::HashFunctionHelper(
        bool littleEndian,
        const char *wordClType, std::size_t wordBytes,
        std::size_t iblockWords, std::size_t oblockWords,
        std::size_t mlWords, const char * const *initState)
    : littleEndian(littleEndian),
      wordClType(wordClType), wordBytes(wordBytes),
      iblockWords(iblockWords), oblockWords(oblockWords),
      mlWords(mlWords), initState()
{
    this->initState.reserve(oblockWords);
    for (std::size_t i = 0; i < oblockWords; i++) {
        this->initState.emplace_back(initState[i]);
    }
}

const HashFunctionHelper &HashFunctionHelper::getHelper(const std::string &hashSpec)
{
    static const std::unordered_map<std::string, const HashFunctionHelper &> helpers = {
        { "sha1", Sha1HashFunctionHelper::INSTANCE },
        { "sha256", Sha256HashFunctionHelper::INSTANCE },
        { "ripemd160", RipeMd160HashFunctionHelper::INSTANCE },
    };

    return helpers.at(hashSpec);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2


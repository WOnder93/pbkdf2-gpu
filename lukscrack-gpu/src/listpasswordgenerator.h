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

#ifndef LUKSCRACK_LISTPASSWORDGENERATOR_H
#define LUKSCRACK_LISTPASSWORDGENERATOR_H

#include "passwordgenerator.h"

#include <memory>
#include <iostream>
#include <fstream>

namespace lukscrack {

class ListPasswordGenerator : public PasswordGenerator
{
private:
    std::unique_ptr<std::istream> streamOwner;
    std::istream *stream;
    std::string passwordBuffer;

public:
    inline ListPasswordGenerator(std::string fileName)
        : streamOwner(), passwordBuffer()
    {
        if (fileName == "-") {
            stream = &std::cin;
        } else {
            stream = new std::ifstream(fileName);
            streamOwner = std::unique_ptr<std::istream>(stream);
        }
        stream->exceptions(std::ios::failbit | std::ios::badbit);
    }

    bool nextPassword(const char *&pwData, std::size_t &pwLength) override;
};

} // namespace lukscrack

#endif // LUKSCRACK_LISTPASSWORDGENERATOR_H

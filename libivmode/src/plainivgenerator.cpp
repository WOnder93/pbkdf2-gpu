/*
 * Based on lib/crypto_backend/crypto_storage.c from Cryptsetup
 * <https://gitlab.com/cryptsetup/cryptsetup>
 *
 * Copyright (C) 2014, Milan Broz
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

#include "plainivgenerator.h"

#include <cstring>

namespace libivmode {

void PlainIVGenerator::Context::generateIV(std::size_t sector, void *dest) const
{
    unsigned char *cursor = (unsigned char *)dest;
    cursor[0] = sector         & 0xff;
    cursor[1] = (sector >>  8) & 0xff;
    cursor[2] = (sector >> 16) & 0xff;
    cursor[3] = (sector >> 24) & 0xff;
    std::memset(cursor + 4, 0, ivLength - 4);
}

std::shared_ptr<const IVGenerator::Context>
    PlainIVGenerator::createContext(
        std::size_t ivLength, const std::string &, const void *, std::size_t) const
{
    return std::make_shared<Context>(ivLength);
}

} // namespace libivmode

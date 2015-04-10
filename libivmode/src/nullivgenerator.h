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

#ifndef LIBIVMODE_NULLIVGENERATOR_H
#define LIBIVMODE_NULLIVGENERATOR_H

#include "ivgenerator.h"

namespace libivmode {

class NullIVGenerator : public IVGenerator
{
private:
    class Context : public IVGenerator::Context
    {
    private:
        std::size_t ivLength;

    public:
        inline Context(std::size_t ivLength)
            : ivLength(ivLength)
        {
        }

        void generateIV(std::size_t sector, void *dest) const override;
    };

public:
    inline NullIVGenerator(const std::string &) { }

    std::shared_ptr<const IVGenerator::Context> createContext(
            std::size_t ivLength, const std::string &cipherName,
            const void *key, std::size_t keyLength) const override;
};

} // namespace libivmode

#endif // LIBIVMODE_NULLIVGENERATOR_H

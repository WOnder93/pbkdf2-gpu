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

#ifndef LIBIVMODE_IVGENERATOR_H
#define LIBIVMODE_IVGENERATOR_H

#include <string>
#include <memory>

namespace libivmode {

class IVGenerator
{
public:
    class Context
    {
    public:
        virtual ~Context();

        virtual void generateIV(std::size_t sector, void *dest) const = 0;
    };

    virtual ~IVGenerator();

    virtual std::shared_ptr<const Context> createContext(
            std::size_t ivLength,
            /* these arguments are used by essiv and such: */
            const std::string &cipherName,
            const void *key, std::size_t keyLength) const = 0;

    static std::shared_ptr<const IVGenerator> getGenerator(const std::string &ivmode);
};

} // namespace libivmode

#endif // LIBIVMODE_IVGENERATOR_H

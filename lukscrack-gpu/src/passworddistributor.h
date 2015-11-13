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

#ifndef LUKSCRACK_PASSWORDDISTRIBUTOR_H
#define LUKSCRACK_PASSWORDDISTRIBUTOR_H

#include "passwordgenerator.h"

#include <memory>
#include <mutex>

namespace lukscrack {

class PasswordDistributor
{
private:
    PasswordGenerator *generator;
    std::mutex mutex;

public:
    inline PasswordGenerator *getPasswordGenerator() const { return generator; }
    inline std::mutex &getMutex() { return mutex; }

    explicit PasswordDistributor(PasswordGenerator *generator)
        : generator(generator), mutex()
    {
    }
};

} // namespace lukscrack

#endif // LUKSCRACK_PASSWORDDISTRIBUTOR_H

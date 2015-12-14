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

#ifndef LIBPBKDF2_COMPUTE_TESTS_PBKDF2PARAMETERS_H
#define LIBPBKDF2_COMPUTE_TESTS_PBKDF2PARAMETERS_H

#include <cstdlib>
#include <string>
#include <vector>

namespace libpbkdf2 {
namespace compute {
namespace tests {

class PBKDF2Parameters
{
private:
    std::string name;

    const void *password;
    std::size_t passwordLength;
    const void *salt;
    std::size_t saltLength;
    std::size_t dkLength;

    std::size_t iter;

public:
    const std::string &getName() const { return name; }

    const void *getPasswordData() const { return password; }
    const void *getSaltData() const { return salt; }

    std::size_t getPasswordLength() const { return passwordLength; }
    std::size_t getSaltLength() const { return saltLength; }

    std::size_t getDerivedKeyLength() const { return dkLength; }

    std::size_t getIterationCount() const { return iter; }

    PBKDF2Parameters() { }
    PBKDF2Parameters(
            const std::string &name,
            const void *password, std::size_t passwordLength,
            const void *salt, std::size_t saltLength,
            std::size_t dkLength, std::size_t iter) :
        name(name),
        password(password), passwordLength(passwordLength),
        salt(salt), saltLength(saltLength),
        dkLength(dkLength), iter(iter) { }

    static const std::vector<PBKDF2Parameters> testParameters;
};

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_PBKDF2PARAMETERS_H

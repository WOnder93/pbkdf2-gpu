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

#ifndef LIBPBKDF2_COMPUTE_TESTS_PBKDF2TESTVECTOR_H
#define LIBPBKDF2_COMPUTE_TESTS_PBKDF2TESTVECTOR_H

#include <cstdlib>
#include <vector>
#include <string>

namespace libpbkdf2 {
namespace compute {
namespace tests {

class PBKDF2TestVector
{
private:
    std::string name;

    const void *password;
    std::size_t password_length;
    const void *salt;
    std::size_t salt_length;
    const void *dk;
    std::size_t dk_length;

    std::size_t iter;

public:
    inline const std::string &getName() const { return name; }

    inline const void *getPasswordData() const { return password; }
    inline const void *getSaltData() const { return salt; }
    inline const void *getDerivedKeyData() const { return dk; }

    inline std::size_t getPasswordLength() const { return password_length; }
    inline std::size_t getSaltLength() const { return salt_length; }
    inline std::size_t getDerivedKeyLength() const { return dk_length; }

    inline std::size_t getIterationCount() const { return iter; }

    inline PBKDF2TestVector() { }
    inline PBKDF2TestVector(
            const std::string &name,
            const void *password, std::size_t password_length,
            const void *salt, std::size_t salt_length,
            const void *dk, std::size_t dk_length,
            std::size_t iter) :
        name(name),
        password(password), password_length(password_length),
        salt(salt), salt_length(salt_length),
        dk(dk), dk_length(dk_length),
        iter(iter) { }

    static const std::vector<PBKDF2TestVector> &getStandardVectors(std::string hashSpec);
};

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_PBKDF2TESTVECTOR_H

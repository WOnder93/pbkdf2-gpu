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

#ifndef LIBPBKDF2_COMPUTE_TESTS_UTILS_H
#define LIBPBKDF2_COMPUTE_TESTS_UTILS_H

#include "testexception.h"

#include <string>
#include <sstream>

namespace libpbkdf2 {
namespace compute {
namespace tests {

namespace Utils
{
    inline static void fail(std::string reason)
    {
        throw TestException(reason);
    }

    inline static void assert(std::string assertion, bool condition)
    {
        if (!condition) {
            throw TestException("Assertion failed: " + assertion);
        }
    }

    template<class T>
    inline static void assertEquals(T actual, T expected)
    {
        std::ostringstream buf;
        buf << "'" << actual << "' should equal '" << expected << "'";
        assert(buf.str(), actual == expected);
    }
}

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_UTILS_H

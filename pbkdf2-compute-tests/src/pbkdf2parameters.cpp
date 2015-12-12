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

#include "pbkdf2parameters.h"

namespace libpbkdf2 {
namespace compute {
namespace tests {

const std::vector<PBKDF2Parameters> PBKDF2Parameters::testParameters {
    {
        "RFC3962 - 1",
        "password", 8,
        "ATHENA.MIT.EDUraeburn", 21,
        32,
        1
    },
    {
        "RFC3962 - 2",
        "password", 8,
        "ATHENA.MIT.EDUraeburn", 21,
        32,
        2
    },
    {
        "RFC3962 - 3",
        "password", 8,
        "ATHENA.MIT.EDUraeburn", 21,
        32,
        1200
    },
    {
        "RFC3962 - 4",
        "password", 8,
        "\x12\x34\x56\x78\x78\x56\x34\x12", 8,
        32,
        5
    },
    {
        "RFC3962 - 5",
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 64,
        "pass phrase equals block size", 29,
        32,
        1200
    },
    {
        "RFC3962 - 6",
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 65,
        "pass phrase exceeds block size", 30,
        32,
        1200
    },
    {
        "RFC3962 - 7",
        "\xf0\x9d\x84\x9e", 4,
        "EXAMPLE.COMpianist", 18,
        32,
        50
    },
    {
        "RFC6070 - 1",
        "password", 8,
        "salt", 4,
        20,
        1
    },
    {
        "RFC6070 - 2",
        "password", 8,
        "salt", 4,
        20,
        2
    },
    {
        "RFC6070 - 3",
        "password", 8,
        "salt", 4,
        20,
        4096
    },
    {
        "RFC6070 - 4",
        "password", 8,
        "salt", 4,
        20,
        16777216
    },
    {
        "RFC6070 - 5",
        "passwordPASSWORDpassword", 24,
        "saltSALTsaltSALTsaltSALTsaltSALTsalt", 36,
        25,
        4096
    },
    {
        "RFC6070 - 6",
        "pass\0word", 9,
        "sa\0lt", 5,
        16,
        4096
    },
};

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

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

#include "pbkdf2testvector.h"

#include <unordered_map>

namespace libpbkdf2 {
namespace compute {
namespace tests {

static const std::unordered_map<std::string, std::vector<PBKDF2TestVector>> STD_VECTORS = {
    {
        // TODO: add TVs for more hash specs
        "sha1", {
            PBKDF2TestVector(
                "RFC3962 - 1",
                "password", 8,
                "ATHENA.MIT.EDUraeburn", 21,
                "\xcd\xed\xb5\x28\x1b\xb2\xf8\x01"
                "\x56\x5a\x11\x22\xb2\x56\x35\x15"
                "\x0a\xd1\xf7\xa0\x4b\xb9\xf3\xa3"
                "\x33\xec\xc0\xe2\xe1\xf7\x08\x37", 32,
                1),
            PBKDF2TestVector(
                "RFC3962 - 2",
                "password", 8,
                "ATHENA.MIT.EDUraeburn", 21,
                "\x01\xdb\xee\x7f\x4a\x9e\x24\x3e"
                "\x98\x8b\x62\xc7\x3c\xda\x93\x5d"
                "\xa0\x53\x78\xb9\x32\x44\xec\x8f"
                "\x48\xa9\x9e\x61\xad\x79\x9d\x86", 32,
                2),
            PBKDF2TestVector(
                "RFC3962 - 3",
                "password", 8,
                "ATHENA.MIT.EDUraeburn", 21,
                "\x5c\x08\xeb\x61\xfd\xf7\x1e\x4e"
                "\x4e\xc3\xcf\x6b\xa1\xf5\x51\x2b"
                "\xa7\xe5\x2d\xdb\xc5\xe5\x14\x2f"
                "\x70\x8a\x31\xe2\xe6\x2b\x1e\x13", 32,
                1200),
            PBKDF2TestVector(
                "RFC3962 - 4",
                "password", 8,
                "\x12\x34\x56\x78\x78\x56\x34\x12", 8,
                "\xd1\xda\xa7\x86\x15\xf2\x87\xe6"
                "\xa1\xc8\xb1\x20\xd7\x06\x2a\x49"
                "\x3f\x98\xd2\x03\xe6\xbe\x49\xa6"
                "\xad\xf4\xfa\x57\x4b\x6e\x64\xee", 32,
                5),
            PBKDF2TestVector(
                "RFC3962 - 5",
                "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 64,
                "pass phrase equals block size", 29,
                "\x13\x9c\x30\xc0\x96\x6b\xc3\x2b"
                "\xa5\x5f\xdb\xf2\x12\x53\x0a\xc9"
                "\xc5\xec\x59\xf1\xa4\x52\xf5\xcc"
                "\x9a\xd9\x40\xfe\xa0\x59\x8e\xd1", 32,
                1200),
            PBKDF2TestVector(
                "RFC3962 - 6",
                "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 65,
                "pass phrase exceeds block size", 30,
                "\x9c\xca\xd6\xd4\x68\x77\x0c\xd5"
                "\x1b\x10\xe6\xa6\x87\x21\xbe\x61"
                "\x1a\x8b\x4d\x28\x26\x01\xdb\x3b"
                "\x36\xbe\x92\x46\x91\x5e\xc8\x2a", 32,
                1200),
            PBKDF2TestVector(
                "RFC3962 - 7",
                "\xf0\x9d\x84\x9e", 4,
                "EXAMPLE.COMpianist", 18,
                "\x6b\x9c\xf2\x6d\x45\x45\x5a\x43"
                "\xa5\xb8\xbb\x27\x6a\x40\x3b\x39"
                "\xe7\xfe\x37\xa0\xc4\x1e\x02\xc2"
                "\x81\xff\x30\x69\xe1\xe9\x4f\x52", 32,
                50),
            PBKDF2TestVector(
                "RFC6070 - 1",
                "password", 8,
                "salt", 4,
                "\x0c\x60\xc8\x0f\x96\x1f\x0e\x71"
                "\xf3\xa9\xb5\x24\xaf\x60\x12\x06"
                "\x2f\xe0\x37\xa6", 20,
                1),
            PBKDF2TestVector(
                "RFC6070 - 2",
                "password", 8,
                "salt", 4,
                "\xea\x6c\x01\x4d\xc7\x2d\x6f\x8c"
                "\xcd\x1e\xd9\x2a\xce\x1d\x41\xf0"
                "\xd8\xde\x89\x57", 20,
                2),
            PBKDF2TestVector(
                "RFC6070 - 3",
                "password", 8,
                "salt", 4,
                "\x4b\x00\x79\x01\xb7\x65\x48\x9a"
                "\xbe\xad\x49\xd9\x26\xf7\x21\xd0"
                "\x65\xa4\x29\xc1", 20,
                4096),
            PBKDF2TestVector(
                "RFC6070 - 4",
                "password", 8,
                "salt", 4,
                "\xee\xfe\x3d\x61\xcd\x4d\xa4\xe4"
                "\xe9\x94\x5b\x3d\x6b\xa2\x15\x8c"
                "\x26\x34\xe9\x84", 20,
                16777216),
            PBKDF2TestVector(
                "RFC6070 - 5",
                "passwordPASSWORDpassword", 24,
                "saltSALTsaltSALTsaltSALTsaltSALTsalt", 36,
                "\x3d\x2e\xec\x4f\xe4\x1c\x84\x9b"
                "\x80\xc8\xd8\x36\x62\xc0\xe4\x4a"
                "\x8b\x29\x1a\x96\x4c\xf2\xf0\x70\x38", 25,
                4096),
            PBKDF2TestVector(
                "RFC6070 - 6",
                "pass\0word", 9,
                "sa\0lt", 5,
                "\x56\xfa\x6a\xa7\x55\x48\x09\x9d"
                "\xcc\x37\xd7\xf0\x34\x25\xe0\xc3", 16,
                4096),
        },
    },
    {
        "sha256", {
            PBKDF2TestVector(
                "cryptsetup - Password exceeds block size test",
                "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 65,
                "pass phrase exceeds block size", 30,
                "\x22\x34\x4b\xc4\xb6\xe3\x26\x75"
                "\xa8\x09\x0f\x3e\xa8\x0b\xe0\x1d"
                "\x5f\x95\x12\x6a\x2c\xdd\xc3\xfa"
                "\xcc\x4a\x5e\x6d\xca\x04\xec\x58", 32,
                1200),
            PBKDF2TestVector(
                "RFC6070-like - 1",
                "password", 8,
                "salt", 4,
                "\x12\x0f\xb6\xcf\xfc\xf8\xb3\x2c"
                "\x43\xe7\x22\x52\x56\xc4\xf8\x37"
                "\xa8\x65\x48\xc9", 20,
                1),
            PBKDF2TestVector(
                "RFC6070-like - 2",
                "password", 8,
                "salt", 4,
                "\xae\x4d\x0c\x95\xaf\x6b\x46\xd3"
                "\x2d\x0a\xdf\xf9\x28\xf0\x6d\xd0"
                "\x2a\x30\x3f\x8e", 20,
                2),
            PBKDF2TestVector(
                "RFC6070-like - 3",
                "password", 8,
                "salt", 4,
                "\xc5\xe4\x78\xd5\x92\x88\xc8\x41"
                "\xaa\x53\x0d\xb6\x84\x5c\x4c\x8d"
                "\x96\x28\x93\xa0", 20,
                4096),
            PBKDF2TestVector(
                "RFC6070-like - 4",
                "password", 8,
                "salt", 4,
                "\xcf\x81\xc6\x6f\xe8\xcf\xc0\x4d"
                "\x1f\x31\xec\xb6\x5d\xab\x40\x89"
                "\xf7\xf1\x79\xe8", 20,
                16777216),
            PBKDF2TestVector(
                "RFC6070-like - 5",
                "passwordPASSWORDpassword", 24,
                "saltSALTsaltSALTsaltSALTsaltSALTsalt", 36,
                "\x34\x8c\x89\xdb\xcb\xd3\x2b\x2f"
                "\x32\xd8\x14\xb8\x11\x6e\x84\xcf"
                "\x2b\x17\x34\x7e\xbc\x18\x00\x18\x1c", 25,
                4096),
            PBKDF2TestVector(
                "RFC6070-like - 6",
                "pass\0word", 9,
                "sa\0lt", 5,
                "\x89\xb6\x9d\x05\x16\xf8\x29\x89"
                "\x3c\x69\x62\x26\x65\x0a\x86\x87", 16,
                4096),
        }
    }
};

const std::vector<PBKDF2TestVector> &PBKDF2TestVector::getStandardVectors(std::string hashSpec)
{
    return STD_VECTORS.at(hashSpec);
}

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

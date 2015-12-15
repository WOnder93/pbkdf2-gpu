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

#ifndef LIBPBKDF2_COMPUTE_OPENCL_SHA2HASHFUNCTIONHELPER_H
#define LIBPBKDF2_COMPUTE_OPENCL_SHA2HASHFUNCTIONHELPER_H

#include "hashfunctionhelper.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

struct Shifts
{
    std::size_t S0_rot1, S0_rot2, S0_rot3;
    std::size_t S1_rot1, S1_rot2, S1_rot3;

    std::size_t s0_rot1, s0_rot2, s0_shift;
    std::size_t s1_rot1, s1_rot2, s1_shift;
};

class Sha2HashFunctionHelper : public HashFunctionHelper
{
private:
    std::size_t iterations;
    const char * const *roundConstants;
    const Shifts &shifts;

protected:
    Sha2HashFunctionHelper(
            const char *wordClType, std::size_t wordBytes,
            const char * const *initState, std::size_t iterations,
            const char * const *roundConstants,
            const Shifts &shifts);

public:
    void writeDefinitions(OpenCLWriter &out) const;

    void writeUpdate(
            OpenCLWriter &out,
            const std::vector<std::string> &prevState,
            const std::vector<std::string> &state,
            const std::vector<std::string> &inputBlock,
            bool swap = false) const;
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_SHA2HASHFUNCTIONHELPER_H

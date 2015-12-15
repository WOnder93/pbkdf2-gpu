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

#ifndef LIBPBKDF2_COMPUTE_OPENCL_KERNELGENERATOR_H
#define LIBPBKDF2_COMPUTE_OPENCL_KERNELGENERATOR_H

#include "hashfunctionhelper.h"

#include <ostream>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class KernelGenerator
{
public:
    static void generateKernel(std::ostream &out, std::size_t saltLength,
                               const HashFunctionHelper &hfHelper);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_KERNELGENERATOR_H

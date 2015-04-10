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

#ifndef LIBPBKDF2_COMPUTE_OPENCL_KERNELLOADER_H
#define LIBPBKDF2_COMPUTE_OPENCL_KERNELLOADER_H

#include "opencl.h"

#include <string>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

namespace KernelLoader
{
    inline static std::string findProgram(
            const std::string &dataFolder, const std::string &kernelName)
    {
        return dataFolder + "/opencl/kernels/" + kernelName + ".cl";
    }

    inline static std::string findPBKDF2Program(
            const std::string &dataFolder, const std::string &hashSpec)
    {
        return findProgram(dataFolder, "pbkdf2_" + hashSpec);
    }

    cl::Program loadProgram(const cl::Context &context,
                            const std::string &sourcePath,
                            const std::string &buildOptions = std::string());

    cl::Program loadPBKDF2Program(const cl::Context &context,
                                  const std::string &sourcePath,
                                  std::size_t saltLength,
                                  bool log = false, bool debug = false, bool tests = false);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_KERNELLOADER_H

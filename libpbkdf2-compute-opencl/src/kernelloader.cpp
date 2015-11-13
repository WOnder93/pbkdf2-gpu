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

#include "kernelloader.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

cl::Program KernelLoader::loadProgram(
        const cl::Context &context,
        const std::string &sourcePath,
        const std::string &buildOptions)
{
    std::ifstream sourceStream(sourcePath);
    if (sourceStream.fail()) {
        throw std::ifstream::failure("unable to open file");
    }
    sourceStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string sourceText { std::istreambuf_iterator<char>(sourceStream),
            std::istreambuf_iterator<char>() };

    cl::Program prog(context, sourceText);
    try {
        prog.build(buildOptions.c_str());
    } catch (const cl::Error &) {
        std::cerr << "ERROR: Failed to build program:" << std::endl;
        for (cl::Device &device : context.getInfo<CL_CONTEXT_DEVICES>()) {
            std::cerr << "  Build log from device '" << device.getInfo<CL_DEVICE_NAME>() << "':" << std::endl;
            std::cerr << prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        }
        throw;
    }
    return prog;
}

cl::Program KernelLoader::loadPBKDF2Program(
        const cl::Context &context,
        const std::string &sourcePath,
        std::size_t saltLength,
        bool log, bool debug, bool tests)
{
    std::stringstream opts;
    opts << "-D SALT_LENGTH=" << saltLength << " ";
    if (log) {
        opts << "-D ENABLE_LOGGING" << " ";
    }
    if (debug) {
        opts << "-g -s \"" << sourcePath << "\"" << " ";
    }
    if (tests) {
        opts << "-D TESTS" << " ";
    }
    return loadProgram(context, sourcePath, opts.str());
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

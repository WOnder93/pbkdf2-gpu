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

#include "computecontext.h"

#include "kernelloader.h"

#include "libpbkdf2-gpu-common/alignment.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

ComputeContext::ComputeContext(const HashFunctionContext *parentContext,
                               const void *salt, std::size_t saltLength,
                               std::size_t dkLength, std::size_t iterationCount)
    : parentContext(parentContext), saltData(salt), saltLength(saltLength),
      dkLength(dkLength), iterationCount(iterationCount)
{
    auto hfHelper = parentContext->getHelper();

    // load and build the kernel program:
    // TODO: check error:
    prog = KernelLoader::loadPBKDF2Program(
                parentContext->getContext(), "/tmp", *hfHelper,
                saltLength, false);

    saltBufferSize = ALIGN(hfHelper->getWordBytes(), saltLength);
    saltBuffer = cl::Buffer(parentContext->getContext(), CL_MEM_READ_ONLY, saltBufferSize);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

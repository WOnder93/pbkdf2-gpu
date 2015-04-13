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

#include "devicecontext.h"

#include <cstring>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

DeviceContext::DeviceContext(const ComputeContext *parentContext, const Device &device)
    : parentContext(parentContext), device(device.getCLDevice())
{
    auto hfContext = parentContext->getParentContext();
    auto queue = cl::CommandQueue(
                hfContext->getContext(), device.getCLDevice());

    void *hostSaltBuffer = queue.enqueueMapBuffer(
                parentContext->getSaltBuffer(), true, CL_MAP_WRITE,
                0, parentContext->getSaltBufferSize());

    std::memcpy(hostSaltBuffer, parentContext->getSaltData(), parentContext->getSaltLength());
    std::memset((char *)hostSaltBuffer + parentContext->getSaltLength(), 0, parentContext->getSaltBufferSize() - parentContext->getSaltLength());

    queue.enqueueUnmapMemObject(parentContext->getSaltBuffer(), hostSaltBuffer);
    queue.finish();
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

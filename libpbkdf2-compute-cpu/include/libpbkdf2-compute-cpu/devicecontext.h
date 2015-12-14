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

#ifndef LIBPBKDF2_COMPUTE_CPU_DEVICECONTEXT_H
#define LIBPBKDF2_COMPUTE_CPU_DEVICECONTEXT_H

#include "computecontext.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class DeviceContext
{
private:
    const ComputeContext *parentContext;
    Device device;

public:
    const ComputeContext *getParentContext() const { return parentContext; }
    GlobalContext::CoreContext &getCoreContext() const
    {
        return parentContext->getParentContext()->
                getGlobalContext()->getCoreContext(device);
    }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    DeviceContext() { }

    DeviceContext(const DeviceContext &) = delete;
    DeviceContext &operator=(const DeviceContext &) = delete;

    DeviceContext(DeviceContext &&) = default;
    DeviceContext &operator=(DeviceContext &&) = default;

    DeviceContext(const ComputeContext *parentContext, const Device &device)
        : parentContext(parentContext), device(device) { }
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_DEVICECONTEXT_H

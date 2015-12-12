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

#include "hashfunctioncontext.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

HashFunctionContext::HashFunctionContext(
        const GlobalContext *globalContext,
        const std::vector<Device> &devices,
        const std::string &hashSpec)
    : globalContext(globalContext), devices(),
      hashSpec(hashSpec),
      hashAlg(&HashAlgorithm::getAlgorithm(hashSpec)),
      helper(&HashFunctionHelper::getHelper(hashSpec))
{
    for (const Device &device : devices) {
        this->devices.push_back(device.getCLDevice());
    }
    context = cl::Context(this->devices);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

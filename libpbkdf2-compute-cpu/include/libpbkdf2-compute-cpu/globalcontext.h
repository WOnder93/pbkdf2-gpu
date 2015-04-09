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

#ifndef LIBPBKDF2_COMPUTE_CPU_GLOBALCONTEXT_H
#define LIBPBKDF2_COMPUTE_CPU_GLOBALCONTEXT_H

#include "device.h"

#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class GlobalContext
{
public:
    class CoreContext {
    private:
        std::thread thread;
        std::mutex mutex;
        std::condition_variable cv;
        std::queue<std::packaged_task<void()>> taskQueue;
        bool finish = false;

        void processTasks();
        std::packaged_task<void()> dequeueTask();

    public:
        CoreContext();
        ~CoreContext();

        CoreContext(const CoreContext &) = delete;
        CoreContext &operator=(const CoreContext &) = delete;

        CoreContext(CoreContext &&) = delete;
        CoreContext &operator=(CoreContext &&) = delete;

        std::future<void> enqueueTask(std::function<void()> task);
    };

private:
    std::vector<Device> devices;
    /* use unique_ptr instead of vector because of mutex being non-movable: */
    std::unique_ptr<CoreContext[]> cores;

public:
    inline const std::vector<Device> &getAllDevices() const { return devices; }

    inline CoreContext &getCoreContext(const Device &device) const
    {
        return cores[device.getCoreIndex()];
    }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline GlobalContext() { }

    GlobalContext(const GlobalContext &) = delete;
    GlobalContext &operator=(const GlobalContext &) = delete;

    GlobalContext(GlobalContext &&) = default;
    GlobalContext &operator=(GlobalContext &&) = default;

    /* NOTE: the argument is present only to distinguish from empty ctor: */
    GlobalContext(std::nullptr_t);
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_GLOBALCONTEXT_H

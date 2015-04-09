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

#include "include/libpbkdf2-compute-cpu/globalcontext.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

GlobalContext::CoreContext::CoreContext()
    : mutex(), cv(), taskQueue()
{
    thread = std::thread(&CoreContext::processTasks, this);
}

GlobalContext::CoreContext::~CoreContext()
{
    mutex.lock();

    finish = true;
    cv.notify_one();

    mutex.unlock();

    thread.join();
}

class FinishException : public std::exception { };

void GlobalContext::CoreContext::processTasks()
{
    try {
        while (true) {
            auto task = dequeueTask();
            task();
        }
    } catch(const FinishException &) {
        return;
    }
}

std::future<void> GlobalContext::CoreContext::enqueueTask(std::function<void ()> taskFunc)
{
    auto task = std::packaged_task<void()>(taskFunc);
    std::future<void> future = task.get_future();
    {
        std::unique_lock<std::mutex> lock(mutex);
        bool wasEmpty = taskQueue.empty();
        taskQueue.push(std::move(task));
        if (wasEmpty) {
            cv.notify_one();
        }
    }
    return future;
}

std::packaged_task<void()> GlobalContext::CoreContext::dequeueTask()
{
    std::unique_lock<std::mutex> lock(mutex);
    if (finish) {
        throw FinishException();
    }
    if (taskQueue.empty()) {
        cv.wait(lock);
        if (finish) {
            throw FinishException();
        }
    }
    auto task = std::move(taskQueue.front());
    taskQueue.pop();
    return task;
}

GlobalContext::GlobalContext(std::nullptr_t)
    : devices()
{
    /* figure out the number of virtual cores: */
    unsigned coreCount = std::thread::hardware_concurrency();
    if (coreCount == 0) {
        coreCount = 1;
    }

    cores = std::unique_ptr<CoreContext[]>(new CoreContext[coreCount]);

    for (unsigned i = 0; i < coreCount; i++) {
        devices.emplace_back(i);
    }
}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

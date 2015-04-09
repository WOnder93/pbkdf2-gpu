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

#include "threadpool.h"

namespace lukscrack {

ThreadPool::ThreadPool(size_t size)
    : threads(), mutex(), cv(), taskQueue(), finish(false)
{
    if (size == 0) {
        size = (size_t)std::thread::hardware_concurrency();
        if (size == 0) {
            size = 1;
        }
    }
    this->size = size;
    threads.resize(size);

    for (size_t i = 0; i < size; i++) {
        threads[i] = std::thread(&ThreadPool::processTasks, this);
    }
}

ThreadPool::~ThreadPool()
{
    mutex.lock();

    finish = true;
    cv.notify_all();

    mutex.unlock();

    for (auto &thread : threads) {
        thread.join();
    }
}

void ThreadPool::processTasks()
{
    while (true) {
        std::packaged_task<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (finish) {
                return;
            }
            while (taskQueue.empty()) {
                cv.wait(lock);
                if (finish) {
                    return;
                }
            }
            task = std::move(taskQueue.front());
            taskQueue.pop();
        }
        task();
    }
}

std::vector<std::future<void>> ThreadPool::enqueueTasks(
        const std::vector<std::function<void()>> &tasks)
{
    std::vector<std::future<void>> res;
    res.reserve(tasks.size());

    std::lock_guard<std::mutex> guard(mutex);
    bool wasEmpty = taskQueue.empty();
    for (auto &task : tasks) {
        std::packaged_task<void()> packaged(task);
        res.push_back(packaged.get_future());
        taskQueue.push(std::move(packaged));
        if (wasEmpty) {
            cv.notify_one();
        }
    }
    return res;
}

} // namespace lukscrack

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

#ifndef LUKSCRACK_THREADPOOL_H
#define LUKSCRACK_THREADPOOL_H

#include <vector>
#include <queue>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace lukscrack {

class ThreadPool
{
private:
    std::size_t size;
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<std::packaged_task<void()>> taskQueue;
    bool finish;

    void processTasks();

public:
    inline std::size_t getSize() const { return size; }

    ThreadPool(std::size_t size);
    ~ThreadPool();

    std::vector<std::future<void>> enqueueTasks(
            const std::vector<std::function<void()>> &tasks);
};

} // namespace lukscrack

#endif // LUKSCRACK_THREADPOOL_H

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
    size_t size;
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<std::packaged_task<void()>> taskQueue;
    bool finish;

    void processTasks();

public:
    inline size_t getSize() const { return size; }

    ThreadPool(size_t size);
    ~ThreadPool();

    std::vector<std::future<void>> enqueueTasks(
            const std::vector<std::function<void()>> &tasks);
};

} // namespace lukscrack

#endif // LUKSCRACK_THREADPOOL_H

#include "include/libpbkdf2-compute-cpu/globalcontext.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

GlobalContext::CoreContext::CoreContext()
    : mutex(), cv(), taskQueue()
{
    thread = std::thread(&GlobalContext::CoreContext::processTasks, this);
}

void GlobalContext::CoreContext::processTasks()
{
    while (true) {
        auto task = dequeueTask();
        task();
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
    if (taskQueue.empty()) {
        cv.wait(lock);
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

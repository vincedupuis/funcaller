#include "ThreadedFunctionQueue.h"
#include <spdlog/spdlog.h>

using namespace funcall;

void ThreadedFunctionQueue::start()
{
    if (running)
        return;

    running = true;
    thread = std::thread(&ThreadedFunctionQueue::processQueue, this);
}

void ThreadedFunctionQueue::stop()
{
    if (!running)
        return;

    running = false;
    condition.notify_one();
    thread.join();

    queue = std::queue<Function>();
}

void ThreadedFunctionQueue::add(Function function)
{
    std::unique_lock lock(mutex);
    queue.push(std::move(function));
    condition.notify_one();
}

void ThreadedFunctionQueue::processQueue()
{
    for (;;) {
        Function function;

        {
            std::unique_lock lock(mutex);

            condition.wait(lock, [this] { return !queue.empty() || !running; });
            if (!running)
                return;

            function = std::move(queue.front());
            queue.pop();
        }

        try {
            function();
        }
        catch (const std::exception &e) {
            spdlog::error("Exception caught in ThreadedFunctionQueue::processQueue: {}", e.what());
        }
    }
}

#include "ThreadedFunctionQueue.h"

using namespace funcall;

void ThreadedFunctionQueue::add(Function &&function) noexcept
{
    std::unique_lock lock(mutex);
    queue.push(std::move(function));
    condition.notify_one();
}

void ThreadedFunctionQueue::start() noexcept
{
    thread = new std::thread(&ThreadedFunctionQueue::processQueue, this);
}

void ThreadedFunctionQueue::stop() noexcept
{
    stopping = true;
    condition.notify_one();

    const auto start = std::chrono::system_clock::now();
    while (!quitting) {
        if (std::chrono::system_clock::now() - start > std::chrono::seconds(5))
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (quitting) {
        thread->join();
        return;
    }

    try {
        thread->detach();
    } catch (const std::exception &e) {
        if (log)
            log(e.what());
    }
}

void ThreadedFunctionQueue::processQueue() noexcept
{
    for (;;) {
        Function function;

        {
            std::unique_lock lock(mutex);

            condition.wait(lock, [&] { return stopping || !queue.empty(); });
            if (stopping) {
                quitting = true;
                return;
            }

            function = std::move(queue.front());
            queue.pop();
        }

        try {
            function();
        } catch (const std::exception &e) {
            if (log)
                log(e.what());
        }
    }
}

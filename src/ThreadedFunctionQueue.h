#pragma once

#include "IFunctionQueue.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>

namespace funcall {

class ThreadedFunctionQueue final : public IFunctionQueue
{
public:
    ThreadedFunctionQueue() noexcept
        : log(nullptr)
    {
        start();
    }

    explicit ThreadedFunctionQueue(void (*log)(std::string &&)) noexcept
        : log(log)
    {
        start();
    }

    ~ThreadedFunctionQueue() override { stop(); }

    void add(Function &&function) noexcept override;

    ThreadedFunctionQueue(const ThreadedFunctionQueue &) = delete;
    ThreadedFunctionQueue &operator=(const ThreadedFunctionQueue &) = delete;
    ThreadedFunctionQueue(ThreadedFunctionQueue &&) = delete;
    ThreadedFunctionQueue &operator=(ThreadedFunctionQueue &&) = delete;

private:
    void start() noexcept;
    void stop() noexcept;

    void (*log)(std::string &&);
    std::thread *thread = nullptr;
    std::mutex mutex;
    std::queue<Function> queue;
    std::condition_variable condition;
    std::atomic_bool stopping = false;
    std::atomic_bool quitting = false;

    void processQueue() noexcept;
};

} // namespace funcall

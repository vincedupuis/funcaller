#pragma once

#include "IFunctionQueue.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace funcall {

class ThreadedFunctionQueue final : public IFunctionQueue
{
public:
    ThreadedFunctionQueue() noexcept
        : mLogger(nullptr)
    {}

    explicit ThreadedFunctionQueue(void (*log)(std::string &&)) noexcept
        : mLogger(log)
    {}

    ~ThreadedFunctionQueue() noexcept override { stop(); }

    std::thread::id start() noexcept;
    bool stop() noexcept;

    void add(Function &&function) noexcept override;

    ThreadedFunctionQueue(const ThreadedFunctionQueue &) = delete;
    ThreadedFunctionQueue &operator=(const ThreadedFunctionQueue &) = delete;
    ThreadedFunctionQueue(ThreadedFunctionQueue &&) = delete;
    ThreadedFunctionQueue &operator=(ThreadedFunctionQueue &&) = delete;

private:
    std::function<void(std::string &&)> mLogger;

    struct Context
    {
        std::thread thread;
        std::mutex mutex;
        std::queue<Function> queue;
        std::condition_variable condition;
        std::atomic_bool running = false;
        std::atomic_bool stopping = false;
    };

    std::mutex mMutex;
    std::atomic<std::shared_ptr<Context>> mContext;

    static void waitFor(const std::atomic_bool &var, bool value) noexcept;

    static void processQueue(
        std::shared_ptr<Context> context,
        std::function<void(std::string &&)> _logger) noexcept;
};

} // namespace funcall

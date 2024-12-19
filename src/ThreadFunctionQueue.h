#pragma once

#include "IFunctionQueue.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace funcall {

// A thread-safe object that queues and execute functions in a separate thread.
class ThreadFunctionQueue final : public IFunctionQueue
{
public:
    ThreadFunctionQueue() noexcept
        : mLogger(nullptr)
    {}

    // Constructor with a static logger function
    explicit ThreadFunctionQueue(void (*log)(std::string &&)) noexcept
        : mLogger(log)
    {}

    // Destructor, will stop the thread if running
    ~ThreadFunctionQueue() noexcept override { stop(); }

    // Start the thread and return the thread id,
    // or return an empty id if the thread is already running
    // or if the thread could not be started
    std::thread::id start() noexcept;

    // Stop the thread and return true if the thread is stopped,
    // or return false if the thread is not running or could not be stopped
    bool stop() noexcept;

    // Add a function to the queue. It will be executed in the thread
    void add(Function &&function) noexcept override;

    // Copy and move constructors and assignment operators are not allowed
    ThreadFunctionQueue(const ThreadFunctionQueue &) = delete;
    ThreadFunctionQueue &operator=(const ThreadFunctionQueue &) = delete;
    ThreadFunctionQueue(ThreadFunctionQueue &&) = delete;
    ThreadFunctionQueue &operator=(ThreadFunctionQueue &&) = delete;

private:
    std::function<void(std::string &&)> mLogger;

    // Define the context needed for the thread
    struct Context
    {
        std::thread thread;
        std::mutex mutex;
        std::queue<Function> queue;
        std::condition_variable condition;
        std::atomic_bool running = false;
        std::atomic_bool stopping = false;
    };

    // Mutex to protect the start and stop functions
    std::mutex mMutex;

    // Atomic shared pointer to the current thread context
    std::atomic<std::shared_ptr<Context>> mContext;

    // Wait for the variable to reach the desired value before timeout
    static void waitFor(const std::atomic_bool &var, bool value) noexcept;

    // Entry point for the thread to process the queue
    static void processQueue(
        std::shared_ptr<Context> context, std::function<void(std::string &&)> _logger) noexcept;
};

} // namespace funcall
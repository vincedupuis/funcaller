#pragma once

#include "IFunctionQueue.h"

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace funcall
{
class ThreadedFunctionQueue final : public IFunctionQueue {
public:
    explicit ThreadedFunctionQueue(std::function<void(std::string &&)> log)
        : log(std::move(log)) {}

    void start();
    void stop();

    void add(Function&& function) override;

private:
    std::function<void(std::string &&)> log;
    std::queue<Function> queue;
    std::mutex mutex;
    std::condition_variable condition;
    std::thread thread;
    std::atomic<bool> running = false;

    void processQueue();
};
}

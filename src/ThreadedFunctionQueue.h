#pragma once

#include "IFunctionQueue.h"

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace funcall
{

class ThreadedFunctionQueue final : public IFunctionQueue
{
public:
    void start();
    void stop();

    void add(Function&& function) override;

private:
    std::queue<Function> queue;
    std::mutex mutex;
    std::condition_variable condition;
    std::thread thread;
    std::atomic<bool> running = false;

    void processQueue();
};

}

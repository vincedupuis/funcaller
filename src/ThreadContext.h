#pragma once

#include "IFunctionQueue.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace funcall {

// Define the context needed for the thread
struct ThreadContext
{
    std::thread thread;
    std::mutex mutex;
    std::queue<funcall::Function> queue;
    std::condition_variable condition;
    std::atomic_bool running = false;
    std::atomic_bool stopping = false;
};

}

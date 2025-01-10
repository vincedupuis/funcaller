//
// Copyright (c) 2025 Vincent Dupuis, Go Software Inc.
//
// This file is part of a project licensed under the MIT License.
// For details, see the LICENSE file in the root of the repository.
//

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

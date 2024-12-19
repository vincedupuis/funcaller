#include "ThreadedFunctionQueue.h"

using namespace funcall;

std::thread::id ThreadedFunctionQueue::start() noexcept
{
    // Prevent multiple threads from starting/stopping the queue at the same time
    std::scoped_lock lock(mMutex);

    // If the context is already created, it means the thread is already running
    // Return the thread ID (already started)
    auto ctx = mContext.load();
    if (ctx)
        return ctx->thread.get_id();

    // Create a new context and start the thread
    ctx = std::make_shared<Context>();
    ctx->thread = std::thread(&ThreadedFunctionQueue::processQueue, ctx, mLogger);

    // Wait for the thread to start.
    // If it starts, store the context and return the thread ID
    waitFor(ctx->running, true);
    if (ctx->running) {
        mContext.store(ctx);
        return ctx->thread.get_id();
    }

    // If the thread did not start, signal stopping and try to detach it
    ctx->stopping.store(true);
    ctx->condition.notify_one();
    try {
        ctx->thread.detach();
    }
    catch (const std::exception &e) {
        if (mLogger)
            mLogger(e.what());
    }

    // Return an empty thread ID
    return {};
}

bool ThreadedFunctionQueue::stop() noexcept
{
    // Prevent multiple threads from starting/stopping the queue at the same time
    std::scoped_lock lock(mMutex);

    // If the context is not created, this means the thread is not running
    // Return true (already stopped)
    const auto ctx = mContext.load();
    if (!ctx)
        return true;

    // Signal stopping and wait for the thread to stop
    ctx->stopping.store(true);
    ctx->condition.notify_one();
    waitFor(ctx->running, false);

    // If the thread stopped, join it and clear the context
    // Return true (stopped)
    if (!ctx->running) {
        ctx->thread.join();
        mContext.store(nullptr);
        return true;
    }

    // The thread did not stop, try to detach it
    try {
        ctx->thread.detach();
    }
    catch (const std::exception &e) {
        if (mLogger)
            mLogger(e.what());
    }

    // Clear the context and return false (not stopped)
    mContext.store(nullptr);
    return false;
}

void ThreadedFunctionQueue::add(Function &&function) noexcept
{
    // Note: if the stop occurred during this call, the last
    // context will be deleted at the end of this function

    // If the context is not created, this means the thread is not running.
    const auto ctx = mContext.load();
    if (!ctx)
        return;

    // Add the function to the queue and notify the thread
    std::unique_lock lock(ctx->mutex);
    ctx->queue.push(std::move(function));
    lock.unlock();
    ctx->condition.notify_one();
}

void ThreadedFunctionQueue::waitFor(const std::atomic_bool &var, const bool value) noexcept
{
    // Wait for the variable to reach the desired value before timeout
    const auto start = std::chrono::system_clock::now();
    while (var != value) {
        if (std::chrono::system_clock::now() - start > std::chrono::seconds(5))
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ThreadedFunctionQueue::processQueue(
    std::shared_ptr<Context> context,
    std::function<void(std::string &&)> _logger) noexcept
{
    // Move the context and logger to the local variables
    const auto ctx = std::move(context);
    const auto logger = std::move(_logger);

    // Acquire the lock and notify the thread is running
    std::unique_lock lock(ctx->mutex);
    ctx->running.store(true);

    // Process the queue until stopping is signaled
    while (!ctx->stopping) {
        // Wait for the queue to have elements or stopping to be signaled
        ctx->condition.wait(lock, [&] {
            return ctx->stopping || !ctx->queue.empty();
        });

        // Empty the queue and execute the functions while the thread is not stopping
        while (!ctx->stopping && !ctx->queue.empty())
        {
            // Get the function from the queue
            auto function = std::move(ctx->queue.front());
            ctx->queue.pop();

            // Execute the function while not holding the lock
            ctx->mutex.unlock();
            try {
                function();
            }
            catch (const std::exception &e) {
                if (logger)
                    logger(e.what());
            }
            ctx->mutex.lock();
        }
    }

    // Notify the thread is not running
    ctx->running.store(false);
}

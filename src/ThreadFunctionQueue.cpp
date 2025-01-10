#include "ThreadFunctionQueue.h"
#include <sstream>
#include <tstd/testpoint.h>

using namespace funcall;

// Convert a thread ID to a long value
static long toLong(const std::thread::id id) noexcept
{
    std::stringstream ss;
    ss << id;
    long value;
    ss >> value;
    return ss.fail() ? 0 : value;
}

long ThreadFunctionQueue::start() noexcept
{
    // Prevent multiple threads from starting/stopping the queue at the same time
    std::scoped_lock lock(mMutex);

    // If the context is already created, it means the thread is already running
    // Return the thread ID (already started)
    auto ctx = mContext.load();
    if (ctx)
        return toLong(ctx->thread.get_id());

    ctx = std::make_shared<ThreadContext>();
    ctx->thread = tp(std::thread(&ThreadFunctionQueue::processQueue, ctx, mLogger));

    waitFor(ctx->running, true);
    if (ctx->running) {
        mContext.store(ctx);
        return toLong(ctx->thread.get_id());
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

    return toLong(std::thread::id());
}

bool ThreadFunctionQueue::stop() noexcept
{
    // Prevent multiple threads from starting/stopping the queue at the same time
    std::scoped_lock lock(mMutex);

    // If the context is not created, this means the thread is not running
    // Return true (already stopped)
    const auto ctx = mContext.load();
    if (!ctx)
        return true;

    ctx->stopping.store(true);
    ctx->condition.notify_one();

    waitFor(ctx->running, false);
    if (tp(!ctx->running)) {
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

    mContext.store(nullptr);
    return false;
}

void ThreadFunctionQueue::add(Function &&function) noexcept
{
    // Note: if the stop occurred during this call, the last
    // context will be deleted at the end of this function

    // If the context is not created, this means the thread is not running.
    const auto ctx = mContext.load();
    if (!ctx)
        return;

    std::unique_lock lock(ctx->mutex);
    ctx->queue.push(std::move(function));
    lock.unlock();
    ctx->condition.notify_one();
}

void ThreadFunctionQueue::waitFor(const std::atomic<bool> &var, const bool value) noexcept
{
    const auto start = tp(std::chrono::system_clock::now());
    while (var != value) {
        if (std::chrono::system_clock::now() - start > std::chrono::seconds(5))
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ThreadFunctionQueue::processQueue(
    std::shared_ptr<ThreadContext> context,
    std::function<void(std::string &&)> _logger) noexcept
{
    const auto ctx = std::move(context);
    const auto logger = std::move(_logger);

    std::unique_lock lock(ctx->mutex);
    ctx->running.store(true);

    while (!ctx->stopping) {
        // Wait for the queue to have elements or stopping have been signaled
        ctx->condition.wait(lock, [&] {
            return ctx->stopping || !ctx->queue.empty();
        });

        while (!ctx->stopping && !ctx->queue.empty())
        {
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

    ctx->running.store(false);
}

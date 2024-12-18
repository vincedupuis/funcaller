#include "ThreadedFunctionQueue.h"

using namespace funcall;

std::thread::id ThreadedFunctionQueue::start() noexcept
{
    std::scoped_lock lock(mMutex);

    auto ctx = mContext.load();
    if (ctx)
        return ctx->thread.get_id();

    ctx = std::make_shared<Context>();
    ctx->thread = std::thread(&ThreadedFunctionQueue::processQueue, ctx, mLogger);

    waitFor(ctx->running, true);
    if (ctx->running) {
        mContext.store(ctx);
        return ctx->thread.get_id();
    }

    ctx->stopping = true;
    ctx->condition.notify_one();
    try {
        ctx->thread.detach();
    }
    catch (const std::exception &e) {
        if (mLogger)
            mLogger(e.what());
    }
    return {};
}

bool ThreadedFunctionQueue::stop() noexcept
{
    std::scoped_lock lock(mMutex);

    const auto ctx = mContext.load();
    if (!ctx)
        return true;

    ctx->stopping = true;
    ctx->condition.notify_one();
    waitFor(ctx->running, false);

    if (!ctx->running) {
        ctx->thread.join();
        mContext.store(nullptr);
        return true;
    }

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

void ThreadedFunctionQueue::add(Function &&function) noexcept
{
    const auto ctx = mContext.load();
    if (!ctx)
        return;

    std::unique_lock lock(ctx->mutex);
    ctx->queue.push(std::move(function));
    ctx->condition.notify_one();
}

void ThreadedFunctionQueue::waitFor(const std::atomic_bool &var, const bool value) noexcept
{
    const auto start = std::chrono::system_clock::now();
    while (var.load() != value) {
        if (std::chrono::system_clock::now() - start > std::chrono::seconds(5))
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ThreadedFunctionQueue::processQueue(
    std::shared_ptr<Context> context,
    std::function<void(std::string &&)> _logger) noexcept
{
    const auto logger = std::move(_logger);
    const auto ctx = std::move(context);

    std::unique_lock lock(ctx->mutex);
    ctx->running = true;

    while (!ctx->stopping) {
        ctx->condition.wait(lock, [&] {
            return ctx->stopping || !ctx->queue.empty();
        });

        if (ctx->stopping) {
            break;
        }

        auto function = std::move(ctx->queue.front());
        ctx->queue.pop();

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

    ctx->running = false;
}

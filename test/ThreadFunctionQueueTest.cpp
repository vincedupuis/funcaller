#include "ThreadFunctionQueue.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("When starting, Then return thread id")
{
    funcall::ThreadFunctionQueue queue;

    const auto id = queue.start();
    CHECK(id != 0);
}

TEST_CASE("When starting twice Then return same thread id")
{
    funcall::ThreadFunctionQueue queue;

    const auto id1 = queue.start();
    const auto id2 = queue.start();
    CHECK(id1 != 0);
    CHECK(id1 == id2);
}

TEST_CASE("When stopping, Then return true")
{
    funcall::ThreadFunctionQueue queue;
    queue.start();

    const auto stopped = queue.stop();
    CHECK(stopped);
}

TEST_CASE("When stopping twice, Then return true")
{
    funcall::ThreadFunctionQueue queue;
    queue.start();

    const auto stopped1 = queue.stop();
    const auto stopped2 = queue.stop();
    CHECK(stopped1);
    CHECK(stopped2);
}

TEST_CASE("When stopping without starting, Then return true")
{
    funcall::ThreadFunctionQueue queue;

    const auto stopped = queue.stop();
    CHECK(stopped);
}

TEST_CASE("When adding a function in the queue, Then the function is called")
{
    std::atomic_bool called = false;
    funcall::ThreadFunctionQueue queue;

    queue.start();
    queue.add([&called]() { called.store(true); });

    while (!called)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

TEST_CASE("When adding a function in the queue withou starting, Then the function is not called")
{
    std::atomic_bool called = false;
    funcall::ThreadFunctionQueue queue;

    queue.add([&called]() { called.store(true); });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    CHECK_FALSE(called);
}

TEST_CASE("When adding a function after stopping, Then the function is not called")
{
    std::atomic_bool called = false;
    funcall::ThreadFunctionQueue queue;

    queue.start();
    queue.stop();
    queue.add([&called]() { called.store(true); });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    CHECK_FALSE(called);
}

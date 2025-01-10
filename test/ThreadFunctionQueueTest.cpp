#include "ThreadFunctionQueue.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <tstd/testpoint.h>

TEST_CASE("When starting, Then return thread id")
{
    funcall::ThreadFunctionQueue queue;

    const auto id = queue.start();
    CHECK(id != 0);
}

TEST_CASE("When starting but thread did not start, Then return 0")
{
    TestPoint<std::thread>()
        // Prevent the thread from starting
        .Return(std::thread());
    TestPoint<std::chrono::system_clock::time_point>()
        // Do not wait for the thread to start
        .Return(std::chrono::system_clock::now() - std::chrono::seconds(10));

    funcall::ThreadFunctionQueue queue;

    const auto id = queue.start();
    CHECK(id == 0);
}

TEST_CASE("When starting twice, Then return same thread id")
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

TEST_CASE("When stopping and thread did not stop, Then return false")
{
    TestPoint<bool>()
        // Simulate the thread did not stop
        .Return(false);
    funcall::ThreadFunctionQueue queue;
    queue.start();

    const auto stopped = queue.stop();
    CHECK_FALSE(stopped);
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

    const auto start = std::chrono::system_clock::now();
    while (!called) {
        if (std::chrono::system_clock::now() - start > std::chrono::seconds(5))
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CHECK(called);
}

TEST_CASE("When adding a function in the queue without starting, Then the function is not called")
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

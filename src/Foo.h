#pragma once

#include "ContextSwitch.h"

#include <spdlog/spdlog.h>

class IFoo
{
public:
    virtual ~IFoo() = default;

    virtual void foo1() = 0;
    virtual void foo2(int a) = 0;
    virtual void foo3(std::shared_ptr<int> ptr) = 0;
};

class Foo : public IFoo
{
public:
    void foo1() override
    {
        spdlog::info("Function foo1 called");
    }

    void foo2(int a) override
    {
        spdlog::info("Function foo2 called with argument: {}", a);
    }

    void foo3(std::shared_ptr<int> ptr) override
    {
        spdlog::info("Function foo3 called with argument: {}", *ptr);
    }
};

class FooContextSwitch : public util::ContextSwitch<IFoo>
{
public:
    void foo1() override QUEUE(foo1)
    void foo2(int a) override QUEUE(foo2, a)
    void foo3(std::shared_ptr<int> ptr) override QUEUE(foo3, ptr)
};

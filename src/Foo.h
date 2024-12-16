#pragma once

#include "ContextSwitch.h"

#include <stdexcept>

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
        printf("Function foo1 called\n");
        throw std::runtime_error("Exception in foo1");
    }

    void foo2(int a) override { printf("Function foo2 called with argument: %d\n", a); }

    void foo3(std::shared_ptr<int> ptr) override
    {
        printf("Function foo3 called with argument: %d\n", *ptr);
    }
};

class FooContextSwitch : public funcall::ContextSwitch<IFoo>
{
public:
    void foo1() override QUEUE(foo1);
    void foo2(int a) override QUEUE(foo2, a);
    void foo3(std::shared_ptr<int> ptr) override QUEUE(foo3, ptr);
};

#pragma once

#include <memory>

class IFoo
{
public:
    virtual ~IFoo() = default;

    virtual void foo1() = 0;
    virtual void foo2(int a) = 0;
    virtual void foo3(std::shared_ptr<int> ptr) = 0;
};

#pragma once

#include "IFoo.h"
#include <memory>

class Bar
{
public:
    explicit Bar(std::shared_ptr<IFoo> foo)
        : foo(std::move(foo))
    {}

    void bar1() const { foo->foo1(); }
    void bar2(const int a) const { foo->foo2(a); }
    void bar3(std::shared_ptr<int> ptr) const { foo->foo3(std::move(ptr)); }

private:
    std::shared_ptr<IFoo> foo;
};

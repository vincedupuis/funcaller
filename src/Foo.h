#pragma once

#include "IFoo.h"

#include <stdexcept>

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

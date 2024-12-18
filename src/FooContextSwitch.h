#pragma once

#include "ContextSwitch.h"
#include "IFoo.h"

class FooContextSwitch : public funcall::ContextSwitch<IFoo>
{
public:
    void foo1() override QUEUE(foo1);
    void foo2(int a) override QUEUE(foo2, a);
    void foo3(std::shared_ptr<int> ptr) override QUEUE(foo3, ptr);
};

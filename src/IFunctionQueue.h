#pragma once

#include <functional>

namespace funcall
{

using Function = std::function<void()>;

class IFunctionQueue
{
public:
    virtual ~IFunctionQueue() = default;

    virtual void add(Function&& function) = 0;
};

}

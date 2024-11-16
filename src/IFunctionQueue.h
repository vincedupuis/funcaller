#pragma once

#include <functional>

namespace util
{
    using Function = std::function<void()>;

    class IFunctionQueue
    {
    public:
        virtual ~IFunctionQueue() = default;

        virtual void add(Function function) = 0;
    };
}

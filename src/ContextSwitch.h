#pragma once

#include "IFunctionQueue.h"

#include <memory>

#define QUEUE(f, ...) { functionQueue->add([=, this] { implementation->f(__VA_ARGS__); }); }

namespace funcall
{
    template<typename T>
    class ContextSwitch : public T
    {
    public:
        void configure(
            std::shared_ptr<IFunctionQueue> fq,
            std::shared_ptr<T> impl)
        {
            implementation = std::move(impl);
            functionQueue = std::move(fq);
        }

    protected:
        std::shared_ptr<T> implementation;
        std::shared_ptr<IFunctionQueue> functionQueue;
    };
}

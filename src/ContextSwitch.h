//
// Copyright (c) 2025 Vincent Dupuis, Go Software Inc.
//
// This file is part of a project licensed under the MIT License.
// For details, see the LICENSE file in the root of the repository.
//

#pragma once

#include "IFunctionQueue.h"

#include <memory>

#define QUEUE(f, ...) \
    { \
        functionQueue->add([=, this] { implementation->f(__VA_ARGS__); }); \
    }

namespace funcall {

template<typename T>
class ContextSwitch : public T
{
public:
    void setup(std::shared_ptr<T> impl, std::shared_ptr<IFunctionQueue> fq)
    {
        implementation = std::move(impl);
        functionQueue = std::move(fq);
    }

protected:
    std::shared_ptr<T> implementation;
    std::shared_ptr<IFunctionQueue> functionQueue;
};

} // namespace funcall

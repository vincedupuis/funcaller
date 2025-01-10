//
// Copyright (c) 2025 Vincent Dupuis, Go Software Inc.
//
// This file is part of a project licensed under the MIT License.
// For details, see the LICENSE file in the root of the repository.
//

#pragma once

#include <functional>

namespace funcall {

using Function = std::function<void()>;

class IFunctionQueue
{
public:
    virtual ~IFunctionQueue() = default;

    virtual void add(Function &&) noexcept = 0;
};

} // namespace funcall

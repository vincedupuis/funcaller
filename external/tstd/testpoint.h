// testpoint.h
//
// MIT License
//
// Copyright (c) 2025 Vincent Dupuis, Go Logiciels Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#ifndef TSTD_UNIT_TESTING

#define tp(exp) exp

#else

#include <functional>
#include <queue>

#define tp(exp) TestPoint<decltype(exp)>::get([&]() { return std::move(exp); })

template <typename T>
class TestPoint {
public:
    TestPoint()
    {
        queue = std::queue<T>();
    }

    TestPoint& Return(T&& value)
    {
        queue.push(std::move(value));
        return *this;
    }

    static void AlwaysReturn(T&& value)
    {
        static_assert(std::is_copy_constructible_v<T>, "AlwaysReturn requires a copy constructible type");
        queue = std::queue<T>();
        queue.push(std::move(value));
        always = true;
    }

    static T get(std::function<T()>&& f) noexcept
    {
        if constexpr (std::is_copy_constructible_v<T>)
            if (always)
                return queue.front();
        if (queue.empty())
            return std::move(f());
        T value = std::move(queue.front());
        queue.pop();
        return std::move(value);
    }

private:
    inline static bool always = false;
    inline static std::queue<T> queue;
};

#endif

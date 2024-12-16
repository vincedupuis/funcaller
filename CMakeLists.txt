cmake_minimum_required(VERSION 3.28)

project(funcall)

set(CMAKE_CXX_STANDARD 20)

add_executable(funcall
        src/ContextSwitch.h
        src/Foo.h
        src/FooContextSwitch.h
        src/IFoo.h
        src/IFunctionQueue.h
        src/main.cpp
        src/ThreadedFunctionQueue.cpp
        src/ThreadedFunctionQueue.h
)

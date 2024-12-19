cmake_minimum_required(VERSION 3.28)

project(funcall)

set(CMAKE_CXX_STANDARD 20)

add_executable(funcall
        src/ContextSwitch.h
        src/IFunctionQueue.h
        src/ThreadFunctionQueue.cpp
        src/ThreadFunctionQueue.h
        test/ThreadFunctionQueueTest.cpp
        example/Foo.h
        example/Bar.h
        example/FooContextSwitch.h
        example/IFoo.h
        example/main.cpp
)

target_include_directories(funcall PRIVATE src example)

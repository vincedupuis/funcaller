cmake_minimum_required(VERSION 3.28)

project(funcall)

set(CMAKE_CXX_STANDARD 20)

add_executable(funcall-test
        src/ContextSwitch.h
        src/IFunctionQueue.h
        src/ThreadFunctionQueue.cpp
        src/ThreadFunctionQueue.h
        src/ThreadContext.h
        test/ThreadFunctionQueueTest.cpp
)
target_compile_definitions(funcall-test PRIVATE TSTD_UNIT_TESTING)
target_include_directories(funcall-test PRIVATE src external)

add_executable(funcall-example
        src/ContextSwitch.h
        src/IFunctionQueue.h
        src/ThreadContext.h
        src/ThreadFunctionQueue.cpp
        src/ThreadFunctionQueue.h
        example/Foo.h
        example/Bar.h
        example/FooContextSwitch.h
        example/IFoo.h
        example/main.cpp
)

target_include_directories(funcall-example PRIVATE src example)

cmake_minimum_required(VERSION 3.28)

project(funcall)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc /Wall /W4 /WX")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else ()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

find_package(spdlog REQUIRED)

add_executable(funcall
        src/ContextSwitch.h
        src/Foo.h
        src/IFunctionQueue.h
        src/main.cpp
        src/ThreadedFunctionQueue.cpp
        src/ThreadedFunctionQueue.h
)

target_link_libraries(funcall spdlog::spdlog)

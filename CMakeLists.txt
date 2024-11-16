cmake_minimum_required(VERSION 3.28)
project(funcall)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-DSPDLOG_COMPILED_LIB -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_LINK_FLAGS "-fuse-ld=gold -L/usr/local/lib")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline -fno-elide-constructors")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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

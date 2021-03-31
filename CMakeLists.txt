cmake_minimum_required(VERSION 3.17.0)
project(cpplox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Debug)

add_executable(cpplox "")
add_subdirectory(src)

target_compile_options(cpplox PRIVATE -Wall -Wextra -pedantic -Wno-narrowing -Wno-sign-compare
        -fno-omit-frame-pointer -ggdb)

target_include_directories(cpplox PRIVATE SYSTEM
        "include")

cmake_minimum_required(VERSION 3.17.0)
project(garm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)

target_include_directories(garm PRIVATE SYSTEM
        "include")

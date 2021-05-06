cmake_minimum_required(VERSION 3.17.0)
project(cpplox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Debug)

add_executable(cpplox "")
add_subdirectory(src)

find_package(fmt)

set(CXX_COMPILE_FLAGS)
set(CXX_LINK_FLAGS)
list(APPEND CXX_COMPILE_FLAGS -Wall -Wextra -pedantic -Wno-narrowing -Wno-sign-compare)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    list(APPEND CXX_COMPILE_FLAGS -fno-omit-frame-pointer
            -fno-optimize-sibling-calls -fsanitize=address)
    list(APPEND CXX_LINK_FLAGS -fsanitize=address
            -fno-omit-frame-pointer -fno-optimize-sibling-calls)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND CXX_COMPILE_FLAGS -O3)
endif()

target_compile_options(cpplox PRIVATE ${CXX_COMPILE_FLAGS})
target_link_options(cpplox PRIVATE ${CXX_LINK_FLAGS})
target_include_directories(cpplox PRIVATE SYSTEM
        "include")
target_link_libraries(cpplox fmt::fmt)

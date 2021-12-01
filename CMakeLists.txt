cmake_minimum_required(VERSION 3.2)

set(This my_archive)

cmake_policy(SET CMP0091 NEW) # msvc, MSVC_RUNTIME_LIBRARY, googletest Md

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

enable_testing()
add_subdirectory(googletest)
add_subdirectory(test)
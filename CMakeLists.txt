cmake_minimum_required(VERSION 3.6)
project(project_name)

set(CMAKE_CXX_STANDARD 14)

enable_testing()

include(gtest.cmake)

add_subdirectory(src)
add_subdirectory(test)

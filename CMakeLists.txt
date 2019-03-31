cmake_minimum_required(VERSION 3.6)
project(project_name)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

option(USE_GTEST "Enable testing" ON)
option(USE_BENCHMARK "Enable benchmarking" ON)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat=2 -march=native -O3")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,now")

if(USE_GTEST OR USE_BENCHMARK)
    enable_testing()
endif()

if(USE_GTEST)
    include(gtest.cmake)
endif()

if(USE_BENCHMARK)
    include(benchmark.cmake)
endif()


add_subdirectory(src)
add_subdirectory(test)

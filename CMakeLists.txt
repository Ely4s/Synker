cmake_minimum_required(VERSION 3.19)
project(Synker)
set(CMAKE_CXX_STANDARD 20)

#flags
SET(GCC_COVERAGE_COMPILE_FLAGS "-fvisibility=hidden")
SET(GCC_COVERAGE_LINK_FLAGS "")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

#include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#executable
add_executable (Synker src/main.cpp)
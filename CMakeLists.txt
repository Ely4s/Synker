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

#find boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_INCLUDE_DIR /usr/local/src/boost_1_46_1)
set(Boost_LIBRARY_DIR /usr/local/src/boost_1_46_1/stage/lib)
find_package(Boost 1.76.0 COMPONENTS system filesystem thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

#find spdlog
if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

#executable
add_executable (Synker src/main.cpp src/Node/Node.cpp include/Node/Node.h src/Node/File.cpp include/Node/File.h src/Node/Directory.cpp include/Node/Directory.h)

#libraries linking
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} spdlog::spdlog)
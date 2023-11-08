cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(LinkedLists CXX)

if (DEBUG_LOGGING)
    add_compile_definitions(DEBUG_LOGGING)
endif()

add_executable(testOL TestOptimisticList.cpp)


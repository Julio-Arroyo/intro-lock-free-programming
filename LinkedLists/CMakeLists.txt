cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++-13")

project(LinkedLists CXX)
set(TARGET test)
add_executable(${TARGET} TestLinkedList.cpp)

include_directories(${TARGET} PRIVATE "./include/" ".")

if (ENABLE_LOGGING)
  target_compile_definitions(${TARGET} PRIVATE ENABLE_LOGGING)
endif()



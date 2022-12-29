cmake_minimum_required(VERSION 3.21.0)
project(lab5 VERSION 0.1)

include(CTest)
enable_testing()

include_directories(${CMAKE_SOURCE_DIR})
add_executable(lab5 extmem.cpp main.cpp)
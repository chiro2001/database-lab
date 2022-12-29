cmake_minimum_required(VERSION 3.21.0)
project(lab5 VERSION 0.1)

include(CTest)
enable_testing()

include_directories(${CMAKE_SOURCE_DIR})
add_executable(lab5 extmem.cpp main.cpp)
add_executable(mem_test extmem.cpp test.cpp)

add_test(NAME run_main WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ${CMAKE_BINARY_DIR}/lab5)
add_test(NAME run_mem_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ${CMAKE_BINARY_DIR}/mem_test)
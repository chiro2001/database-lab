cmake_minimum_required(VERSION 3.21.0)
project(lab5 VERSION 0.1)

include(CTest)
enable_testing()

set(CPM_DOWNLOAD_VERSION 0.36.0)

if (CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif (DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else ()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif ()

# Expand relative path. This is important if the provided path contains a tilde (~)
get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)
if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
            https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
            ${CPM_DOWNLOAD_LOCATION}
            )
endif ()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
        NAME debug_macros
        VERSION 1.03
        GITHUB_REPOSITORY chiro2001/debug-macros)

link_libraries(debug_macros)
target_compile_definitions(debug_macros INTERFACE
#        -DCONFIG_DEBUG=1
#        -DCONFIG_FILE_LOG=1
#        -DCONFIG_LOG_COLOR_DISABLED=1
)

include_directories(${CMAKE_SOURCE_DIR})
set(SOURCE_LIST
        extmem.c
        buffered_queue.c buffered_queue.h
        cache.c cache.h
        main_utils.h main_utils.c
        iterator.c iterator.h
        questions.h question1.c question2.c question3.c question4.c)
add_executable(lab5 main.c ${SOURCE_LIST})
add_executable(unit_test unit_test.c ${SOURCE_LIST})
add_executable(mem_test test.c ${SOURCE_LIST})

#target_compile_options(lab5 INTERFACE Og)

add_test(NAME run_main WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ${CMAKE_BINARY_DIR}/lab5)
add_test(NAME run_unit_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ${CMAKE_BINARY_DIR}/unit_test)
add_test(NAME run_mem_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ${CMAKE_BINARY_DIR}/mem_test)


add_custom_command(TARGET lab5 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:lab5>/data/)
add_custom_command(TARGET unit_test PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:lab5>/data/)

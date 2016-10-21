cmake_minimum_required(VERSION 3.6)

project(got2d)

option(BUILD_GOT2D_TESTBED OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include(${CMAKE_CURRENT_LIST_DIR}/extern/download_cxx.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/extern/download_res.cmake)

add_subdirectory(got2d)

if(BUILD_GOT2D_TESTBED)
    if(MSVC)
        add_subdirectory(testbed)
    endif()
endif()
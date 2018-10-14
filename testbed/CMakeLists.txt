cmake_minimum_required(VERSION 3.8)

set(GOT2D_TESTBED_SOURCE_FILES
	framework.h
	framework.cpp
	hexagon.h
	hexagon.cpp
	stdafx.h
	stdafx.cpp
	winmain.cpp
)

include_directories(..)
add_executable(got2d_testbed_win32 WIN32 ${GOT2D_TESTBED_SOURCE_FILES})
set_target_properties(got2d_testbed_win32 PROPERTIES COMPILE_DEFINITIONS UNICODE)
set_target_properties(got2d_testbed_win32 PROPERTIES COMPILE_DEFINITIONS _UNICODE)
get_target_property(TESTBED_OUTPUT_DIR got2d_testbed_win32 RUNTIME_OUTPUT_DIRECTORY) 
set_target_properties(got2d_testbed_win32 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${TESTBED_OUTPUT_DIR})
target_link_libraries(got2d_testbed_win32 got2d cxx winmm.lib)
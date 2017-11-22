#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for testrunerswitcher. It only defines a global include 
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()

set(testrunnerswitcher_c_files
./src/testmutex.c
)

set(testrunnerswitcher_h_files
./inc/testrunnerswitcher.h
./inc/testmutex.h
)

add_library(testrunnerswitcher ${testrunnerswitcher_c_files} ${testrunnerswitcher_h_files})

set_target_properties(testrunnerswitcher
               PROPERTIES
               FOLDER "test_tools") 

#these are the include folders
#the following "set" statetement exports across the project a global variable called TESTRUNNERSWITCHER_INC_FOLDER that expands to whatever needs to included when using tesrtrunnerswitcher library
set(TESTRUNNERSWITCHER_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using testrunnerswitcher header" FORCE)

include_directories(${TESTRUNNERSWITCHER_INC_FOLDER})

add_files_to_install("./testtools/testrunnerswitcher/inc/testrunnerswitcher.h")
add_files_to_install("./testtools/testrunnerswitcher/inc/testmutex.h")

if(WIN32)
else()
	install (TARGETS testrunnerswitcher DESTINATION lib)
endif (WIN32)

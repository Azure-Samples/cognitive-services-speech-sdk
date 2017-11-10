#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for ctest.
project(ctest)

option(skip_unittests "set skip_unittests to ON to skip unittests (default is OFF)[if possible, they are always build]" OFF)

cmake_minimum_required(VERSION 2.8.11)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
  endif()
else()
  set (CMAKE_C_STANDARD 99)
  set (CMAKE_CXX_STANDARD 11)
endif()

set(ctest_c_files
./src/ctest.c
)

set(ctest_h_files
./inc/ctest.h
./inc/ctest_macros.h)

#these are the include folders
#the following "set" statetement exports across the project a global variable called CTEST_INC_FOLDER that expands to whatever needs to included when using ctest library
set(CTEST_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using ctest lib" FORCE)

include_directories(${CTEST_INC_FOLDER})

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

add_library(
ctest ${ctest_c_files} ${ctest_h_files}
)

set_target_properties(ctest
               PROPERTIES
               FOLDER "test_tools")

if (NOT ${skip_unittests})
     add_subdirectory(unittests)
endif()

if(WIN32)
else()
    install (TARGETS ctest DESTINATION lib)
endif (WIN32)

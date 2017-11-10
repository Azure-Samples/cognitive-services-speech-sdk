#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

if(MSVC)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /IGNORE:4217")
set(CMAKE_SHARED_LINKER_FLAGS "$(CMAKE_SHARED_LINKER_FLAGS) /IGNORE:4217")
endif()

compileAsC99()
set(theseTestsName socketio_win32_unittests)
set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
../../adapters/socketio_win32.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "azure_c_shared_utility_tests")
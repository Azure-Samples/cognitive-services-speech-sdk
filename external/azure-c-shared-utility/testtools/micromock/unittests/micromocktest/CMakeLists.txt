#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for micromock unittests
cmake_minimum_required(VERSION 2.8.11)

set(theseTestsName micromock_unittests)

set(${theseTestsName}_cpp_files
stdafx.cpp
cmockvalueunittests.cpp
micromockcallcomparisonunittests.cpp
micromocktest.cpp
micromockvalidateargumentbufferunittests.cpp
nullargsstringificationunittests.cpp
#timediscretemicromocktestvoidvoid.cpp
#timediscretemicromocktest_stim.cpp
#timediscretemicromockwithreturnandparameters.cpp
)

set(${theseTestsName}_c_files
#hypothetic_module.c
#micromocktest_with_ctest.c
)

set(${theseTestsName}_h_files
#hypothetic_module.h
hypothetic_module_interfaces.h
stdafx.h
)


build_test_artifacts(${theseTestsName} ON)

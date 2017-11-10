#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

set(theseTestsName umock_c_negt_noint_tests)

set(${theseTestsName}_test_files
umock_c_negt_noint_tests.c
)

set(${theseTestsName}_c_files
../../src/umock_c_negative_tests.c
)

umockc_build_test_artifacts(${theseTestsName} ON)

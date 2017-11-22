#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

set(theseTestsName umock_c_gen_func_decl_inttests)

set(${theseTestsName}_test_files
umock_c_gen_func_decl_inttests.c
)

set(${theseTestsName}_c_files
../../src/umock_c.c
../../src/umockalloc.c
../../src/umockcall.c
../../src/umockcallrecorder.c
../../src/umocktypes.c
../../src/umocktypes_charptr.c
../../src/umocktypes_c.c
../../src/umocktypename.c
)

set(${theseTestsName}_h_files
)

umockc_build_test_artifacts(${theseTestsName} ON)

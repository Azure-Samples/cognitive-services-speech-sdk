#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for hmacsha256_unittests
cmake_minimum_required(VERSION 2.8.11)

compileAsC11()
set(theseTestsName hmacsha256_unittests)

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
../../src/hmacsha256.c
../../src/hmac.c
../../src/usha.c
../../src/sha1.c
../../src/sha224.c
../../src/sha384-512.c

../../src/gballoc.c
${LOCK_C_FILE}
../../src/buffer.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "azure_c_shared_utility_tests")

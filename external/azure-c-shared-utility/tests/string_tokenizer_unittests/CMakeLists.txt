#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for $string_tokenizer_unittests
cmake_minimum_required(VERSION 2.8.11)

compileAsC11()
set(theseTestsName string_tokenizer_unittests)

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
../../src/string_tokenizer.c

../../src/strings.c
../../src/crt_abstractions.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "azure_c_shared_utility_tests")

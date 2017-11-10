// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

static int x;

CTEST_BEGIN_TEST_SUITE(TestSuiteInitializeCleanupTests)

CTEST_SUITE_INITIALIZE()
{
    x = 42;
}

CTEST_SUITE_CLEANUP()
{
}

CTEST_FUNCTION(Test1)
{
    CTEST_ASSERT_ARE_EQUAL(int, 42, x);
}

CTEST_END_TEST_SUITE(TestSuiteInitializeCleanupTests)

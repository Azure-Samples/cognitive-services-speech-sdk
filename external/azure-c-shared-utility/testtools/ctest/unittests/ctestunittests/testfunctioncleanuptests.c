// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

static int x = 0;
static int y = 0;

CTEST_BEGIN_TEST_SUITE(TestFunctionCleanupTests)

CTEST_FUNCTION_INITIALIZE()
{
}

CTEST_FUNCTION_CLEANUP()
{
    x++;
}

CTEST_FUNCTION(Test1)
{
    CTEST_ASSERT_ARE_EQUAL(int, y, x);
    y++;
}

CTEST_FUNCTION(Test2)
{
    CTEST_ASSERT_ARE_EQUAL(int, y, x);
    y++;
}

CTEST_FUNCTION(Test3)
{
    CTEST_ASSERT_ARE_EQUAL(int, y, x);
    y++;
}

CTEST_END_TEST_SUITE(TestFunctionCleanupTests)

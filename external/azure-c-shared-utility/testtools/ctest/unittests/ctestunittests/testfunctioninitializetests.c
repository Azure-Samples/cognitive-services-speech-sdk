// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

static int x = 0;
static int y = 0;

CTEST_BEGIN_TEST_SUITE(TestFunctionInitializeTests)

CTEST_FUNCTION_INITIALIZE()
{
    x++;
}

CTEST_FUNCTION_CLEANUP()
{
}

CTEST_FUNCTION(Test1)
{
    y++;
    CTEST_ASSERT_ARE_EQUAL(int, y, x);
}

CTEST_FUNCTION(Test2)
{
    y++;
    CTEST_ASSERT_ARE_EQUAL(int, y, x);
}

CTEST_END_TEST_SUITE(TestFunctionInitializeTests)

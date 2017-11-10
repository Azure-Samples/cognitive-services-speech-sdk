// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

static size_t test_count;

CTEST_BEGIN_TEST_SUITE(testfunctioncleanupfailstests)

CTEST_SUITE_INITIALIZE()
{
    test_count = 0;
}

CTEST_SUITE_CLEANUP()
{
}

CTEST_FUNCTION_CLEANUP()
{
    if (test_count == 2)
    {
        CTEST_ASSERT_FAIL("it needs to fail");
    }
}

CTEST_FUNCTION(Test1)
{
    test_count++;
}

CTEST_FUNCTION(Test2)
{
    test_count++;
}

CTEST_FUNCTION(Test3)
{
    test_count++;
}

CTEST_END_TEST_SUITE(testfunctioncleanupfailstests)

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

CTEST_BEGIN_TEST_SUITE(whentestsuiteinitializefailstests)

CTEST_SUITE_INITIALIZE()
{
    CTEST_ASSERT_FAIL("it needs to fail");
}

CTEST_SUITE_CLEANUP()
{
}

CTEST_FUNCTION(Test1)
{
    
}

CTEST_END_TEST_SUITE(whentestsuiteinitializefailstests)

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

static int nFunction = 0;

CTEST_BEGIN_TEST_SUITE(whentestfunctioninitializefailstests)

CTEST_FUNCTION_INITIALIZE()
{
    /*make fail 2 out of 3 tests*/
    nFunction++;
    if (nFunction <= 2)
    {
        CTEST_ASSERT_FAIL("needs to fail");
    }
    
}

CTEST_FUNCTION(Test1)
{
}

CTEST_FUNCTION(Test2)
{

}

CTEST_FUNCTION(Test3)
{

}

CTEST_END_TEST_SUITE(whentestfunctioninitializefailstests)

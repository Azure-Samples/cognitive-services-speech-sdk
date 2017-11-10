// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
using namespace std;

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
BEGIN_TEST_SUITE(CMockValue_tests)

typedef int arrayOf3Int[3];
typedef int arrayOf4Int[4];

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = MicroMockCreateMutex();
    ASSERT_IS_NOT_NULL(g_testByTest);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    MicroMockDestroyMutex(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    if (!MicroMockAcquireMutex(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}

/*test a normal array*/
TEST_FUNCTION(CMockValue_TN_toString_1)
{
    arrayOf3Int a = { 1, 2, 3 };
    CMockValue<arrayOf3Int> b(a);

    ASSERT_ARE_EQUAL(char_ptr, "{1,2,3}", b.ToString().c_str());
}

/*test a NULL*/
TEST_FUNCTION(CMockValue_TN_toString_2)
{
    CMockValue<arrayOf3Int> b(NULL);

    ASSERT_ARE_EQUAL(char_ptr, "NULL", b.ToString().c_str());
}

/*test EqualTo from the same source*/
TEST_FUNCTION(CMockValue_TN_EqualTo_1)
{
    arrayOf3Int source = { 1, 2, 3 };
    CMockValue<arrayOf3Int> a(source);
    CMockValue<arrayOf3Int> b(source);

    ASSERT_ARE_EQUAL(int, true, b.EqualTo(&a));
    ASSERT_ARE_EQUAL(int, true, a.EqualTo(&b));
}

/*test EqualTo from different source*/
TEST_FUNCTION(CMockValue_TN_EqualTo_2)
{
    arrayOf3Int source1 = { 1, 2, 3 };
    arrayOf3Int source2 = { 1, 2, 3 };
    CMockValue<arrayOf3Int> a(source1);
    CMockValue<arrayOf3Int> b(source2);

    ASSERT_ARE_EQUAL(int, true, b.EqualTo(&a));
    ASSERT_ARE_EQUAL(int, true, a.EqualTo(&b));
}

/*test EqualTo from different source sizes*/
TEST_FUNCTION(CMockValue_TN_EqualTo_3)
{
    arrayOf4Int source1 = { 1, 2, 3, 4};
    arrayOf3Int source2 = { 1, 2, 3 };
    CMockValue<arrayOf4Int> a3(source1);
    CMockValue<arrayOf3Int> b4(source2);

    ASSERT_ARE_EQUAL(int, false, b4.EqualTo(&a3));
    ASSERT_ARE_EQUAL(int, false, a3.EqualTo(&b4));
}

/*test EqualTo from NULL and NULL*/
TEST_FUNCTION(CMockValue_TN_EqualTo_4)
{
    CMockValue<arrayOf3Int> a(NULL);
    CMockValue<arrayOf3Int> b(NULL);

    ASSERT_ARE_EQUAL(int, true, b.EqualTo(&a));
    ASSERT_ARE_EQUAL(int, true, a.EqualTo(&b));
}

/*test EqualTo from NULL and non-NULL*/
TEST_FUNCTION(CMockValue_TN_EqualTo_5)
{
    arrayOf3Int source = { 1, 2, 3 };
    CMockValue<arrayOf3Int> a(source);
    CMockValue<arrayOf3Int> b(NULL);

    ASSERT_ARE_EQUAL(int, false, b.EqualTo(&a));
    ASSERT_ARE_EQUAL(int, false, a.EqualTo(&b));
}


TEST_FUNCTION(CMockValue_TN_setValue_from_NULL_to_NULL)
{
    CMockValue<arrayOf3Int> a(NULL);
    a.SetValue(NULL);

    ASSERT_ARE_EQUAL(char_ptr, "NULL", a.ToString().c_str());
}

TEST_FUNCTION(CMockValue_TN_setValue_from_NULL_to_array)
{
    arrayOf3Int source1 = { 1, 2, 3 };
    CMockValue<arrayOf3Int> a(NULL);
    a.SetValue(source1);

    ASSERT_ARE_EQUAL(char_ptr, "{1,2,3}", a.ToString().c_str());
}

TEST_FUNCTION(CMockValue_TN_setValue_from_array_to_NULL)
{
    arrayOf3Int source1 = { 1, 2, 3 };
    CMockValue<arrayOf3Int> a(source1);
    a.SetValue(NULL);

    ASSERT_ARE_EQUAL(char_ptr, "NULL", a.ToString().c_str());
}

TEST_FUNCTION(CMockValue_TN_setValue_from_array_to_array)
{
    arrayOf3Int source1 = { 1, 2, 3 };
    arrayOf3Int source2 = { 4, 5, 6 };
    CMockValue<arrayOf3Int> a(source1);
    a.SetValue(source2);

    ASSERT_ARE_EQUAL(char_ptr, "{4,5,6}", a.ToString().c_str());
}

TEST_FUNCTION(CMockValue_TN_getValue_from_NULL_returns_NULL)
{
    CMockValue<arrayOf3Int> a(NULL);

    ASSERT_ARE_EQUAL(int, 1, ((void*)NULL==(void*)a.GetValue())?1:0);
}

TEST_FUNCTION(CMockValue_TN_getValue_from_something_returns_equal_array)
{
    arrayOf3Int source1 = { 1, 2, 3 };
    CMockValue<arrayOf3Int> a(source1);

    const int* v = a.GetValue();

    for (size_t i = 0; i < 3; i++)
    {
        ASSERT_ARE_EQUAL(int, v[i], source1[i]);
    }
}

TEST_FUNCTION(CMockValue_TN_setValue_from_array_to_array_does_not_destroy_original_array)
{
    arrayOf3Int source1 = { 1, 2, 3 };
    arrayOf3Int source2 = { 4, 5, 6 };
    CMockValue<arrayOf3Int> a(source1);
    a.SetValue(source2);

    ASSERT_ARE_EQUAL(char_ptr, "{4,5,6}", a.ToString().c_str());
    
    ASSERT_ARE_EQUAL(int, 1, source1[0]);
    ASSERT_ARE_EQUAL(int, 2, source1[1]);
    ASSERT_ARE_EQUAL(int, 3, source1[2]);

    ASSERT_ARE_EQUAL(int, 4, source2[0]);
    ASSERT_ARE_EQUAL(int, 5, source2[1]);
    ASSERT_ARE_EQUAL(int, 6, source2[2]);

}



END_TEST_SUITE(CMockValue_tests)
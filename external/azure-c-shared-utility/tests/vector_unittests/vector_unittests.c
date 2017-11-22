// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>
#include "testrunnerswitcher.h"

#include "azure_c_shared_utility/vector.h"

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

typedef struct VECTOR_UNITTEST_TAG
{
    size_t nValue1;
    long lValue2;
} VECTOR_UNITTEST;

static bool PredicateFunction(const VECTOR_UNITTEST* handle, const VECTOR_UNITTEST* otherHandle)
{
    return (handle->nValue1 == otherHandle->nValue1 && handle->lValue2 == otherHandle->lValue2);
}

VECTOR_HANDLE g_handle;

#define NUM_ITEM_PUSH_BACK      128

static TEST_MUTEX_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(Vector_UnitTests)

    TEST_SUITE_INITIALIZE(a)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(initialize)
    {
        g_handle = VECTOR_create(sizeof(VECTOR_UNITTEST));
    }

    TEST_FUNCTION_CLEANUP(cleans)
    {
        /// Clean Up
        VECTOR_clear(g_handle);
        VECTOR_destroy(g_handle);
    }

    /* Vector_Tests BEGIN */
    TEST_FUNCTION(Vector_push_Back_with_NULL_Vector_fails)
    {
        ///arrange
        VECTOR_UNITTEST vItem;

        ///act
        int result = VECTOR_push_back(NULL, &vItem, 1);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
    }

    TEST_FUNCTION(Vector_push_Back_with_NULL_Element_fails)
    {
        ///arrange

        ///act
        int result = VECTOR_push_back(g_handle, NULL, 1);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
    }

    TEST_FUNCTION(Vector_push_Back_with_Zero_numElement_fails)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        ///act
        int result = VECTOR_push_back(g_handle, &sItem, 0);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
    }

    TEST_FUNCTION(Vector_push_Back_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        ///act
        int result = VECTOR_push_back(g_handle, &sItem, 1);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);
    }

    TEST_FUNCTION(Vector_size_with_NULL_Vector_fails)
    {
        ///arrange

        ///act
        size_t num = VECTOR_size(NULL);

        ///assert
        ASSERT_ARE_EQUAL(size_t, 0, num);
    }

    TEST_FUNCTION(Vector_size_Empty_Success)
    {
        ///arrange

        ///act
        size_t num = VECTOR_size(g_handle);

        ///assert
        ASSERT_ARE_EQUAL(size_t, 0, num);
    }

    TEST_FUNCTION(Vector_size_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        ///act
        int result = VECTOR_push_back(g_handle, &sItem, 1);
        size_t num = VECTOR_size(g_handle);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(size_t, 1, num);
    }

    TEST_FUNCTION(Vector_Vector_Find_If_NULL_Vector_Fail)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        ///act
        VECTOR_UNITTEST* pfindItem = (VECTOR_UNITTEST*)VECTOR_find_if(NULL, (PREDICATE_FUNCTION)PredicateFunction, &sItem);

        ///assert
        ASSERT_IS_NULL(pfindItem);
    }

    TEST_FUNCTION(Vector_Vector_Find_If_NULL_Predicate_Func_Fail)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        ///act
        VECTOR_UNITTEST* pfindItem = (VECTOR_UNITTEST*)VECTOR_find_if(g_handle, NULL, &sItem);

        ///assert
        ASSERT_IS_NULL(pfindItem);
    }

    TEST_FUNCTION(Vector_Vector_Find_If_NULL_value_Fail)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        (void)VECTOR_push_back(g_handle, &sItem, 1);

        ///act
        VECTOR_UNITTEST* pfindItem = (VECTOR_UNITTEST*)VECTOR_find_if(g_handle, (PREDICATE_FUNCTION)PredicateFunction, NULL);

        ///assert
        ASSERT_IS_NULL(pfindItem);
    }

    TEST_FUNCTION(Vector_Vector_Find_If_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        ///act
        int result = VECTOR_push_back(g_handle, &sItem, 1);

        VECTOR_UNITTEST* pfindItem = (VECTOR_UNITTEST*)VECTOR_find_if(g_handle, (PREDICATE_FUNCTION)PredicateFunction, &sItem);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(size_t, sItem.nValue1, pfindItem->nValue1);
        ASSERT_ARE_EQUAL(long, sItem.lValue2, pfindItem->lValue2);
    }

    TEST_FUNCTION(Vector_Clear_NULL_Vector_Fail)
    {
        ///arrange

        ///act
        VECTOR_clear(NULL);

        ///assert
        // Make sure this clear doesn't crash
    }

    TEST_FUNCTION(Vector_Clear_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        int result = VECTOR_push_back(g_handle, &sItem, 1);
        result = VECTOR_push_back(g_handle, &sItem, 1);

        ///act
        VECTOR_clear(g_handle);

        size_t num = VECTOR_size(g_handle);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(size_t, 0, num);
    }

    TEST_FUNCTION(Vector_Element_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem1 = {1, 2};
        VECTOR_UNITTEST sItem2 = {3, 4};

        (void)VECTOR_push_back(g_handle, &sItem1, 1);
        (void)VECTOR_push_back(g_handle, &sItem2, 1);

        ///act
        VECTOR_UNITTEST* pResult = (VECTOR_UNITTEST*)VECTOR_element(g_handle, 1);

        ///assert
        ASSERT_IS_NOT_NULL(pResult);
        ASSERT_ARE_EQUAL(size_t, sItem2.nValue1, pResult->nValue1);
        ASSERT_ARE_EQUAL(long, sItem2.lValue2, pResult->lValue2);
    }

    TEST_FUNCTION(Vector_Element_NULL_Vector_Fail)
    {
        ///arrange

        ///act
        void* pResult = VECTOR_element(NULL, 1);

        ///assert
        ASSERT_IS_NULL(pResult);
    }

    TEST_FUNCTION(Vector_Element_Index_Larger_Than_Size_Fail)
    {
        ///arrange
        VECTOR_UNITTEST sItem = {1, 2};

        (void)VECTOR_push_back(g_handle, &sItem, 1);
        (void)VECTOR_push_back(g_handle, &sItem, 1);

        size_t num = VECTOR_size(g_handle);

        ///act
        VECTOR_UNITTEST* pResult = (VECTOR_UNITTEST*)VECTOR_element(g_handle, num+1);

        ///assert
        ASSERT_IS_NULL(pResult);
    }

    TEST_FUNCTION(Vector_Front_NULL_Vector_Fail)
    {
        ///arrange

        ///act
        void* pResult = VECTOR_front(NULL);

        ///assert
        ASSERT_IS_NULL(pResult);
    }

    TEST_FUNCTION(Vector_Front_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem1 = {1, 2};
        VECTOR_UNITTEST sItem2 = {3, 4};

        (void)VECTOR_push_back(g_handle, &sItem1, 1);
        (void)VECTOR_push_back(g_handle, &sItem2, 1);

        ///act
        VECTOR_UNITTEST* pResult = (VECTOR_UNITTEST*)VECTOR_front(g_handle);

        ///assert
        ASSERT_IS_NOT_NULL(pResult);
        ASSERT_ARE_EQUAL(size_t, sItem1.nValue1, pResult->nValue1);
        ASSERT_ARE_EQUAL(long, sItem1.lValue2, pResult->lValue2);
    }

    TEST_FUNCTION(Vector_Back_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem1 = {1, 2};
        VECTOR_UNITTEST sItem2 = {3, 4};
        VECTOR_UNITTEST sItem3 = {5, 6};

        (void)VECTOR_push_back(g_handle, &sItem1, 1);
        (void)VECTOR_push_back(g_handle, &sItem2, 1);
        (void)VECTOR_push_back(g_handle, &sItem3, 1);

        ///act
        VECTOR_UNITTEST* pResult = (VECTOR_UNITTEST*)VECTOR_back(g_handle);

        ///assert
        ASSERT_IS_NOT_NULL(pResult);
        ASSERT_ARE_EQUAL(size_t, sItem3.nValue1, pResult->nValue1);
        ASSERT_ARE_EQUAL(long, sItem3.lValue2, pResult->lValue2);
    }

    TEST_FUNCTION(Vector_Back_NULL_Vector_Fail)
    {
        ///arrange

        ///act
        void* pResult = VECTOR_back(NULL);

        ///assert
        ASSERT_IS_NULL(pResult);
    }

    TEST_FUNCTION(Vector_erase_with_NULL_Vector_Fail)
    {
        ///arrange
        VECTOR_UNITTEST vItem;

        ///act
        VECTOR_erase(NULL, &vItem, 1);

        ///assert
        // Make sure this erase doesn't crash
    }

    TEST_FUNCTION(Vector_erase_with_NULL_Element_fails)
    {
        ///arrange

        ///act
        VECTOR_erase(g_handle, NULL, 1);

        ///assert
        // Make sure this erase doesn't crash
    }

    TEST_FUNCTION(Vector_erase_with_NULL_NumElement_fails)
    {
        ///arrange
        VECTOR_UNITTEST sItem;

        ///act
        VECTOR_erase(g_handle, &sItem, 0);

        ///assert
        // Make sure this erase doesn't crash
    }

    TEST_FUNCTION(Vector_erase_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem1 = {1, 2};
        VECTOR_UNITTEST sItem2 = {3, 4};

        ///act
        int result = VECTOR_push_back(g_handle, &sItem1, 1);
        ASSERT_ARE_EQUAL(int, 0, result);

        result = VECTOR_push_back(g_handle, &sItem2, 1);
        ASSERT_ARE_EQUAL(int, 0, result);

        VECTOR_UNITTEST* pfindItem = (VECTOR_UNITTEST*)VECTOR_find_if(g_handle, (PREDICATE_FUNCTION)PredicateFunction, &sItem1);
        ASSERT_IS_NOT_NULL(pfindItem);
        VECTOR_erase(g_handle, pfindItem, 1);

        pfindItem = (VECTOR_UNITTEST*)VECTOR_find_if(g_handle, (PREDICATE_FUNCTION)PredicateFunction, &sItem2);
        ASSERT_IS_NOT_NULL(pfindItem);
        VECTOR_erase(g_handle, pfindItem, 1);

        size_t num = VECTOR_size(g_handle);

        ///assert
        ASSERT_ARE_EQUAL(size_t, 0, num);
    }

    TEST_FUNCTION(Vector_Multiple_push_back_Success)
    {
        ///arrange
        VECTOR_UNITTEST sItem1 = {1, 2};

        ///act
        for (size_t nIndex = 0; nIndex < NUM_ITEM_PUSH_BACK; nIndex++)
        {
            sItem1.nValue1++;
            sItem1.lValue2++;
            int result = VECTOR_push_back(g_handle, &sItem1, 1);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        VECTOR_UNITTEST* pResult = (VECTOR_UNITTEST*)VECTOR_back(g_handle);

        ///assert
        ASSERT_IS_NOT_NULL(pResult);
        ASSERT_ARE_EQUAL(size_t, sItem1.nValue1, pResult->nValue1);
        ASSERT_ARE_EQUAL(long, sItem1.lValue2, pResult->lValue2);
    }

    /* Vector_Tests END */

END_TEST_SUITE(Vector_UnitTests)

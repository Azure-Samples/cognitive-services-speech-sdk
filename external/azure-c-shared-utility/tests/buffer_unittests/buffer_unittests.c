// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE !!!!
//
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>
#include "umock_c.h"
#include "azure_c_shared_utility/buffer_.h"
#include "testrunnerswitcher.h"

static size_t currentmalloc_call = 0;
static size_t whenShallmalloc_fail = 0;

static size_t currentrealloc_call = 0;
static size_t whenShallrealloc_fail = 0;

void* my_gballoc_malloc(size_t size)
{
    void* result;
    currentmalloc_call++;
    if (whenShallmalloc_fail > 0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = malloc(size);
        }
    }
    else
    {
        result = malloc(size);
    }
    return result;
}

void* my_gballoc_realloc(void* ptr, size_t size)
{
    void* result;
    currentrealloc_call++;
    if (whenShallrealloc_fail > 0)
    {
        if (currentrealloc_call == whenShallrealloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = realloc(ptr, size);
        }
    }
    else
    {
        result = realloc(ptr, size);
    }

    return result;
}

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

#define ALLOCATION_SIZE             16
#define TOTAL_ALLOCATION_SIZE       32

unsigned char BUFFER_TEST_VALUE[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16};
unsigned char ADDITIONAL_BUFFER[] = {0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26};
unsigned char TOTAL_BUFFER[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26};

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(Buffer_UnitTests)

    TEST_SUITE_INITIALIZE(setsBufferTempSize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        umock_c_init(on_umock_c_error);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        umock_c_deinit();

        TEST_MUTEX_DESTROY(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(f)
    {
        if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }

        umock_c_reset_all_calls();

        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;

        currentrealloc_call = 0;
        whenShallrealloc_fail = 0;
    }

    TEST_FUNCTION_CLEANUP(cleans)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    /* Tests_SRS_BUFFER_07_001: [BUFFER_new shall allocate a BUFFER_HANDLE that will contain a NULL unsigned char*.] */
    TEST_FUNCTION(BUFFER_new_Succeed)
    {
        ///arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        
        ///act
        BUFFER_HANDLE g_hBuffer = BUFFER_new();

        ///assert
        ASSERT_IS_NOT_NULL(g_hBuffer);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_delete Tests BEGIN */
    /* Tests_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_delete_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        BUFFER_delete(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        //none
    }

    /* Tests_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_delete_Alloc_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        BUFFER_delete(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        //none
    }

    /* Tests_SRS_BUFFER_07_004: [BUFFER_delete shall not delete any BUFFER_HANDLE that is NULL.] */
    TEST_FUNCTION(BUFFER_delete_NULL_HANDLE_Succeed)
    {
        ///arrange

        ///act
        BUFFER_delete(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* BUFFER_pre_Build Tests BEGIN */
    /* Tests_SRS_BUFFER_07_005: [BUFFER_pre_build allocates size_t bytes of BUFFER_HANDLE and returns zero on success.] */
    TEST_FUNCTION(BUFFER_pre_build_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_malloc(ALLOCATION_SIZE));

        ///act
        int nResult = BUFFER_pre_build(g_hBuffer, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(size_t, BUFFER_length(g_hBuffer), ALLOCATION_SIZE);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
    /* Tests_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
    TEST_FUNCTION(BUFFER_pre_build_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        int nResult = BUFFER_pre_build(NULL, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        //none
    }

    /* Tests_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
    TEST_FUNCTION(BUFFER_pre_Size_Zero_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        umock_c_reset_all_calls();

        ///act
        int nResult = BUFFER_pre_build(g_hBuffer, 0);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
    TEST_FUNCTION(BUFFER_pre_build_HANDLE_NULL_Size_Zero_Fail)
    {
        ///arrange

        ///act
        int nResult = BUFFER_pre_build(NULL, 0);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_007: [BUFFER_pre_build shall return nonzero if the buffer has been previously allocated and is not NULL.] */
    /* Tests_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
    TEST_FUNCTION(BUFFER_pre_build_Multiple_Alloc_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_pre_build(g_hBuffer, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_pre_build(g_hBuffer, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_008: [BUFFER_build allocates size_t bytes, copies the unsigned char* into the buffer and returns zero on success.] */
    TEST_FUNCTION(BUFFER_build_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        g_hBuffer = BUFFER_new();

        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(size_t, BUFFER_length(g_hBuffer), ALLOCATION_SIZE);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(g_hBuffer), BUFFER_TEST_VALUE, ALLOCATION_SIZE));
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_009: [BUFFER_build shall return nonzero if handle is NULL ] */
    TEST_FUNCTION(BUFFER_build_NULL_HANDLE_Fail)
    {
        ///arrange

        ///act
        int nResult = BUFFER_build(NULL, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_01_001: [If size is positive and source is NULL, BUFFER_build shall return nonzero] */
    TEST_FUNCTION(BUFFER_build_Content_NULL_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        umock_c_reset_all_calls();

        ///act
        int nResult = BUFFER_build(g_hBuffer, NULL, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_01_002: [The size argument can be zero, in which case the underlying buffer held by the buffer instance shall be freed.] */
    TEST_FUNCTION(BUFFER_build_Size_Zero_non_NULL_buffer_Succeeds)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, 0);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_01_002: [The size argument can be zero, in which case the underlying buffer held by the buffer instance shall be freed.] */
    TEST_FUNCTION(BUFFER_build_Size_Zero_NULL_buffer_Succeeds)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        int nResult = BUFFER_build(g_hBuffer, NULL, 0);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
    TEST_FUNCTION(BUFFER_build_when_the_buffer_is_already_allocated_and_the_same_amount_of_bytes_is_needed_succeeds)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
    TEST_FUNCTION(BUFFER_build_when_the_buffer_is_already_allocated_and_more_bytes_are_needed_succeeds)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE - 1);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
    TEST_FUNCTION(BUFFER_build_when_the_buffer_is_already_allocated_and_less_bytes_are_needed_succeeds)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE - 1))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE - 1);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_unbuild Tests BEGIN */
    /* Tests_SRS_BUFFER_07_012: [BUFFER_unbuild shall clear the underlying unsigned char* data associated with the BUFFER_HANDLE this will return zero on success.] */
    TEST_FUNCTION(BUFFER_unbuild_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_unbuild(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_014: [BUFFER_unbuild shall return a nonzero value if BUFFER_HANDLE is NULL.] */
    TEST_FUNCTION(BUFFER_unbuild_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        int nResult = BUFFER_unbuild(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_015: [BUFFER_unbuild shall return a nonzero value if the unsigned char* referenced by BUFFER_HANDLE is NULL.] */
    TEST_FUNCTION(BUFFER_unbuild_Multiple_Alloc_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        nResult = BUFFER_unbuild(g_hBuffer);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_unbuild(g_hBuffer);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_enlarge Tests BEGIN */
    /* Tests_SRS_BUFFER_07_016: [BUFFER_enlarge shall increase the size of the unsigned char* referenced by BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_enlarge_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, 2 * ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_enlarge(g_hBuffer, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(size_t, BUFFER_length(g_hBuffer), TOTAL_ALLOCATION_SIZE);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
    /* Tests_SRS_BUFFER_07_018: [BUFFER_enlarge shall return a nonzero result if any error is encountered.] */
    TEST_FUNCTION(BUFFER_enlarge_NULL_HANDLE_Fail)
    {
        ///arrange

        ///act
        int nResult = BUFFER_enlarge(NULL, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
    /* Tests_SRS_BUFFER_07_018: [BUFFER_enlarge shall return a nonzero result if any error is encountered.] */
    TEST_FUNCTION(BUFFER_enlarge_Size_Zero_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_enlarge(g_hBuffer, 0);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_content Tests BEGIN */
    /* Tests_SRS_BUFFER_07_019: [BUFFER_content shall return the data contained within the BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_content_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        const unsigned char* content = NULL;
        nResult = BUFFER_content(g_hBuffer, &content);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, 0, memcmp(content, BUFFER_TEST_VALUE, ALLOCATION_SIZE));
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_020: [If the handle and/or content*is NULL BUFFER_content shall return nonzero.] */
    TEST_FUNCTION(BUFFER_content_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        const unsigned char* content = NULL;
        int nResult = BUFFER_content(NULL, &content);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_IS_NULL(content);

        ///cleanup

    }

    /* Tests_SRS_BUFFER_07_020: [If the handle and/or content*is NULL BUFFER_content shall return nonzero.] */
    TEST_FUNCTION(BUFFER_content_Char_NULL_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_content(g_hBuffer, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_size Tests BEGIN */
    /* Tests_SRS_BUFFER_07_021: [BUFFER_size shall place the size of the associated buffer in the size variable and return zero on success.] */
    TEST_FUNCTION(BUFFER_size_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        size_t size = 0;
        nResult = BUFFER_size(g_hBuffer, &size);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(size_t, size, ALLOCATION_SIZE);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());


        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_022: [BUFFER_size shall return a nonzero value for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_size_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        size_t size = 0;
        int nResult = BUFFER_size(NULL, &size);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_022: [BUFFER_size shall return a nonzero value for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_size_Size_t_NULL_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_size(g_hBuffer, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_append Tests BEGIN */
    /* Tests_SRS_BUFFER_07_024: [BUFFER_append concatenates b2 onto b1 without modifying b2 and shall return zero on success.] */
    TEST_FUNCTION(BUFFER_append_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        BUFFER_HANDLE hAppend = BUFFER_new();
        nResult = BUFFER_build(hAppend, ADDITIONAL_BUFFER, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE + ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_append(g_hBuffer, hAppend);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(g_hBuffer), TOTAL_BUFFER, TOTAL_ALLOCATION_SIZE));
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(hAppend), ADDITIONAL_BUFFER, ALLOCATION_SIZE));
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(hAppend);
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
    TEST_FUNCTION(BUFFER_append_HANDLE_NULL_Fail)
    {
        ///arrange
        BUFFER_HANDLE hAppend = BUFFER_new();
        int nResult = BUFFER_build(hAppend, ADDITIONAL_BUFFER, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_append(NULL, hAppend);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(hAppend);
    }

    /* Tests_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
    TEST_FUNCTION(BUFFER_append_APPEND_HANDLE_NULL_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_append(g_hBuffer, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    TEST_FUNCTION(BUFFER_prepend_APPEND_HANDLE1_NULL_Fail)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_prepend(g_hBuffer, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    TEST_FUNCTION(BUFFER_prepend_APPEND_HANDLE2_NULL_Fail)
    {
        ///arrange
        BUFFER_HANDLE hAppend = BUFFER_new();
        int nResult = BUFFER_build(hAppend, ADDITIONAL_BUFFER, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        nResult = BUFFER_prepend(NULL, hAppend);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(hAppend);
    }

    TEST_FUNCTION(BUFFER_prepend_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, ADDITIONAL_BUFFER, ALLOCATION_SIZE);
        BUFFER_HANDLE hAppend = BUFFER_new();
        nResult = BUFFER_build(hAppend, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_malloc(ALLOCATION_SIZE + ALLOCATION_SIZE));
        EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

        ///act
        nResult = BUFFER_prepend(g_hBuffer, hAppend);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(g_hBuffer), TOTAL_BUFFER, TOTAL_ALLOCATION_SIZE));
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(hAppend), BUFFER_TEST_VALUE, ALLOCATION_SIZE));

        //TOTAL_BUFFER
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(hAppend);
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_u_char Tests BEGIN */
    /* Tests_SRS_BUFFER_07_025: [BUFFER_u_char shall return a pointer to the underlying unsigned char*.] */
    TEST_FUNCTION(BUFFER_U_CHAR_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        (void)BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();
        
        ///act
        unsigned char* u = BUFFER_u_char(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, memcmp(u, BUFFER_TEST_VALUE, ALLOCATION_SIZE) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_026: [BUFFER_u_char shall return NULL for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_U_CHAR_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        ASSERT_IS_NULL(BUFFER_u_char(NULL));

        /// assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* BUFFER_length Tests BEGIN */
    /* Tests_SRS_BUFFER_07_027: [BUFFER_length shall return the size of the underlying buffer.] */
    TEST_FUNCTION(BUFFER_length_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        (void)BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        ///act
        size_t l = BUFFER_length(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(size_t, l, ALLOCATION_SIZE);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_028: [BUFFER_length shall return zero for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_length_HANDLE_NULL_Succeed)
    {
        ///arrange

        ///act
        size_t size = BUFFER_length(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(size_t, size, 0);
    }

    TEST_FUNCTION(BUFFER_Clone_Succeed)
    {
        ///arrange
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        (void)BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_malloc(ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        BUFFER_HANDLE hclone = BUFFER_clone(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(hclone), BUFFER_TEST_VALUE, ALLOCATION_SIZE) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(g_hBuffer);
        BUFFER_delete(hclone);
    }

    TEST_FUNCTION(BUFFER_Clone_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        BUFFER_HANDLE result = BUFFER_clone(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(result);
    }

    /*Tests_SRS_BUFFER_02_001: [If source is NULL then BUFFER_create shall return NULL.] */
    TEST_FUNCTION(BUFFER_create_with_NULL_source_fails)
    {
        ///arrange

        ///act
        BUFFER_HANDLE res = BUFFER_create(NULL, 0);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(res);

        ///cleanup
    }

    /*Tests_SRS_BUFFER_02_002: [Otherwise, BUFFER_create shall allocate memory to hold size bytes and shall copy from source size bytes into the newly allocated memory.] */
    /*Tests_SRS_BUFFER_02_004: [Otherwise, BUFFER_create shall return a non-NULL handle*/
    TEST_FUNCTION(BUFFER_create_happy_path)
    {
        ///arrange
        char c = '3';

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_malloc(1));

        ///act
        BUFFER_HANDLE res = BUFFER_create((const unsigned char*)&c, 1);

        ///assert
        ASSERT_IS_NOT_NULL(res);
        size_t howBig = BUFFER_length(res);
        ASSERT_ARE_EQUAL(size_t, 1, howBig);
        const unsigned char* data = BUFFER_u_char(res);
        ASSERT_ARE_EQUAL(uint8_t, '3', data[0]);

        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(res);
    }

    /*Tests_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.] */
    TEST_FUNCTION(BUFFER_create_fails_when_gballoc_fails_1)
    {
        ///arrange
        char c = '3';

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(1));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = 2;

        ///act
        BUFFER_HANDLE res = BUFFER_create((const unsigned char*)&c, 1);

        ///assert
        ASSERT_IS_NULL(res);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(res);
    }

    /*Tests_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.] */
    TEST_FUNCTION(BUFFER_create_fails_when_gballoc_fails_2)
    {
        ///arrange
        char c = '3';

        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        BUFFER_HANDLE res = BUFFER_create((const unsigned char*)&c, 1);

        ///assert
        ASSERT_IS_NULL(res);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        BUFFER_delete(res);
    }

END_TEST_SUITE(Buffer_UnitTests)

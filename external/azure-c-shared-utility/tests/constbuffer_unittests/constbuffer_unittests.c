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

//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE !!!!
//
#include "testrunnerswitcher.h"

static size_t currentmalloc_call = 0;
static size_t whenShallmalloc_fail = 0;

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

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#define ENABLE_MOCKS
#include "umock_c.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/gballoc.h"

#undef ENABLE_MOCKS
#include "azure_c_shared_utility/constbuffer.h"

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static const char* buffer1 = "le buffer no 1";
static const char* buffer2 = NULL;
static const char* buffer3 = "three";

#define BUFFER1_HANDLE (BUFFER_HANDLE)1
#define BUFFER1_u_char ((unsigned char*)buffer1)
#define BUFFER1_length strlen(buffer1)

#define BUFFER2_HANDLE (BUFFER_HANDLE)2
#define BUFFER2_u_char ((unsigned char*)buffer2)
#define BUFFER2_length ((size_t)0)

#define BUFFER3_HANDLE (BUFFER_HANDLE)3
#define BUFFER3_u_char ((unsigned char*)buffer3)
#define BUFFER3_length ((size_t)0)

unsigned char* my_BUFFER_u_char(BUFFER_HANDLE handle)
{
    unsigned char* result;
    if (handle == BUFFER1_HANDLE)
    {
        result = BUFFER1_u_char;
    }
    else
    {
        ASSERT_FAIL("who am I?");
    }
    return result;
}

size_t my_BUFFER_length(BUFFER_HANDLE handle)
{
    size_t result;
    if (handle == BUFFER1_HANDLE)
    {
        result = BUFFER1_length;
    }
    else
    {
        ASSERT_FAIL("who am I?");
    }
    return result;
}

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(constbuffer_unittests)

    TEST_SUITE_INITIALIZE(setsBufferTempSize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        umock_c_init(on_umock_c_error);

        REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
        REGISTER_GLOBAL_MOCK_HOOK(BUFFER_u_char, my_BUFFER_u_char);
        REGISTER_GLOBAL_MOCK_HOOK(BUFFER_length, my_BUFFER_length);
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

    }

    TEST_FUNCTION_CLEANUP(cleans)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    /*Tests_SRS_CONSTBUFFER_02_001: [If source is NULL and size is different than 0 then CONSTBUFFER_Create shall fail and return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_Create_with_invalid_args_fails)
    {
        ///arrange
        
        ///act
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(NULL, 1);

        ///assert
        ASSERT_IS_NULL(handle);

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_002: [Otherwise, CONSTBUFFER_Create shall create a copy of the memory area pointed to by source having size bytes.]*/
    /*Tests_SRS_CONSTBUFFER_02_004: [Otherwise CONSTBUFFER_Create shall return a non-NULL handle.]*/
    TEST_FUNCTION(CONSTBUFFER_Create_succeeds)
    {
        ///arrange

        ///act
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        /*this is the content*/
        STRICT_EXPECTED_CALL(gballoc_malloc(BUFFER1_length));

        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);

        ///assert
        ASSERT_IS_NOT_NULL(handle);
        /*testing the "copy"*/
        const CONSTBUFFER* content = CONSTBUFFER_GetContent(handle);
        ASSERT_ARE_EQUAL(size_t, BUFFER1_length, content->size);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER1_u_char, content->buffer, BUFFER1_length));
        /*testing that it is a copy and not a pointer assignment*/
        ASSERT_ARE_NOT_EQUAL(void_ptr, BUFFER1_u_char, content->buffer);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_009: [Otherwise, CONSTBUFFER_CreateFromBuffer shall return a non-NULL handle.]*/
    /*Tests_SRS_CONSTBUFFER_02_007: [Otherwise, CONSTBUFFER_CreateFromBuffer shall copy the content of buffer.]*/
    TEST_FUNCTION(CONSTBUFFER_CreateFromBuffer_succeeds)
    {
        ///arrange

        ///act

        STRICT_EXPECTED_CALL(BUFFER_length(BUFFER1_HANDLE));
        STRICT_EXPECTED_CALL(BUFFER_u_char(BUFFER1_HANDLE));
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        /*this is the content*/
        STRICT_EXPECTED_CALL(gballoc_malloc(BUFFER1_length));

        CONSTBUFFER_HANDLE handle = CONSTBUFFER_CreateFromBuffer(BUFFER1_HANDLE);

        ///assert
        ASSERT_IS_NOT_NULL(handle);
        /*testing the "copy"*/
        const CONSTBUFFER* content = CONSTBUFFER_GetContent(handle);
        ASSERT_ARE_EQUAL(size_t, BUFFER1_length, content->size);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER1_u_char, content->buffer, BUFFER1_length));
        /*testing that it is a copy and not a pointer assignment*/
        ASSERT_ARE_NOT_EQUAL(void_ptr, BUFFER1_u_char, content->buffer);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_008: [If copying the content fails, then CONSTBUFFER_CreateFromBuffer shall fail and return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_CreateFromBuffer_fails_when_malloc_fails_1)
    {
        ///arrange

        ///act
        STRICT_EXPECTED_CALL(BUFFER_length(BUFFER1_HANDLE));
        STRICT_EXPECTED_CALL(BUFFER_u_char(BUFFER1_HANDLE));

        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        /*this is the content*/
        whenShallmalloc_fail = 2;
        STRICT_EXPECTED_CALL(gballoc_malloc(BUFFER1_length));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);


        CONSTBUFFER_HANDLE handle = CONSTBUFFER_CreateFromBuffer(BUFFER1_HANDLE);

        ///assert
        ASSERT_IS_NULL(handle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_008: [If copying the content fails, then CONSTBUFFER_CreateFromBuffer shall fail and return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_CreateFromBuffer_fails_when_malloc_fails_2)
    {
        ///arrange

        ///act
        STRICT_EXPECTED_CALL(BUFFER_length(BUFFER1_HANDLE));
        STRICT_EXPECTED_CALL(BUFFER_u_char(BUFFER1_HANDLE));

        /*this is the handle*/
        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        CONSTBUFFER_HANDLE handle = CONSTBUFFER_CreateFromBuffer(BUFFER1_HANDLE);

        ///assert
        ASSERT_IS_NULL(handle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_006: [If buffer is NULL then CONSTBUFFER_CreateFromBuffer shall fail and return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_CreateFromBuffer_with_NULL_fails)
    {
        ///arrange

        ///act
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_CreateFromBuffer(NULL);

        ///assert
        ASSERT_IS_NULL(handle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_010: [The non-NULL handle returned by CONSTBUFFER_CreateFromBuffer shall have its ref count set to "1".]*/
    /*Tests_SRS_CONSTBUFFER_02_005: [The non-NULL handle returned by CONSTBUFFER_Create shall have its ref count set to "1".]*/
    /*Tests_SRS_CONSTBUFFER_02_017: [If the refcount reaches zero, then CONSTBUFFER_Destroy shall deallocate all resources used by the CONSTBUFFER_HANDLE.]*/
    TEST_FUNCTION(CONSTBUFFER_CreateFromBuffer_is_ref_counted_1)
    {
        ///arrange
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_CreateFromBuffer(BUFFER1_HANDLE);
        umock_c_reset_all_calls();
        ///act

        /*this is the content*/
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        CONSTBUFFER_Destroy(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_003: [If creating the copy fails then CONSTBUFFER_Create shall return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_Create_fails_when_malloc_fails_1)
    {
        ///arrange

        ///act
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        /*this is the content*/
        whenShallmalloc_fail = 2;
        STRICT_EXPECTED_CALL(gballoc_malloc(BUFFER1_length));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);

        ///assert
        ASSERT_IS_NULL(handle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_003: [If creating the copy fails then CONSTBUFFER_Create shall return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_Create_fails_when_malloc_fails_2)
    {
        ///arrange

        ///act
        /*this is the handle*/
        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);

        ///assert
        ASSERT_IS_NULL(handle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_005: [The non-NULL handle returned by CONSTBUFFER_Create shall have its ref count set to "1".]*/
    /*Tests_SRS_CONSTBUFFER_02_017: [If the refcount reaches zero, then CONSTBUFFER_Destroy shall deallocate all resources used by the CONSTBUFFER_HANDLE.]*/
    TEST_FUNCTION(CONSTBUFFER_Create_is_ref_counted_1)
    {
        ///arrange
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);
        umock_c_reset_all_calls();
        ///act

        /*this is the content*/
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        CONSTBUFFER_Destroy(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_002: [Otherwise, CONSTBUFFER_Create shall create a copy of the memory area pointed to by source having size bytes.]*/
    TEST_FUNCTION(CONSTBUFFER_Create_from_0_size_succeeds_1)
    {
        ///arrange

        ///act
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER2_u_char, BUFFER2_length);

        ///assert
        ASSERT_IS_NOT_NULL(handle);
        /*testing the "copy"*/
        const CONSTBUFFER* content = CONSTBUFFER_GetContent(handle);
        ASSERT_ARE_EQUAL(size_t, BUFFER2_length, content->size);
        /*testing that it is a copy and not a pointer assignment*/
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_002: [Otherwise, CONSTBUFFER_Create shall create a copy of the memory area pointed to by source having size bytes.]*/
    /*Tests_SRS_CONSTBUFFER_02_009: [Otherwise, CONSTBUFFER_CreateFromBuffer shall return a non-NULL handle.]*/
    TEST_FUNCTION(CONSTBUFFER_Create_from_0_size_succeeds_2)
    {
        ///arrange

        ///act
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER3_u_char, BUFFER3_length);

        ///assert
        ASSERT_IS_NOT_NULL(handle);
        /*testing the "copy"*/
        const CONSTBUFFER* content = CONSTBUFFER_GetContent(handle);
        ASSERT_ARE_EQUAL(size_t, BUFFER3_length, content->size);
        /*testing that it is a copy and not a pointer assignment*/
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_011: [If constbufferHandle is NULL then CONSTBUFFER_GetContent shall return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_GetContent_with_NULL_returns_NULL)
    {
        ///arrange

        ///act
        const CONSTBUFFER* content = CONSTBUFFER_GetContent(NULL);

        ///assert
        ASSERT_IS_NULL(content);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_012: [Otherwise, CONSTBUFFER_GetContent shall return a const CONSTBUFFER* that matches byte by byte the original bytes used to created the const buffer and has the same length.]*/
    TEST_FUNCTION(CONSTBUFFER_GetContent_succeeds_1)
    {
        ///arrange
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);
        umock_c_reset_all_calls();

        ///act
        const CONSTBUFFER* content = CONSTBUFFER_GetContent(handle);

        ///assert
        ASSERT_IS_NOT_NULL(content);
        /*testing the "copy"*/
        ASSERT_ARE_EQUAL(size_t, BUFFER1_length, content->size);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER1_u_char, content->buffer, BUFFER1_length));
        /*testing that it is a copy and not a pointer assignment*/
        ASSERT_ARE_NOT_EQUAL(void_ptr, BUFFER1_u_char, content->buffer);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_012: [Otherwise, CONSTBUFFER_GetContent shall return a const CONSTBUFFER* that matches byte by byte the original bytes used to created the const buffer and has the same length.]*/
    TEST_FUNCTION(CONSTBUFFER_GetContent_succeeds_2)
    {
        ///arrange
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(NULL, 0);
        umock_c_reset_all_calls();

        ///act
        const CONSTBUFFER* content = CONSTBUFFER_GetContent(handle);

        ///assert
        ASSERT_IS_NOT_NULL(content);
        /*testing the "copy"*/
        ASSERT_ARE_EQUAL(size_t, 0, content->size);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_013: [If constbufferHandle is NULL then CONSTBUFFER_Clone shall fail and return NULL.]*/
    TEST_FUNCTION(CONSTBUFFER_Clone_with_NULL_returns_NULL)
    {
        ///arrange

        ///act
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Clone(NULL);

        ///assert
        ASSERT_IS_NULL(handle);

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_014: [Otherwise, CONSTBUFFER_Clone shall increment the reference count and return constbufferHandle.]*/
    TEST_FUNCTION(CONSTBUFFER_Clone_increments_ref_count_1)
    {
        ///arrange
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);
        umock_c_reset_all_calls();

        ///act
        CONSTBUFFER_HANDLE clone = CONSTBUFFER_Clone(handle);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, handle, clone);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
        CONSTBUFFER_Destroy(clone);
    }

    /*Tests_SRS_CONSTBUFFER_02_014: [Otherwise, CONSTBUFFER_Clone shall increment the reference count and return constbufferHandle.]*/
    /*Tests_SRS_CONSTBUFFER_02_016: [Otherwise, CONSTBUFFER_Destroy shall decrement the refcount on the constbufferHandle handle.]*/
    /*Tests_SRS_CONSTBUFFER_02_017: [If the refcount reaches zero, then CONSTBUFFER_Destroy shall deallocate all resources used by the CONSTBUFFER_HANDLE.]*/
    TEST_FUNCTION(CONSTBUFFER_Clone_increments_ref_count_2)
    {
        ///arrange
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);
        CONSTBUFFER_HANDLE clone = CONSTBUFFER_Clone(handle);
        umock_c_reset_all_calls();

        ///act
        CONSTBUFFER_Destroy(clone); /*only a dec_Ref is expected here, so no effects*/

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        CONSTBUFFER_Destroy(handle);
    }

    /*Tests_SRS_CONSTBUFFER_02_014: [Otherwise, CONSTBUFFER_Clone shall increment the reference count and return constbufferHandle.]*/
    /*Tests_SRS_CONSTBUFFER_02_016: [Otherwise, CONSTBUFFER_Destroy shall decrement the refcount on the constbufferHandle handle.]*/
    /*Tests_SRS_CONSTBUFFER_02_017: [If the refcount reaches zero, then CONSTBUFFER_Destroy shall deallocate all resources used by the CONSTBUFFER_HANDLE.]*/
    TEST_FUNCTION(CONSTBUFFER_Clone_increments_ref_count_3)
    {
        ///arrange
        CONSTBUFFER_HANDLE handle = CONSTBUFFER_Create(BUFFER1_u_char, BUFFER1_length);
        CONSTBUFFER_HANDLE clone = CONSTBUFFER_Clone(handle);
        CONSTBUFFER_Destroy(handle); /*only a dec_Ref is expected here, so no effects*/
        umock_c_reset_all_calls();

        ///act
        /*this is the content*/
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        /*this is the handle*/
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        CONSTBUFFER_Destroy(clone);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_CONSTBUFFER_02_015: [If constbufferHandle is NULL then CONSTBUFFER_Destroy shall do nothing.]*/
    TEST_FUNCTION(CONSTBUFFER_Destroy_with_NULL_argument_does_nothing)
    {
        ///arrange
        
        ///act
        CONSTBUFFER_Destroy(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

END_TEST_SUITE(constbuffer_unittests)

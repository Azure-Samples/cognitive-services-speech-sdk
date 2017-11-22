// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stddef.h>

#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/condition.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/threadapi.h"

#define ENABLE_MOCKS

#include "umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif
    MOCKABLE_FUNCTION(, void*, gballoc_malloc, size_t, size);
    MOCKABLE_FUNCTION(, void, gballoc_free, void*, ptr);
#ifdef __cplusplus
}
#endif

#include "umock_c.h"

#define GBALLOC_H

void* real_gballoc_malloc(size_t size);
void* real_gballoc_calloc(size_t nmemb, size_t size);
void* real_gballoc_realloc(void* ptr, size_t size);
void real_gballoc_free(void* ptr);

#define TEST_LOCK_HANDLE    (LOCK_HANDLE)0x4443
#define TEST_TIME ((double)3600)
#define TEST_TIME_T ((time_t)TEST_TIME)
#define CONDITION_WAIT_MS   2000

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static bool malloc_will_fail = false;

TEST_DEFINE_ENUM_TYPE(COND_RESULT, COND_RESULT_VALUES)

#ifdef __cplusplus
extern "C" {
#endif

void* my_gballoc_malloc(size_t size)
{
    void* result = NULL;
        if (malloc_will_fail == false)
        {
        result = real_gballoc_malloc(size);
        }

    return result;
}

void my_gballoc_free(void* ptr)
{
    real_gballoc_free(ptr);
}

#ifdef __cplusplus
}
#endif

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

COND_RESULT Condition_Handle_ToString(COND_HANDLE handle)
{
    COND_RESULT result = COND_OK;
    if (handle == NULL)
    {
        result = COND_ERROR;
    }
    return result;
}

BEGIN_TEST_SUITE(Condition_UnitTests)

TEST_SUITE_INITIALIZE(a)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
}

TEST_SUITE_CLEANUP(b)
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
    malloc_will_fail = false;
}

TEST_FUNCTION_CLEANUP(cleans)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

// Tests_SRS_CONDITION_18_002: [ Condition_Init shall create and return a CONDITION_HANDLE ]
TEST_FUNCTION(Condition_Init_Success)
{
    //arrange
    COND_HANDLE handle = NULL;
    EXPECTED_CALL(gballoc_malloc(4));
    EXPECTED_CALL(gballoc_free(0));

    //act
    handle = Condition_Init();

    //assert
    ASSERT_IS_NOT_NULL(handle);

    //free
    Condition_Deinit(handle);
}

// Tests_SRS_CONDITION_18_001: [ Condition_Post shall return COND_INVALID_ARG if handle is NULL ]
TEST_FUNCTION(Condition_Post_Handle_NULL_Failure)
{
    //arrange
    COND_HANDLE handle = NULL;
    COND_RESULT result;

    //act
    result = Condition_Post(NULL);

    //assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_INVALID_ARG, result);

    //free
}

// Tests_SRS_CONDITION_18_003: [ Condition_Post shall return COND_OK if it succcessfully posts the condition ]
TEST_FUNCTION(Condition_Post_Handle_Succeed)
{
    //arrange
    COND_HANDLE handle = NULL;
    COND_RESULT result;

    EXPECTED_CALL(gballoc_malloc(4));
    EXPECTED_CALL(gballoc_free(0));

    handle = Condition_Init();

    //act
    result = Condition_Post(handle);

    //assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_OK, result);

    //free
    Condition_Deinit(handle);
}

// Tests_SRS_CONDITION_18_004: [ Condition_Wait shall return COND_INVALID_ARG if handle is NULL ]
TEST_FUNCTION(Condition_Wait_Handle_NULL_Fail)
{
    //arrange
    COND_RESULT result;
    LOCK_HANDLE lock;

    lock = Lock_Init();

    //act
    result = Condition_Wait(NULL, lock, 0);

    //assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_INVALID_ARG, result);

    //free
    Lock_Deinit(lock);
}

// Tests_SRS_CONDITION_18_005: [ Condition_Wait shall return COND_INVALID_ARG if lock is NULL and timeout_milliseconds is 0 ]
TEST_FUNCTION(Condition_Wait_LOCK_NULL_Fail)
{
    //arrange
    COND_HANDLE handle = NULL;
    COND_RESULT result;
    EXPECTED_CALL(gballoc_malloc(4));
    EXPECTED_CALL(gballoc_free(0));

    handle = Condition_Init();

    //act
    result = Condition_Wait(handle, NULL, 0);

    //assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_INVALID_ARG, result);

    //free
    Condition_Deinit(handle);
}

// Tests_SRS_CONDITION_18_006: [ Condition_Wait shall return COND_INVALID_ARG if lock is NULL and timeout_milliseconds is not 0. ]
TEST_FUNCTION(Condition_Wait_LOCK_NULL_Ms_Fail)
{
    //arrange
    COND_HANDLE handle = NULL;
    COND_RESULT result;
    EXPECTED_CALL(gballoc_malloc(4));
    EXPECTED_CALL(gballoc_free(0));

    handle = Condition_Init();

    //act
    result = Condition_Wait(handle, NULL, CONDITION_WAIT_MS);

    //assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_INVALID_ARG, result);

    //free
    Condition_Deinit(handle);
}

// Tests_SRS_CONDITION_18_007: [ Condition_Deinit will not fail if handle is NULL ]
TEST_FUNCTION(Condition_Deinit_Fail)
{
    //arrange
    COND_HANDLE handle = NULL;

    //act

    //assert

    //free
    Condition_Deinit(handle);
}

// Tests_SRS_CONDITION_18_008: [ Condition_Init shall return NULL if it fails to allocate the CONDITION_HANDLE ]
TEST_FUNCTION(Condition_Init_allocation_fail)
{
    // arrange
    COND_HANDLE handle = NULL;
    malloc_will_fail = true;

    EXPECTED_CALL(gballoc_malloc(4));

    // act
    handle = Condition_Init();

    //assert
    ASSERT_IS_NULL(handle);

    //free
}

// Tests_SRS_CONDITION_18_009: [ Condition_Deinit will deallocate handle if it is not NULL ]
TEST_FUNCTION(Condition_Deinit_deallocates_handle)
{
    // arrange
    COND_HANDLE handle = NULL;
    handle = Condition_Init();
    umock_c_reset_all_calls();
    EXPECTED_CALL(gballoc_free(NULL));

    // act
    Condition_Deinit(handle);

    // assert

}

typedef struct _tagLockAndConfition {
    LOCK_HANDLE lock;
    COND_HANDLE condition;
} LockAndCondition;

static int trigger_thread_proc(void *h)
{
    LockAndCondition *m = (LockAndCondition*)h;

    ThreadAPI_Sleep(50);
    Lock(m->lock);
    Condition_Post(m->condition);
    Unlock(m->lock);
    return 0;
}

THREAD_HANDLE trigger_after_50_ms(COND_HANDLE handle)
{
    THREAD_HANDLE th = NULL;

    ThreadAPI_Create(&th, trigger_thread_proc, handle);

    return th;
}

// Tests_SRS_CONDITION_18_010: [ Condition_Wait shall return COND_OK if the condition is triggered and timeout_milliseconds is 0 ]
TEST_FUNCTION(Condition_Wait_ok_on_trigger_and_zero_timeout)
{
    // arrange
    LockAndCondition m;
    m.condition = Condition_Init();
    m.lock = Lock_Init();
    
    // act
    THREAD_HANDLE th = trigger_after_50_ms(&m);
    Lock(m.lock);
    COND_RESULT result = Condition_Wait(m.condition, m.lock, 0);
    ThreadAPI_Join(th, NULL);
    Unlock(m.lock);

    // assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_OK, result);
    Lock_Deinit(m.lock);
    Condition_Deinit(m.condition);
    umock_c_reset_all_calls();
}

// Tests_SRS_CONDITION_18_011: [Condition_wait shall return COND_TIMEOUT if the condition is NOT triggered and timeout_milliseconds is not 0]
TEST_FUNCTION(Condition_Wait_timeout_when_not_triggered)
{
    // arrange
    LockAndCondition m; 
    m.condition = Condition_Init();
    m.lock = Lock_Init();

    // act
    Lock(m.lock);
    COND_RESULT result = Condition_Wait(m.condition, m.lock, 150);
    Unlock(m.lock);

    // assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_TIMEOUT, result);
    Lock_Deinit(m.lock);
    Condition_Deinit(m.condition);
}

// Tests_SRS_CONDITION_18_012: [ Condition_Wait shall return COND_OK if the condition is triggered and timeout_milliseconds is not 0 ]
// Tests_SRS_CONDITION_18_013: [ Condition_Wait shall accept relative timeouts ]
TEST_FUNCTION(Condition_Wait_ok_on_trigger_with_timeout)
{
    // arrange
    LockAndCondition m;
    m.condition = Condition_Init();
    m.lock = Lock_Init();

    // act
    THREAD_HANDLE th = trigger_after_50_ms(&m);
    Lock(m.lock);
    COND_RESULT result = Condition_Wait(m.condition, m.lock, 1000);
    Unlock(m.lock);
    ThreadAPI_Join(th, NULL);

    // assert
    ASSERT_ARE_EQUAL(COND_RESULT, COND_OK, result);
    Lock_Deinit(m.lock);
    Condition_Deinit(m.condition);
    umock_c_reset_all_calls();
}

END_TEST_SUITE(Condition_UnitTests);

/*if malloc is defined as gballoc_malloc at this moment, there'd be serious trouble*/
#define Lock(x) (LOCK_OK + gballocState - gballocState) /*compiler warning about constant in if condition*/
#define Unlock(x) (LOCK_OK + gballocState - gballocState)
#define Lock_Init() (LOCK_HANDLE)0x42
#define Lock_Deinit(x) (LOCK_OK + gballocState - gballocState)
#define gballoc_malloc real_gballoc_malloc
#define gballoc_realloc real_gballoc_realloc
#define gballoc_calloc real_gballoc_calloc
#define gballoc_free real_gballoc_free
#include "gballoc.c"
#undef Lock
#undef Unlock
#undef Lock_Init
#undef Lock_Deinit

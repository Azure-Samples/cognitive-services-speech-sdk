// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/uniqueid.h"

static TEST_MUTEX_HANDLE g_testByTest;

TEST_DEFINE_ENUM_TYPE(UNIQUEID_RESULT, UNIQUEID_RESULT_VALUES);

#define BUFFER_SIZE         37

static char* uidBuffer = NULL;
static char* uidBuffer2 = NULL;

BEGIN_TEST_SUITE(uniqueid_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    if (uidBuffer != NULL)
    {
        free(uidBuffer);
        uidBuffer = NULL;
    }

    if (uidBuffer2 != NULL)
    {
        free(uidBuffer2);
        uidBuffer2 = NULL;
    }

    TEST_MUTEX_RELEASE(g_testByTest);
}

/* UniqueId_Generate */
TEST_FUNCTION(UniqueId_Generate_UID_NULL_Fail)
{
    //Arrange

    //Act
    UNIQUEID_RESULT result = UniqueId_Generate(NULL, BUFFER_SIZE);

    //Assert
    ASSERT_ARE_EQUAL(UNIQUEID_RESULT, UNIQUEID_INVALID_ARG, result);
}

TEST_FUNCTION(UniqueId_Generate_Len_too_small_Fail)
{
    //Arrange
    char uid[BUFFER_SIZE];

    //Act
    UNIQUEID_RESULT result = UniqueId_Generate(uid, BUFFER_SIZE/2);

    //Assert
    ASSERT_ARE_EQUAL(UNIQUEID_RESULT, UNIQUEID_INVALID_ARG, result);
}

TEST_FUNCTION(UniqueId_Generate_Succeed)
{
    //Arrange
    char uid[BUFFER_SIZE];

    //Act
    UNIQUEID_RESULT result = UniqueId_Generate(uid, BUFFER_SIZE);

    //Assert
    ASSERT_ARE_EQUAL(UNIQUEID_RESULT, UNIQUEID_OK, result);
    ASSERT_ARE_EQUAL(size_t, 36, strlen(uid) );
}

END_TEST_SUITE(uniqueid_unittests)

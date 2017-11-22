// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
using namespace std;
TYPED_MOCK_CLASS(CTestAllArgsMock, CMock)
{
public:
    MOCK_METHOD_0(, UINT8, TestFunctionWithNoArgs);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_METHOD_1(, UINT8, TestFunctionWith1Arg, UINT8, arg1);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_METHOD_2(, UINT8, TestFunctionWith2Args, UINT8, arg1, UINT8, arg2);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_METHOD_3(, UINT8, TestFunctionWith3Args, UINT8, arg1, UINT8, arg2, UINT8, arg3);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_METHOD_4(, UINT8, TestFunctionWith4Args, UINT8, arg1, UINT8, arg2, UINT8, arg3, UINT8, arg4);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_METHOD_5(, UINT8, TestFunctionWith5Args, UINT8, arg1, UINT8, arg2, UINT8, arg3, UINT8, arg4, UINT8, arg5);
    MOCK_METHOD_END(UINT8, 0)

    MOCK_METHOD_6(, UINT8, TestFunctionWith6Args, UINT8, arg1, UINT8, arg2, UINT8, arg3, UINT8, arg4, UINT8, arg5, UINT8, arg6);
    MOCK_METHOD_END(UINT8, 0)
};

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

    BEGIN_TEST_SUITE(MicroMockCallComparisonUnitTests)

        // GetUnexpectedCalls

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

        TEST_FUNCTION(MicroMock_GetUnexpectedCalls_When_No_Actual_Call_Is_Made_Returns_An_Empty_String)
        {
            // arrange
            CTestAllArgsMock testMock;

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("")).c_str(), testMock.GetUnexpectedCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetUnexpectedCalls_Returns_Unexpected_Calls)
        {
            // arrange
            CTestAllArgsMock testMock;

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("[TestFunctionWithNoArgs()]")).c_str(), testMock.GetUnexpectedCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetUnexpectedCalls_When_An_Expected_Call_And_An_Actual_Call_Match_Returns_Empty_String)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs());

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("")).c_str(), testMock.GetUnexpectedCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetUnexpectedCalls_When_Only_The_Expected_Call_Happens_Returns_Empty_String)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs());

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("")).c_str(), testMock.GetUnexpectedCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetUnexpectedCalls_When_More_Than_One_Unexpected_Calls_Are_Made_Reports_All_Calls)
        {
            // arrange
            CTestAllArgsMock testMock;

            // act
            testMock.TestFunctionWithNoArgs();
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("[TestFunctionWithNoArgs()][TestFunctionWithNoArgs()]")).c_str(), testMock.GetUnexpectedCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        // GetMissingCalls

        TEST_FUNCTION(MicroMock_GetMissingCalls_When_No_Expected_Call_Is_Programmed_Returns_An_Empty_String)
        {
            // arrange
            CTestAllArgsMock testMock;

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("")).c_str(), testMock.GetMissingCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetMissingCalls_Returns_MissingCalls)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs());

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("[TestFunctionWithNoArgs()]")).c_str(), testMock.GetMissingCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetMissingCalls_When_An_Expected_Call_And_An_Actual_Call_Match_Returns_Empty_String)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs());

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("")).c_str(), testMock.GetMissingCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetMissingCalls_When_Only_The_Actual_Call_Happens_Returns_Empty_String)
        {
            // arrange
            CTestAllArgsMock testMock;

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("")).c_str(), testMock.GetMissingCalls().c_str(),
                _T("Incorrect actual calls"));
        }

        TEST_FUNCTION(MicroMock_GetMissingCalls_When_More_Than_One_Missing_Calls_Exists_Reports_All_Missing_Calls)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs());
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs());

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("[TestFunctionWithNoArgs()][TestFunctionWithNoArgs()]")).c_str(), testMock.GetMissingCalls().c_str(),
                _T("Incorrect actual calls"));
        }
        END_TEST_SUITE(MicroMockCallComparisonUnitTests);

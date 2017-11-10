// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "stdafx.h"

int c(int a, int b);
int d(int a, int b);

#ifdef _MSC_VER
TD_MOCK_CLASS(MyMocks2)
{
public:
    MOCK_STATIC_TD_METHOD_2(, int, c, int, a, int, b)
    MOCK_METHOD_END(int, 42)

    MOCK_STATIC_TD_METHOD_2(, int, d, int, a, int, b)
    MOCK_METHOD_END(int, 43)
};

DECLARE_GLOBAL_MOCK_METHOD_2(MyMocks2, , int, c, int, a, int, b)
DECLARE_GLOBAL_MOCK_METHOD_2(MyMocks2, , int, d, int, a, int, b)

static int iCall=0;
static void theTask(void)
{
    if(iCall==1)
    {
        c(2,3);
    }    
    if(iCall==10)
    {
        d(1,1);
        c(1,1);
        c(1,1);
    }
    iCall++;
    
}

static HANDLE g_hMutex=NULL;
#endif

BEGIN_TEST_SUITE(MicroMockTestWithReturnAndParameters)


#ifdef _MSC_VER

        TEST_SUITE_INITIALIZE(a)
        {
            g_hMutex = CreateMutex(NULL, FALSE, _T("onlyOneTestAtATime"));
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(void*, (HANDLE)NULL, g_hMutex, _T("Failed to create mutex for test by test"));
        }

        TEST_SUITE_CLEANUP(b)
        {
            (void)CloseHandle(g_hMutex);
        }

        /*missing lock doesn't work with CppUnitTests TEST_FUNCTION_CLEANUP/ TEST_FUNCTION_INITIALIZE*/
        #pragma warning(disable: 26135)
        TEST_FUNCTION_INITIALIZE(c)
        {
            if (WAIT_OBJECT_0 != WaitForSingleObject(g_hMutex, INFINITE))
            {
                ASSERT_FAIL(_T("Failed acquiring mutex for test by test."));
            }

            iCall = 0;
        }

        TEST_FUNCTION_CLEANUP(d)
        {
            if (ReleaseMutex(g_hMutex) == 0)
            {
                ASSERT_FAIL(_T("failure in test framework at ReleaseMutex"));
            }
        }

        TEST_FUNCTION(DiscreteMicroMockWithReturnAndParameters_ThereIsNothingCalledAtTimeZero)
        {
            // arrange
            MyMocks2 mocks(theTask);

            // act
            mocks.RunUntilTick(0);

            // assert
        }

        TEST_FUNCTION(DiscreteMicroMockWithReturnAndParameters_c_isCalledAtTimeOne)
        {
            // arrange
            MyMocks2 mocks(theTask);
            STRICT_EXPECTED_CALL_AT(mocks, 1, c(2,3));

            // act
            mocks.RunUntilTick(1);

            // assert
        }

        TEST_FUNCTION(DiscreteMicroMockWithReturnAndParameters_c_isNotCalledAtTimeTwo)
        {
            // arrange
            MyMocks2 mocks(theTask);
            STRICT_EXPECTED_CALL_AT(mocks, 1, c(2,3));

            // act
            mocks.RunUntilTick(2);

            // assert
        }

        TEST_FUNCTION(DiscreteMicroMockWithReturnAndParameters_InTheSameTickOrderOfCallsIsDisregarded_1)
        {
            ///arrange
            MyMocks2 mocks(theTask);
            STRICT_EXPECTED_CALL_AT(mocks, 1, c(2,3));
            STRICT_EXPECTED_CALL_AT(mocks, 10, c(1,1));
            STRICT_EXPECTED_CALL_AT(mocks, 10, c(1,1));
            STRICT_EXPECTED_CALL_AT(mocks, 10, d(1,1));

            ///act
            mocks.RunUntilTick(20);

            ///assert - left to uM_TD
        }

        TEST_FUNCTION(DiscreteMicroMockWithReturnAndParameters_InTheSameTickOrderOfCallsIsDisregarded_2)
        {
            ///arrange
            MyMocks2 mocks(theTask);
            STRICT_EXPECTED_CALL_AT(mocks, 1, c(2,3));
            STRICT_EXPECTED_CALL_AT(mocks, 10, d(1,1));
            STRICT_EXPECTED_CALL_AT(mocks, 10, c(1,1));
            STRICT_EXPECTED_CALL_AT(mocks, 10, c(1,1));

            ///act
            mocks.RunUntilTick(20);

            ///assert - left to uM_TD
        }

        TEST_FUNCTION(DiscreteMicroMockWithReturnAndParameters_WithoutStrictExpectedCallOnlyValidatesTheCall)
        {
            ///arrange
            MyMocks2 mocks(theTask);
            EXPECTED_CALL_AT(mocks, 1, c(72,73));
            EXPECTED_CALL_AT(mocks, 10, d(1234,14324));
            EXPECTED_CALL_AT(mocks, 10, c(1236,1234));
            EXPECTED_CALL_AT(mocks, 10, c(1235,61));

            ///act
            mocks.RunUntilTick(20);

            ///assert - left to uM_TD
        }
#endif
        END_TEST_SUITE(MicroMockTestWithReturnAndParameters)


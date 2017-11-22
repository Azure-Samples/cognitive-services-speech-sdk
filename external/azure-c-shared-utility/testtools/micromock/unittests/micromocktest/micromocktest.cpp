// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "azure_c_shared_utility/macro_utils.h"


using namespace std;

DEFINE_MICROMOCK_ENUM_TO_STRING(MICROMOCK_EXCEPTION, MICROMOCK_EXCEPTION_VALUES);

#define TEST_ORIGINAL_FUNCTION_RESULT   (42)
#define TEST_MOCK_FUNCTION_RESULT       (43)

void TestFunction()
{
}

void StaticTestFunction1()
{
}

void StaticTestFunction2()
{
}

UINT8 TestFunctionThatReturns42()
{
    return TEST_ORIGINAL_FUNCTION_RESULT;
}

void TestFunctionWithByteArg(UINT8 argument)
{
    UNREFERENCED_PARAMETER(argument);
}

typedef struct TEST_STRUCTURE_TAG
{
    UINT8 m_Value1;
    UINT32 m_Value2;
} TEST_STRUCTURE;

std::tostringstream& operator<< (std::tostringstream &out, TEST_STRUCTURE const &t)
{
    out << _T("{");
    out << (unsigned int)t.m_Value1;
    out << _T(",");
    out << (unsigned int)t.m_Value2;
    out << _T("}");
    return out;
}

static int UINT8_Compare(UINT8 a, UINT8 b)
{
    return (a == b) ? 0 : 1;
}
static void UINT8_ToString(char* string, size_t bufferSize, UINT8 val)
{
    sprintf(string, "%d", (int)val);
}

/*CTest spec says: tstring_Compare*/

static int tstring_Compare(const std::tstring& a, const std::tstring&b)
{
    return strcmp(a.c_str(), b.c_str());
}
static void tstring_ToString(char* string, size_t bufferSize, std::tstring val)
{
    size_t val_size = strlen(val.c_str());
    strncpy(string, val.c_str(), (val_size >= bufferSize) ? (bufferSize - 1) : val_size);
}

static int bool_Compare(bool a, bool b)
{
    return (a == b) ? 0 : 1;
}
static void bool_ToString(char* string, size_t bufferSize, bool val)
{
    sprintf(string, "%s", val?"true":"false");
}


bool operator==(const TEST_STRUCTURE& lhs, const TEST_STRUCTURE& rhs)
{
    return ((lhs.m_Value1 == rhs.m_Value1) &&
            (lhs.m_Value2 == rhs.m_Value2));
}

TYPED_MOCK_CLASS(CTestNoMethodsMock, CMock)
{
};

TYPED_MOCK_CLASS(CTestNoArgsNoReturnMethodMock, CMock)
{
public:
    MOCK_METHOD_0(, void, TestFunction)
    MOCK_VOID_METHOD_END()
};

#ifdef _MSC_VER
TYPED_MOCK_CLASS(CTestStaticNoArgsNoReturnMethodMock, CThreadSafeGlobalMock)
{
public:
    MOCK_STATIC_METHOD_0(, void, StaticTestFunction);
    MOCK_VOID_METHOD_END()
};
#endif

#ifdef _MSC_VER
TYPED_MOCK_CLASS(CTypedMock, CThreadSafeGlobalMock)
{
public:
    MOCK_STATIC_METHOD_1(, void, StaticTestFunctionWithByteArg, UINT8, argument1)
    MOCK_VOID_METHOD_END()
};
#endif

#ifdef _MSC_VER
TYPED_MOCK_CLASS(CTestCustomMock, CThreadSafeGlobalMock)
{
public:
    MOCK_STATIC_METHOD_1(, UINT8, StaticTestFunction, UINT8, argument1)
        if (40 == argument1)
        {
            return 42;
        }
    MOCK_METHOD_END(UINT8, 0)
};
#endif

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

static int gInt = 0;

TYPED_MOCK_CLASS(CTestStaticAllArgsMock, CGlobalMock)
{
public:
    MOCK_STATIC_METHOD_0(, UINT8, StaticTestFunctionWithNoArgs);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_STATIC_METHOD_1(, UINT8, StaticTestFunctionWith1Arg, UINT8, arg1);
    MOCK_METHOD_END(UINT8, 0)

    MOCK_STATIC_METHOD_1(, UINT8, StaticTestFunctionWith1Arg_Clone, UINT8, arg1);
        gInt++;
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_STATIC_METHOD_2(, UINT8, StaticTestFunctionWith2Args, UINT8, arg1, UINT8, arg2);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_STATIC_METHOD_3(, UINT8, StaticTestFunctionWith3Args, UINT8, arg1, UINT8, arg2, UINT8, arg3);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_STATIC_METHOD_4(, UINT8, StaticTestFunctionWith4Args, UINT8, arg1, UINT8, arg2, UINT8, arg3, UINT8, arg4);
    MOCK_METHOD_END(UINT8, 0)
    
    MOCK_STATIC_METHOD_5(, UINT8, StaticTestFunctionWith5Args, UINT8, arg1, UINT8, arg2, UINT8, arg3, UINT8, arg4, UINT8, arg5);
    MOCK_METHOD_END(UINT8, 0)

    MOCK_STATIC_METHOD_6(, UINT8, StaticTestFunctionWith6Args, UINT8, arg1, UINT8, arg2, UINT8, arg3, UINT8, arg4, UINT8, arg5, UINT8, arg6);
    MOCK_METHOD_END(UINT8, 0)
};

#ifdef _MSC_VER
TYPED_MOCK_CLASS(CTestStaticVariousArgsMock, CThreadSafeGlobalMock)
{
public:
    MOCK_STATIC_METHOD_1(, UINT8, StaticTestFunctionWithStructArg, TEST_STRUCTURE, testStructure);
    MOCK_METHOD_END(UINT8, 0)

    MOCK_STATIC_METHOD_1(, UINT8, StaticTestFunctionWithUINT16Arg, UINT16, arg1);
    MOCK_METHOD_END(UINT8, 0)
};
#endif

TYPED_MOCK_CLASS(CTestArgBufferMock, CMock)
{
public:
    MOCK_METHOD_1(, void, TestFunction, UINT8*, bufferArg);
    MOCK_VOID_METHOD_END()
};

#ifdef _MSC_VER
TYPED_MOCK_CLASS(CCustomMockCodeNotExecuted, CThreadSafeGlobalMock)
{
public:
    MOCK_STATIC_METHOD_0(, UINT8, StaticTestFunction);
        static UINT8 counter;
        counter++;
    MOCK_METHOD_END(UINT8, counter)
};
#endif

TYPED_MOCK_CLASS(CSimpleTestMock, CMock)
{
public:
    MOCK_METHOD_1(, UINT8, TestFunction, UINT8, arg1);
    MOCK_METHOD_END(UINT8, 0)
};

TYPED_MOCK_CLASS(CStaticTestMock, CGlobalMock)
{
public:
    MOCK_STATIC_METHOD_0(, void, StaticTestFunction);
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_0(CStaticTestMock, , void, StaticTestFunction);

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

    BEGIN_TEST_SUITE(MicroMockTest)

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
        gInt = 0;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

        tstring BytesToString(UINT8* buffer, size_t bufferLength)
        {
            tostringstream strStream;

            strStream << std::uppercase << std::hex;

            strStream << "[";

            for (size_t index = 0; index < bufferLength; index++)
            {
                if (index > 0)
                {
                    strStream << " ";
                }

                strStream << (unsigned int)buffer[index];
            }

            strStream << "]";

            return strStream.str();
        }

        TEST_FUNCTION(MicroMock_Instantiating_A_Mock_With_No_Methods_Succeeds)
        {
            // arrange

            // act
            CTestNoMethodsMock testNoArgsNoReturnMock;

            // assert
            // no explicit assert, no exception expected
        }

        TEST_FUNCTION(MicroMock_Creating_A_Mock_Object_Succeeds)
        {
            // arrange

            // act
            CTestNoArgsNoReturnMethodMock testMemoryMock;

            // assert
            // no explicit assert, no exception expected
        }

        TEST_FUNCTION(MicroMock_Comparing_Expected_And_Actual_Calls_When_No_Expected_And_Actual_Calls_Are_Made_Returns_No_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock;
            testNoArgsNoReturnMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            // act
            tstring result = testNoArgsNoReturnMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("")).c_str(), result.c_str(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_Strict_Expected_Call_With_No_Actual_Calls_Indicates_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock;
            testNoArgsNoReturnMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock, TestFunction());

            // act
            tstring result = testNoArgsNoReturnMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, tstring(_T("[Expected:TestFunction()]")).c_str(), result.c_str() ,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_Two_Strict_Expected_Calls_With_No_Actual_Calls_Indicates_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock;
            testNoArgsNoReturnMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock, TestFunction());
            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock, TestFunction());

            // act
            tstring result = testNoArgsNoReturnMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction()]")
                _T("[Expected:TestFunction()]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_Strict_Expected_Call_With_No_Actual_Calls_Checked_At_Mock_Level_Indicates_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock;
            testNoArgsNoReturnMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock, TestFunction());

            // act
            tstring result = testNoArgsNoReturnMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction()]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_On_One_Of_Two_Mocks_When_No_Actual_Calls_Are_Made_Indicates_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock1;
            testNoArgsNoReturnMock1.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock2;
            testNoArgsNoReturnMock2.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock1, TestFunction());

            // act
            tstring result = testNoArgsNoReturnMock1.CompareActualAndExpectedCalls() +
                testNoArgsNoReturnMock2.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction()]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_Strict_Expected_Call_On_One_Of_Two_Mocks_When_No_Actual_Calls_Are_Made_Indicates_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock1;
            testNoArgsNoReturnMock1.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock2;
            testNoArgsNoReturnMock2.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock1, TestFunction());

            // act
            tstring result = testNoArgsNoReturnMock1.CompareActualAndExpectedCalls() +
                testNoArgsNoReturnMock2.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction()]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_Strict_Expected_Call_On_Both_Mocks_When_No_Actual_Calls_Are_Made_Indicates_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock1;
            testNoArgsNoReturnMock1.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock2;
            testNoArgsNoReturnMock2.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock1, TestFunction());
            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock2, TestFunction());

            // act
            tstring result = testNoArgsNoReturnMock1.CompareActualAndExpectedCalls() +
                testNoArgsNoReturnMock2.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction()]")
                _T("[Expected:TestFunction()]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_Strict_Expected_Call_And_One_Actual_Call_Indicates_No_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock;
            testNoArgsNoReturnMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock, TestFunction());

            testNoArgsNoReturnMock.TestFunction();

            // act
            tstring result = testNoArgsNoReturnMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result, _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_Actual_Call_Without_An_Expected_Call_Indicates_A_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock;
            testNoArgsNoReturnMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            testNoArgsNoReturnMock.TestFunction();

            // act
            tstring result = testNoArgsNoReturnMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunction()]")),
                result, _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_One_Actual_Call_On_A_Different_Mock_Than_The_Expected_Call_Indicates_A_Mismatch)
        {
            // arrange
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock1;
            testNoArgsNoReturnMock1.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
            CTestNoArgsNoReturnMethodMock testNoArgsNoReturnMock2;
            testNoArgsNoReturnMock2.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testNoArgsNoReturnMock1, TestFunction());

            testNoArgsNoReturnMock2.TestFunction();

            // act
            tstring result = testNoArgsNoReturnMock1.CompareActualAndExpectedCalls() +
                testNoArgsNoReturnMock2.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction()]")
                _T("[Actual:TestFunction()]")),
                result, _T("Expected and actual calls do not match"));
        }

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_1_Of_3_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith3Args(42, 43, 44));

            CTestStaticAllArgsMock::StaticTestFunctionWith3Args(1, 43, 44);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith3Args(42,43,44)]")
                _T("[Actual:StaticTestFunctionWith3Args(1,43,44)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_2_Of_3_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith3Args(42, 43, 44));

            CTestStaticAllArgsMock::StaticTestFunctionWith3Args(42, 1, 44);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith3Args(42,43,44)]")
                _T("[Actual:StaticTestFunctionWith3Args(42,1,44)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_3_Of_3_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith3Args(42, 43, 44));

            CTestStaticAllArgsMock::StaticTestFunctionWith3Args(42, 43, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith3Args(42,43,44)]")
                _T("[Actual:StaticTestFunctionWith3Args(42,43,1)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_All_3_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith3Args(42, 43, 44));

            CTestStaticAllArgsMock::StaticTestFunctionWith3Args(42, 43, 44);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_1_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith4Args(42, 43, 44, 45));

            CTestStaticAllArgsMock::StaticTestFunctionWith4Args(1, 43, 44, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:StaticTestFunctionWith4Args(1,43,44,45)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_2_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith4Args(42, 43, 44, 45));

            CTestStaticAllArgsMock::StaticTestFunctionWith4Args(42, 1, 44, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:StaticTestFunctionWith4Args(42,1,44,45)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_3_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith4Args(42, 43, 44, 45));

            CTestStaticAllArgsMock::StaticTestFunctionWith4Args(42, 43, 1, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:StaticTestFunctionWith4Args(42,43,1,45)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_4_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith4Args(42, 43, 44, 45));

            CTestStaticAllArgsMock::StaticTestFunctionWith4Args(42, 43, 44, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:StaticTestFunctionWith4Args(42,43,44,1)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_All_4_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith4Args(42, 43, 44, 45));

            CTestStaticAllArgsMock::StaticTestFunctionWith4Args(42, 43, 44, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_1_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith5Args(42, 43, 44, 45, 46));

            CTestStaticAllArgsMock::StaticTestFunctionWith5Args(1, 43, 44, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:StaticTestFunctionWith5Args(1,43,44,45,46)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_2_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith5Args(42, 43, 44, 45, 46));

            CTestStaticAllArgsMock::StaticTestFunctionWith5Args(42, 1, 44, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:StaticTestFunctionWith5Args(42,1,44,45,46)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_3_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith5Args(42, 43, 44, 45, 46));

            CTestStaticAllArgsMock::StaticTestFunctionWith5Args(42, 43, 1, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:StaticTestFunctionWith5Args(42,43,1,45,46)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_4_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith5Args(42, 43, 44, 45, 46));

            CTestStaticAllArgsMock::StaticTestFunctionWith5Args(42, 43, 44, 1, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:StaticTestFunctionWith5Args(42,43,44,1,46)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_5_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith5Args(42, 43, 44, 45, 46));

            CTestStaticAllArgsMock::StaticTestFunctionWith5Args(42, 43, 44, 45, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:StaticTestFunctionWith5Args(42,43,44,45,1)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_All_5_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith5Args(42, 43, 44, 45, 46));

            CTestStaticAllArgsMock::StaticTestFunctionWith5Args(42, 43, 44, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_1_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            CTestStaticAllArgsMock::StaticTestFunctionWith6Args(1, 43, 44, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:StaticTestFunctionWith6Args(1,43,44,45,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_2_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            CTestStaticAllArgsMock::StaticTestFunctionWith6Args(42, 1, 44, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:StaticTestFunctionWith6Args(42,1,44,45,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_3_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            CTestStaticAllArgsMock::StaticTestFunctionWith6Args(42, 43, 1, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:StaticTestFunctionWith6Args(42,43,1,45,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_4_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            CTestStaticAllArgsMock::StaticTestFunctionWith6Args(42, 43, 44, 1, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:StaticTestFunctionWith6Args(42,43,44,1,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_5_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            CTestStaticAllArgsMock::StaticTestFunctionWith6Args(42, 43, 44, 45, 1, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:StaticTestFunctionWith6Args(42,43,44,45,1,47)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_Arg_6_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            CTestStaticAllArgsMock::StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:StaticTestFunctionWith6Args(42,43,44,45,46,1)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMockStatic_Method_When_All_6_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            CTestStaticAllArgsMock::StaticTestFunctionWith6Args(42, 43, 44, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

        TEST_FUNCTION(MicroMock_When_Arg_1_Of_3_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith3Args(42, 43, 44));

            testMock.TestFunctionWith3Args(1, 43, 44);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith3Args(42,43,44)]")
                _T("[Actual:TestFunctionWith3Args(1,43,44)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_2_Of_3_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith3Args(42, 43, 44));

            testMock.TestFunctionWith3Args(42, 1, 44);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith3Args(42,43,44)]")
                _T("[Actual:TestFunctionWith3Args(42,1,44)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_3_Of_3_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith3Args(42, 43, 44));

            testMock.TestFunctionWith3Args(42, 43, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith3Args(42,43,44)]")
                _T("[Actual:TestFunctionWith3Args(42,43,1)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_All_3_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith3Args(42, 43, 44));

            testMock.TestFunctionWith3Args(42, 43, 44);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_1_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith4Args(42, 43, 44, 45));

            testMock.TestFunctionWith4Args(1, 43, 44, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:TestFunctionWith4Args(1,43,44,45)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_2_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith4Args(42, 43, 44, 45));

            testMock.TestFunctionWith4Args(42, 1, 44, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:TestFunctionWith4Args(42,1,44,45)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_3_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith4Args(42, 43, 44, 45));

            testMock.TestFunctionWith4Args(42, 43, 1, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:TestFunctionWith4Args(42,43,1,45)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_4_Of_4_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith4Args(42, 43, 44, 45));

            testMock.TestFunctionWith4Args(42, 43, 44, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith4Args(42,43,44,45)]")
                _T("[Actual:TestFunctionWith4Args(42,43,44,1)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_All_4_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith4Args(42, 43, 44, 45));

            testMock.TestFunctionWith4Args(42, 43, 44, 45);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_1_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith5Args(42, 43, 44, 45, 46));

            testMock.TestFunctionWith5Args(1, 43, 44, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:TestFunctionWith5Args(1,43,44,45,46)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_2_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith5Args(42, 43, 44, 45, 46));

            testMock.TestFunctionWith5Args(42, 1, 44, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:TestFunctionWith5Args(42,1,44,45,46)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_3_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith5Args(42, 43, 44, 45, 46));

            testMock.TestFunctionWith5Args(42, 43, 1, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:TestFunctionWith5Args(42,43,1,45,46)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_4_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith5Args(42, 43, 44, 45, 46));

            testMock.TestFunctionWith5Args(42, 43, 44, 1, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:TestFunctionWith5Args(42,43,44,1,46)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_5_Of_5_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith5Args(42, 43, 44, 45, 46));

            testMock.TestFunctionWith5Args(42, 43, 44, 45, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith5Args(42,43,44,45,46)]")
                _T("[Actual:TestFunctionWith5Args(42,43,44,45,1)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_All_5_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith5Args(42, 43, 44, 45, 46));

            testMock.TestFunctionWith5Args(42, 43, 44, 45, 46);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_1_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            testMock.TestFunctionWith6Args(1, 43, 44, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:TestFunctionWith6Args(1,43,44,45,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_2_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            testMock.TestFunctionWith6Args(42, 1, 44, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:TestFunctionWith6Args(42,1,44,45,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_3_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            testMock.TestFunctionWith6Args(42, 43, 1, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:TestFunctionWith6Args(42,43,1,45,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_4_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            testMock.TestFunctionWith6Args(42, 43, 44, 1, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:TestFunctionWith6Args(42,43,44,1,46,47)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_5_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            testMock.TestFunctionWith6Args(42, 43, 44, 45, 1, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:TestFunctionWith6Args(42,43,44,45,1,47)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_Arg_6_Of_6_Is_Mismatched_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            testMock.TestFunctionWith6Args(42, 43, 44, 45, 46, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWith6Args(42,43,44,45,46,47)]")
                _T("[Actual:TestFunctionWith6Args(42,43,44,45,46,1)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_When_All_6_Args_Match_No_Mismatch_Is_Reported)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWith6Args(42, 43, 44, 45, 46, 47));

            testMock.TestFunctionWith6Args(42, 43, 44, 45, 46, 47);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_When_A_Structure_Is_Used_As_Argument_And_Member_1_Of_2_Is_Different_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticVariousArgsMock testMock;
            TEST_STRUCTURE expectedCallArgumentStruct = { 42, 43 };
            TEST_STRUCTURE actualCallArgumentStruct = { 1, 43 };

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWithStructArg(expectedCallArgumentStruct));

            CTestStaticVariousArgsMock::StaticTestFunctionWithStructArg(actualCallArgumentStruct);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWithStructArg({42,43})]")
                _T("[Actual:StaticTestFunctionWithStructArg({1,43})]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_When_A_Structure_Is_Used_As_Argument_And_Member_2_Of_2_Is_Different_A_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticVariousArgsMock testMock;
            TEST_STRUCTURE expectedCallArgumentStruct = { 42, 43 };
            TEST_STRUCTURE actualCallArgumentStruct = { 42, 1 };

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWithStructArg(expectedCallArgumentStruct));

            CTestStaticVariousArgsMock::StaticTestFunctionWithStructArg(actualCallArgumentStruct);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWithStructArg({42,43})]")
                _T("[Actual:StaticTestFunctionWithStructArg({42,1})]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_When_A_Structure_Is_Used_As_Argument_And_Members_Match_No_Call_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticVariousArgsMock testMock;
            TEST_STRUCTURE expectedCallArgumentStruct = { 42, 43 };
            TEST_STRUCTURE actualCallArgumentStruct = { 42, 43 };

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWithStructArg(expectedCallArgumentStruct));

            CTestStaticVariousArgsMock::StaticTestFunctionWithStructArg(actualCallArgumentStruct);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_When_A_Structure_Is_Used_As_Argument_And_Members_Match_But_Filling_Does_Not_No_Call_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticVariousArgsMock testMock;
            TEST_STRUCTURE expectedCallArgumentStruct;
            TEST_STRUCTURE actualCallArgumentStruct;

            memset(&expectedCallArgumentStruct, 0, sizeof(expectedCallArgumentStruct));
            memset(&actualCallArgumentStruct, 0xFF, sizeof(actualCallArgumentStruct));

            actualCallArgumentStruct.m_Value1 = expectedCallArgumentStruct.m_Value1 = 42;
            actualCallArgumentStruct.m_Value2 = expectedCallArgumentStruct.m_Value2 = 42;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWithStructArg(expectedCallArgumentStruct));

            CTestStaticVariousArgsMock::StaticTestFunctionWithStructArg(actualCallArgumentStruct);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_When_A_UINT16_Is_Used_As_Argument_And_Argument_Values_Match_No_Call_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticVariousArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWithUINT16Arg(4242));

            CTestStaticVariousArgsMock::StaticTestFunctionWithUINT16Arg(4242);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_When_A_UINT16_Is_Used_As_Argument_And_Argument_Values_Do_Not_Match_A_Call_Mismatch_Is_Reported)
        {
            // arrange
            CTestStaticVariousArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWithUINT16Arg(1));

            CTestStaticVariousArgsMock::StaticTestFunctionWithUINT16Arg(4242);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWithUINT16Arg(1)][Actual:StaticTestFunctionWithUINT16Arg(4242)]")),
                 result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Validate_First_Argument_For_Mock_Without_Arguments_Throws)
        {
            // arrange
            CTestStaticNoArgsNoReturnMethodMock testMock;
            bool exceptionWasThrown = false;

            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            ///act
            try
            {
                EXPECTED_CALL(testMock, StaticTestFunction())
                    .ValidateArgument(1);
            }
            catch(CMicroMockException &e)
            {
                ///assert
                exceptionWasThrown=true;
                ASSERT_ARE_EQUAL(MICROMOCK_EXCEPTION, MICROMOCK_EXCEPTION_INVALID_ARGUMENT, e.GetMicroMockExceptionCode());
            }
            catch(...)
            {
                ASSERT_FAIL(_T("it was expected a CMicroMockException, but something else was thrown"));
            }

            ASSERT_ARE_EQUAL_WITH_MSG(bool, true, exceptionWasThrown, _T("there was no exception thrown"));

        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Validating_Argument_Zero_Throws)
        {
            // arrange
            CTestStaticAllArgsMock testMock;
            bool exceptionWasThrown = false;

            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            try
            {
                EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(1))
                    .ValidateArgument(0);
            }
            catch(CMicroMockException &e)
            {
                ///assert
                exceptionWasThrown=true;
                ASSERT_ARE_EQUAL(MICROMOCK_EXCEPTION, MICROMOCK_EXCEPTION_INVALID_ARGUMENT, e.GetMicroMockExceptionCode());
            }
            catch(...)
            {
                ASSERT_FAIL(_T("it was expected a CMicroMockException, but something else was thrown"));
            }

            ASSERT_ARE_EQUAL_WITH_MSG(bool, true, exceptionWasThrown, _T("there was no exception thrown"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Validating_First_Argument_Checks_Expected_vs_Actual)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(1))
                .ValidateArgument(1);

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(2);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith1Arg(1)]")
                _T("[Actual:StaticTestFunctionWith1Arg(2)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Validating_First_Arg_When_Arg_1_Of_2_Is_Different_Indicates_A_Call_Mismatch)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith2Args(42, 43))
                .ValidateArgument(1);

            CTestStaticAllArgsMock::StaticTestFunctionWith2Args(1, 43);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith2Args(42,43)]")
                _T("[Actual:StaticTestFunctionWith2Args(1,43)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Validating_Second_Arg_When_Arg_2_Of_2_Is_Different_Indicates_A_Call_Mismatch)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith2Args(42, 43))
                .ValidateArgument(2);

            CTestStaticAllArgsMock::StaticTestFunctionWith2Args(42, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith2Args(42,43)]")
                _T("[Actual:StaticTestFunctionWith2Args(42,1)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Validating_First_Arg_When_Arg_2_Of_2_Is_Different_Indicates_No_Call_Mismatch)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith2Args(42, 43))
                .ValidateArgument(1);

            CTestStaticAllArgsMock::StaticTestFunctionWith2Args(42, 1);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_WhenCalled_Strict_Verifies_Arguments)
        {
            // arrange
            CTestStaticAllArgsMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            WHEN_CALLED_STRICT(testMock, StaticTestFunctionWith1Arg(42))
                .SetReturn((UINT8)42);

            // act
            UINT8 result = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(43);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)0, result, _T("Incorrect result value"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_WhenCalled_Strict_Gets_Correct_Return_Value_When_Arguments_Match)
        {
            // arrange
            CTestStaticAllArgsMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            WHEN_CALLED_STRICT(testMock, StaticTestFunctionWith1Arg(42))
                .SetReturn((UINT8)42);

            // act
            UINT8 result = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)42, result, _T("Incorrect result value"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Non_Strict_WhenCalled_Ignores_Arguments)
        {
            // arrange
            CTestStaticAllArgsMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            WHEN_CALLED(testMock, StaticTestFunctionWith1Arg(42))
                .SetReturn((UINT8)42);

            // act
            UINT8 result = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(43);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)42, result, _T("Incorrect result value"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetExpectedCalls_Without_Any_Expected_Calls_Does_Nothing)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            testMock.ResetExpectedCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetExpectedCalls_Clears_The_List_Of_Expected_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));

            testMock.ResetExpectedCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetExpectedCalls_With_One_Expected_And_One_Actual_Call_Reports_A_Call_Mismatch)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));

            testMock.ResetExpectedCalls();
            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:StaticTestFunctionWith1Arg(42)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_2_Consecutive_ResetExpectedCalls_Clear_The_Expected_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));

            testMock.ResetExpectedCalls();
            testMock.ResetExpectedCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_2_ResetExpectedCalls_Each_After_One_Expected_Call_Clear_The_Expected_Calls_List)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            testMock.ResetExpectedCalls();

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            testMock.ResetExpectedCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_After_A_ResetExpectedCalls_It_Is_Possible_To_Set_Expectations)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            testMock.ResetExpectedCalls();

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetExpectedCalls_Clears_2_Expected_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            testMock.ResetExpectedCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetActualCalls_Without_Any_Actual_Calls_Does_Nothing)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            testMock.ResetActualCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetActualCalls_Clears_The_List_Of_Actual_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetActualCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetActualCalls_With_One_Expected_And_One_Actual_Call_Reports_A_Call_Mismatch)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetActualCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:StaticTestFunctionWith1Arg(42)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_2_Consecutive_ResetActualCalls_Clear_The_Actual_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetActualCalls();
            testMock.ResetActualCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_2_ResetActualCalls_Each_After_One_Actual_Call_Clear_The_Actual_Calls_List)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetActualCalls();

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetActualCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_After_A_ResetActualCalls_It_Is_Possible_To_Set_Expectations)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetActualCalls();

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetActualCalls_Clears_2_Actual_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetActualCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetAllCalls_Clears_Expected_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(42));
            testMock.ResetAllCalls();

            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:StaticTestFunctionWith1Arg(42)]")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ResetAllCalls_Clears_Both_Actual_And_Expected_Calls)
        {
            // arrange
            CTestStaticAllArgsMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(43));
            CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(42);
            testMock.ResetAllCalls();

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_TypedMock_Instantiation_Succeeds)
        {
            // arrange

            // act
            CTypedMock testMock;

            // assert
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_Custom_Mocks_Execute_The_Mock_Method_Code)
        {
            // arrange
            CTestCustomMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunction(40));

            // act
            UINT8 result = CTestCustomMock::StaticTestFunction(40);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)42, result, _T("The custom mock code should get executed"));
        }
#endif

#ifdef _MSC_VER
        TEST_FUNCTION(MicroMock_ExpectedCall_Does_Not_Execute_Custom_Mock_Code)
        {
            // arrange
            CCustomMockCodeNotExecuted testMock;

            EXPECTED_CALL(testMock, StaticTestFunction());

            // act
            UINT8 result = CCustomMockCodeNotExecuted::StaticTestFunction();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)1, result, _T("The custom code in the mock should only be called once"));
        }
#endif

        TEST_FUNCTION(MicroMock_Instantiating_A_Mock_Method_With_An_Out_Argument_Succeeds)
        {
            // arrange

            // act
            CTestArgBufferMock testMock;

            // assert
            // no explicit assert, no exception expected
        }

        TEST_FUNCTION(MicroMock_Instantiating_A_Mock_With_Strict_Ordered_Call_Comparer_Succeeds)
        {
            // arrange

            // act
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            // assert
            // no explicit assert, no exception expected
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_With_Only_One_Expected_Call_Indicates_Call_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunction(42));

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction(42)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_With_Only_One_Actual_Call_Indicates_Call_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            testMock.TestFunction(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunction(42)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_With_One_Expected_Call_And_An_Actual_Call_Indicates_No_Call_Mismatches)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunction(42));
            testMock.TestFunction(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_With_An_Extra_Expected_Call_Before_2_Matching_Calls_Indicates_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            STRICT_EXPECTED_CALL(testMock, TestFunction(42));
            testMock.TestFunction(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction(41)][Expected:TestFunction(42)][Actual:TestFunction(42)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_With_An_Extra_Expected_Call_After_2_Matching_Calls_Indicates_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunction(42));
            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            testMock.TestFunction(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction(41)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_Between_2_Matching_Calls_Indicates_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            STRICT_EXPECTED_CALL(testMock, TestFunction(42));
            STRICT_EXPECTED_CALL(testMock, TestFunction(43));
            testMock.TestFunction(41);
            testMock.TestFunction(43);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunction(42)][Expected:TestFunction(43)]")
                _T("[Actual:TestFunction(43)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_One_Extra_Actual_Call_Indicates_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            testMock.TestFunction(41);
            testMock.TestFunction(43);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunction(43)]")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_Ignores_A_When_Called_Entry_Before_All_Expected_Calls)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            WHEN_CALLED(testMock, TestFunction(41))
                .SetReturn((UINT8)42);
            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            testMock.TestFunction(41);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_Ignores_A_When_Called_Entry_In_The_Middle_Of_Other_Expetced_Calls)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            WHEN_CALLED(testMock, TestFunction(43))
                .SetReturn((UINT8)42);
            STRICT_EXPECTED_CALL(testMock, TestFunction(42));
            testMock.TestFunction(41);
            testMock.TestFunction(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_Ignores_A_When_Called_Entry_After_All_Expected_Calls)
        {
            // arrange
            CSimpleTestMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            STRICT_EXPECTED_CALL(testMock, TestFunction(42));
            WHEN_CALLED(testMock, TestFunction(43))
                .SetReturn((UINT8)42);
            testMock.TestFunction(41);
            testMock.TestFunction(42);

            // act
            tstring result = testMock.CompareActualAndExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), result,
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_Uses_When_Called_Entries)
        {
            // arrange
            CSimpleTestMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            WHEN_CALLED(testMock, TestFunction(43))
                .SetReturn((UINT8)1);

            // act
            UINT8 result = testMock.TestFunction(43);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)1, result,
                _T("Incorrect result injected by the mock"));
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_Uses_When_Called_Entries_Even_When_Other_Expeted_Calls_Exist)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            WHEN_CALLED(testMock, TestFunction(43))
                .SetReturn((UINT8)1);

            // act
            UINT8 result = testMock.TestFunction(43);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)1, result,
                _T("Incorrect result injected by the mock"));
        }

        TEST_FUNCTION(MicroMock_Resetting_Expected_Calls_With_1_Actual_Call_Does_Clears_The_Matching_Of_Actual_Calls)
        {
            // arrange
            CSimpleTestMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunction(41));
            testMock.TestFunction(41);

            // act
            testMock.ResetExpectedCalls();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunction(41)]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_Nice_Call_Comparer_With_Automcatic_Comparison_Off_Can_Be_Instantiated)
        {
            // arrange

            // act
            CNiceCallComparer<CSimpleTestMock> testMock(AUTOMATIC_CALL_COMPARISON_OFF);

            // assert
            // no explicit assert
        }

        TEST_FUNCTION(MicroMock_StrictUnordered_Call_Comparer_With_Automcatic_Comparison_Off_Can_Be_Instantiated)
        {
            // arrange

            // act
            CStrictUnorderedCallComparer<CSimpleTestMock> testMock(AUTOMATIC_CALL_COMPARISON_OFF);

            // assert
            // no explicit assert
        }

        TEST_FUNCTION(MicroMock_StrictOrdered_Call_Comparer_With_Automcatic_Comparison_Off_Can_Be_Instantiated)
        {
            // arrange

            // act
            CStrictOrderedCallComparer<CSimpleTestMock> testMock(AUTOMATIC_CALL_COMPARISON_OFF);

            // assert
            // no explicit assert
        }

        TEST_FUNCTION(MicroMock_StrictOrderedCallComparer_Last_When_Called_Data_Is_Used_When_Multiple_Entries_Are_Available)
        {
            // arrange
            CStrictOrderedCallComparer<CSimpleTestMock> testMock(AUTOMATIC_CALL_COMPARISON_OFF);

            WHEN_CALLED(testMock, TestFunction(42))
                .SetReturn((UINT8)42);
            WHEN_CALLED(testMock, TestFunction(42))
                .SetReturn((UINT8)43);

            // act
            UINT8 result = testMock.TestFunction(42);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(UINT8, (UINT8)43, result, _T("Incorrect result injected by the mock"));
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunction(42)]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_With_Zero_Argument_Index_Does_Nothing)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 injectedOutBuffer[1] = {0x42};
            UINT8 actualArgumentBuffer[1] = {0};

            STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                .CopyOutArgumentBuffer(0, injectedOutBuffer, sizeof(injectedOutBuffer));

            // act
            testMock.TestFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[0]")), BytesToString(actualArgumentBuffer, sizeof(actualArgumentBuffer)),
                _T("No out argument buffer content should be copied"));
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_With_Argument_Index_Out_Of_Bounds_Does_Nothing)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 injectedOutBuffer[1] = {0x42};
            UINT8 actualArgumentBuffer[1] = {0};

            STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                .CopyOutArgumentBuffer(2, injectedOutBuffer, sizeof(injectedOutBuffer));

            // act
            testMock.TestFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[0]")), BytesToString(actualArgumentBuffer, sizeof(actualArgumentBuffer)),
                _T("No out argument buffer content should be copied"));
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_With_1_Byte_Payload_Copies_1_Byte)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 injectedOutBuffer[1] = {0x42};
            UINT8 actualArgumentBuffer[1] = {0};

            STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                .CopyOutArgumentBuffer(1, injectedOutBuffer, sizeof(injectedOutBuffer));

            // act
            testMock.TestFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[42]")), BytesToString(actualArgumentBuffer, sizeof(actualArgumentBuffer)),
                _T("1 byte of out argument buffer should be injected"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_With_NULL_Buffer_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            MICROMOCK_EXCEPTION exceptionCode = MICROMOCK_EXCEPTION_INTERNAL_ERROR; // anything that is different than the expected code

            try
            {
                // act
                STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                    .CopyOutArgumentBuffer(1, NULL, 1);
            }
            catch (CMicroMockException e)
            {
                exceptionCode = e.GetMicroMockExceptionCode();
            }

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)exceptionCode,
                _T("Expected an invalid argument exception"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_With_Zero_Byte_Count_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 injectedOutBuffer[1] = {0x42};
            MICROMOCK_EXCEPTION exceptionCode = MICROMOCK_EXCEPTION_INTERNAL_ERROR; // anything that is different than the expected code

            try
            {
                // act
                STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                    .CopyOutArgumentBuffer(1, injectedOutBuffer, 0);
            }
            catch (CMicroMockException e)
            {
                exceptionCode = e.GetMicroMockExceptionCode();
            }

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)exceptionCode,
                _T("Expected an invalid argument exception"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_Only_First_Byte_Out_Of_2_Bytes_Is_Copied)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 injectedOutBuffer[2] = {0x42, 0x42};
            UINT8 actualArgumentBuffer[2] = {0, 0};

            STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                .CopyOutArgumentBuffer(1, injectedOutBuffer, 1);

            // act
            testMock.TestFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[42 0]")), BytesToString(actualArgumentBuffer, sizeof(actualArgumentBuffer)),
                _T("Only the first byte should be copied"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_With_Offset_1_Copies_Starting_At_Offset_1)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 injectedOutBuffer[2] = {0x42, 0x42};
            UINT8 actualArgumentBuffer[2] = {0, 0};

            STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                .CopyOutArgumentBuffer(1, injectedOutBuffer, 1, 1);

            // act
            testMock.TestFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[0 42]")), BytesToString(actualArgumentBuffer, sizeof(actualArgumentBuffer)),
                _T("Only the second byte should be copied"));
        }

        TEST_FUNCTION(MicroMock_CopyOutArgumentBuffer_Makes_A_Copy_Of_The_Expected_Buffer)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 injectedArgumentBuffer[1] = {0x42};
            UINT8 actualArgumentBuffer[1] = {0};

            STRICT_EXPECTED_CALL(testMock, TestFunction(NULL))
                .CopyOutArgumentBuffer(1, injectedArgumentBuffer, sizeof(injectedArgumentBuffer));

            injectedArgumentBuffer[0] = 0x41;

            // act
            testMock.TestFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[42]")), BytesToString(actualArgumentBuffer, sizeof(actualArgumentBuffer)),
                _T("The initial expected injected buffer contents should be copied"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Time_With_Strict_Comparer_When_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Time_With_Strict_Comparer_When_No_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_Zero_Times_With_Strict_Comparer_When_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(0);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_Zero_Times_With_Strict_Comparer_When_No_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(0);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Times_With_Strict_Comparer_When_2_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Strict_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Strict_Comparer_When_2_Actual_Calls_Are_Made_Yields_No_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Strict_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()](x2)")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Time_With_Strict_Ordered_Comparer_When_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Time_With_Strict_Ordered_Comparer_When_No_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_Zero_Times_With_Strict_Ordered_Comparer_When_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(0);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_Zero_Times_With_Strict_Ordered_Comparer_When_No_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(0);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Times_With_Strict_Ordered_Comparer_When_2_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Strict_Ordered_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Strict_Ordered_Comparer_When_2_Actual_Calls_Are_Made_Yields_No_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Strict_Ordered_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()](x2)")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Time_With_Nice_Comparer_When_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Time_With_Nice_Comparer_When_No_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_Zero_Times_With_Nice_Comparer_When_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(0);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_Zero_Times_With_Nice_Comparer_When_No_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(0);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_One_Times_With_Nice_Comparer_When_2_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Nice_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Nice_Comparer_When_2_Actual_Calls_Are_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_2_Times_With_Nice_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(2);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()](x2)")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Time_With_Strict_Comparer_When_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Time_With_Strict_Comparer_When_No_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_Zero_Times_With_Strict_Comparer_When_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(0);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_Zero_Times_With_Strict_Comparer_When_No_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(0);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Times_With_Strict_Comparer_When_2_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Strict_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Strict_Comparer_When_2_Actual_Calls_Are_Made_Yields_No_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Strict_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()](x2)")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Time_With_Strict_Ordered_Comparer_When_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Time_With_Strict_Ordered_Comparer_When_No_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_Zero_Times_With_Strict_Ordered_Comparer_When_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(0);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_Zero_Times_With_Strict_Ordered_Comparer_When_No_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(0);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Times_With_Strict_Ordered_Comparer_When_2_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Strict_Ordered_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Strict_Ordered_Comparer_When_2_Actual_Calls_Are_Made_Yields_No_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Strict_Ordered_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()](x2)")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Time_With_Nice_Comparer_When_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Time_With_Nice_Comparer_When_No_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_Zero_Times_With_Nice_Comparer_When_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(0);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_Zero_Times_With_Nice_Comparer_When_No_Actual_Call_Is_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(0);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_One_Times_With_Nice_Comparer_When_2_Actual_Calls_Are_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Nice_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Nice_Comparer_When_2_Actual_Calls_Are_Made_Yields_No_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            testMock.TestFunctionWithNoArgs();

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_2_Times_With_Nice_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(2);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()](x2)")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_1_Times_On_2_Different_Calls_With_Strict_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()][Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_1_Times_On_2_Different_Calls_With_Strict_Ordered_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()][Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimes_1_Times_On_2_Different_Calls_With_Nice_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedAtLeastTimes(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()][Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_1_Times_On_2_Different_Calls_With_Strict_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()][Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_1_Times_On_2_Different_Calls_With_Strict_Ordered_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()][Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_1_Times_On_2_Different_Calls_With_Nice_Comparer_When_No_Actual_Calls_Are_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()][Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_1_Times_On_2_Different_Calls_With_Strict_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CTestAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_1_Times_On_2_Different_Calls_With_Strict_Ordered_Comparer_When_1_Actual_Call_Is_Made_Yields_Mismatch)
        {
            // arrange
            CStrictOrderedCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_ExpectedTimesExactly_1_Times_On_2_Different_Calls_With_Nice_Comparer_When_1_Actual_Calls_Is_Made_Yields_Mismatch)
        {
            // arrange
            CNiceCallComparer<CTestAllArgsMock> testMock;

            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);
            STRICT_EXPECTED_CALL(testMock, TestFunctionWithNoArgs())
                .ExpectedTimesExactly(1);

            // act
            testMock.TestFunctionWithNoArgs();

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:TestFunctionWithNoArgs()]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_Validating_Argument_Buffer_Zero_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 someRealUINT8=7;
            bool exceptionWasThrown = false;

            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            try
            {
                EXPECTED_CALL(testMock, TestFunction(IGNORED_PTR_ARG))
                    .ValidateArgumentBuffer(0, &someRealUINT8, 1);
            }
            catch(CMicroMockException &e)
            {
                ///assert
                exceptionWasThrown=true;
                ASSERT_ARE_EQUAL(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)e.GetMicroMockExceptionCode());
            }
            catch(...)
            {
                ASSERT_FAIL(_T("it was expected a CMicroMockException, but something else was thrown"));
            }

            ASSERT_ARE_EQUAL_WITH_MSG(bool, true, exceptionWasThrown, _T("there was no exception thrown"));
        }

        TEST_FUNCTION(MicroMock_Validating_Argument_Buffer_With_Too_Big_Argument_Number_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 someRealUINT8=7;
            bool exceptionWasThrown=false;

            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            try
            {
                EXPECTED_CALL(testMock, TestFunction(IGNORED_PTR_ARG))
                    .ValidateArgumentBuffer(2, &someRealUINT8, 1); /*2 is too big here*/
            }
            catch(CMicroMockException &e)
            {
                ///assert
                exceptionWasThrown=true;
                ASSERT_ARE_EQUAL(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)e.GetMicroMockExceptionCode());
            }
            catch(...)
            {
                ASSERT_FAIL(_T("it was expected a CMicroMockException, but something else was thrown"));
            }

            ASSERT_ARE_EQUAL_WITH_MSG(bool, true, exceptionWasThrown, _T("there was no exception thrown"));

        }

        TEST_FUNCTION(MicroMock_Ignores_A_Certain_FunctionCall_All_The_Time)
        {
            // arrange
            CStaticTestMock testMock;

            EXPECTED_CALL(testMock, StaticTestFunction())
                .IgnoreAllCalls();

            StaticTestFunction();
            StaticTestFunction();

            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_Ignores_A_Certain_FunctionCall_All_The_Time_With_Ordered_Call_Comparer)
        {
            // arrange
            CStrictOrderedCallComparer<CStaticTestMock> testMock;

            EXPECTED_CALL(testMock, StaticTestFunction())
                .IgnoreAllCalls();

            StaticTestFunction();
            StaticTestFunction();

            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_Does_Not_Check_Again_Calls_That_Have_Been_Matched)
        {
            // arrange
            CTestArgBufferMock testMock;
            unsigned char expectedArgumentBuffer[] = { 0x42 };
            unsigned char actualArgumentBuffer[] = { 0x42 };

            EXPECTED_CALL(testMock, TestFunction(IGNORED_PTR_ARG))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            testMock.TestFunction(actualArgumentBuffer);
            testMock.TestFunction(NULL);

            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunction(NULL)]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_Does_Not_Check_Again_Calls_That_Have_Been_Matched_StrictOrderedComparer)
        {
            // arrange
            CStrictOrderedCallComparer<CTestArgBufferMock> testMock;
            unsigned char expectedArgumentBuffer[] = { 0x42 };
            unsigned char actualArgumentBuffer[] = { 0x42 };

            EXPECTED_CALL(testMock, TestFunction(IGNORED_PTR_ARG))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            testMock.TestFunction(actualArgumentBuffer);
            testMock.TestFunction(NULL);

            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Actual:TestFunction(NULL)]")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_Does_Not_Check_Again_Calls_That_Have_Been_Matched_NiceCallComparer)
        {
            // arrange
            CNiceCallComparer<CTestArgBufferMock> testMock;
            unsigned char expectedArgumentBuffer[] = { 0x42 };
            unsigned char actualArgumentBuffer[] = { 0x42 };

            EXPECTED_CALL(testMock, TestFunction(IGNORED_PTR_ARG))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            testMock.TestFunction(actualArgumentBuffer);
            testMock.TestFunction(NULL);

            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Incorrect calls"));
        }

        TEST_FUNCTION(MicroMock_SetFailReturn_called_1_times_returns_fail_value)
        {

            ///arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(1))
                .ValidateArgument(1)
                .SetFailReturn(3);

            ///act
            int result = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(1);

            /// assert
            ASSERT_ARE_EQUAL(int, 3, result);
            testMock.AssertActualAndExpectedCalls();
        }

        TEST_FUNCTION(MicroMock_SetFailReturn_called_2_times_with_fail_at_two_succeeds)
        {

            ///arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(1))
                .ValidateArgument(1);

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(1))
                .ValidateArgument(1)
                .SetFailReturn(3);

            ///act
            int result1 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(1);
            int result2 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(1);

            /// assert
            ASSERT_ARE_EQUAL(int, 0, result1);
            ASSERT_ARE_EQUAL(int, 3, result2);
            testMock.AssertActualAndExpectedCalls();
        }

        TEST_FUNCTION(MicroMock_SetFailReturn_called_2_times_with_fail_at_one_succeeds)
        {

            ///arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(1))
                .ValidateArgument(1)
                .SetFailReturn(3);

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg(1))
                .ValidateArgument(1);
               

            ///act
            int result1 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(1);
            int result2 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg(1);

            /// assert
            ASSERT_ARE_EQUAL(int, 3, result1);
            ASSERT_ARE_EQUAL(int, 0, result2);
            testMock.AssertActualAndExpectedCalls();
        }

        TEST_FUNCTION(MicroMock_SetFailReturn_does_not_execute_the_mock_code)
        {

            ///arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg_Clone(1))
                .SetFailReturn(3);

            ///act
            int result1 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg_Clone(1);

            /// assert
            ASSERT_ARE_EQUAL(int, 3, result1);
            ASSERT_ARE_EQUAL(int, 0, gInt);
            testMock.AssertActualAndExpectedCalls();
        }

        TEST_FUNCTION(MicroMock_SetReturn_does_execute_the_mock_code)
        {

            ///arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg_Clone(1))
                .SetReturn(3);

            ///act
            int result1 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg_Clone(1);

            /// assert
            ASSERT_ARE_EQUAL(int, 3, result1);
            ASSERT_ARE_EQUAL(int, 1, gInt);
            testMock.AssertActualAndExpectedCalls();
        }

        TEST_FUNCTION(MicroMock_SetReturn_does_execute_the_mock_code_and_SetFailDoesNotExecuteMockCode)
        {

            ///arrange
            CTestStaticAllArgsMock testMock;

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg_Clone(1))
                .SetReturn(3);

            STRICT_EXPECTED_CALL(testMock, StaticTestFunctionWith1Arg_Clone(1))
                .SetFailReturn(5);


            ///act
            int result1 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg_Clone(1);
            int result2 = CTestStaticAllArgsMock::StaticTestFunctionWith1Arg_Clone(1);

            /// assert
            ASSERT_ARE_EQUAL(int, 3, result1);
            ASSERT_ARE_EQUAL(int, 5, result2);
            ASSERT_ARE_EQUAL(int, 1, gInt);
            testMock.AssertActualAndExpectedCalls();

        }

        END_TEST_SUITE(MicroMockTest)


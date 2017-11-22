// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"

using namespace std;

static int tstring_Compare(const std::tstring& a, const std::tstring&b)
{
    return strcmp(a.c_str(), b.c_str());
}
static void tstring_ToString(char* string, size_t bufferSize, std::tstring val)
{
    size_t val_size = strlen(val.c_str());
    strncpy(string, val.c_str(), (val_size >= bufferSize) ? (bufferSize - 1) : val_size);
}

TYPED_MOCK_CLASS(CTestArgBufferMock, CMock)
{
public:
    MOCK_METHOD_1(, void, Test1ArgFunction, UINT8*, bufferArg);
    MOCK_VOID_METHOD_END()

    MOCK_METHOD_2(, void, Test2ArgsFunction, UINT8*, buffer1Arg, UINT8*, buffer2Arg);
    MOCK_VOID_METHOD_END()
};

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

    BEGIN_TEST_SUITE(MicroMockValidateArgumentBufferTests)

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

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_Zero_Argument_Index_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer[1] = {42};
            MICROMOCK_EXCEPTION exceptionCode = MICROMOCK_EXCEPTION_INTERNAL_ERROR; // anything that is different than the expected code

            try
            {
                // act
                STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                    .ValidateArgumentBuffer(0, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));
            }
            catch (CMicroMockException e)
            {
                exceptionCode = e.GetMicroMockExceptionCode();
            }

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)exceptionCode,
                _T("Expected an invalid argument exception"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_Argument_Index_Out_Of_Bounds_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer[1] = {42};
            MICROMOCK_EXCEPTION exceptionCode = MICROMOCK_EXCEPTION_INTERNAL_ERROR; // anything that is different than the expected code

            try
            {
                // act
                STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                    .ValidateArgumentBuffer(2, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));
            }
            catch (CMicroMockException e)
            {
                exceptionCode = e.GetMicroMockExceptionCode();
            }

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)exceptionCode,
                _T("Expected an invalid argument exception"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_When_Expected_Memory_Matches_Indicates_No_Call_Mismatch)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[1] = {42};
            UINT8 actualArgumentBuffer[1] = {42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_When_Expected_Memory_Matches_And_Expetced_Call_Argument_Value_Is_NULL_Indicates_No_Call_Mismatch)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[1] = {42};
            UINT8 actualArgumentBuffer[1] = {42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_When_Expected_Memory_Does_Not_Match_Indicates_A_Call_Mismatch)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[1] = { 0x43 };
            UINT8 actualArgumentBuffer[1] = { 0x42 };

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([43])][Actual:Test1ArgFunction([42])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_When_Expected_Buffer_Is_NULL_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer[1] = {42};
            MICROMOCK_EXCEPTION exceptionCode = MICROMOCK_EXCEPTION_INTERNAL_ERROR; // anything that is different than the expected code

            try
            {
                // act
                STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                    .ValidateArgumentBuffer(1, NULL, sizeof(expectedArgumentBuffer));
            }
            catch (CMicroMockException e)
            {
                exceptionCode = e.GetMicroMockExceptionCode();
            }

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)exceptionCode,
                _T("Expected an invalid argument exception"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_0_Size_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer[1] = {42};
            MICROMOCK_EXCEPTION exceptionCode = MICROMOCK_EXCEPTION_INTERNAL_ERROR; // anything that is different than the expected code

            try
            {
                // act
                STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                    .ValidateArgumentBuffer(1, expectedArgumentBuffer, 0);
            }
            catch (CMicroMockException e)
            {
                exceptionCode = e.GetMicroMockExceptionCode();
            }

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(int, (int)MICROMOCK_EXCEPTION_INVALID_ARGUMENT, (int)exceptionCode,
                _T("Expected an invalid argument exception"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Only_Byte_1_Of_2_Checked_When_1st_Byte_Matches)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[2] = {42, 43};
            UINT8 actualArgumentBuffer[2] = {42, 42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, 1);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Only_Byte_1_Of_2_Checked_When_1st_Byte_Does_Not_Match)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[2] = {0x42, 0x42};
            UINT8 actualArgumentBuffer[2] = {0x43, 0x42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, 1);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([42])][Actual:Test1ArgFunction([43])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Only_Byte_2_Of_2_Checked_When_2nd_Byte_Matches)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {42};
            UINT8 actualArgumentBuffer[2] = {43, 42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, 1, 1);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Only_Byte_2_Of_2_Checked_When_2nd_Byte_Does_Not_Match)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0x42};
            UINT8 actualArgumentBuffer[2] = {0x42, 0x43};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, 1, 1);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([.. 42])][Actual:Test1ArgFunction([.. 43])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Makes_A_Copy_Of_The_Expected_Buffer)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[1] = {42};
            UINT8 actualArgumentBuffer[1] = {42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            expectedArgumentBuffer[0] = 41;

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_For_2_Bytes_Formats_Both_Bytes_Into_The_Assert_String)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[2] = {0x42, 0x42};
            UINT8 actualArgumentBuffer[2] = {0x42, 0x43};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, 2);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([42 42])][Actual:Test1ArgFunction([42 43])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_For_1_Digit_Values_Works)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0x2};
            UINT8 actualArgumentBuffer[] = {0x3};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([02])][Actual:Test1ArgFunction([03])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_Expected_0x0F_Formats_The_Argument_Buffer_Correctly)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0xF};
            UINT8 actualArgumentBuffer[] = {0x0};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([0F])][Actual:Test1ArgFunction([00])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_Expected_0xFF_Formats_The_Argument_Buffer_Correctly)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0xFF};
            UINT8 actualArgumentBuffer[] = {0x0};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([FF])][Actual:Test1ArgFunction([00])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_Actual_0x0F_Formats_The_Argument_Buffer_Correctly)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0x0};
            UINT8 actualArgumentBuffer[] = {0xF};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([00])][Actual:Test1ArgFunction([0F])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_Actual_0xFF_Formats_The_Argument_Buffer_Correctly)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0x00};
            UINT8 actualArgumentBuffer[] = {0xFF};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([00])][Actual:Test1ArgFunction([FF])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_For_2_Bytes_With_1_Digit_Values_Works)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0x2, 0x3};
            UINT8 actualArgumentBuffer[] = {0x3, 0x2};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([02 03])][Actual:Test1ArgFunction([03 02])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_For_2_Bytes_Containing_0x0F_And_0xFF_Outputs_The_Correct_Formatted_Buffer)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0xF, 0xFF};
            UINT8 actualArgumentBuffer[] = {0x0, 0x0};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, sizeof(expectedArgumentBuffer));

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([0F FF])][Actual:Test1ArgFunction([00 00])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_2_Arguments_Formats_Property_Both_Arguments)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgument1Buffer[] = {0x2};
            UINT8 actualArgument1Buffer[] = {0x3};
            UINT8 expectedArgument2Buffer[] = {0x4, 0x5};
            UINT8 actualArgument2Buffer[] = {0x5, 0x4};

            STRICT_EXPECTED_CALL(testMock, Test2ArgsFunction(expectedArgument1Buffer, expectedArgument2Buffer))
                .ValidateArgumentBuffer(1, expectedArgument1Buffer, sizeof(expectedArgument1Buffer))
                .ValidateArgumentBuffer(2, expectedArgument2Buffer, sizeof(expectedArgument2Buffer));

            // act
            testMock.Test2ArgsFunction(actualArgument1Buffer, actualArgument2Buffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test2ArgsFunction([02],[04 05])][Actual:Test2ArgsFunction([03],[05 04])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Expected and actual calls do not match"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_For_The_3rd_Byte_Issues_Two_Placeholders_In_The_Formatted_Argument)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer[] = {0x42};
            UINT8 actualArgumentBuffer[] = {0x42, 0x43, 0x44};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(expectedArgumentBuffer))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer, 1, 2);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([.. .. 42])][Actual:Test1ArgFunction([.. .. 44])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Something is wrong with argument buffer validations"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_2_Validation_Entries_When_Expected_And_Actual_Values_Match_Does_Not_Yield_Any_Call_Difference)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x43};
            UINT8 actualArgumentBuffer[] = {0x42, 0x43};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2), 1);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_2_Validation_Entries_When_First_Byte_Is_Different_Than_Expected_Yields_A_Call_Difference)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer1[] = {0x43};
            UINT8 expectedArgumentBuffer2[] = {0x42};
            UINT8 actualArgumentBuffer[] = {0x42, 0x42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2), 1);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([43 42])][Actual:Test1ArgFunction([42 42])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Something is wrong with argument buffer validations"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_With_2_Validation_Entries_When_Second_Byte_Is_Different_Than_Expected_Yields_A_Call_Difference)
        {
            // arrange
            CTestArgBufferMock testMock;
            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x43};
            UINT8 actualArgumentBuffer[] = {0x42, 0x42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2), 1);

            // act
            testMock.Test1ArgFunction(actualArgumentBuffer);

            // assert
            ASSERT_ARE_EQUAL_WITH_MSG(tstring, tstring(_T("[Expected:Test1ArgFunction([42 43])][Actual:Test1ArgFunction([42 42])]")), testMock.CompareActualAndExpectedCalls(),
                _T("Something is wrong with argument buffer validations"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_2_Times_On_The_Same_Byte_With_Different_Content_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x43};
            bool exceptionThrown = false;

            try
            {
                STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                    .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1))
                    .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2));
            }
            catch (CMicroMockException e)
            {
                exceptionThrown = (e.GetMicroMockExceptionCode() == MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS);
            }

            // act

            // assert
            ASSERT_IS_TRUE_WITH_MSG(exceptionThrown, _T("ValidateArgumentBuffer should assert/throw"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_On_The_Same_Byte_With_Same_Byte_Content_Does_Not_Throws)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2));

            // act

            // assert
            // no exception expected
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Overlapping_Validation_Buffers_Left_Side_The_Same_Throws_When_Bytes_Are_Different)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x43, 0x42};
            bool exceptionThrown = false;

            try
            {
                STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                    .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1), 1)
                    .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2), 1);
            }
            catch (CMicroMockException e)
            {
                exceptionThrown = (e.GetMicroMockExceptionCode() == MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS);
            }

            // act

            // assert
            ASSERT_IS_TRUE_WITH_MSG(exceptionThrown, _T("ValidateArgumentBuffer should assert/throw"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Overlapping_Validation_Buffers_Right_Side_The_Same_Throws_When_Bytes_Are_Different)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x42, 0x43};
            bool exceptionThrown = false;

            try
            {
                STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                    .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1), 1)
                    .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2), 0);
            }
            catch (CMicroMockException e)
            {
                exceptionThrown = (e.GetMicroMockExceptionCode() == MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS);
            }

            // act

            // assert
            ASSERT_IS_TRUE_WITH_MSG(exceptionThrown, _T("ValidateArgumentBuffer should assert/throw"));
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Overlapping_Validation_Buffer_Left_Side_The_Same_Does_Not_Thrown_When_Bytes_Are_The_Same)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x42, 0x42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1), 1)
                .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2), 1);

            // act

            // assert
            // no exception expected
        }

        TEST_FUNCTION(MicroMock_ValidateArgumentBuffer_Overlapping_Validation_Buffer_Right_Side_The_Same_Does_Not_Thrown_When_Bytes_Are_The_Same)
        {
            // arrange
            CTestArgBufferMock testMock;
            testMock.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            UINT8 expectedArgumentBuffer1[] = {0x42};
            UINT8 expectedArgumentBuffer2[] = {0x42, 0x42};

            STRICT_EXPECTED_CALL(testMock, Test1ArgFunction(NULL))
                .ValidateArgumentBuffer(1, expectedArgumentBuffer1, sizeof(expectedArgumentBuffer1), 1)
                .ValidateArgumentBuffer(1, expectedArgumentBuffer2, sizeof(expectedArgumentBuffer2), 0);

            // act

            // assert
            // no exception expected
        }
        END_TEST_SUITE(MicroMockValidateArgumentBufferTests)


// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"
#include "stdio.h"

const TEST_FUNCTION_DATA* g_CurrentTestFunction;
jmp_buf g_ExceptionJump;
size_t RunTests(const TEST_FUNCTION_DATA* testListHead, const char* testSuiteName)
{
    size_t totalTestCount = 0;
    size_t failedTestCount = 0;
    const TEST_FUNCTION_DATA* currentTestFunction = (const TEST_FUNCTION_DATA*)testListHead->NextTestFunctionData;
    const TEST_FUNCTION_DATA* testSuiteInitialize = NULL;
    const TEST_FUNCTION_DATA* testSuiteCleanup = NULL;
    const TEST_FUNCTION_DATA* testFunctionInitialize = NULL;
    const TEST_FUNCTION_DATA* testFunctionCleanup = NULL;

    g_CurrentTestFunction = NULL;
    int testSuiteInitializeFailed = 0;

    (void)printf(" === Executing test suite %s ===\n", testSuiteName);

    while (currentTestFunction->TestFunction != NULL)
    {
        if (currentTestFunction->FunctionType == CTEST_TEST_FUNCTION_INITIALIZE)
        {
            testFunctionInitialize = currentTestFunction;
        }

        if (currentTestFunction->FunctionType == CTEST_TEST_FUNCTION_CLEANUP)
        {
            testFunctionCleanup = currentTestFunction;
        }

        if (currentTestFunction->FunctionType == CTEST_TEST_SUITE_INITIALIZE)
        {
            testSuiteInitialize = currentTestFunction;
        }

        if (currentTestFunction->FunctionType == CTEST_TEST_SUITE_CLEANUP)
        {
            testSuiteCleanup = currentTestFunction;
        }

        currentTestFunction = (TEST_FUNCTION_DATA*)currentTestFunction->NextTestFunctionData;
    }

    if (testSuiteInitialize != NULL)
    {
        if (setjmp(g_ExceptionJump) == 0)
        {
            testSuiteInitialize->TestFunction();
        }
        else
        {
            testSuiteInitializeFailed = 1;
            (void)printf("TEST_SUITE_INITIALIZE failed - suite ending\n");
        }
    }

    if (testSuiteInitializeFailed == 1)
    {
        /* print results */
        (void)printf("0 tests ran, ALL failed, NONE succeeded.\n");
        failedTestCount = 1;
    }
    else
    {
        unsigned int is_test_runner_ok = 1;

        currentTestFunction = (const TEST_FUNCTION_DATA*)testListHead->NextTestFunctionData;
        while (currentTestFunction->TestFunction != NULL)
        {
            if (currentTestFunction->FunctionType == CTEST_TEST_FUNCTION)
            {
                if (is_test_runner_ok == 1)
                {
                    int testFunctionInitializeFailed = 0;

                    if (testFunctionInitialize != NULL)
                    {
                        if (setjmp(g_ExceptionJump) == 0)
                        {
                            testFunctionInitialize->TestFunction();
                        }
                        else
                        {
                            testFunctionInitializeFailed = 1;
                            (void)printf("TEST_FUNCTION_INITIALIZE failed - next TEST_FUNCTION will fail\n");
                        }
                    }


                    if (testFunctionInitializeFailed)
                    {
                        *currentTestFunction->TestResult = TEST_FAILED;
                        (void)printf("Not executing test %s ...\n", currentTestFunction->TestFunctionName);
                    }
                    else
                    {
                        (void)printf("Executing test %s ...\n", currentTestFunction->TestFunctionName);

                        g_CurrentTestFunction = currentTestFunction;

                        if (setjmp(g_ExceptionJump) == 0)
                        {
                            currentTestFunction->TestFunction();
                        }
                        else
                        {
                            /*can only get here if there was a longjmp called while executing currentTestFunction->TestFunction();*/
                            /*we don't do anything*/
                        }
                        g_CurrentTestFunction = NULL;/*g_CurrentTestFunction is limited to actually executing a TEST_FUNCTION, otherwise it should be NULL*/

                        /*in the case when the cleanup can assert... have to prepare the long jump*/
                        if (setjmp(g_ExceptionJump) == 0)
                        {
                            if (testFunctionCleanup != NULL)
                            {
                                testFunctionCleanup->TestFunction();
                            }
                        }
                        else
                        {
                            /* this is a fatal error, if we got a fail in cleanup we can't do much */
                            *currentTestFunction->TestResult = TEST_FAILED;
                            is_test_runner_ok = 0;
                        }
                    }
                }
                else
                {
                    *currentTestFunction->TestResult = TEST_NOT_EXECUTED;
                }

                if (*currentTestFunction->TestResult == TEST_FAILED)
                {
                    failedTestCount++;
                    (void)printf("!!! FAILED !!!\n");
                }
                else if (*currentTestFunction->TestResult == TEST_NOT_EXECUTED)
                {
                    failedTestCount++;
                    (void)printf("Test %s ... SKIPPED due to a failure in test function cleanup. \n", currentTestFunction->TestFunctionName);
                }
                else
                {
                    (void)printf("Suceeded.\n");
                }
                totalTestCount++;
            }

            currentTestFunction = (TEST_FUNCTION_DATA*)currentTestFunction->NextTestFunctionData;
        }

        if (testSuiteCleanup != NULL)
        {
            testSuiteCleanup->TestFunction();
        }

        /* print results */
        (void)printf("%d tests ran, %d failed, %d succeeded.\n", (int)totalTestCount, (int)failedTestCount, (int)(totalTestCount - failedTestCount));
    }

    return failedTestCount;
}

void int_ToString(char* string, size_t bufferSize, int val)
{
    (void)snprintf(string, bufferSize, "%d", val);
}

void char_ToString(char* string, size_t bufferSize, char val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void short_ToString(char* string, size_t bufferSize, short val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void long_ToString(char* string, size_t bufferSize, long val)
{
    (void)snprintf(string, bufferSize, "%ld", (long)val);
}

void size_t_ToString(char* string, size_t bufferSize, size_t val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void float_ToString(char* string, size_t bufferSize, float val)
{
    (void)snprintf(string, bufferSize, "%.02f", val);
}

void double_ToString(char* string, size_t bufferSize, double val)
{
    (void)snprintf(string, bufferSize, "%.02f", val);
}

void char_ptr_ToString(char* string, size_t bufferSize, const char* val)
{
    (void)snprintf(string, bufferSize, "%s", val);
}

void void_ptr_ToString(char* string, size_t bufferSize, const void* val)
{
    (void)snprintf(string, bufferSize, "%p", val);
}

void unsigned_long_ToString(char* string, size_t bufferSize, unsigned long val)
{
    (void)snprintf(string, bufferSize, "%lu", val);
}

int int_Compare(int left, int right)
{
    return left != right;
}

int char_Compare(char left, char right)
{
    return left != right;
}

int short_Compare(short left, short right)
{
    return left != right;
}

int long_Compare(long left, long right)
{
    return left != right;
}

int unsigned_long_Compare(unsigned long left, unsigned long right)
{
    return left != right;
}

int size_t_Compare(size_t left, size_t right)
{
    return left != right;
}

int float_Compare(float left, float right)
{
    return left != right;
}

int double_Compare(double left, double right)
{
    return left != right;
}

int char_ptr_Compare(const char* left, const char* right)
{
    if ((left == NULL) &&
        (right == NULL))
    {
        return 0;
    }
    else if (left == NULL)
    {
        return -1;
    }
    else if (left == NULL)
    {
        return 1;
    }
    else
    {
        return strcmp(left, right);
    }
}

int void_ptr_Compare(const void* left, const void* right)
{
    return left != right;
}

#if defined CTEST_USE_STDINT
void uint8_t_ToString(char* string, size_t bufferSize, uint8_t val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void int8_t_ToString(char* string, size_t bufferSize, int8_t val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void uint16_t_ToString(char* string, size_t bufferSize, uint16_t val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void int16_t_ToString(char* string, size_t bufferSize, int16_t val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void uint32_t_ToString(char* string, size_t bufferSize, uint32_t val)
{
    (void)snprintf(string, bufferSize, "%llu", (unsigned long long)val);
}

void int32_t_ToString(char* string, size_t bufferSize, int32_t val)
{
    (void)snprintf(string, bufferSize, "%lld", (long long)val);
}

void uint64_t_ToString(char* string, size_t bufferSize, uint64_t val)
{
    (void)snprintf(string, bufferSize, "%llu", (unsigned long long)val);
}

void int64_t_ToString(char* string, size_t bufferSize, int64_t val)
{
    (void)snprintf(string, bufferSize, "%lld", (long long)val);
}

int uint8_t_Compare(uint8_t left, uint8_t right)
{
    return left != right;
}

int int8_t_Compare(int8_t left, int8_t right)
{
    return left != right;
}

int uint16_t_Compare(uint16_t left, uint16_t right)
{
    return left != right;
}

int int16_t_Compare(int16_t left, int16_t right)
{
    return left != right;
}

int uint32_t_Compare(uint32_t left, uint32_t right)
{
    return left != right;
}

int int32_t_Compare(int32_t left, int32_t right)
{
    return left != right;
}

int uint64_t_Compare(uint64_t left, uint64_t right)
{
    return left != right;
}

int int64_t_Compare(int64_t left, int64_t right)
{
    return left != right;
}

#endif

#if defined _MSC_VER
#elif defined __cpluplus
#elif defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L))
#else

int snprintf(char * s, size_t n, const char * format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = vsprintf(s, format, args);
    va_end(args);
    return result;
}

#endif
#endif

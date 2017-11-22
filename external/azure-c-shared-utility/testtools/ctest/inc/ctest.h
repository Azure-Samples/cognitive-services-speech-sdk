// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CTEST_H
#define CTEST_H

#ifdef __cplusplus
#include <cstddef>
#include <cstring>
#include <cstdio>
#include "ctest_macros.h"
#include <csetjmp>
#include <setjmp.h> /* Some compilers do not want to play by the standard, specifically ARM CC */
#include <stdio.h> /* Some compilers do not want to play by the standard, specifically ARM CC */
extern "C" {
#define C_LINKAGE "C"
#define C_LINKAGE_PREFIX extern "C"
#else
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "ctest_macros.h"
#include <setjmp.h>
#define C_LINKAGE
#define C_LINKAGE_PREFIX
#endif

#if defined _MSC_VER
#define CTEST_USE_STDINT
#define snprintf _snprintf
#elif defined __cplusplus
#define CTEST_USE_STDINT
#elif defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L))
#define CTEST_USE_STDINT
#else
C_LINKAGE_PREFIX int snprintf(char * s, size_t n, const char * format, ...);
#endif
#endif

#if defined CTEST_USE_STDINT
#include <stdint.h>
#endif

typedef void(*TEST_FUNC)(void);

typedef enum CTEST_FUNCTION_TYPE_TAG
{
    CTEST_BEGIN_SUITE,
    CTEST_END_SUITE,
    CTEST_TEST_FUNCTION,
    CTEST_TEST_SUITE_INITIALIZE,
    CTEST_TEST_SUITE_CLEANUP,
    CTEST_TEST_FUNCTION_INITIALIZE,
    CTEST_TEST_FUNCTION_CLEANUP
} CTEST_FUNCTION_TYPE;

typedef enum TEST_RESULT_TAG
{
    TEST_SUCCESS,
    TEST_FAILED,
    TEST_NOT_EXECUTED
} TEST_RESULT;

typedef struct TEST_FUNCTION_DATA_TAG
{
    const TEST_FUNC TestFunction;
    const char* TestFunctionName;
    const void* const NextTestFunctionData;
    TEST_RESULT* const TestResult;
    const CTEST_FUNCTION_TYPE FunctionType;
} TEST_FUNCTION_DATA;

#define STR_CONCAT2(x,y) x##y
#define STR_CONCAT(x,y) STR_CONCAT2(x,y)

#define EXPAND_1(A) A

extern const TEST_FUNCTION_DATA* g_CurrentTestFunction;
extern jmp_buf g_ExceptionJump;

#define CTEST_BEGIN_TEST_SUITE(testSuiteName) \
    C_LINKAGE_PREFIX const TEST_FUNCTION_DATA TestListHead_##testSuiteName; \
    static const TEST_FUNCTION_DATA STR_CONCAT(TestFunctionData, EXPAND_1(__COUNTER__)) = { NULL, NULL, NULL, NULL, CTEST_BEGIN_SUITE }; \

#define CTEST_FUNCTION(funcName)	\
    static void funcName(void); \
    static TEST_RESULT funcName##_TestResult; \
    static const TEST_FUNCTION_DATA STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_INC(__COUNTER__))) = \
{ funcName, #funcName, &STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_DEC(CTEST_DEC(__COUNTER__)))), &funcName##_TestResult, CTEST_TEST_FUNCTION }; \
    static void funcName(void)

#define CTEST_SUITE_INITIALIZE()	\
    static void TestSuiteInitialize(void); \
    static const TEST_FUNCTION_DATA STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_INC(__COUNTER__))) = \
{ TestSuiteInitialize, "TestSuiteInitialize", &STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_DEC(CTEST_DEC(__COUNTER__)))), NULL, CTEST_TEST_SUITE_INITIALIZE }; \
    static void TestSuiteInitialize(void)

#define CTEST_SUITE_CLEANUP()	\
    static void TestSuiteCleanup(void); \
    static const TEST_FUNCTION_DATA STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_INC(__COUNTER__))) = \
{ &TestSuiteCleanup, "TestSuiteCleanup", &STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_DEC(CTEST_DEC(__COUNTER__)))), NULL, CTEST_TEST_SUITE_CLEANUP }; \
    static void TestSuiteCleanup(void)

#define CTEST_FUNCTION_INITIALIZE()	\
    static void TestFunctionInitialize(void); \
    static const TEST_FUNCTION_DATA STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_INC(__COUNTER__))) = \
{ TestFunctionInitialize, "TestFunctionInitialize", &STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_DEC(CTEST_DEC(__COUNTER__)))), NULL, CTEST_TEST_FUNCTION_INITIALIZE }; \
    static void TestFunctionInitialize(void)

#define CTEST_FUNCTION_CLEANUP()	\
    static void TestFunctionCleanup(void); \
    static const TEST_FUNCTION_DATA STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_INC(__COUNTER__))) = \
{ &TestFunctionCleanup, "TestFunctionCleanup", &STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_DEC(CTEST_DEC(__COUNTER__)))), NULL, CTEST_TEST_FUNCTION_CLEANUP }; \
    static void TestFunctionCleanup(void)

#define CTEST_END_TEST_SUITE(testSuiteName) \
    static const TEST_FUNCTION_DATA STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_INC(__COUNTER__))) = { NULL, NULL, NULL, NULL, CTEST_TEST_FUNCTION }; \
    const TEST_FUNCTION_DATA TestListHead_##testSuiteName = { NULL, NULL, &STR_CONCAT(TestFunctionData, EXPAND_1(CTEST_DEC(CTEST_DEC(__COUNTER__)))), NULL, CTEST_END_SUITE }; \

#define PRINT_MY_ARG_2(A)

#define PRINT_MY_ARG_1(A) \
    A += 

#ifdef _MSC_VER
#define PRINT_SECOND_ARG(argCount, B) \
    C2(PRINT_MY_ARG_,argCount) LPAREN B )
#else
#define PRINT_SECOND_ARG(argCount, B) \
    C2(PRINT_MY_ARG_,argCount) (B)
#endif

#define CTEST_RUN_TEST_SUITE(...) \
extern C_LINKAGE TEST_FUNCTION_DATA C2(TestListHead_,FIRST_ARG(__VA_ARGS__)); \
IF(DIV2(COUNT_ARG(__VA_ARGS__)),FOR_EACH_1_COUNTED(PRINT_SECOND_ARG, __VA_ARGS__),) RunTests(&C2(TestListHead_, FIRST_ARG(__VA_ARGS__)), TOSTRING(FIRST_ARG(__VA_ARGS__)))

typedef const char* char_ptr;
typedef void* void_ptr;

extern C_LINKAGE void int_ToString(char* string, size_t bufferSize, int val);
extern C_LINKAGE void char_ToString(char* string, size_t bufferSize, char val);
extern C_LINKAGE void short_ToString(char* string, size_t bufferSize, short val);
extern C_LINKAGE void long_ToString(char* string, size_t bufferSize, long val);
extern C_LINKAGE void size_t_ToString(char* string, size_t bufferSize, size_t val);
extern C_LINKAGE void float_ToString(char* string, size_t bufferSize, float val);
extern C_LINKAGE void double_ToString(char* string, size_t bufferSize, double val);
extern C_LINKAGE void char_ptr_ToString(char* string, size_t bufferSize, const char* val);
extern C_LINKAGE void void_ptr_ToString(char* string, size_t bufferSize, const void* val);
extern C_LINKAGE void unsigned_long_ToString(char* string, size_t bufferSize, unsigned long val);
extern C_LINKAGE int int_Compare(int left, int right);
extern C_LINKAGE int char_Compare(char left, char right);
extern C_LINKAGE int short_Compare(short left, short right);
extern C_LINKAGE int long_Compare(long left, long right);
extern C_LINKAGE int size_t_Compare(size_t left, size_t right);
extern C_LINKAGE int float_Compare(float left, float right);
extern C_LINKAGE int double_Compare(double left, double right);
extern C_LINKAGE int char_ptr_Compare(const char* left, const char* right);
extern C_LINKAGE int void_ptr_Compare(const void * left, const void* right);
extern C_LINKAGE int unsigned_long_Compare(unsigned long left, unsigned long right);

#if defined CTEST_USE_STDINT
extern C_LINKAGE void uint8_t_ToString(char* string, size_t bufferSize, uint8_t val);
extern C_LINKAGE void int8_t_ToString(char* string, size_t bufferSize, int8_t val);
extern C_LINKAGE void uint16_t_ToString(char* string, size_t bufferSize, uint16_t val);
extern C_LINKAGE void int16_t_ToString(char* string, size_t bufferSize, int16_t val);
extern C_LINKAGE void uint32_t_ToString(char* string, size_t bufferSize, uint32_t val);
extern C_LINKAGE void int32_t_ToString(char* string, size_t bufferSize, int32_t val);
extern C_LINKAGE void uint64_t_ToString(char* string, size_t bufferSize, uint64_t val);
extern C_LINKAGE void int64_t_ToString(char* string, size_t bufferSize, int64_t val);
extern C_LINKAGE int uint8_t_Compare(uint8_t left, uint8_t right);
extern C_LINKAGE int int8_t_Compare(int8_t left, int8_t right);
extern C_LINKAGE int uint16_t_Compare(uint16_t left, uint16_t right);
extern C_LINKAGE int int16_t_Compare(int16_t left, int16_t right);
extern C_LINKAGE int uint32_t_Compare(uint32_t left, uint32_t right);
extern C_LINKAGE int int32_t_Compare(int32_t left, int32_t right);
extern C_LINKAGE int uint64_t_Compare(uint64_t left, uint64_t right);
extern C_LINKAGE int int64_t_Compare(int64_t left, int64_t right);

#endif

#define CTEST_COMPARE(toStringType, cType) \
    typedef cType toStringType; \
    static int toStringType##_Compare(toStringType left, toStringType right)

#define CTEST_TO_STRING(toStringType, cType, string, bufferSize, value) \
typedef cType toStringType; \
static void toStringType##_ToString(char* string, size_t bufferSize, cType value)


#define EAT_2(X1,X2, NAME,...) NAME
#define EAT_3(X1,X2,X3, NAME,...) NAME
#define EAT_4(X1,X2,X3,X4, NAME,...) NAME

#ifdef _MSC_VER
#define LPAREN (
#define CTEST_ASSERT_ARE_EQUAL(...) EAT_4 LPAREN __VA_ARGS__, CTEST_ASSERT_ARE_EQUAL_WITH_MSG, CTEST_ASSERT_ARE_EQUAL_WITHOUT_MSG)(__VA_ARGS__)
#else
#define CTEST_ASSERT_ARE_EQUAL(...) EAT_4 (__VA_ARGS__, CTEST_ASSERT_ARE_EQUAL_WITH_MSG, CTEST_ASSERT_ARE_EQUAL_WITHOUT_MSG)(__VA_ARGS__)
#endif

#define CTEST_ASSERT_ARE_EQUAL_WITH_MSG(type, A, B, message) \
if (type##_Compare((A), (B))) \
{ \
    char expectedString[1024]; \
    char actualString[1024]; \
    type##_ToString(expectedString, sizeof(expectedString), (A)); \
    type##_ToString(actualString, sizeof(actualString), (B)); \
    printf("  Assert failed: %s Expected: %s, Actual: %s\n", message, expectedString, actualString); \
    if(g_CurrentTestFunction!=NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    longjmp(g_ExceptionJump, 0xca1e4); \
    ; \
}


#define CTEST_ASSERT_ARE_EQUAL_WITHOUT_MSG(type, A, B) CTEST_ASSERT_ARE_EQUAL_WITH_MSG(type, (A), (B), "")

#ifdef _MSC_VER
#define LPAREN (
#define CTEST_ASSERT_ARE_NOT_EQUAL(...) EAT_4 LPAREN __VA_ARGS__, CTEST_ASSERT_ARE_NOT_EQUAL_WITH_MSG, CTEST_ASSERT_ARE_NOT_EQUAL_WITHOUT_MSG)(__VA_ARGS__)
#else
#define CTEST_ASSERT_ARE_NOT_EQUAL(...) EAT_4 (__VA_ARGS__, CTEST_ASSERT_ARE_NOT_EQUAL_WITH_MSG, CTEST_ASSERT_ARE_NOT_EQUAL_WITHOUT_MSG)(__VA_ARGS__)
#endif

#define CTEST_ASSERT_ARE_NOT_EQUAL_WITH_MSG(type, A, B, message) \
if (!type##_Compare((A), (B))) \
{ \
    char actualString[1024]; \
    type##_ToString(actualString, sizeof(actualString), (A)); \
    printf("  Assert failed: Inequality expected. Got equality, value:%s. %s\n", actualString, (message)); \
    if(g_CurrentTestFunction!=NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    longjmp(g_ExceptionJump, 0xca1e4); \
; \
}

#define CTEST_ASSERT_ARE_NOT_EQUAL_WITHOUT_MSG(type, A, B) CTEST_ASSERT_ARE_NOT_EQUAL_WITH_MSG(type, (A), (B), "")

#ifdef _MSC_VER
#define LPAREN (
#define CTEST_ASSERT_IS_NULL(...) EAT_2 LPAREN __VA_ARGS__, CTEST_ASSERT_IS_NULL_WITH_MSG, CTEST_ASSERT_IS_NULL_WITHOUT_MSG)(__VA_ARGS__)
#else
#define CTEST_ASSERT_IS_NULL(...) EAT_2 (__VA_ARGS__, CTEST_ASSERT_IS_NULL_WITH_MSG, CTEST_ASSERT_IS_NULL_WITHOUT_MSG)(__VA_ARGS__)
#endif

#define CTEST_ASSERT_IS_NULL_WITH_MSG(value, message) \
if ((value) != NULL) \
{ \
    printf("  Assert failed: NULL expected, actual: 0x%p. %s\n", (void*)(value), (message)); \
    if(g_CurrentTestFunction!=NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    longjmp(g_ExceptionJump, 0xca1e4); \
 \
}

#define CTEST_ASSERT_IS_NULL_WITHOUT_MSG(value) CTEST_ASSERT_IS_NULL_WITH_MSG((value), "")

#ifdef _MSC_VER
#define LPAREN (
#define CTEST_ASSERT_IS_NOT_NULL(...) EAT_2 LPAREN __VA_ARGS__, CTEST_ASSERT_IS_NOT_NULL_WITH_MSG, CTEST_ASSERT_IS_NOT_NULL_WITHOUT_MSG)(__VA_ARGS__)
#else
#define CTEST_ASSERT_IS_NOT_NULL(...) EAT_2 (__VA_ARGS__, CTEST_ASSERT_IS_NOT_NULL_WITH_MSG, CTEST_ASSERT_IS_NOT_NULL_WITHOUT_MSG)(__VA_ARGS__)
#endif

#define CTEST_ASSERT_IS_NOT_NULL_WITH_MSG(value, message) \
if ((value) == NULL) \
{ \
    printf("  Assert failed: non-NULL expected. %s\n", (message)); \
    if(g_CurrentTestFunction!=NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    longjmp(g_ExceptionJump, 0xca1e4); \
}

#define CTEST_ASSERT_IS_NOT_NULL_WITHOUT_MSG(value) CTEST_ASSERT_IS_NOT_NULL_WITH_MSG((value), "")

#ifdef _MSC_VER
#define LPAREN (
#define CTEST_ASSERT_IS_TRUE(...) EAT_2 LPAREN __VA_ARGS__, CTEST_ASSERT_IS_TRUE_WITH_MSG, CTEST_ASSERT_IS_TRUE_WITHOUT_MSG)(__VA_ARGS__)
#else
#define CTEST_ASSERT_IS_TRUE(...) EAT_2 (__VA_ARGS__, CTEST_ASSERT_IS_TRUE_WITH_MSG, CTEST_ASSERT_IS_TRUE_WITHOUT_MSG)(__VA_ARGS__)
#endif

#define CTEST_ASSERT_IS_TRUE_WITH_MSG(expression, message) \
if (!(expression)) \
{ \
    printf("  Assert failed: Expression should be true: %s. %s\n", #expression, (message)); \
    if(g_CurrentTestFunction!=NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    longjmp(g_ExceptionJump, 0xca1e4); \
}

#define CTEST_ASSERT_IS_TRUE_WITHOUT_MSG(expression) CTEST_ASSERT_IS_TRUE_WITH_MSG(expression, "")

#ifdef _MSC_VER
#define LPAREN (
#define CTEST_ASSERT_IS_FALSE(...) EAT_2 LPAREN __VA_ARGS__, CTEST_ASSERT_IS_FALSE_WITH_MSG, CTEST_ASSERT_IS_FALSE_WITHOUT_MSG)(__VA_ARGS__)
#else
#define CTEST_ASSERT_IS_FALSE(...) EAT_2 (__VA_ARGS__, CTEST_ASSERT_IS_FALSE_WITH_MSG, CTEST_ASSERT_IS_FALSE_WITHOUT_MSG)(__VA_ARGS__)
#endif

#define CTEST_ASSERT_IS_FALSE_WITH_MSG(expression, message) \
if (expression) \
{ \
    printf("  Assert failed: Expression should be false: %s. %s\n", #expression, (message)); \
    if(g_CurrentTestFunction!=NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    longjmp(g_ExceptionJump, 0xca1e4); \
}

#define CTEST_ASSERT_IS_FALSE_WITHOUT_MSG(expression) CTEST_ASSERT_IS_FALSE_WITH_MSG(expression, "")

#define CTEST_ASSERT_FAIL(message) \
    printf("  Assert failed: %s\n", (message)); \
    if(g_CurrentTestFunction!=NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    longjmp(g_ExceptionJump, 0xca1e4); \

extern C_LINKAGE size_t RunTests(const TEST_FUNCTION_DATA* testListHead, const char* testSuiteName);

#ifdef __cplusplus
}
#endif

#endif

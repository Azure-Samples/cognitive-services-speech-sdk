// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"

/* Tested by unit tests for umock_c:
Tests_SRS_UMOCK_C_LIB_01_006: [umock_c_init shall initialize umock_c.]
Tests_SRS_UMOCK_C_LIB_01_007: [umock_c_init called if already initialized shall fail and return a non-zero value.]
Tests_SRS_UMOCK_C_LIB_01_008: [umock_c_init shall initialize the umock supported types (C native types).]
Tests_SRS_UMOCK_C_LIB_01_009: [on_umock_c_error can be NULL.]
Tests_SRS_UMOCK_C_LIB_01_010: [If on_umock_c_error is non-NULL it shall be saved for later use (to be invoked whenever an umock_c error needs to be signaled to the user).]
Tests_SRS_UMOCK_C_LIB_01_011: [umock_c_deinit shall free all umock_c used resources.]
Tests_SRS_UMOCK_C_LIB_01_012: [If umock_c was not initialized, umock_c_deinit shall do nothing.]
*/

/* Tested by unittests of umocktypes and umocktypename:
Tests_SRS_UMOCK_C_LIB_01_145: [ Since umock_c needs to maintain a list of registered types, the following rules shall be applied: ]
Tests_SRS_UMOCK_C_LIB_01_146: [ Each type shall be normalized to a form where all extra spaces are removed. ]
Tests_SRS_UMOCK_C_LIB_01_147: [ Type names are case sensitive. ]
*/

#include "umock_c.h"
#define ENABLE_MOCKS
#include "test_dependency.h"

/* Tests_SRS_UMOCK_C_LIB_01_067: [char\* and const char\* shall be supported out of the box through a separate header, umockvalue_charptr.h.]*/
#include "umocktypes_charptr.h"

typedef struct test_on_umock_c_error_CALL_TAG
{
    UMOCK_C_ERROR_CODE error_code;
} test_on_umock_c_error_CALL;

static test_on_umock_c_error_CALL* test_on_umock_c_error_calls;
static size_t test_on_umock_c_error_call_count;

TEST_DEFINE_ENUM_TYPE(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES);

DECLARE_UMOCK_POINTER_TYPE_FOR_TYPE(int, int);
DECLARE_UMOCK_POINTER_TYPE_FOR_TYPE(unsigned char, unsignedchar);

static void test_on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    test_on_umock_c_error_CALL* new_calls = (test_on_umock_c_error_CALL*)realloc(test_on_umock_c_error_calls, sizeof(test_on_umock_c_error_CALL) * (test_on_umock_c_error_call_count + 1));
    if (new_calls != NULL)
    {
        test_on_umock_c_error_calls = new_calls;
        test_on_umock_c_error_calls[test_on_umock_c_error_call_count].error_code = error_code;
        test_on_umock_c_error_call_count++;
    }
}

static int my_hook_test_dependency_with_global_mock_hook(void)
{
    return 43;
}

static int my_hook_result;
static int my_hook_test_dependency_no_args(void)
{
    return my_hook_result++;
}

static int my_hook_test_dependency_no_args_2(void)
{
    return 0x21;
}

static int arg_a;
static int arg_b;
static int my_hook_test_dependency_2_args(int a, int b)
{
    arg_a = a;
    arg_b = b;
    return 0;
}

static unsigned int test_dependency_void_return_called = 0;
static void my_hook_test_dependency_void_return(void)
{
    test_dependency_void_return_called = 1;
}

char* stringify_func_TEST_STRUCT_COPY_FAILS(const TEST_STRUCT_COPY_FAILS* value)
{
    char* result = (char*)malloc(1);
    result[0] = '\0';
    return result;
}

int are_equal_func_TEST_STRUCT_COPY_FAILS(const TEST_STRUCT_COPY_FAILS* left, const TEST_STRUCT_COPY_FAILS* right)
{
    return 1;
}

int copy_func_TEST_STRUCT_COPY_FAILS(TEST_STRUCT_COPY_FAILS* destination, const TEST_STRUCT_COPY_FAILS* source)
{
    return 0;
}

void free_func_TEST_STRUCT_COPY_FAILS(TEST_STRUCT_COPY_FAILS* value)
{
}

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

MOCK_FUNCTION_WITH_CODE(, void, test_mock_function_with_code_1_arg, int, a);
MOCK_FUNCTION_END()

MOCK_FUNCTION_WITH_CODE(, char*, test_mock_function_returning_string_with_code);
MOCK_FUNCTION_END("a")

typedef int funkytype;

/* Tests_SRS_UMOCK_C_LIB_01_150: [ MOCK_FUNCTION_WITH_CODE shall define a mock function and allow the user to embed code between this define and a MOCK_FUNCTION_END call. ]*/
MOCK_FUNCTION_WITH_CODE(, funkytype, test_mock_function_with_funkytype, funkytype, x);
MOCK_FUNCTION_END(42)

static unsigned char*** result_value = (unsigned char***)0x4242;

MOCK_FUNCTION_WITH_CODE(, unsigned char***, test_mock_function_with_unregistered_ptr_type, unsigned char***, x);
MOCK_FUNCTION_END(result_value)

IMPLEMENT_UMOCK_C_ENUM_TYPE(TEST_ENUM, TEST_ENUM_VALUE_1, TEST_ENUM_VALUE_2)

static int test_return_value = 42;

MOCK_FUNCTION_WITH_CODE(, int, test_dependency_for_capture_return)
MOCK_FUNCTION_END(test_return_value)

MOCK_FUNCTION_WITH_CODE(, int, test_dependency_for_capture_return_with_arg, int, a)
MOCK_FUNCTION_END(test_return_value)

BEGIN_TEST_SUITE(umock_c_integrationtests)

TEST_SUITE_INITIALIZE(suite_init)
{
    int result;

    TEST_INITIALIZE_MEMORY_DEBUG(global_mutex);

    test_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_mutex);

    result = umock_c_init(test_on_umock_c_error);
    ASSERT_ARE_EQUAL(int, 0, result);
    /* Tests_SRS_UMOCK_C_LIB_01_069: [The signature shall be: ...*/
    /* Tests_SRS_UMOCK_C_LIB_01_070: [umockvalue_charptr_register_types shall return 0 on success and non-zero on failure.]*/
    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);
    REGISTER_UMOCK_VALUE_TYPE(int*, stringify_func_intptr, are_equal_func_intptr, copy_func_intptr, free_func_intptr);
    REGISTER_UMOCK_VALUE_TYPE(unsigned char*, stringify_func_unsignedcharptr, are_equal_func_unsignedcharptr, copy_func_unsignedcharptr, free_func_unsignedcharptr);
    REGISTER_UMOCK_VALUE_TYPE(TEST_STRUCT_COPY_FAILS, stringify_func_TEST_STRUCT_COPY_FAILS, are_equal_func_TEST_STRUCT_COPY_FAILS, copy_func_TEST_STRUCT_COPY_FAILS, free_func_TEST_STRUCT_COPY_FAILS);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(test_mutex);
    TEST_DEINITIALIZE_MEMORY_DEBUG(global_mutex);
}

TEST_FUNCTION_INITIALIZE(test_function_init)
{
    int mutex_acquire_result = TEST_MUTEX_ACQUIRE(test_mutex);
    ASSERT_ARE_EQUAL(int, 0, mutex_acquire_result);

    test_on_umock_c_error_calls = NULL;
    test_on_umock_c_error_call_count = 0;

    test_return_value = 42;
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    umock_c_reset_all_calls();

    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_no_args, NULL);

    free(test_on_umock_c_error_calls);
    test_on_umock_c_error_calls = NULL;
    test_on_umock_c_error_call_count = 0;

    TEST_MUTEX_RELEASE(test_mutex);
}

/* STRICT_EXPECTED_CALL */

/* Tests_SRS_UMOCK_C_LIB_01_013: [STRICT_EXPECTED_CALL shall record that a certain call is expected.] */
/* Tests_SRS_UMOCK_C_LIB_01_015: [The call argument shall be the complete function invocation.]*/
TEST_FUNCTION(STRICT_EXPECTED_CALL_without_an_actual_call_yields_a_missing_call)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_no_args());

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_no_args()]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_013: [STRICT_EXPECTED_CALL shall record that a certain call is expected.] */
TEST_FUNCTION(two_STRICT_EXPECTED_CALL_without_an_actual_call_yields_2_missing_calls)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_no_args());
    STRICT_EXPECTED_CALL(test_dependency_no_args());

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_no_args()][test_dependency_no_args()]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_014: [For each argument the argument value shall be stored for later comparison with actual calls.] */
TEST_FUNCTION(a_STRICT_EXPECTED_CALL_with_one_argument_without_an_actual_call_yields_1_missing_call)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_1_arg(42)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_014: [For each argument the argument value shall be stored for later comparison with actual calls.] */
TEST_FUNCTION(a_STRICT_EXPECTED_CALL_with_2_arguments_without_an_actual_call_yields_1_missing_call)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_013: [STRICT_EXPECTED_CALL shall record that a certain call is expected.] */
TEST_FUNCTION(two_different_STRICT_EXPECTED_CALL_instances_without_an_actual_call_yields_2_missing_calls)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_no_args());
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_no_args()][test_dependency_1_arg(42)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_115: [ umock_c shall compare calls in order. ]*/
TEST_FUNCTION(two_different_STRICT_EXPECTED_CALL_instances_without_an_actual_call_yields_2_missing_calls_with_order_preserved)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42));
    STRICT_EXPECTED_CALL(test_dependency_no_args());

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_1_arg(42)][test_dependency_no_args()]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_115: [ umock_c shall compare calls in order. ]*/
TEST_FUNCTION(inverted_order_for_calls_is_detected_as_mismatch)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42));
    STRICT_EXPECTED_CALL(test_dependency_no_args());

    // act
    test_dependency_no_args();

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_1_arg(42)][test_dependency_no_args()]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_no_args()]", umock_c_get_actual_calls());
}

/* EXPECTED_CALL */

/* Tests_SRS_UMOCK_C_LIB_01_016: [EXPECTED_CALL shall record that a certain call is expected.] */
/* Tests_SRS_UMOCK_C_LIB_01_018: [The call argument shall be the complete function invocation.] */
TEST_FUNCTION(EXPECTED_CALL_without_an_actual_call_yields_a_missing_call)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_no_args());

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_no_args()]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_016: [EXPECTED_CALL shall record that a certain call is expected.] */
TEST_FUNCTION(two_EXPECTED_CALL_without_an_actual_call_yields_2_missing_calls)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_no_args());
    EXPECTED_CALL(test_dependency_no_args());

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_no_args()][test_dependency_no_args()]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_017: [No arguments shall be saved by default, unless other modifiers state it.] */
TEST_FUNCTION(an_EXPECTED_CALL_with_one_argument_without_an_actual_call_yields_1_missing_call)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_1_arg(42));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_1_arg(42)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_017: [No arguments shall be saved by default, unless other modifiers state it.] */
TEST_FUNCTION(an_EXPECTED_CALL_with_2_arguments_without_an_actual_call_yields_1_missing_call)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_2_args(42, 43));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_016: [EXPECTED_CALL shall record that a certain call is expected.] */
TEST_FUNCTION(two_different_EXPECTED_CALL_instances_without_an_actual_call_yields_2_missing_calls)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_no_args());
    EXPECTED_CALL(test_dependency_1_arg(42));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_no_args()][test_dependency_1_arg(42)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_115: [ umock_c shall compare calls in order. ]*/
TEST_FUNCTION(two_different_EXPECTED_CALL_instances_without_an_actual_call_yields_2_missing_calls_with_order_preserved)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_1_arg(42));
    EXPECTED_CALL(test_dependency_no_args());

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_1_arg(42)][test_dependency_no_args()]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_017: [No arguments shall be saved by default, unless other modifiers state it.]*/
TEST_FUNCTION(EXPECTED_CALL_does_not_compare_arguments)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_1_arg(42));

    test_dependency_1_arg(43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_017: [No arguments shall be saved by default, unless other modifiers state it.]*/
TEST_FUNCTION(EXPECTED_CALL_with_2_args_does_not_compare_arguments)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_2_args(42, 43));

    test_dependency_2_args(43, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Call modifiers */

/* Tests_SRS_UMOCK_C_LIB_01_074: [When an expected call is recorded a call modifier interface in the form of a structure containing function pointers shall be returned to the caller.] */
TEST_FUNCTION(STRICT_EXPECTED_CALL_allows_call_modifiers)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateAllArguments();

    test_dependency_2_args(42, 43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Chaining modifiers */

/* Tests_SRS_UMOCK_C_LIB_01_075: [The last modifier in a chain overrides previous modifiers if any collision occurs.]*/
TEST_FUNCTION(STRICT_EXPECTED_CALL_with_ignore_all_arguments_and_then_validate_all_args_still_validates_args)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .IgnoreAllArguments()
        .ValidateAllArguments();

    test_dependency_2_args(43, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(43,44)]", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_075: [The last modifier in a chain overrides previous modifiers if any collision occurs.]*/
TEST_FUNCTION(EXPECTED_CALL_with_validate_all_arguments_and_then_ignore_all_args_still_ignores_args)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateAllArguments()
        .IgnoreAllArguments();

    test_dependency_2_args(43, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_075: [The last modifier in a chain overrides previous modifiers if any collision occurs.]*/
TEST_FUNCTION(STRICT_EXPECTED_CALL_with_ignore_validate_ignore_all_arguments_ignores_args)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .IgnoreAllArguments()
        .ValidateAllArguments()
        .IgnoreAllArguments();

    test_dependency_2_args(43, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_075: [The last modifier in a chain overrides previous modifiers if any collision occurs.]*/
TEST_FUNCTION(STRICT_EXPECTED_CALL_with_validate_ignore_validate_all_arguments_validates_args)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateAllArguments()
        .IgnoreAllArguments()
        .ValidateAllArguments();

    test_dependency_2_args(43, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(43,44)]", umock_c_get_actual_calls());
}

/* IgnoreAllArguments */

/* Tests_SRS_UMOCK_C_LIB_01_076: [The IgnoreAllArguments call modifier shall record that for that specific call all arguments will be ignored for that specific call.] */
TEST_FUNCTION(IgnoreAllArguments_ignores_args_on_a_STRICT_EXPECTED_CALL)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .IgnoreAllArguments();

    test_dependency_2_args(43, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* ValidateAllArguments */

/* Tests_SRS_UMOCK_C_LIB_01_077: [The ValidateAllArguments call modifier shall record that for that specific call all arguments will be validated.] */
TEST_FUNCTION(ValidateAllArguments_validates_all_args_on_an_EXPECTED_CALL)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateAllArguments();

    test_dependency_2_args(43, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(43,44)]", umock_c_get_actual_calls());
}

/* IgnoreArgument_{arg_name} */

/* Tests_SRS_UMOCK_C_LIB_01_078: [The IgnoreArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be ignored for that specific call.] */
TEST_FUNCTION(IgnoreArgument_by_name_ignores_only_that_argument_on_a_STRICT_EXPECTED_CALL)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .IgnoreArgument_a();

    test_dependency_2_args(41, 43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_078: [The IgnoreArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be ignored for that specific call.] */
TEST_FUNCTION(IgnoreArgument_by_name_with_second_argument_ignores_only_that_argument_on_a_STRICT_EXPECTED_CALL)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .IgnoreArgument_b();

    // act
    test_dependency_2_args(42, 42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* ValidateArgument_{arg_name} */

/* Tests_SRS_UMOCK_C_LIB_01_079: [The ValidateArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_name_validates_only_that_argument_on_an_EXPECTED_CALL)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateArgument_a();

    // act
    test_dependency_2_args(42, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_079: [The ValidateArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_name_validates_only_that_argument_on_an_EXPECTED_CALL_and_args_are_different)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateArgument_a();

    // act
    test_dependency_2_args(41, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(41,44)]", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_079: [The ValidateArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_name_2nd_arg_validates_only_that_argument_on_an_EXPECTED_CALL)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateArgument_b();

    // act
    test_dependency_2_args(41, 43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_079: [The ValidateArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_name_2nd_arg_validates_only_that_argument_on_an_EXPECTED_CALL_and_args_are_different)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateArgument_b();

    // act
    test_dependency_2_args(42, 44);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,44)]", umock_c_get_actual_calls());
}

/* IgnoreArgument */

/* Tests_SRS_UMOCK_C_LIB_01_080: [The IgnoreArgument call modifier shall record that the indexth argument will be ignored for that specific call.]*/
TEST_FUNCTION(IgnoreArgument_by_index_for_first_arg_ignores_the_first_argument)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .IgnoreArgument(1);

    // act
    test_dependency_2_args(41, 43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_080: [The IgnoreArgument call modifier shall record that the indexth argument will be ignored for that specific call.]*/
TEST_FUNCTION(IgnoreArgument_by_index_for_second_arg_ignores_the_second_argument)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_2_args(41, 42))
        .IgnoreArgument(2);

    // act
    test_dependency_2_args(41, 43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_080: [The IgnoreArgument call modifier shall record that the indexth argument will be ignored for that specific call.]*/
TEST_FUNCTION(IgnoreArgument_by_index_for_first_arg_ignores_only_the_first_argument)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_2_args(42, 43))
        .IgnoreArgument(1);

    // act
    test_dependency_2_args(42, 42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,42)]", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_080: [The IgnoreArgument call modifier shall record that the indexth argument will be ignored for that specific call.]*/
TEST_FUNCTION(IgnoreArgument_by_index_for_second_arg_ignores_only_the_second_argument)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_2_args(41, 42))
        .IgnoreArgument(2);

    // act
    test_dependency_2_args(42, 42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(41,42)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,42)]", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_081: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.] */
TEST_FUNCTION(IgnoreArgument_by_index_with_index_0_triggers_the_on_error_callback)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(41, 42))
        .IgnoreArgument(0);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_081: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.] */
TEST_FUNCTION(IgnoreArgument_by_index_with_index_greater_than_arg_count_triggers_the_on_error_callback)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_2_args(41, 42))
        .IgnoreArgument(3);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* ValidateArgument */

/* Tests_SRS_UMOCK_C_LIB_01_082: [The ValidateArgument call modifier shall record that the indexth argument will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_index_for_first_arg_ignores_the_first_argument)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateArgument(1);

    // act
    test_dependency_2_args(41, 43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(41,43)]", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_082: [The ValidateArgument call modifier shall record that the indexth argument will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_index_for_second_arg_validates_the_second_argument)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 42))
        .ValidateArgument(2);

    // act
    test_dependency_2_args(42, 43);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,42)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_2_args(42,43)]", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_082: [The ValidateArgument call modifier shall record that the indexth argument will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_index_for_first_arg_validates_only_the_first_argument)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 43))
        .ValidateArgument(1);

    // act
    test_dependency_2_args(42, 42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_082: [The ValidateArgument call modifier shall record that the indexth argument will be validated for that specific call.]*/
TEST_FUNCTION(ValidateArgument_by_index_for_second_arg_validates_only_the_second_argument)
{
    // arrange
    EXPECTED_CALL(test_dependency_2_args(42, 42))
        .ValidateArgument(2);

    // act
    test_dependency_2_args(43, 42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_083: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
TEST_FUNCTION(ValidateArgument_by_index_with_0_index_triggers_the_on_error_callback)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_2_args(42, 42))
        .ValidateArgument(0);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_083: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
TEST_FUNCTION(ValidateArgument_by_index_with_index_greater_than_arg_count_triggers_the_on_error_callback)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_2_args(42, 42))
        .ValidateArgument(3);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* SetReturn */

/* Tests_SRS_UMOCK_C_LIB_01_084: [The SetReturn call modifier shall record that when an actual call is matched with the specific expected call, it shall return the result value to the code under test.] */
TEST_FUNCTION(SetReturn_sets_the_return_value_for_a_strict_expected_call)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_no_args())
        .SetReturn(42);

    // act
    int result = test_dependency_no_args();

    // assert
    ASSERT_ARE_EQUAL(int, 42, result);
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_084: [The SetReturn call modifier shall record that when an actual call is matched with the specific expected call, it shall return the result value to the code under test.] */
TEST_FUNCTION(SetReturn_sets_the_return_value_for_an_expected_call)
{
    // arrange
    EXPECTED_CALL(test_dependency_no_args())
        .SetReturn(42);

    // act
    int result = test_dependency_no_args();

    // assert
    ASSERT_ARE_EQUAL(int, 42, result);
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_084: [The SetReturn call modifier shall record that when an actual call is matched with the specific expected call, it shall return the result value to the code under test.] */
TEST_FUNCTION(SetReturn_sets_the_return_value_only_for_a_matched_call)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42))
        .SetReturn(42);

    // act
    int result = test_dependency_1_arg(41);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_084: [The SetReturn call modifier shall record that when an actual call is matched with the specific expected call, it shall return the result value to the code under test.] */
TEST_FUNCTION(SetReturn_sets_independent_return_values_for_each_call)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42))
        .SetReturn(142);
    STRICT_EXPECTED_CALL(test_dependency_1_arg(43))
        .SetReturn(143);

    // act
    int result1 = test_dependency_1_arg(42);
    int result2 = test_dependency_1_arg(43);

    // assert
    ASSERT_ARE_EQUAL(int, 142, result1);
    ASSERT_ARE_EQUAL(int, 143, result2);
}

/* CopyOutArgumentBuffer */

/* Tests_SRS_UMOCK_C_LIB_01_087: [The CopyOutArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_116: [ The argument targetted by CopyOutArgument shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_copies_bytes_to_the_out_argument_for_a_strict_expected_call)
{
    // arrange
    int injected_int = 0x42;
    STRICT_EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_087: [The CopyOutArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_116: [ The argument targetted by CopyOutArgument shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_copies_bytes_to_the_out_argument_for_an_expected_call)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_087: [The CopyOutArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_116: [ The argument targetted by CopyOutArgument shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_only_copies_bytes_to_the_out_argument_that_was_specified)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_2_out_args(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int));
    int actual_int_1 = 0;
    int actual_int_2 = 0;

    // act
    (void)test_dependency_2_out_args(&actual_int_1, &actual_int_2);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int, actual_int_1);
    ASSERT_ARE_EQUAL(int, 0, actual_int_2);
}

/* Tests_SRS_UMOCK_C_LIB_01_087: [The CopyOutArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_116: [ The argument targetted by CopyOutArgument shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_only_copies_bytes_to_the_second_out_argument)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_2_out_args(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &injected_int, sizeof(injected_int));
    int actual_int_1 = 0;
    int actual_int_2 = 0;

    // act
    (void)test_dependency_2_out_args(&actual_int_1, &actual_int_2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, actual_int_1);
    ASSERT_ARE_EQUAL(int, injected_int, actual_int_2);
}

/* Tests_SRS_UMOCK_C_LIB_01_088: [The memory shall be copied.]*/
TEST_FUNCTION(CopyOutArgumentBuffer_copies_the_memory_for_later_use)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int));
    int actual_int = 0;
    injected_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_089: [The buffers for previous CopyOutArgumentBuffer calls shall be freed.]*/
/* Tests_SRS_UMOCK_C_LIB_01_133: [ If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept. ]*/
TEST_FUNCTION(CopyOutArgumentBuffer_frees_allocated_buffers_for_previous_CopyOutArgumentBuffer)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_091: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
TEST_FUNCTION(CopyOutArgumentBuffer_with_0_index_triggers_the_error_callback)
{
    // arrange
    int injected_int = 0x42;

    // act
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(0, &injected_int, sizeof(injected_int));

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_091: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
TEST_FUNCTION(CopyOutArgumentBuffer_with_index_higher_than_count_of_args_triggers_the_error_callback)
{
    // arrange
    int injected_int = 0x42;

    // act
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &injected_int, sizeof(injected_int));

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_092: [If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.] */
TEST_FUNCTION(CopyOutArgumentBuffer_with_NULL_bytes_triggers_the_error_callback)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, NULL, sizeof(int));

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_INVALID_ARGUMENT_BUFFER, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_092: [If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.] */
TEST_FUNCTION(CopyOutArgumentBuffer_with_0_length_triggers_the_error_callback)
{
    // arrange
    int injected_int = 0x42;

    // act
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, &injected_int, 0);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_INVALID_ARGUMENT_BUFFER, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_087: [The CopyOutArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_116: [ The argument targetted by CopyOutArgument shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_when_an_error_occurs_preserves_the_previous_state)
{
    // arrange
    int injected_int = 0x42;
    int injected_int_2 = 0x43;
    EXPECTED_CALL(test_dependency_2_out_args(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &injected_int, sizeof(injected_int))
        .CopyOutArgumentBuffer(0, &injected_int_2, sizeof(injected_int_2));
    int actual_int_1 = 0;
    int actual_int_2 = 0;

    // act
    (void)test_dependency_2_out_args(&actual_int_1, &actual_int_2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, actual_int_1);
    ASSERT_ARE_EQUAL(int, injected_int, actual_int_2);
}

/* CopyOutArgumentBuffer_{arg_name} */

/* Tests_SRS_UMOCK_C_LIB_01_154: [ The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function. ] */
/* Tests_SRS_UMOCK_C_LIB_01_159: [ The argument targetted by CopyOutArgumentBuffer_{arg_name} shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_copies_bytes_to_the_out_argument_for_a_strict_expected_call)
{
    // arrange
    int injected_int = 0x42;
    STRICT_EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(&injected_int, sizeof(injected_int));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_154: [ The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function. ] */
/* Tests_SRS_UMOCK_C_LIB_01_159: [ The argument targetted by CopyOutArgumentBuffer_{arg_name} shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_copies_bytes_to_the_out_argument_for_an_expected_call)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(&injected_int, sizeof(injected_int));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_154: [ The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function. ] */
/* Tests_SRS_UMOCK_C_LIB_01_159: [ The argument targetted by CopyOutArgumentBuffer_{arg_name} shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_only_copies_bytes_to_the_out_argument_that_was_specified)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_2_out_args(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(&injected_int, sizeof(injected_int));
    int actual_int_1 = 0;
    int actual_int_2 = 0;

    // act
    (void)test_dependency_2_out_args(&actual_int_1, &actual_int_2);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int, actual_int_1);
    ASSERT_ARE_EQUAL(int, 0, actual_int_2);
}

/* Tests_SRS_UMOCK_C_LIB_01_154: [ The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function. ] */
/* Tests_SRS_UMOCK_C_LIB_01_159: [ The argument targetted by CopyOutArgumentBuffer_{arg_name} shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_only_copies_bytes_to_the_second_out_argument)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_2_out_args(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_b(&injected_int, sizeof(injected_int));
    int actual_int_1 = 0;
    int actual_int_2 = 0;

    // act
    (void)test_dependency_2_out_args(&actual_int_1, &actual_int_2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, actual_int_1);
    ASSERT_ARE_EQUAL(int, injected_int, actual_int_2);
}

/* Tests_SRS_UMOCK_C_LIB_01_155: [ The memory shall be copied. ]*/
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_copies_the_memory_for_later_use)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(&injected_int, sizeof(injected_int));
    int actual_int = 0;
    injected_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_163: [ The buffers for previous CopyOutArgumentBuffer calls shall be freed. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_156: [ If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept. ]*/
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_frees_allocated_buffers_for_previous_CopyOutArgumentBuffer)
{
    // arrange
    int injected_int = 0x42;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(&injected_int, sizeof(injected_int))
        .CopyOutArgumentBuffer_a(&injected_int, sizeof(injected_int));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, actual_int);
}

/* Tests_SRS_UMOCK_C_LIB_01_158: [ If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_with_NULL_bytes_triggers_the_error_callback)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(NULL, sizeof(int));

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_INVALID_ARGUMENT_BUFFER, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_158: [ If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_with_0_length_triggers_the_error_callback)
{
    // arrange
    int injected_int = 0x42;

    // act
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(&injected_int, 0);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_INVALID_ARGUMENT_BUFFER, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_154: [ The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function. ] */
/* Tests_SRS_UMOCK_C_LIB_01_159: [ The argument targetted by CopyOutArgumentBuffer_{arg_name} shall also be marked as ignored. ] */
TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_when_an_error_occurs_preserves_the_previous_state)
{
    // arrange
    int injected_int = 0x42;
    int injected_int_2 = 0x43;
    EXPECTED_CALL(test_dependency_2_out_args(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_b(&injected_int, sizeof(injected_int))
        .CopyOutArgumentBuffer(0, &injected_int_2, sizeof(injected_int_2));
    int actual_int_1 = 0;
    int actual_int_2 = 0;

    // act
    (void)test_dependency_2_out_args(&actual_int_1, &actual_int_2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, actual_int_1);
    ASSERT_ARE_EQUAL(int, injected_int, actual_int_2);
}

TEST_FUNCTION(CopyOutArgumentBuffer_arg_name_overrides_the_buffer_for_CopyOutArgumentBuffer)
{
    // arrange
    int injected_int = 0x42;
    int injected_int_2 = 0x43;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int))
        .CopyOutArgumentBuffer_a(&injected_int_2, sizeof(injected_int_2));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int_2, actual_int);
}

TEST_FUNCTION(CopyOutArgumentBuffer_overrides_the_buffer_for_CopyOutArgumentBuffer_arg_name)
{
    // arrange
    int injected_int = 0x42;
    int injected_int_2 = 0x43;
    EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer_a(&injected_int_2, sizeof(injected_int_2))
        .CopyOutArgumentBuffer(1, &injected_int, sizeof(injected_int));
    int actual_int = 0;

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(int, injected_int, actual_int);
}

/* ValidateArgumentBuffer */

/* Tests_SRS_UMOCK_C_LIB_01_095: [The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_096: [If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.]*/
/* Tests_SRS_UMOCK_C_LIB_01_097: [ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.]*/
TEST_FUNCTION(ValidateArgumentBuffer_checks_the_argument_buffer)
{
    // arrange
    int expected_int = 0x42;
    int actual_int = 0x42;
    STRICT_EXPECTED_CALL(test_dependency_1_out_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, &expected_int, sizeof(expected_int));

    // act
    (void)test_dependency_1_out_arg(&actual_int);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_095: [The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_096: [If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.]*/
/* Tests_SRS_UMOCK_C_LIB_01_097: [ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.]*/
TEST_FUNCTION(ValidateArgumentBuffer_checks_the_argument_buffer_and_mismatch_is_detected_when_content_does_not_match)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42 };
    unsigned char actual_buffer[] = { 0x43 };
    char actual_string[64];
    STRICT_EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, expected_buffer, sizeof(expected_buffer));

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    (void)sprintf(actual_string, "[test_dependency_buffer_arg(%p)]", actual_buffer);
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_buffer_arg([0x42])]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, actual_string, umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_095: [The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_096: [If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.]*/
/* Tests_SRS_UMOCK_C_LIB_01_097: [ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.]*/
TEST_FUNCTION(ValidateArgumentBuffer_checks_the_argument_buffer_and_mismatch_is_detected_when_content_does_not_match_for_expected_call)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42 };
    unsigned char actual_buffer[] = { 0x43 };
    char actual_string[64];
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, expected_buffer, sizeof(expected_buffer));

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    (void)sprintf(actual_string, "[test_dependency_buffer_arg(%p)]", actual_buffer);
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_buffer_arg([0x42])]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, actual_string, umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_099: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
TEST_FUNCTION(ValidateArgumentBuffer_with_0_index_triggers_an_error)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42 };
    unsigned char actual_buffer[] = { 0x43 };
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(0, expected_buffer, sizeof(expected_buffer));

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_099: [If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.]*/
TEST_FUNCTION(ValidateArgumentBuffer_with_index_higher_than_the_Arg_count_triggers_an_error)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42 };
    unsigned char actual_buffer[] = { 0x43 };
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(2, expected_buffer, sizeof(expected_buffer));

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_ARG_INDEX_OUT_OF_RANGE, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_100: [If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.] */
TEST_FUNCTION(ValidateArgumentBuffer_with_NULL_buffer_triggers_the_error_callback)
{
    // arrange

    // act
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, NULL, 1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_INVALID_ARGUMENT_BUFFER, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_100: [If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.] */
TEST_FUNCTION(ValidateArgumentBuffer_with_0_length_triggers_the_error_callback)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42 };

    // act
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, expected_buffer, 0);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_INVALID_ARGUMENT_BUFFER, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_095: [The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_096: [If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.]*/
/* Tests_SRS_UMOCK_C_LIB_01_097: [ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.]*/
TEST_FUNCTION(ValidateArgumentBuffer_with_2_bytes_and_first_byte_different_checks_the_content)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42, 0x41 };
    unsigned char actual_buffer[] = { 0x43, 0x41 };
    char actual_string[64];
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, expected_buffer, sizeof(expected_buffer));

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    (void)sprintf(actual_string, "[test_dependency_buffer_arg(%p)]", actual_buffer);
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_buffer_arg([0x42 0x41])]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, actual_string, umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_095: [The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.] */
/* Tests_SRS_UMOCK_C_LIB_01_096: [If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.]*/
/* Tests_SRS_UMOCK_C_LIB_01_097: [ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.]*/
TEST_FUNCTION(ValidateArgumentBuffer_with_2_bytes_and_second_byte_different_checks_the_content)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42, 0x41 };
    unsigned char actual_buffer[] = { 0x42, 0x42 };
    char actual_string[64];
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, expected_buffer, sizeof(expected_buffer));

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    (void)sprintf(actual_string, "[test_dependency_buffer_arg(%p)]", actual_buffer);
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_buffer_arg([0x42 0x41])]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, actual_string, umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_131: [ The memory pointed by bytes shall be copied. ]*/
TEST_FUNCTION(ValidateArgumentBuffer_copies_the_bytes_to_compare)
{
    // arrange
    unsigned char expected_buffer[] = { 0x42 };
    unsigned char actual_buffer[] = { 0x42 };
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, expected_buffer, sizeof(expected_buffer));

    expected_buffer[0] = 0x43;

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_132: [ If several calls to ValidateArgumentBuffer are made, only the last buffer shall be kept. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_130: [ The buffers for previous ValidateArgumentBuffer calls shall be freed. ]*/
TEST_FUNCTION(When_ValidateArgumentBuffer_is_called_twice_the_last_buffer_is_used)
{
    // arrange
    unsigned char expected_buffer1[] = { 0x43 };
    unsigned char expected_buffer2[] = { 0x42 };
    unsigned char actual_buffer[] = { 0x42 };
    EXPECTED_CALL(test_dependency_buffer_arg(IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(1, expected_buffer1, sizeof(expected_buffer1))
        .ValidateArgumentBuffer(1, expected_buffer2, sizeof(expected_buffer2));

    // act
    test_dependency_buffer_arg(actual_buffer);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* REGISTER_GLOBAL_MOCK_HOOK */

/* Tests_SRS_UMOCK_C_LIB_01_104: [The REGISTER_GLOBAL_MOCK_HOOK shall register a mock hook to be called every time the mocked function is called by production code.]*/
/* Tests_SRS_UMOCK_C_LIB_01_105: [The hook’s result shall be returned by the mock to the production code.]*/
/* Tests_SRS_UMOCK_C_LIB_01_106: [The signature for the hook shall be assumed to have exactly the same arguments and return as the mocked function.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_HOOK_registers_a_hook_for_the_mock)
{
    // arrange
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_no_args, my_hook_test_dependency_no_args);
    my_hook_result = 0x42;

    // act
    int result = test_dependency_no_args();

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_104: [The REGISTER_GLOBAL_MOCK_HOOK shall register a mock hook to be called every time the mocked function is called by production code.]*/
/* Tests_SRS_UMOCK_C_LIB_01_105: [The hook’s result shall be returned by the mock to the production code.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_HOOK_registers_a_hook_for_the_mock_that_returns_2_different_values)
{
    // arrange
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_no_args, my_hook_test_dependency_no_args);
    my_hook_result = 0x42;

    // act
    int call1_result = test_dependency_no_args();
    int call2_result = test_dependency_no_args();

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, call1_result);
    ASSERT_ARE_EQUAL(int, 0x43, call2_result);
}

/* Tests_SRS_UMOCK_C_LIB_01_107: [If there are multiple invocations of REGISTER_GLOBAL_MOCK_HOOK, the last one shall take effect over the previous ones.] */
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_HOOK_twice_makes_the_last_hook_stick)
{
    // arrange
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_no_args, my_hook_test_dependency_no_args);
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_no_args, my_hook_test_dependency_no_args_2);
    my_hook_result = 0x42;

    // act
    int result = test_dependency_no_args();

    // assert
    ASSERT_ARE_EQUAL(int, 0x21, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_134: [ REGISTER_GLOBAL_MOCK_HOOK called with a NULL hook unregisters a previously registered hook. ]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_HOOK_with_NULL_unregisters_a_previously_registered_hook)
{
    // arrange
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_no_args, my_hook_test_dependency_no_args);
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_no_args, NULL);

    // act
    int result = test_dependency_no_args();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_135: [ All parameters passed to the mock shall be passed down to the mock hook. ]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_HOOK_the_args_of_the_mock_get_passed_to_the_hook)
{
    // arrange
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_2_args, my_hook_test_dependency_2_args);

    // act
    (void)test_dependency_2_args(0x42, 0x43);

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, arg_a);
    ASSERT_ARE_EQUAL(int, 0x43, arg_b);
}

/* Tests_SRS_UMOCK_C_LIB_01_104: [The REGISTER_GLOBAL_MOCK_HOOK shall register a mock hook to be called every time the mocked function is called by production code.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_HOOK_with_a_function_that_returns_void_works)
{
    // arrange
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_void_return, my_hook_test_dependency_void_return);

    // act
    test_dependency_void_return();

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_dependency_void_return_called);
}

/* REGISTER_GLOBAL_MOCK_RETURN */

/* Tests_SRS_UMOCK_C_LIB_01_108: [The REGISTER_GLOBAL_MOCK_RETURN shall register a return value to always be returned by a mock function.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_RETURN_makes_a_subsequent_call_to_the_mock_return_the_value)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_global_mock_return_test, 0x45);

    // act
    int result = test_dependency_global_mock_return_test();

    // assert
    ASSERT_ARE_EQUAL(int, 0x45, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_109: [If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURN, the last one shall take effect over the previous ones.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_RETURN_twice_only_makes_the_second_call_stick)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_global_mock_return_test, 0x45);
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_global_mock_return_test, 0x46);

    // act
    int result = test_dependency_global_mock_return_test();

    // assert
    ASSERT_ARE_EQUAL(int, 0x46, result);
}

/* REGISTER_GLOBAL_MOCK_FAIL_RETURN */

/* Tests_SRS_UMOCK_C_LIB_01_111: [The REGISTER_GLOBAL_MOCK_FAIL_RETURN shall register a fail return value to be returned by a mock function when marked as failed in the expected calls.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_FAIL_RETURN_is_possible_and_does_not_affect_the_return_value)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_global_mock_return_test, 0x42);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(test_dependency_global_mock_return_test, 0x45);

    // act
    int result = test_dependency_global_mock_return_test();

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_112: [If there are multiple invocations of REGISTER_GLOBAL_FAIL_MOCK_RETURN, the last one shall take effect over the previous ones.]*/
TEST_FUNCTION(Multiple_REGISTER_GLOBAL_MOCK_FAIL_RETURN_calls_are_possible)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_global_mock_return_test, 0x42);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(test_dependency_global_mock_return_test, 0x45);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(test_dependency_global_mock_return_test, 0x46);

    // act
    int result = test_dependency_global_mock_return_test();

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_142: [ If any error occurs during REGISTER_GLOBAL_MOCK_FAIL_RETURN, umock_c shall raise an error with the code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(When_copy_fails_in_REGISTER_GLOBAL_MOCK_FAIL_RETURN_then_on_error_is_triggered)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_global_mock_return_test, 0x42);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(test_dependency_global_mock_return_test, 0x45);

    // act
    int result = test_dependency_global_mock_return_test();

    // assert
    ASSERT_ARE_EQUAL(int, 0x42, result);
}

/* REGISTER_GLOBAL_MOCK_RETURNS */

/* Tests_SRS_UMOCK_C_LIB_01_113: [The REGISTER_GLOBAL_MOCK_RETURNS shall register both a success and a fail return value associated with a mock function.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_RETURNS_registers_the_return_value)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURNS(test_dependency_global_mock_return_test, 0xAA, 0x43);

    // act
    int result = test_dependency_global_mock_return_test();

    // assert
    ASSERT_ARE_EQUAL(int, 0xAA, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_114: [If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURNS, the last one shall take effect over the previous ones.]*/
TEST_FUNCTION(REGISTER_GLOBAL_MOCK_RETURNS_twice_makes_only_the_last_call_stick)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURNS(test_dependency_global_mock_return_test, 0xAA, 0x43);
    REGISTER_GLOBAL_MOCK_RETURNS(test_dependency_global_mock_return_test, 0xAB, 0x44);

    // act
    int result = test_dependency_global_mock_return_test();

    // assert
    ASSERT_ARE_EQUAL(int, 0xAB, result);
}

/* Type names */

/* Tests_SRS_UMOCK_C_LIB_01_145: [ Since umock_c needs to maintain a list of registered types, the following rules shall be applied: ]*/
/* Tests_SRS_UMOCK_C_LIB_01_146: [ Each type shall be normalized to a form where all extra spaces are removed. ]*/
TEST_FUNCTION(spaces_are_stripped_from_typenames)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_type_with_space("b"));

    // act
    test_dependency_type_with_space("b");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Supported types */

/* Tests_SRS_UMOCK_C_LIB_01_144: [ Out of the box umock_c shall support the following types through the header umocktypes_c.h: ]*/
/* Tests_SRS_UMOCK_C_LIB_01_028: [**char**] */
/* Tests_SRS_UMOCK_C_LIB_01_029 : [**unsigned char**] */
/* Tests_SRS_UMOCK_C_LIB_01_030 : [**short**] */
/* Tests_SRS_UMOCK_C_LIB_01_031 : [**unsigned short**] */
/* Tests_SRS_UMOCK_C_LIB_01_032 : [**int**] */
/* Tests_SRS_UMOCK_C_LIB_01_033 : [**unsigned int**] */
/* Tests_SRS_UMOCK_C_LIB_01_034 : [**long**] */
/* Tests_SRS_UMOCK_C_LIB_01_035 : [**unsigned long**] */
/* Tests_SRS_UMOCK_C_LIB_01_036 : [**long long**] */
/* Tests_SRS_UMOCK_C_LIB_01_037 : [**unsigned long long**] */
/* Tests_SRS_UMOCK_C_LIB_01_038 : [**float**] */
/* Tests_SRS_UMOCK_C_LIB_01_039 : [**double**] */
/* Tests_SRS_UMOCK_C_LIB_01_040 : [**long double**] */
/* Tests_SRS_UMOCK_C_LIB_01_041 : [**size_t**] */
/* Tests_SRS_UMOCK_C_LIB_01_151: [ void\* ]*/
/* Tests_SRS_UMOCK_C_LIB_01_152: [ const void\* ]*/
TEST_FUNCTION(native_c_types_are_supported)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_all_types(-42, 42, /* char */
        -43, 43, /* short */
        -44, 44, /* int */
        -45, 45, /* long */
        -46, 46, /* long long */
        -42.42f,  /* float */
        4242.42, /* double */
        4242.42, /* long double */
        0x42, /* size_t*/
        (void*)0x42, /* void* */
        (const void*)0x42 /* const void* */
        ));

    // act
    test_dependency_all_types(-42, 42, /* char */
        -43, 43, /* short */
        -44, 44, /* int */
        -45, 45, /* long */
        -46, 46, /* long long */
        -42.42f,  /* float */
        4242.42, /* double */
        4242.42, /* long double */
        0x42, /* size_t*/
        (void*)0x42, /* void* */
        (const void*)0x42 /* const void* */
        );

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, test_on_umock_c_error_call_count);
}

/* Tests_SRS_UMOCK_C_LIB_01_148: [ If call comparison fails an error shall be indicated by calling the error callback with UMOCK_C_COMPARE_CALL_ERROR. ]*/
TEST_FUNCTION(when_a_type_is_not_supported_an_error_is_triggered)
{
    TEST_STRUCT_NOT_REGISTERED a = { 0 };

    // arrange
    STRICT_EXPECTED_CALL(test_dependency_type_not_registered(a));

    // act
    test_dependency_type_not_registered(a);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
}

/* Call comparison rules */

/* Tests_SRS_UMOCK_C_LIB_01_136: [ When multiple return values are set for a mock function by using different means (such as SetReturn), the following order shall be in effect: ]*/
/* Tests_SRS_UMOCK_C_LIB_01_137: [ - If a return value has been specified for an expected call then that value shall be returned. ]*/
TEST_FUNCTION(when_the_return_value_is_given_by_SetReturn_then_it_is_returned)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42))
        .SetReturn(42);

    // act
    int result = test_dependency_1_arg(42);

    // assert
    ASSERT_ARE_EQUAL(int, 42, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_136: [ When multiple return values are set for a mock function by using different means (such as SetReturn), the following order shall be in effect: ]*/
/* Tests_SRS_UMOCK_C_LIB_01_137: [ - If a return value has been specified for an expected call then that value shall be returned. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_138: [ - If a global mock hook has been specified then it shall be called and its result returned. ]*/
TEST_FUNCTION(when_the_return_value_is_given_by_SetReturn_for_a_function_with_a_global_return_hook_the_SetReturn_value_is_returned)
{
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_with_global_mock_hook, my_hook_test_dependency_with_global_mock_hook);

    // arrange
    STRICT_EXPECTED_CALL(test_dependency_with_global_mock_hook())
        .SetReturn(42);

    // act
    int result = test_dependency_with_global_mock_hook();

    // assert
    ASSERT_ARE_EQUAL(int, 42, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_136: [ When multiple return values are set for a mock function by using different means (such as SetReturn), the following order shall be in effect: ]*/
/* Tests_SRS_UMOCK_C_LIB_01_137: [ - If a return value has been specified for an expected call then that value shall be returned. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_138: [ - If a global mock hook has been specified then it shall be called and its result returned. ]*/
TEST_FUNCTION(when_the_return_value_is_not_given_by_SetReturn_for_a_function_with_a_global_return_hook_the_mock_hook_return_value_is_returned)
{
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_with_global_mock_hook, my_hook_test_dependency_with_global_mock_hook);

    // arrange
    STRICT_EXPECTED_CALL(test_dependency_with_global_mock_hook());

    // act
    int result = test_dependency_with_global_mock_hook();

    // assert
    ASSERT_ARE_EQUAL(int, 43, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_139: [ - If a global return value has been specified then it shall be returned. ]*/
TEST_FUNCTION(when_the_return_value_is_given_by_SetReturn_for_a_function_with_a_global_return_hook_and_global_return_the_SetReturn_value_is_returned)
{
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_with_global_mock_hook, my_hook_test_dependency_with_global_mock_hook);
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_with_global_mock_hook, 44);

    // arrange
    STRICT_EXPECTED_CALL(test_dependency_with_global_mock_hook())
        .SetReturn(42);

    // act
    int result = test_dependency_with_global_mock_hook();

    // assert
    ASSERT_ARE_EQUAL(int, 42, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_139: [ - If a global return value has been specified then it shall be returned. ]*/
TEST_FUNCTION(when_the_return_value_is_not_given_by_SetReturn_for_a_function_with_a_global_return_hook_and_global_return_the_global_mock_hook_value_is_returned)
{
    REGISTER_GLOBAL_MOCK_HOOK(test_dependency_with_global_mock_hook, my_hook_test_dependency_with_global_mock_hook);
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_with_global_mock_hook, 44);

    // arrange
    STRICT_EXPECTED_CALL(test_dependency_with_global_mock_hook());

    // act
    int result = test_dependency_with_global_mock_hook();

    // assert
    ASSERT_ARE_EQUAL(int, 43, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_139: [ - If a global return value has been specified then it shall be returned. ]*/
TEST_FUNCTION(when_the_return_value_is_specified_only_by_global_return_that_global_return_value_is_returned)
{
    REGISTER_GLOBAL_MOCK_RETURN(test_dependency_with_global_return, 44);

    // arrange
    STRICT_EXPECTED_CALL(test_dependency_with_global_return());

    // act
    int result = test_dependency_with_global_return();

    // assert
    ASSERT_ARE_EQUAL(int, 44, result);
}

/* Tests_SRS_UMOCK_C_LIB_01_140: [ - Otherwise the value of a static variable of the same type as the return type shall be returned. ]*/
TEST_FUNCTION(when_no_return_value_is_specified_for_a_function_returning_int_0_is_returned)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_returning_int());

    // act
    int result = test_dependency_returning_int();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* MOCK_FUNCTION_WITH_CODE tests */

TEST_FUNCTION(a_strict_expected_Call_mock_function_with_code_validates_args)
{
    // arrange
    STRICT_EXPECTED_CALL(test_mock_function_with_code_1_arg(42));

    // act
    test_mock_function_with_code_1_arg(42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

TEST_FUNCTION(an_expected_call_for_a_mock_function_with_code_ignores_args)
{
    // arrange
    EXPECTED_CALL(test_mock_function_with_code_1_arg(0));

    // act
    test_mock_function_with_code_1_arg(42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

TEST_FUNCTION(the_value_for_a_function_that_returns_a_char_ptr_is_freed)
{
    // arrange
    EXPECTED_CALL(test_mock_function_returning_string())
        .SetReturn("a");

    // act
    const char* result = test_mock_function_returning_string();

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "a", result);
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

TEST_FUNCTION(the_value_for_a_function_that_returns_a_char_ptr_is_freed_when_no_matched_return)
{
    // arrange

    // act
    const char* result = test_mock_function_returning_string();

    // assert
    ASSERT_IS_NULL(result);
}

TEST_FUNCTION(the_value_for_a_function_that_returns_a_char_ptr_with_a_default_is_freed_when_no_matched_return)
{
    // arrange

    // act
    const char* result = test_mock_function_returning_string_with_code();

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "a", result);
}

TEST_FUNCTION(the_value_for_a_function_that_returns_a_char_ptr_set_by_macro_is_freed)
{
    // arrange
    REGISTER_GLOBAL_MOCK_RETURN(test_mock_function_returning_string_with_macro, "a");

    // act
    const char* result = test_mock_function_returning_string_with_macro();

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "a", result);
}

/* REGISTER_UMOCK_ALIAS_TYPE */

/* Tests SRS_UMOCK_C_LIB_01_149: [ REGISTER_UMOCK_ALIAS_TYPE registers a new alias type for another type. ]*/
TEST_FUNCTION(registering_an_alias_type_works)
{
    // arrange
    REGISTER_UMOCK_ALIAS_TYPE(funkytype, int);
    STRICT_EXPECTED_CALL(test_mock_function_with_funkytype(42))
        .SetReturn(42);

    // act
    funkytype result = test_mock_function_with_funkytype(42);

    // assert
    ASSERT_ARE_EQUAL(int, 42, (int)result);
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_153: [ If no custom handler has beed registered for a pointer type, it shall be trated as void*. ] */
TEST_FUNCTION(when_an_unregistered_pointer_type_is_used_it_defaults_to_void_ptr)
{
    // arrange
    REGISTER_UMOCK_ALIAS_TYPE(funkytype, int);
    STRICT_EXPECTED_CALL(test_mock_function_with_unregistered_ptr_type((unsigned char***)0x42))
        .SetReturn((unsigned char***)0x42);

    // act
    unsigned char*** result = test_mock_function_with_unregistered_ptr_type((unsigned char***)0x42);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x42, result);
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_179: [ IMPLEMENT_UMOCK_C_ENUM_TYPE shall implement umock_c handlers for an enum type. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_180: [ The variable arguments are a list making up the enum values. ]*/
TEST_FUNCTION(matching_with_an_enum_type_works)
{
    // arrange
    REGISTER_TYPE(TEST_ENUM, TEST_ENUM);
    STRICT_EXPECTED_CALL(test_mock_function_with_enum_type(TEST_ENUM_VALUE_1));

    // act
    test_mock_function_with_enum_type(TEST_ENUM_VALUE_2);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_mock_function_with_enum_type(TEST_ENUM_VALUE_1)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_mock_function_with_enum_type(TEST_ENUM_VALUE_2)]", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_179: [ IMPLEMENT_UMOCK_C_ENUM_TYPE shall implement umock_c handlers for an enum type. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_181: [ If a value that is not part of the enum is used, it shall be treated as an int value. ]*/
TEST_FUNCTION(when_the_enum_value_is_not_within_the_enum_the_int_value_is_filled_in)
{
    // arrange
    REGISTER_TYPE(TEST_ENUM, TEST_ENUM);
    STRICT_EXPECTED_CALL(test_mock_function_with_enum_type((TEST_ENUM)(TEST_ENUM_VALUE_1+2)));

    // act
    test_mock_function_with_enum_type(TEST_ENUM_VALUE_2);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_mock_function_with_enum_type(2)]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_mock_function_with_enum_type(TEST_ENUM_VALUE_2)]", umock_c_get_actual_calls());
}

/* CaptureReturn */

/* Tests_SRS_UMOCK_C_LIB_01_179: [ The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call. ]*/
TEST_FUNCTION(capture_return_captures_the_return_value)
{
    // arrange
    int captured_return;

    STRICT_EXPECTED_CALL(test_dependency_for_capture_return())
        .CaptureReturn(&captured_return);

    // act
    test_dependency_for_capture_return();

    // assert
    ASSERT_ARE_EQUAL(int, 42, captured_return);
}

/* Tests_SRS_UMOCK_C_LIB_01_180: [ If CaptureReturn is called multiple times for the same call, an error shall be indicated with the code UMOCK_C_CAPTURE_RETURN_ALREADY_USED. ]*/
TEST_FUNCTION(capture_return_twice_captures_the_return_value_in_the_pointer_indicated_by_the_second_call)
{
    // arrange
    int captured_return_1;
    int captured_return_2;

    STRICT_EXPECTED_CALL(test_dependency_for_capture_return())
        .CaptureReturn(&captured_return_1)
        .CaptureReturn(&captured_return_2);

    // act
    test_dependency_for_capture_return();

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_CAPTURE_RETURN_ALREADY_USED, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_182: [ If captured_return_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT. ]*/
TEST_FUNCTION(capture_return_with_NULL_argument_indicates_an_error)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_for_capture_return())
        .CaptureReturn(NULL);

    // act
    test_dependency_for_capture_return();

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_NULL_ARGUMENT, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_179: [ The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call. ]*/
TEST_FUNCTION(capture_return_when_no_actual_call_does_not_capture_anything)
{
    // arrange
    int captured_return = 0;

    STRICT_EXPECTED_CALL(test_dependency_for_capture_return())
        .CaptureReturn(&captured_return);

    // act

    // assert
    ASSERT_ARE_EQUAL(int, 0, captured_return);
}

/* Tests_SRS_UMOCK_C_LIB_01_179: [ The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call. ]*/
TEST_FUNCTION(capture_return_when_no_matching_actual_call_does_not_capture_anything)
{
    // arrange
    int captured_return = 0;

    STRICT_EXPECTED_CALL(test_dependency_for_capture_return_with_arg(42))
        .CaptureReturn(&captured_return);

    // act
    test_dependency_for_capture_return_with_arg(41);

    // assert
    ASSERT_ARE_EQUAL(int, 0, captured_return);
}

/* Tests_SRS_UMOCK_C_LIB_01_179: [ The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call. ]*/
TEST_FUNCTION(capture_return_takes_into_account_a_set_return_call)
{
    // arrange
    int captured_return = 0;

    STRICT_EXPECTED_CALL(test_dependency_for_capture_return())
        .SetReturn(42)
        .CaptureReturn(&captured_return);

    // act
    test_dependency_for_capture_return();

    // assert
    ASSERT_ARE_EQUAL(int, 42, captured_return);
}

/* Tests_SRS_UMOCK_C_LIB_01_179: [ The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call. ]*/
TEST_FUNCTION(capture_return_captures_the_return_value_different_value)
{
    // arrange
    int captured_return = 0;

    STRICT_EXPECTED_CALL(test_dependency_for_capture_return())
        .CaptureReturn(&captured_return);

    test_return_value = 45;

    // act
    test_dependency_for_capture_return();

    // assert
    ASSERT_ARE_EQUAL(int, 45, captured_return);
}

/* ValidateArgumentValue_{arg_name} */

/* Tests_SRS_UMOCK_C_LIB_01_183: [ The ValidateArgumentValue_{arg_name} shall validate that the value of an argument matches the value pointed by arg_value. ]*/
TEST_FUNCTION(validate_argument_value_validates_the_value_pointed_by_arg_value)
{
    // arrange
    int arg_value = 0;

    STRICT_EXPECTED_CALL(test_dependency_1_arg(0))
        .ValidateArgumentValue_a(&arg_value);

    arg_value = 42;

    // act
    (void)test_dependency_1_arg(42);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_183: [ The ValidateArgumentValue_{arg_name} shall validate that the value of an argument matches the value pointed by arg_value. ]*/
TEST_FUNCTION(validate_argument_value_validates_the_value_pointed_by_arg_value_for_a_char_star)
{
    // arrange
    char* arg_value = "42";

    STRICT_EXPECTED_CALL(test_dependency_char_star_arg(NULL))
        .ValidateArgumentValue_s(&arg_value);

    arg_value = "43";

    // act
    (void)test_dependency_char_star_arg("43");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_184: [ If arg_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT. ]*/
TEST_FUNCTION(validate_argument_value_with_NULL_value_triggers_an_error)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(test_dependency_char_star_arg(NULL))
        .ValidateArgumentValue_s(NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_NULL_ARGUMENT, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_LIB_01_185: [ The ValidateArgumentValue_{arg_name} modifier shall inhibit comparing with any value passed directly as an argument in the expected call. ]*/
TEST_FUNCTION(validate_argument_value_overrides_existing_arg_value)
{
    // arrange
    char* arg_value = "42";

    STRICT_EXPECTED_CALL(test_dependency_char_star_arg("42"))
        .ValidateArgumentValue_s(&arg_value);

    arg_value = "43";

    // act
    (void)test_dependency_char_star_arg("43");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "", umock_c_get_actual_calls());
}

/* Tests_SRS_UMOCK_C_LIB_01_186: [ The ValidateArgumentValue_{arg_name} shall implicitly do a ValidateArgument for the arg_name argument, making sure the argument is not ignored. ]*/
TEST_FUNCTION(validate_argument_value_shall_implicitly_validate_the_argument)
{
    // arrange
    char* arg_value = "42";

    STRICT_EXPECTED_CALL(test_dependency_char_star_arg("42"))
        .IgnoreArgument_s()
        .ValidateArgumentValue_s(&arg_value);

    arg_value = "41";

    // act
    (void)test_dependency_char_star_arg("43");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_char_star_arg(\"41\")]", umock_c_get_expected_calls());
    ASSERT_ARE_EQUAL(char_ptr, "[test_dependency_char_star_arg(\"43\")]", umock_c_get_actual_calls());
}

END_TEST_SUITE(umock_c_integrationtests)

#umock_c
 
#Overview

umock_c is a C mocking library that exposes APIs to allow:
-	defining mock functions, 
-	recording expected calls 
-	comparing expected calls with actual calls. 
On top of the basic functionality, additional convenience features like modifiers on expected calls are provided.

#Simple example

A test written with umock_c looks like below:

Let’s assume unit A depends on unit B. unit B has a function called test_dependency_1_arg.

In unit B’s header one would write:

```c
#include "umock_prod.h"

MOCKABLE_FUNCTION(, int, test_dependency_1_arg, int, a);
```

Let’s assume unit A has a function called function_under_test.

```c
int function_under_test();
{
    int result = test_dependency_1_arg(x);
    return result;
}
```

A test that checks that function_under_test calls its dependency and injects a return value, while ignoring all arguments on the call looks like this:

```c
TEST_FUNCTION(my_first_test)
{
    // arrange
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42))
        .SetReturn(44)
        .IgnoreAllArguments();

    // act
    int result = function_under_test();

    // assert
    ASSERT_ARE_EQUAL(int, 44, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}
```

#Exposed API (umock_c.h)

```c
typedef enum UMOCK_C_ERROR_CODE_TAG
{
    UMOCK_C_ARG_INDEX_OUT_OF_RANGE,
    UMOCK_C_MALLOC_ERROR,
    UMOCK_C_INVALID_ARGUMENT_BUFFER,
    UMOCK_C_COMPARE_CALL_ERROR,
    UMOCK_C_RESET_CALLS_ERROR,
    UMOCK_C_CAPTURE_RETURN_ALREADY_USED,
    UMOCK_C_NULL_ARGUMENT,
    UMOCK_C_ERROR
} UMOCK_C_ERROR_CODE;

typedef void(*ON_UMOCK_C_ERROR)(UMOCK_C_ERROR_CODE error_code);

#define IGNORED_PTR_ARG (NULL)
#define IGNORED_NUM_ARG (0)

#define MOCKABLE_FUNCTION(modifiers, result, function, ...) \
	...

#define REGISTER_GLOBAL_MOCK_HOOK(mock_function, mock_hook_function) \
    ...

#define REGISTER_GLOBAL_MOCK_RETURN(mock_function, return_value) \
    ...

#define REGISTER_GLOBAL_MOCK_FAIL_RETURN(mock_function, fail_return_value) \
    ...

#define REGISTER_GLOBAL_MOCK_RETURNS(mock_function, return_value, fail_return_value) \
    ...
    ...

#define STRICT_EXPECTED_CALL(call) \
	...

#define EXPECTED_CALL(call) \
	...

    extern int umock_c_init(ON_UMOCK_C_ERROR on_umock_c_error);
    extern void umock_c_deinit(void);
    extern void umock_c_reset_all_calls(void);
    extern const char* umock_c_get_actual_calls(void);
    extern const char* umock_c_get_expected_calls(void);
```

##Mock definitions API

###MOCKABLE_FUNCTION

```c
MOCKABLE_FUNCTION(modifiers, result, function, ...)
```

MOCKABLE_FUNCTION shall be used to wrap function definition allow the user to declare a function that can be mocked.

The macro shall generate a function signature in case ENABLE_MOCKS is not defined.

Example:

```c
MOCKABLE_FUNCTION(FAR, int, test_function, int, arg1)
```

should generate for production code:

```c
int FAR test_function(int arg1);
```

###MOCK_FUNCTION_WITH_CODE

MOCK_FUNCTION_WITH_CODE shall define a mock function and allow the user to embed code between this define and a MOCK_FUNCTION_END call.

```c
MOCK_FUNCTION_WITH_CODE(, void, test_mock_function_with_code_1_arg, int, a);
    int some_value = 42;
    /* more code here */
MOCK_FUNCTION_END()
```

###ENABLE_MOCKS

If ENABLE_MOCKS is defined, MOCKABLE_FUNCTION shall generate the declaration of the function and code for the mocked function, thus allowing setting up of expectations in test functions. If ENABLE_MOCKS is not defined, MOCKABLE_FUNCTION shall only generate a declaration for the function.

ENABLE_MOCKS should be used in the translation unit that contains the tests just before including the headers for all the units that the code under test depends on. Example:

```c

#include <stdlib.h>
// ... other various includes

#define ENABLE_MOCKS
#include "test_dependency.h"

// ... tests

```

Note that it is possible (and sometimes necessary) to undefine ENABLE_MOCKS:

```c

#include <stdlib.h>
// ... other various includes

#define ENABLE_MOCKS
#include "test_dependency.h"
#undef ENABLE_MOCKS

#include "unit_under_test.h"

// ... tests

```

##umock init/deinit

###umock_c_init

```c
int umock_c_init(ON_UMOCK_C_ERROR on_umock_c_error);
```

umock_c_init is needed before performing any action related to umock_c calls (or registering any types).

umock_c_init shall initialize umock_c.

umock_c_init called if already initialized shall fail and return a non-zero value.
 
umock_c_init shall initialize the umock supported types (C native types).

on_umock_c_error can be NULL.

If on_umock_c_error is non-NULL it shall be saved for later use (to be invoked whenever an umock_c error needs to be signaled to the user).

###umock_c_deinit

```c
void umock_c_deinit(void);
```

umock_c_deinit shall free all umock_c used resources.
If umock_c was not initialized, umock_c_deinit shall do nothing.

##Expected calls recording API

###STRICT_EXPECTED_CALL

```c
STRICT_EXPECTED_CALL(call)
```

STRICT_EXPECTED_CALL shall record that a certain call is expected.
For each argument the argument value shall be stored for later comparison with actual calls.

The call argument shall be the complete function invocation.

Examples:

```c
STRICT_EXPECTED_CALL(test_dependency_1_arg(42));
STRICT_EXPECTED_CALL(test_dependency_string("test"));
```

###EXPECTED_CALL

```c
EXPECTED_CALL(call)
```

EXPECTED_CALL shall record that a certain call is expected.
No arguments shall be saved by default, unless other modifiers state it.

The call argument shall be the complete function invocation.

Example:

```c
EXPECTED_CALL(test_dependency_1_arg(42));
```

##Call comparison API

###umock_c_reset_all_calls

```c
void umock_c_reset_all_calls(void);
```

umock_c_reset_all_calls shall reset all calls (actual and expected).
In case of any error, umock_c_reset_all_calls shall indicate the error through a call to the on_error callback.

###umock_c_get_expected_calls

```c
const char* umock_c_get_expected_calls(void);
```

umock_c_get_expected_calls shall return all the calls that were expected, but were not fulfilled.

For each call, the format shall be "functionName(argument 1 value, ...)".
Each call shall be enclosed in "[]".

Example:

For a call with the signature:

```c
int test_dependency_2_args(int a, int b)
```

if an expected call was recorded:

```c
STRICT_EXPECTED_CALL(test_dependency_2_args(42, 1));
```

umock_c_get_expected_calls would return:

```c
"[test_dependency_2_args(42,1)]"
```

###umock_c_get_actual_calls

```c
const char* umock_c_get_actual_calls(void);
```

umock_c_get_actual_calls shall return all the actual calls that were not matched to expected calls.

For each call, the format shall be "functionName(argument 1 value, ...)".
Each call shall be enclosed in "[]". A call to umock_c_get_actual_calls shall not modify the actual calls that were recorded. 

Example:

For a call with the signature:

```c
int test_dependency_2_args(int a, int b)
```

if an actual call was recorded:

```c
test_dependency_2_args(42, 2);
```

umock_c_get_actual_calls would return:

```c
"[test_dependency_2_args(42,2)]"
```

### Call comparison rules

umock_c shall compare calls in order. That means that "[A()][B()]" is different than "[B()][A()]". 

When multiple return values are set for a mock function by using different means, the following order shall be in effect:

- If a return value has been specified for an expected call then that value shall be returned.
- If a global mock hook has been specified then it shall be called and its result returned.
- If a global return value has been specified then it shall be returned.
- Otherwise the value of a static variable of the same type as the return type shall be returned.

If call comparison fails an error shall be indicated by calling the error callback with UMOCK_C_COMPARE_CALL_ERROR.

##Supported types

###Out of the box

Out of the box umock_c shall support the following types through the header umocktypes_c.h:
-	char
-	unsigned char
-	short
-	unsigned short
-	int
-	unsigned int
-	long
-	unsigned long
-	long long
-	unsigned long long
-	float
-	double
-	long double
-	size_t
-	void*
-	const void*

###Pointer types

If no custom handler has beed registered for a pointer type, it shall be trated as void*. 

###Custom types

Custom types, like structures shall be supported by allowing the user to define a set of functions that can be used by umock_c to operate with these types.

Five functions shall be provided to umock_c:
-	A stringify function.

This function shall return the string representation of a value of the given type.

-	An are_equal function.

This function shall compare 2 values of the given type and return an int indicating whether they are equal (1 means equal, 0 means different).

-	A copy function.

This function shall make a copy of a value for the given type.

-	A free function.

This function shall free a copied value.

####umockvalue_stringify_type

```c
char* umockvalue_stringify_{type}(const {type}* value)
```

A stringify function shall allocate using malloc a char\* and fill it with a string representation of value.

If any error is encountered during building the string representation, umockvalue_stringify_type shall return NULL.

Example:

```c
char* umockvalue_stringify_int(const int* value)
{
    char* result;

    if (value == NULL)
    {
        result = NULL;
    }
    else
    {
        char temp_buffer[32];
        int length = sprintf(temp_buffer, "%d", *value);
        if (length < 0)
        {
            result = NULL;
        }
        else
        {
            result = (char*)malloc(length + 1);
            if (result != NULL)
            {
                memcpy(result, temp_buffer, length + 1);
            }
        }
    }

    return result;
}
```

####umockvalue_are_equal_type

```c
int umockvalue_are_equal_{type}(const {type}* left, const {type}* right)
```

The umockvalue_are_equal_type function shall return 1 if the 2 values are equal and 0 if they are not.

If both left and right are NULL, umockvalue_are_equal_type shall return 1.

If only one of left and right is NULL, umockvalue_are_equal_{type} shall return 0.

Example:

```c
int umockvalue_are_equal_int(const int* left, const int* right)
{
    int result;

    if (left == right)
    {
        result = 1;
    }
    else if ((left == NULL) || (right == NULL))
    {
        result = 0;
    }
    else
    {
        result = ((*left) == (*right)) ? 1 : 0;
    }

    return result;
}
```

####umockvalue_copy_type

```c
int umockvalue_copy_{type}({type}* destination, const {type}* source)
```

The umockvalue_copy_type function shall copy the value from source to destination.

On success umockvalue_copy_type shall return 0.

If any of the arguments is NULL, umockvalue_copy_type shall return a non-zero value.

If any error occurs during copying the value, umockvalue_copy_type shall return a non-zero value.

Example:

```c
int umockvalue_copy_int(int* destination, const int* source)
{
    int result;

    if ((destination == NULL) ||
        (source == NULL))
    {
        result = __LINE__;
    }
    else
    {
        *destination = *source;
        result = 0;
    }

    return result;
}
```

####umockvalue_free_type

```c
void umockvalue_free_{type}({type}* value)
```

The umockvalue_free_type function shall free a value previously copied using umockvalue_copy_type. If value is NULL, no free shall be performed.

Example:

```c
void umockvalue_free_int(int* value)
{
    /* no free required for int */
}
```

###Custom enum types

####IMPLEMENT_UMOCK_C_ENUM_TYPE

```c
IMPLEMENT_UMOCK_C_ENUM_TYPE(type, ...)
```

IMPLEMENT_UMOCK_C_ENUM_TYPE shall implement umock_c handlers for an enum type.
The variable arguments are a list making up the enum values.
If a value that is not part of the enum is used, it shall be treated as an int value.
Note: IMPLEMENT_UMOCK_C_ENUM_TYPE only generates the handlers, registering the handlers still has to be done by using the macro REGISTER_UMOCK_VALUE_TYPE.

Example:

```c
IMPLEMENT_UMOCK_C_ENUM_TYPE(my_enum, enum_value1, enum_value2)
```  

This provides the handlers (stringify, are_equal, etc.) for the below C enum:

```c
typedef enum my_enum_tag
{
    enum_value1,
    enum_value2
} my_enum
```

### Type names

Since umock_c needs to maintain a list of registered types, the following rules shall be applied:
Each type shall be normalized to a form where all extra spaces are removed.
Type names are case sensitive. 

####REGISTER_UMOCK_VALUE_TYPE

```c
REGISTER_UMOCK_VALUE_TYPE(value_type, stringify_func, are_equal_func, copy_func, free_func)
```

REGISTER_UMOCK_VALUE_TYPE shall register the type identified by value_type to be usable by umock_c for argument and return types and instruct umock_c which functions to use for getting the stringify, are_equal, copy and free.

Example:

```c
REGISTER_UMOCK_VALUE_TYPE(TEST_STRUCT*, umockvalue_stringify_TEST_STRUCT_ptr, umockvalue_are_equal_TEST_STRUCT_ptr, umockvalue_copy_TEST_STRUCT_ptr, umockvalue_free_TEST_STRUCT_ptr);
```

If only the value_type is specified in the macro invocation then the stringify, are_equal, copy and free function names shall be automatically derived from the type as: umockvalue_stringify_value_type, umockvalue_are_equal_value_type, umockvalue_copy_value_type, umockvalue_free_value_type.

Example:

```c
REGISTER_UMOCK_VALUE_TYPE(TEST_STRUCT);
```

####REGISTER_UMOCK_ALIAS_TYPE

```c
REGISTER_UMOCK_ALIAS_TYPE(value_type, is_value_type)
```

REGISTER_UMOCK_ALIAS_TYPE registers a new alias type for another type. That means that the handlers used for is_value_type will also be used for the new alias value_type.

###Extra optional C types

####umockvalue_charptr

char\* and const char\* shall be supported out of the box through a separate header, umockvalue_charptr.h.

In order to enable the usage of char\*, the function umockvalue_charptr_register_types can be used in the test suite init.

The signature shall be:

```c
int umockvalue_charptr_register_types(void);
```

umockvalue_charptr_register_types returns 0 on success and non-zero on failure.

####umockvalue_stdint

The types in stdint.h shall be supported out of the box by including umockvalue_stdint.h.

In order to enable the usage of stdint types, the function umockvalue_stdint_register_types shall be used in the test suite init.

```c
int umockvalue_stdint_register_types(void);
```

umockvalue_stdint_register_types returns 0 on success and non-zero on failure.

##Call modifiers

When an expected call is recorded a call modifier interface in the form of a structure containing function pointers shall be returned to the caller.

That allows constructs like:

```c
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42))
        .SetReturn(44)
        .IgnoreAllArguments();
```

Note that each modifier function shall return a full modifier structure that allows chaining further call modifiers.

The last modifier in a chain overrides previous modifiers if any collision occurs. Example: A ValidateAllArguments after a previous IgnoreAllArgument will still validate all arguments.

###IgnoreAllArguments(void)

The IgnoreAllArguments call modifier shall record that for that specific call all arguments will be ignored for that specific call.

IgnoreAllArguments shall only be available for mock functions that have arguments.

###ValidateAllArguments(void)

The ValidateAllArguments call modifier shall record that for that specific call all arguments will be validated.

ValidateAllArguments shall only be available for mock functions that have arguments.

###IgnoreArgument_{arg_name}(void)

The IgnoreArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be ignored for that specific call.

IgnoreArgument_{arg_name} shall only be available for mock functions that have arguments.

###ValidateArgument_{arg_name}(void)

The ValidateArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be validated for that specific call.

ValidateArgument_{arg_name} shall only be available for mock functions that have arguments.

###IgnoreArgument(size_t index)

The IgnoreArgument call modifier shall record that the indexth argument will be ignored for that specific call.

If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.

IgnoreArgument shall only be available for mock functions that have arguments.

###ValidateArgument(size_t index)

The ValidateArgument call modifier shall record that the indexth argument will be validated for that specific call.

If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.

ValidateArgument shall only be available for mock functions that have arguments.

###SetReturn(return_type result)

The SetReturn call modifier shall record that when an actual call is matched with the specific expected call, it shall return the result value to the code under test.

SetReturn shall only be available if the return type is not void.

###SetFailReturn(return_type result)

The SetFailReturn call modifier shall record a fail return value.
The fail return value can be recorded for more advanced features that would require failing or succeeding certain calls based on decisions made at runtime.

SetFailReturn shall only be available if the return type is not void.

###CopyOutArgumentBuffer(size_t index, const void* bytes, size_t length)

The CopyOutArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.

The memory shall be copied.
If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept.

The buffers for previous CopyOutArgumentBuffer calls shall be freed.

CopyOutArgumentBuffer shall only be applicable to pointer types.

If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.

If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.

The argument targetted by CopyOutArgumentBuffer shall also be marked as ignored.

If any memory allocation error occurs, umock_c shall raise an error with the code UMOCK_C_MALLOC_ERROR.

If any other error occurs, umock_c shall raise an error with the code UMOCK_C_ERROR.

CopyOutArgumentBuffer shall only be available for mock functions that have arguments.

###CopyOutArgumentBuffer_{arg_name}(const void* bytes, size_t length)

The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.

The memory shall be copied.
If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept.

CopyOutArgumentBuffer_{arg_name} shall only be applicable to pointer types.

If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.

The argument targetted by CopyOutArgumentBuffer_{arg_name} shall also be marked as ignored.

If any memory allocation error occurs, umock_c shall raise an error with the code UMOCK_C_MALLOC_ERROR.

If any other error occurs, umock_c shall raise an error with the code UMOCK_C_ERROR.

CopyOutArgumentBuffer_{arg_name} shall only be available for mock functions that have arguments.

###CopyOutArgument(arg_type value)

The CopyOutArgument call modifier shall copy an argument value to be injected as an out argument value when the code under test calls the mock function.

CopyOutArgument shall only be applicable to pointer types.

CopyOutArgument shall only be available for mock functions that have arguments.

###ValidateArgumentBuffer(size_t index, const void* bytes, size_t length)

The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.

If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.
ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.

The memory pointed by bytes shall be copied. If several calls to ValidateArgumentBuffer are made, only the last buffer shall be kept.

The buffers for previous ValidateArgumentBuffer calls shall be freed.

ValidateArgumentBuffer shall only be applicable to pointer types.

If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.

If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.

ValidateArgumentBuffer shall only be available for mock functions that have arguments.

###IgnoreAllCalls(void)

The IgnoreAllCalls call modifier shall record that all calls matching the expected call shall be ignored. If no matching call occurs no missing call shall be reported. If multiple matching actual calls occur no unexpected calls shall be reported.
The call matching shall be done taking into account arguments and call modifiers referring to arguments.

###CaptureReturn(return_type* captured_return_value)

The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call.
If CaptureReturn is called multiple times for the same call, an error shall be indicated with the code UMOCK_C_CAPTURE_RETURN_ALREADY_USED.
If captured_return_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT.
CaptureReturn shall only be available if the return type is not void.

Example:

```c
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
```

###ValidateArgumentValue_{arg_name}(arg_type* arg_value)

The ValidateArgumentValue_{arg_name} shall validate that the value of an argument matches the value pointed by arg_value.
If arg_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT.
The ValidateArgumentValue_{arg_name} modifier shall inhibit comparing with any value passed directly as an argument in the expected call. 
The ValidateArgumentValue_{arg_name} shall implicitly do a ValidateArgument for the arg_name argument, making sure the argument is not ignored.

Example:

Given a function with the prototype:

```c
void function_with_int_arg(int a);
```

```c
TEST_FUNCTION(validate_argument_sample)
{
    // arrange
    int arg_value = 0;

    STRICT_EXPECTED_CALL(function_with_int_arg(0))
        .ValidateArgumentValue_a(&arg_value);

    arg_value = 42;

    // act
    function_with_int_arg(42);

    // assert
    // ... calls should match ...
}
```

##Global mock modifiers

###REGISTER_GLOBAL_MOCK_HOOK

```c
REGISTER_GLOBAL_MOCK_HOOK(mock_function, mock_hook_function)
```

The REGISTER_GLOBAL_MOCK_HOOK shall register a mock hook to be called every time the mocked function is called by production code. The hook’s result shall be returned by the mock to the production code.

The signature for the hook shall be assumed to have exactly the same arguments and return as the mocked function.

If there are multiple invocations of REGISTER_GLOBAL_MOCK_HOOK, the last one shall take effect over the previous ones.

REGISTER_GLOBAL_MOCK_HOOK called with a NULL hook unregisters a previously registered hook.

All parameters passed to the mock shall be passed down to the mock hook.

###REGISTER_GLOBAL_MOCK_RETURN

```c
REGISTER_GLOBAL_MOCK_RETURN(mock_function, return_value)
```

The REGISTER_GLOBAL_MOCK_RETURN shall register a return value to always be returned by a mock function.

If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURN, the last one shall take effect over the previous ones.

If any error occurs during REGISTER_GLOBAL_MOCK_RETURN, umock_c shall raise an error with the code UMOCK_C_ERROR.

###REGISTER_GLOBAL_MOCK_FAIL_RETURN

```c
REGISTER_GLOBAL_MOCK_FAIL_RETURN(mock_function, fail_return_value)
```

The REGISTER_GLOBAL_MOCK_FAIL_RETURN shall register a fail return value to be returned by a mock function when marked as failed in the expected calls.

If there are multiple invocations of REGISTER_GLOBAL_FAIL_MOCK_RETURN, the last one shall take effect over the previous ones.

If any error occurs during REGISTER_GLOBAL_MOCK_FAIL_RETURN, umock_c shall raise an error with the code UMOCK_C_ERROR.

###REGISTER_GLOBAL_MOCK_RETURNS

```c
REGISTER_GLOBAL_MOCK_RETURNS(mock_function, return_value, fail_return_value)
```

The REGISTER_GLOBAL_MOCK_RETURNS shall register both a success and a fail return value associated with a mock function.

If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURNS, the last one shall take effect over the previous ones.

If any error occurs during REGISTER_GLOBAL_MOCK_RETURNS, umock_c shall raise an error with the code UMOCK_C_ERROR.

## negative tests addon

In order to automate negative tests writing, a separate API surface is provided: umock_c_negative_tests.

Example:

Given a function under test with the following code:
```c
int function_under_test(void)
{
    int result;
    
    if (function_1() != 0)
    {
        result = __LINE__;
    }
    else
    {
        if (function_2() != 0)
        {
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}
```

The function calls two functions that return int:

```c
    MOCKABLE_FUNCTION(, int, function_1);
    MOCKABLE_FUNCTION(, int, function_2);
```

In order to test that for each case where either function_1 or function_2 fails and returns a non-zero value, one could write the following test that loops through all cases as opposed to writing individual negative tests:

```c
    size_t i;
    STRICT_EXPECTED_CALL(function_1())
        .SetReturn(0).SetFailReturn(1);
    STRICT_EXPECTED_CALL(function_2())
        .SetReturn(0).SetFailReturn(1);
    umock_c_negative_tests_snapshot();

    for (i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        // arrange
        char temp_str[128];
        int result;
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        // act
        result = function_under_test();

        // assert
        sprintf(temp_str, "On failed call %zu", i + 1);
        ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, result, temp_str);
    }
```

Note that a return value and a fail return value must be specified for the negative tests. If they are not specified that will result in undefined behavior.  

###umock_c_negative_tests_init

```c
int umock_c_negative_tests_init(void)
```

umock_c_negative_tests_init shall initialize the negative tests umock_c module.
On success it shall return 0. If any error occurs, it shall return a non-zero value.
This call is typically made in the test function setup.

###umock_c_negative_tests_deinit

```c
void umock_c_negative_tests_deinit(void)
```

umock_c_negative_tests_deinit shall free all resources used by the negative tests module.

###umock_c_negative_tests_snapshot

```c
void umock_c_negative_tests_snapshot(void)
```

umock_c_negative_tests_snapshot shall take a snapshot of the current setup of expected calls (a.k.a happy path).
This is in order for these calls to be replayed as many times as needed, each time allowing different calls to be failed.
If umock_c_negative_tests_snapshot is called without the module being initialized, it shall do nothing.
All errors shall be reported by calling the umock_c on error function. 

###umock_c_negative_tests_reset

```c
void umock_c_negative_tests_reset(void)
```

umock_c_negative_tests_reset shall bring umock_c expected and actual calls to the state recorded when umock_c_negative_tests_snapshot was called.
This is done typically in preparation of running each negative test.
If umock_c_negative_tests_reset is called without the module being initialized, it shall do nothing.
All errors shall be reported by calling the umock_c on error function.

###umock_c_negative_tests_fail_call

```c
void umock_c_negative_tests_fail_call(size_t index)
```

umock_c_negative_tests_fail_call shall instruct the negative tests module to fail a specific call.
If umock_c_negative_tests_fail_call is called without the module being initialized, it shall do nothing.
All errors shall be reported by calling the umock_c on error function.

###umock_c_negative_tests_call_count

```c
size_t umock_c_negative_tests_call_count(void)
```

umock_c_negative_tests_call_count shall provide the number of expected calls, so that the test code can iterate through all negative cases.
If umock_c_negative_tests_fail_call is called without the module being initialized, it shall return 0.
All errors shall be reported by calling the umock_c on error function.

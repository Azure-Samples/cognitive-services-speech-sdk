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

MOCKABLE_FUNCTION(int, test_dependency_1_arg, int, a);
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
    extern int umock_c_reset_all_calls(void);
    extern const char* umock_c_get_actual_calls(void);
    extern const char* umock_c_get_expected_calls(void);
```

##Mock definitions API

###MOCKABLE_FUNCTION

```c
MOCKABLE_FUNCTION(modifiers, result, function, ...)
```

XX**SRS_UMOCK_C_LIB_01_001: [**MOCKABLE_FUNCTION shall be used to wrap function definition allowing the user to declare a function that can be mocked.**]**

XX**SRS_UMOCK_C_LIB_01_002: [**The macro shall generate a function signature in case ENABLE_MOCKS is not defined.**]**

Example:

```c
MOCKABLE_FUNCTION(FAR, int, test_function, int, arg1)
```

should generate for production code:

```c
int FAR test_function(int arg1);
```

###MOCK_FUNCTION_WITH_CODE

XX**SRS_UMOCK_C_LIB_01_150: [** MOCK_FUNCTION_WITH_CODE shall define a mock function and allow the user to embed code between this define and a MOCK_FUNCTION_END call. **]**

```c
MOCK_FUNCTION_WITH_CODE(, void, test_mock_function_with_code_1_arg, int, a);
    int some_value = 42;
    /* more code here */
MOCK_FUNCTION_END()
```

###ENABLE_MOCKS

XX**SRS_UMOCK_C_LIB_01_004: [**If ENABLE_MOCKS is defined, MOCKABLE_FUNCTION shall generate the declaration of the function and code for the mocked function, thus allowing setting up of expectations in test functions.**]**
XX**SRS_UMOCK_C_LIB_01_005: [**If ENABLE_MOCKS is not defined, MOCKABLE_FUNCTION shall only generate a declaration for the function.**]**

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

XX**SRS_UMOCK_C_LIB_01_006: [**umock_c_init shall initialize umock_c.**]**

XX**SRS_UMOCK_C_LIB_01_007: [**umock_c_init called if already initialized shall fail and return a non-zero value.**]**
 
XX**SRS_UMOCK_C_LIB_01_008: [**umock_c_init shall initialize the umock supported types (C native types).**]**

XX**SRS_UMOCK_C_LIB_01_009: [**on_umock_c_error can be NULL.**]**

XX**SRS_UMOCK_C_LIB_01_010: [**If on_umock_c_error is non-NULL it shall be saved for later use (to be invoked whenever an umock_c error needs to be signaled to the user).**]**

###umock_c_deinit

```c
void umock_c_deinit(void);
```

XX**SRS_UMOCK_C_LIB_01_011: [**umock_c_deinit shall free all umock_c used resources.**]**
XX**SRS_UMOCK_C_LIB_01_012: [**If umock_c was not initialized, umock_c_deinit shall do nothing.**]**

##Expected calls recording API

###STRICT_EXPECTED_CALL

```c
STRICT_EXPECTED_CALL(call)
```

XX**SRS_UMOCK_C_LIB_01_013: [**STRICT_EXPECTED_CALL shall record that a certain call is expected.**]**
XX**SRS_UMOCK_C_LIB_01_014: [**For each argument the argument value shall be stored for later comparison with actual calls.**]**

XX**SRS_UMOCK_C_LIB_01_015: [**The call argument shall be the complete function invocation.**]**

Examples:

```c
STRICT_EXPECTED_CALL(test_dependency_1_arg(42));
STRICT_EXPECTED_CALL(test_dependency_string("test"));
```

###EXPECTED_CALL

```c
EXPECTED_CALL(call)
```

XX**SRS_UMOCK_C_LIB_01_016: [**EXPECTED_CALL shall record that a certain call is expected.**]**
XX**SRS_UMOCK_C_LIB_01_017: [**No arguments shall be saved by default, unless other modifiers state it.**]**

XX**SRS_UMOCK_C_LIB_01_018: [**The call argument shall be the complete function invocation.**]**

Example:

```c
EXPECTED_CALL(test_dependency_1_arg(42));
```

##Call comparison API

###umock_c_reset_all_calls

```c
void umock_c_reset_all_calls(void);
```

**SRS_UMOCK_C_LIB_01_019: [**umock_c_reset_all_calls shall reset all calls (actual and expected).**]**
**SRS_UMOCK_C_LIB_01_021: [**In case of any error, umock_c_reset_all_calls shall indicate the error through a call to the on_error callback.**]**

###umock_c_get_expected_calls

```c
const char* umock_c_get_expected_calls(void);
```

**SRS_UMOCK_C_LIB_01_022: [**umock_c_get_expected_calls shall return all the calls that were expected, but were not fulfilled.**]**

**SRS_UMOCK_C_LIB_01_023: [**For each call, the format shall be "functionName(argument 1 value, ...)".**]**
**SRS_UMOCK_C_LIB_01_024: [**Each call shall be enclosed in "[]".**]**

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

**SRS_UMOCK_C_LIB_01_025: [**umock_c_get_actual_calls shall return all the actual calls that were not matched to expected calls.**]**

**SRS_UMOCK_C_LIB_01_026: [**For each call, the format shall be "functionName(argument 1 value, ...)".**]**
**SRS_UMOCK_C_LIB_01_027: [**Each call shall be enclosed in "[]".**]** 

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

XX**SRS_UMOCK_C_LIB_01_115: [** umock_c shall compare calls in order. **]** That means that "[A()][B()]" is different than "[B()][A()]". 

XX**SRS_UMOCK_C_LIB_01_136: [** When multiple return values are set for a mock function by using different means (such as SetReturn), the following order shall be in effect: **]**

XX**SRS_UMOCK_C_LIB_01_137: [** - If a return value has been specified for an expected call then that value shall be returned. **]**
XX**SRS_UMOCK_C_LIB_01_138: [** - If a global mock hook has been specified then it shall be called and its result returned. **]**
XX**SRS_UMOCK_C_LIB_01_139: [** - If a global return value has been specified then it shall be returned. **]**
XX**SRS_UMOCK_C_LIB_01_140: [** - Otherwise the value of a static variable of the same type as the return type shall be returned. **]**

XX**SRS_UMOCK_C_LIB_01_148: [** If call comparison fails an error shall be indicated by calling the error callback with UMOCK_C_COMPARE_CALL_ERROR. **]**

##Supported types

###Out of the box

**SRS_UMOCK_C_LIB_01_144: [** Out of the box umock_c shall support the following types through the header umocktypes_c.h: **]**
-	**SRS_UMOCK_C_LIB_01_028: [**char**]**
-	**SRS_UMOCK_C_LIB_01_029: [**unsigned char**]**
-	**SRS_UMOCK_C_LIB_01_030: [**short**]**
-	**SRS_UMOCK_C_LIB_01_031: [**unsigned short**]**
-	**SRS_UMOCK_C_LIB_01_032: [**int**]**
-	**SRS_UMOCK_C_LIB_01_033: [**unsigned int**]**
-	**SRS_UMOCK_C_LIB_01_034: [**long**]**
-	**SRS_UMOCK_C_LIB_01_035: [**unsigned long**]**
-	**SRS_UMOCK_C_LIB_01_036: [**long long**]**
-	**SRS_UMOCK_C_LIB_01_037: [**unsigned long long**]**
-	**SRS_UMOCK_C_LIB_01_038: [**float**]**
-	**SRS_UMOCK_C_LIB_01_039: [**double**]**
-	**SRS_UMOCK_C_LIB_01_040: [**long double**]**
-	**SRS_UMOCK_C_LIB_01_041: [**size_t**]**
-   **SRS_UMOCK_C_LIB_01_151: [** void\* **]**
-   **SRS_UMOCK_C_LIB_01_152: [** const void\* **]**

###Pointer types

**SRS_UMOCK_C_LIB_01_153: [** If no custom handler has beed registered for a pointer type, it shall be trated as void*. **]**

###Custom types

**SRS_UMOCK_C_LIB_01_045: [**Custom types, like structures shall be supported by allowing the user to define a set of functions that can be used by umock_c to operate with these types.**]**

Five functions shall be provided to umock_c:
-	**SRS_UMOCK_C_LIB_01_046: [**A stringify function.**]**

**SRS_UMOCK_C_LIB_01_047: [**This function shall return the string representation of a value of the given type.**]**

-	**SRS_UMOCK_C_LIB_01_048: [**An are_equal function.**]**

**SRS_UMOCK_C_LIB_01_049: [**This function shall compare 2 values of the given type and return an int indicating whether they are equal (1 means equal, 0 means different).**]**

-	**SRS_UMOCK_C_LIB_01_050: [**A copy function.**]**

**SRS_UMOCK_C_LIB_01_051: [**This function shall make a copy of a value for the given type.**]**

-	**SRS_UMOCK_C_LIB_01_052: [**A free function.**]**

**SRS_UMOCK_C_LIB_01_053: [**This function shall free a copied value.**]**

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

If only one of left and right is NULL, umockvalue_are_equal_type shall return 0.

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

The umockvalue_free_type function shall free a value previously copied using umockvalue_copy_type.
If value is NULL, no free shall be performed.

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

XX**SRS_UMOCK_C_LIB_01_179: [** IMPLEMENT_UMOCK_C_ENUM_TYPE shall implement umock_c handlers for an enum type. **]**
XX**SRS_UMOCK_C_LIB_01_180: [** The variable arguments are a list making up the enum values. **]**
XX**SRS_UMOCK_C_LIB_01_181: [** If a value that is not part of the enum is used, it shall be treated as an int value. **]**
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

XX**SRS_UMOCK_C_LIB_01_145: [** Since umock_c needs to maintain a list of registered types, the following rules shall be applied: **]**

XX**SRS_UMOCK_C_LIB_01_146: [** Each type shall be normalized to a form where all extra spaces are removed. **]**

XX**SRS_UMOCK_C_LIB_01_147: [** Type names are case sensitive. **]** 

####REGISTER_UMOCK_VALUE_TYPE

```c
REGISTER_UMOCK_VALUE_TYPE(value_type, stringify_func, are_equal_func, copy_func, free_func)
```

**SRS_UMOCK_C_LIB_01_065: [**REGISTER_UMOCK_VALUE_TYPE shall register the type identified by value_type to be usable by umock_c for argument and return types and instruct umock_c which functions to use for getting the stringify, are_equal, copy and free.**]**

Example:

```c
REGISTER_UMOCK_VALUE_TYPE(TEST_STRUCT*, umockvalue_stringify_TEST_STRUCT_ptr, umockvalue_are_equal_TEST_STRUCT_ptr, umockvalue_copy_TEST_STRUCT_ptr, umockvalue_free_TEST_STRUCT_ptr);
```

**SRS_UMOCK_C_LIB_01_066: [**If only the value_type is specified in the macro invocation then the stringify, are_equal, copy and free function names shall be automatically derived from the type as: umockvalue_stringify_value_type, umockvalue_are_equal_value_type, umockvalue_copy_value_type, umockvalue_free_value_type.**]**

Example:

```c
REGISTER_UMOCK_VALUE_TYPE(TEST_STRUCT);
```

####REGISTER_UMOCK_ALIAS_TYPE

```c
REGISTER_UMOCK_ALIAS_TYPE(value_type, is_value_type)
```

**SRS_UMOCK_C_LIB_01_149: [** REGISTER_UMOCK_ALIAS_TYPE registers a new alias type for another type. **]** That means that the handlers used for is_value_type will also be used for the new alias value_type.

###Extra optional C types

####umockvalue_charptr

XX**SRS_UMOCK_C_LIB_01_067: [**char\* and const char\* shall be supported out of the box through a separate header, umockvalue_charptr.h.**]**

In order to enable the usage of char\*, the function umockvalue_charptr_register_types can be used in the test suite init.

XX**SRS_UMOCK_C_LIB_01_069: [**The signature shall be:

```c
int umockvalue_charptr_register_types(void);
```
**]**

umockvalue_charptr_register_types returns 0 on success and non-zero on failure.

####umockvalue_stdint

**SRS_UMOCK_C_LIB_01_071: [**The types in stdint.h shall be supported out of the box by including umockvalue_stdint.h.**]**

**SRS_UMOCK_C_LIB_01_072: [**In order to enable the usage of stdint types, the function umockvalue_stdint_register_types shall be used in the test suite init.**]**

```c
int umockvalue_stdint_register_types(void);
```

umockvalue_stdint_register_types returns 0 on success and non-zero on failure.

##Call modifiers

XX**SRS_UMOCK_C_LIB_01_074: [**When an expected call is recorded a call modifier interface in the form of a structure containing function pointers shall be returned to the caller.**]**

That allows constructs like:

```c
    STRICT_EXPECTED_CALL(test_dependency_1_arg(42))
        .SetReturn(44)
        .IgnoreAllArguments();
```

Note that each modifier function shall return a full modifier structure that allows chaining further call modifiers.

XX**SRS_UMOCK_C_LIB_01_075: [**The last modifier in a chain overrides previous modifiers if any collision occurs.**]**
Example: A ValidateAllArguments after a previous IgnoreAllArgument will still validate all arguments.

###IgnoreAllArguments(void)

XX**SRS_UMOCK_C_LIB_01_076: [**The IgnoreAllArguments call modifier shall record that for that specific call all arguments will be ignored for that specific call.**]**

X**SRS_UMOCK_C_LIB_01_127: [** IgnoreAllArguments shall only be available for mock functions that have arguments. **]**

###ValidateAllArguments(void)

XX**SRS_UMOCK_C_LIB_01_077: [**The ValidateAllArguments call modifier shall record that for that specific call all arguments will be validated.**]**

X**SRS_UMOCK_C_LIB_01_126: [** ValidateAllArguments shall only be available for mock functions that have arguments. **]**

###IgnoreArgument_{arg_name}(void)

XX**SRS_UMOCK_C_LIB_01_078: [**The IgnoreArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be ignored for that specific call.**]**

X**SRS_UMOCK_C_LIB_01_125: [** IgnoreArgument_{arg_name} shall only be available for mock functions that have arguments. **]**

###ValidateArgument_{arg_name}(void)

XX**SRS_UMOCK_C_LIB_01_079: [**The ValidateArgument_{arg_name} call modifier shall record that the argument identified by arg_name will be validated for that specific call.**]**

X**SRS_UMOCK_C_LIB_01_124: [** ValidateArgument_{arg_name} shall only be available for mock functions that have arguments. **]**

###IgnoreArgument(size_t index)

XX**SRS_UMOCK_C_LIB_01_080: [**The IgnoreArgument call modifier shall record that the indexth argument will be ignored for that specific call.**]**

XX**SRS_UMOCK_C_LIB_01_081: [**If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.**]**

X**SRS_UMOCK_C_LIB_01_123: [** IgnoreArgument shall only be available for mock functions that have arguments. **]**

###ValidateArgument(size_t index)

XX**SRS_UMOCK_C_LIB_01_082: [**The ValidateArgument call modifier shall record that the indexth argument will be validated for that specific call.**]**

XX**SRS_UMOCK_C_LIB_01_083: [**If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.**]**

X**SRS_UMOCK_C_LIB_01_122: [** ValidateArgument shall only be available for mock functions that have arguments. **]**

###SetReturn(return_type result)

XX**SRS_UMOCK_C_LIB_01_084: [**The SetReturn call modifier shall record that when an actual call is matched with the specific expected call, it shall return the result value to the code under test.**]**

X**SRS_UMOCK_C_LIB_01_121: [** SetReturn shall only be available if the return type is not void. **]**

###SetFailReturn(return_type result)

**SRS_UMOCK_C_LIB_01_085: [**The SetFailReturn call modifier shall record a fail return value.**]**
The fail return value can be recorded for more advanced features that would require failing or succeeding certain calls based on decisions made at runtime.

X**SRS_UMOCK_C_LIB_01_120: [** SetFailReturn shall only be available if the return type is not void. **]**

###CopyOutArgumentBuffer(size_t index, const void* bytes, size_t length)

XX**SRS_UMOCK_C_LIB_01_087: [**The CopyOutArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function.**]**

XX**SRS_UMOCK_C_LIB_01_088: [**The memory shall be copied.**]** 
XX**SRS_UMOCK_C_LIB_01_133: [** If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept. **]**

XX**SRS_UMOCK_C_LIB_01_089: [**The buffers for previous CopyOutArgumentBuffer calls shall be freed.**]**

**SRS_UMOCK_C_LIB_01_090: [**CopyOutArgumentBuffer shall only be applicable to pointer types.**]**

XX**SRS_UMOCK_C_LIB_01_091: [**If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.**]**

XX**SRS_UMOCK_C_LIB_01_092: [**If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.**]**

XX**SRS_UMOCK_C_LIB_01_116: [** The argument targetted by CopyOutArgumentBuffer shall also be marked as ignored. **]**

**SRS_UMOCK_C_LIB_01_117: [** If any memory allocation error occurs, umock_c shall raise an error with the code UMOCK_C_MALLOC_ERROR. **]**

**SRS_UMOCK_C_LIB_01_118: [** If any other error occurs, umock_c shall raise an error with the code UMOCK_C_ERROR. **]**

X**SRS_UMOCK_C_LIB_01_119: [** CopyOutArgumentBuffer shall only be available for mock functions that have arguments. **]**

###CopyOutArgumentBuffer_{arg_name}(const void* bytes, size_t length)

XX**SRS_UMOCK_C_LIB_01_154: [** The CopyOutArgumentBuffer_{arg_name} call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later injected as an out argument when the code under test calls the mock function. **]**

XX**SRS_UMOCK_C_LIB_01_155: [** The memory shall be copied. **]**
XX**SRS_UMOCK_C_LIB_01_156: [** If several calls to CopyOutArgumentBuffer are made, only the last buffer shall be kept. **]**

XX**SRS_UMOCK_C_LIB_01_163: [** The buffers for previous CopyOutArgumentBuffer calls shall be freed. **]**

**SRS_UMOCK_C_LIB_01_157: [** CopyOutArgumentBuffer_{arg_name} shall only be applicable to pointer types. **]**

XX**SRS_UMOCK_C_LIB_01_158: [** If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER. **]**

XX**SRS_UMOCK_C_LIB_01_159: [** The argument targetted by CopyOutArgumentBuffer_{arg_name} shall also be marked as ignored. **]**

**SRS_UMOCK_C_LIB_01_160: [** If any memory allocation error occurs, umock_c shall raise an error with the code UMOCK_C_MALLOC_ERROR. **]**

**SRS_UMOCK_C_LIB_01_161: [** If any other error occurs, umock_c shall raise an error with the code UMOCK_C_ERROR. **]**

**SRS_UMOCK_C_LIB_01_162: [** CopyOutArgumentBuffer_{arg_name} shall only be available for mock functions that have arguments. **]**

###CopyOutArgument(arg_type value)

**SRS_UMOCK_C_LIB_01_093: [**The CopyOutArgument call modifier shall copy an argument value to be injected as an out argument value when the code under test calls the mock function.**]**

**SRS_UMOCK_C_LIB_01_094: [**CopyOutArgument shall only be applicable to pointer types.**]**

X**SRS_UMOCK_C_LIB_01_128: [** CopyOutArgument shall only be available for mock functions that have arguments. **]**

###ValidateArgumentBuffer(size_t index, const void* bytes, size_t length)

XX**SRS_UMOCK_C_LIB_01_095: [**The ValidateArgumentBuffer call modifier shall copy the memory pointed to by bytes and being length bytes so that it is later compared against a pointer type argument when the code under test calls the mock function.**]**

XX**SRS_UMOCK_C_LIB_01_096: [**If the content of the code under test buffer and the buffer supplied to ValidateArgumentBuffer does not match then this should be treated as a mismatch in argument comparison for that argument.**]**
XX**SRS_UMOCK_C_LIB_01_097: [**ValidateArgumentBuffer shall implicitly perform an IgnoreArgument on the indexth argument.**]**

XX**SRS_UMOCK_C_LIB_01_131: [** The memory pointed by bytes shall be copied. **]**
XX**SRS_UMOCK_C_LIB_01_132: [** If several calls to ValidateArgumentBuffer are made, only the last buffer shall be kept. **]**

XX**SRS_UMOCK_C_LIB_01_130: [** The buffers for previous ValidateArgumentBuffer calls shall be freed. **]**

**SRS_UMOCK_C_LIB_01_098: [**ValidateArgumentBuffer shall only be applicable to pointer types.**]**

XX**SRS_UMOCK_C_LIB_01_099: [**If the index is out of range umock_c shall raise an error with the code UMOCK_C_ARG_INDEX_OUT_OF_RANGE.**]**

XX**SRS_UMOCK_C_LIB_01_100: [**If bytes is NULL or length is 0, umock_c shall raise an error with the code UMOCK_C_INVALID_ARGUMENT_BUFFER.**]**

X**SRS_UMOCK_C_LIB_01_129: [** ValidateArgumentBuffer shall only be available for mock functions that have arguments. **]**

###IgnoreAllCalls(void)

**SRS_UMOCK_C_LIB_01_101: [**The IgnoreAllCalls call modifier shall record that all calls matching the expected call shall be ignored. If no matching call occurs no missing call shall be reported.**]**
**SRS_UMOCK_C_LIB_01_102: [**If multiple matching actual calls occur no unexpected calls shall be reported.**]**
**SRS_UMOCK_C_LIB_01_103: [**The call matching shall be done taking into account arguments and call modifiers referring to arguments.**]**

###CaptureReturn(return_type* captured_return_value)

XX**SRS_UMOCK_C_LIB_01_179: [** The CaptureReturn call modifier shall copy the return value that is being returned to the code under test when an actual call is matched with the expected call. **]**
XX**SRS_UMOCK_C_LIB_01_180: [** If CaptureReturn is called multiple times for the same call, an error shall be indicated with the code UMOCK_C_CAPTURE_RETURN_ALREADY_USED. **]**
XX**SRS_UMOCK_C_LIB_01_182: [** If captured_return_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT. **]**
**SRS_UMOCK_C_LIB_01_181: [** CaptureReturn shall only be available if the return type is not void. **]**

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

XX**SRS_UMOCK_C_LIB_01_183: [** The ValidateArgumentValue_{arg_name} shall validate that the value of an argument matches the value pointed by arg_value. **]**
XX**SRS_UMOCK_C_LIB_01_184: [** If arg_value is NULL, umock_c shall raise an error with the code UMOCK_C_NULL_ARGUMENT. **]**
XX**SRS_UMOCK_C_LIB_01_185: [** The ValidateArgumentValue_{arg_name} modifier shall inhibit comparing with any value passed directly as an argument in the expected call. **]**
XX**SRS_UMOCK_C_LIB_01_186: [** The ValidateArgumentValue_{arg_name} shall implicitly do a ValidateArgument for the arg_name argument, making sure the argument is not ignored. **]**

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

XX**SRS_UMOCK_C_LIB_01_104: [**The REGISTER_GLOBAL_MOCK_HOOK shall register a mock hook to be called every time the mocked function is called by production code.**]**
XX**SRS_UMOCK_C_LIB_01_105: [**The hook’s result shall be returned by the mock to the production code.**]**

XX**SRS_UMOCK_C_LIB_01_106: [**The signature for the hook shall be assumed to have exactly the same arguments and return as the mocked function.**]**

XX**SRS_UMOCK_C_LIB_01_107: [**If there are multiple invocations of REGISTER_GLOBAL_MOCK_HOOK, the last one shall take effect over the previous ones.**]**

XX**SRS_UMOCK_C_LIB_01_134: [** REGISTER_GLOBAL_MOCK_HOOK called with a NULL hook unregisters a previously registered hook. **]**

XX**SRS_UMOCK_C_LIB_01_135: [** All parameters passed to the mock shall be passed down to the mock hook. **]** 

###REGISTER_GLOBAL_MOCK_RETURN

```c
REGISTER_GLOBAL_MOCK_RETURN(mock_function, return_value)
```

XX**SRS_UMOCK_C_LIB_01_108: [**The REGISTER_GLOBAL_MOCK_RETURN shall register a return value to always be returned by a mock function.**]**

XX**SRS_UMOCK_C_LIB_01_109: [**If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURN, the last one shall take effect over the previous ones.**]**

XX**SRS_UMOCK_C_LIB_01_141: [** If any error occurs during REGISTER_GLOBAL_MOCK_RETURN, umock_c shall raise an error with the code UMOCK_C_ERROR. **]**

###REGISTER_GLOBAL_MOCK_FAIL_RETURN

```c
REGISTER_GLOBAL_MOCK_FAIL_RETURN(mock_function, fail_return_value)
```

XX**SRS_UMOCK_C_LIB_01_111: [**The REGISTER_GLOBAL_MOCK_FAIL_RETURN shall register a fail return value to be returned by a mock function when marked as failed in the expected calls.**]**

XX**SRS_UMOCK_C_LIB_01_112: [**If there are multiple invocations of REGISTER_GLOBAL_FAIL_MOCK_RETURN, the last one shall take effect over the previous ones.**]**

XX**SRS_UMOCK_C_LIB_01_142: [** If any error occurs during REGISTER_GLOBAL_MOCK_FAIL_RETURN, umock_c shall raise an error with the code UMOCK_C_ERROR. **]**

###REGISTER_GLOBAL_MOCK_RETURNS

```c
REGISTER_GLOBAL_MOCK_RETURNS(mock_function, return_value, fail_return_value)
```

XX**SRS_UMOCK_C_LIB_01_113: [**The REGISTER_GLOBAL_MOCK_RETURNS shall register both a success and a fail return value associated with a mock function.**]**

XX**SRS_UMOCK_C_LIB_01_114: [**If there are multiple invocations of REGISTER_GLOBAL_MOCK_RETURNS, the last one shall take effect over the previous ones.**]**

XX**SRS_UMOCK_C_LIB_01_143: [** If any error occurs during REGISTER_GLOBAL_MOCK_RETURNS, umock_c shall raise an error with the code UMOCK_C_ERROR. **]**

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

**SRS_UMOCK_C_LIB_01_164: [** umock_c_negative_tests_init shall initialize the negative tests umock_c module. **]**
**SRS_UMOCK_C_LIB_01_165: [** On success it shall return 0. If any error occurs, it shall return a non-zero value. **]**
This call is typically made in the test function setup.

###umock_c_negative_tests_deinit

```c
void umock_c_negative_tests_deinit(void)
```

**SRS_UMOCK_C_LIB_01_166: [** umock_c_negative_tests_deinit shall free all resources used by the negative tests module. **]**

###umock_c_negative_tests_snapshot

```c
void umock_c_negative_tests_snapshot(void)
```

XX**SRS_UMOCK_C_LIB_01_167: [** umock_c_negative_tests_snapshot shall take a snapshot of the current setup of expected calls (a.k.a happy path). **]**
This is in order for these calls to be replayed as many times as needed, each time allowing different calls to be failed.
X**SRS_UMOCK_C_LIB_01_168: [** If umock_c_negative_tests_snapshot is called without the module being initialized, it shall do nothing. **]**
X**SRS_UMOCK_C_LIB_01_169: [** All errors shall be reported by calling the umock_c on error function. **]** 

###umock_c_negative_tests_reset

```c
void umock_c_negative_tests_reset(void)
```

XX**SRS_UMOCK_C_LIB_01_170: [** umock_c_negative_tests_reset shall bring umock_c expected and actual calls to the state recorded when umock_c_negative_tests_snapshot was called. **]**
This is done typically in preparation of running each negative test.
X**SRS_UMOCK_C_LIB_01_171: [** If umock_c_negative_tests_reset is called without the module being initialized, it shall do nothing. **]**
X**SRS_UMOCK_C_LIB_01_172: [** All errors shall be reported by calling the umock_c on error function. **]**

###umock_c_negative_tests_fail_call

```c
void umock_c_negative_tests_fail_call(size_t index)
```

XX**SRS_UMOCK_C_LIB_01_173: [** umock_c_negative_tests_fail_call shall instruct the negative tests module to fail a specific call. **]**
X**SRS_UMOCK_C_LIB_01_174: [** If umock_c_negative_tests_fail_call is called without the module being initialized, it shall do nothing. **]**
X**SRS_UMOCK_C_LIB_01_175: [** All errors shall be reported by calling the umock_c on error function. **]**

###umock_c_negative_tests_call_count

```c
size_t umock_c_negative_tests_call_count(void)
```

XX**SRS_UMOCK_C_LIB_01_176: [** umock_c_negative_tests_call_count shall provide the number of expected calls, so that the test code can iterate through all negative cases. **]**
X**SRS_UMOCK_C_LIB_01_177: [** If umock_c_negative_tests_fail_call is called without the module being initialized, it shall return 0. **]**
X**SRS_UMOCK_C_LIB_01_178: [** All errors shall be reported by calling the umock_c on error function. **]**

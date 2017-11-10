// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCK_C_H
#define UMOCK_C_H

#ifdef __cplusplus
extern "C" {
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#include "azure_c_shared_utility/macro_utils.h"
#include "umockcallrecorder.h"

#define UMOCK_C_ERROR_CODE_VALUES \
        UMOCK_C_ARG_INDEX_OUT_OF_RANGE, \
        UMOCK_C_MALLOC_ERROR, \
        UMOCK_C_INVALID_ARGUMENT_BUFFER, \
        UMOCK_C_COMPARE_CALL_ERROR, \
        UMOCK_C_RESET_CALLS_ERROR, \
        UMOCK_C_CAPTURE_RETURN_ALREADY_USED, \
        UMOCK_C_NULL_ARGUMENT, \
        UMOCK_C_ERROR

DEFINE_ENUM(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

    /* This is the type for the error callback by which umock_c shall indicate errors to the user */
    typedef void(*ON_UMOCK_C_ERROR)(UMOCK_C_ERROR_CODE error_code);

#define IGNORED_PTR_ARG (NULL)
#define IGNORED_NUM_ARG (0)

#define REGISTER_GLOBAL_MOCK_HOOK(mock_function, mock_hook_function) \
    C2(set_global_mock_hook_,mock_function)(mock_hook_function);

#define REGISTER_GLOBAL_MOCK_RETURN(mock_function, return_value) \
    C2(set_global_mock_return_,mock_function)(return_value);

#define REGISTER_GLOBAL_MOCK_FAIL_RETURN(mock_function, fail_return_value) \
    C2(set_global_mock_fail_return_,mock_function)(fail_return_value);

#define REGISTER_GLOBAL_MOCK_RETURNS(mock_function, return_value, fail_return_value) \
    C2(set_global_mock_returns_,mock_function)(return_value, fail_return_value);

/* Codes_SRS_UMOCK_C_LIB_01_013: [STRICT_EXPECTED_CALL shall record that a certain call is expected.] */
#define STRICT_EXPECTED_CALL(call) \
	C2(umock_c_strict_expected_,call)

#define EXPECTED_CALL(call) \
	C2(umock_c_expected_,call)

#define DECLARE_UMOCK_POINTER_TYPE_FOR_TYPE(value_type, alias) \
    char* C3(stringify_func_,alias,ptr)(const value_type** value) \
    { \
        char temp_buffer[32]; \
        char* result; \
        size_t length = sprintf(temp_buffer, "%p", (void*)*value); \
        if (length < 0) \
        { \
            result = NULL; \
        } \
        else \
        { \
            result = (char*)malloc(length + 1); \
            if (result != NULL) \
            { \
                (void)memcpy(result, temp_buffer, length + 1); \
            } \
        } \
        return result; \
    } \
    int C3(are_equal_func_,alias,ptr)(const value_type** left, const value_type** right) \
    { \
        return *left == *right; \
    } \
    int C3(copy_func_,alias,ptr)(value_type** destination, const value_type** source) \
    { \
        *destination = (value_type*)*source; \
        return 0; \
    } \
    void C3(free_func_,alias,ptr)(value_type** value) \
    { \
    } \

#define REGISTER_UMOCK_VALUE_TYPE(value_type, stringify_func, are_equal_func, copy_func, free_func) \
{ \
    char* stringify_func(const value_type* value); \
    int are_equal_func(const value_type* left, const value_type* right); \
    int copy_func(value_type* destination, const value_type* source); \
    void free_func(value_type* value); \
    umocktypes_register_type(TOSTRING(value_type), (UMOCKTYPE_STRINGIFY_FUNC)stringify_func, (UMOCKTYPE_ARE_EQUAL_FUNC)are_equal_func, (UMOCKTYPE_COPY_FUNC)copy_func, (UMOCKTYPE_FREE_FUNC)free_func); \
}

#define REGISTER_UMOCK_ALIAS_TYPE(value_type, is_value_type) \
    umocktypes_register_alias_type(TOSTRING(value_type), TOSTRING(is_value_type));

extern int umock_c_init(ON_UMOCK_C_ERROR on_umock_c_error);
extern void umock_c_deinit(void);
extern void umock_c_reset_all_calls(void);
extern const char* umock_c_get_actual_calls(void);
extern const char* umock_c_get_expected_calls(void);
extern UMOCKCALLRECORDER_HANDLE umock_c_get_call_recorder(void);
extern int umock_c_set_call_recorder(UMOCKCALLRECORDER_HANDLE umockc_call_recorder);

#include "umock_c_internal.h"
#include "umock_c_prod.h"

#ifdef __cplusplus
}
#endif

#endif /* UMOCK_C_H */

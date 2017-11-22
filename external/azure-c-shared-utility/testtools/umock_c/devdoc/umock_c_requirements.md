#umock_c requirements
 
#Overview

umock_c is a module that exposes the user facing API for umock_c.
It exposes a set of macros and APIs that allow:
- initializing/deinitializing the library
- resetting the calls
- getting the expected calls string
- getting the actual calls string

#Exposed API

```c
    typedef enum UMOCK_C_ERROR_CODE_TAG
    {
        UMOCK_C_ARG_INDEX_OUT_OF_RANGE,
        UMOCK_C_MALLOC_ERROR,
        UMOCK_C_INVALID_ARGUMENT_BUFFER,
        UMOCK_C_COMPARE_CALL_ERROR,
        UMOCK_C_RESET_CALLS_ERROR,
        UMOCK_C_ERROR
    } UMOCK_C_ERROR_CODE;

    typedef void(*ON_UMOCK_C_ERROR)(UMOCK_C_ERROR_CODE error_code);

#define IGNORED_PTR_ARG (NULL)
#define IGNORED_NUM_ARG (0)

#define MOCKABLE_FUNCTION(result, function, ...) \
    ...

#define REGISTER_GLOBAL_MOCK_HOOK(mock_function, mock_hook_function) \
    ...

#define REGISTER_GLOBAL_MOCK_RETURN(mock_function, return_value) \
    ...

#define REGISTER_GLOBAL_MOCK_FAIL_RETURN(mock_function, fail_return_value) \
    ...

#define REGISTER_GLOBAL_MOCK_RETURNS(mock_function, return_value, fail_return_value) \
    ...

#define STRICT_EXPECTED_CALL(call) \
	...

#define EXPECTED_CALL(call) \
	...

#define DECLARE_UMOCK_POINTER_TYPE_FOR_TYPE(value_type, alias) \
    ...

#define REGISTER_UMOCK_VALUE_TYPE(value_type, stringify_func, are_equal_func, copy_func, free_func) \
    ...

extern int umock_c_init(ON_UMOCK_C_ERROR on_umock_c_error);
extern void umock_c_deinit(void);
extern void umock_c_reset_all_calls(void);
extern const char* umock_c_get_actual_calls(void);
extern const char* umock_c_get_expected_calls(void);
extern UMOCKCALL_HANDLE umock_c_get_last_expected_call(void);
extern int umock_c_add_expected_call(UMOCKCALL_HANDLE mock_call);
extern int umock_c_add_actual_call(UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call);
extern UMOCKCALLRECORDER_HANDLE umock_c_get_call_recorder(void);
extern int umock_c_set_call_recorder(UMOCKCALLRECORDER_HANDLE call_recorder);
```

##umock_c_init

```c
extern int umock_c_init(ON_UMOCK_C_ERROR on_umock_c_error);
```

**SRS_UMOCK_C_01_001: [**umock_c_init shall initialize the umock library.**]**
**SRS_UMOCK_C_01_023: [** umock_c_init shall initialize the umock types by calling umocktypes_init. **]**
**SRS_UMOCK_C_01_002: [** umock_c_init shall register the C naive types by calling umocktypes_c_register_types. **]**
**SRS_UMOCK_C_01_003: [** umock_c_init shall create a call recorder by calling umockcallrecorder_create. **]**
**SRS_UMOCK_C_01_004: [** On success, umock_c_init shall return 0. **]**
**SRS_UMOCK_C_01_005: [** If any of the calls fails, umock_c_init shall fail and return a non-zero value. **]**
**SRS_UMOCK_C_01_006: [** The on_umock_c_error callback shall be stored to be used for later error callbacks. **]**
**SRS_UMOCK_C_01_007: [** umock_c_init when umock is already initialized shall fail and return a non-zero value. **]**
**SRS_UMOCK_C_01_024: [** on_umock_c_error shall be optional. **]**

##umock_c_deinit

```c
extern void umock_c_deinit(void);
```

**SRS_UMOCK_C_01_008: [** umock_c_deinit shall deinitialize the umock types by calling umocktypes_deinit. **]**
**SRS_UMOCK_C_01_009: [** umock_c_deinit shall free the call recorder created in umock_c_init. **]**
**SRS_UMOCK_C_01_010: [** If the module is not initialized, umock_c_deinit shall do nothing. **]**

##umock_c_reset_all_calls

```c
extern void umock_c_reset_all_calls(void);
```

**SRS_UMOCK_C_01_011: [** umock_c_reset_all_calls shall reset all calls by calling umockcallrecorder_reset_all_calls on the call recorder created in umock_c_init. **]**
**SRS_UMOCK_C_01_025: [** If the underlying umockcallrecorder_reset_all_calls fails, the on_umock_c_error callback shall be triggered with UMOCK_C_RESET_CALLS_ERROR. **]**
**SRS_UMOCK_C_01_012: [** If the module is not initialized, umock_c_reset_all_calls shall do nothing. **]**

##umock_c_get_actual_calls

```c
extern const char* umock_c_get_actual_calls(void);
```

**SRS_UMOCK_C_01_013: [** umock_c_get_actual_calls shall return the string for the recorded actual calls by calling umockcallrecorder_get_actual_calls on the call recorder created in umock_c_init. **]**
**SRS_UMOCK_C_01_014: [** If the module is not initialized, umock_c_get_actual_calls shall return NULL. **]**

##umock_c_get_expected_calls

```c
extern const char* umock_c_get_expected_calls(void);
```

**SRS_UMOCK_C_01_015: [** umock_c_get_expected_calls shall return the string for the recorded expected calls by calling umockcallrecorder_get_expected_calls on the call recorder created in umock_c_init. **]**
**SRS_UMOCK_C_01_016: [** If the module is not initialized, umock_c_get_expected_calls shall return NULL. **]**

##umock_c_get_last_expected_call

```c
extern UMOCKCALL_HANDLE umock_c_get_last_expected_call(void);
```

**SRS_UMOCK_C_01_017: [** umock_c_get_last_expected_call shall return the last expected call by calling umockcallrecorder_get_last_expected_call on the call recorder created in umock_c_init. **]**
**SRS_UMOCK_C_01_018: [** If the module is not initialized, umock_c_get_last_expected_call shall return NULL. **]**

##umock_c_add_expected_call

```c
extern int umock_c_add_expected_call(UMOCKCALL_HANDLE mock_call);
```

**SRS_UMOCK_C_01_019: [** umock_c_add_expected_call shall add an expected call by calling umockcallrecorder_add_expected_call on the call recorder created in umock_c_init. **]**
**SRS_UMOCK_C_01_020: [** If the module is not initialized, umock_c_add_expected_call shall return a non-zero value. **]**

##umock_c_add_actual_call

```c
extern int umock_c_add_actual_call(UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call);
```

**SRS_UMOCK_C_01_021: [** umock_c_add_actual_call shall add an actual call by calling umockcallrecorder_add_actual_call on the call recorder created in umock_c_init. **]**
**SRS_UMOCK_C_01_022: [** If the module is not initialized, umock_c_add_actual_call shall return a non-zero value. **]**

##umock_c_get_call_recorder

```c
extern UMOCKCALLRECORDER_HANDLE umock_c_get_call_recorder(void);
```

**SRS_UMOCK_C_01_026: [** umock_c_get_call_recorder shall return the handle to the currently used call recorder. **]**
**SRS_UMOCK_C_01_027: [** If the module is not initialized, umock_c_get_call_recorder shall return NULL. **]**

##umock_c_set_call_recorder

```c
extern int umock_c_set_call_recorder(UMOCKCALLRECORDER_HANDLE call_recorder);
```

**SRS_UMOCK_C_01_028: [** umock_c_set_call_recorder shall replace the currently used call recorder with the one identified by the call_recorder argument. **]**
**SRS_UMOCK_C_01_029: [** On success, umock_c_set_call_recorder shall return 0. **]**
**SRS_UMOCK_C_01_030: [** If call_recorder is NULL, umock_c_set_call_recorder shall return a non-zero value. **]**
**SRS_UMOCK_C_01_031: [** umock_c_set_call_recorder shall make a copy of call_recorder by calling umockcallrecorder_clone and use the copy for future actions. **]**
**SRS_UMOCK_C_01_032: [** If umockcallrecorder_clone fails, umock_c_set_call_recorder shall return a non-zero value. **]**
**SRS_UMOCK_C_01_033: [** If the module is not initialized, umock_c_set_call_recorder shall return a non-zero value. **]**
**SRS_UMOCK_C_01_034: [** The previously used call recorder shall be destroyed by calling umockcallrecorder_destroy. **]**

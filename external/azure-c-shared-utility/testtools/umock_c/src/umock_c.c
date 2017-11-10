// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdio.h>

#include "umock_c.h"
#include "umockcall.h"
#include "umocktypes.h"
#include "umocktypes_c.h"
#include "umockcallrecorder.h"
#include "umock_log.h"

typedef enum UMOCK_C_STATE_TAG
{
    UMOCK_C_STATE_NOT_INITIALIZED,
    UMOCK_C_STATE_INITIALIZED
} UMOCK_C_STATE;

static ON_UMOCK_C_ERROR on_umock_c_error_function;
static UMOCK_C_STATE umock_c_state = UMOCK_C_STATE_NOT_INITIALIZED;
static UMOCKCALLRECORDER_HANDLE umock_call_recorder = NULL;

int umock_c_init(ON_UMOCK_C_ERROR on_umock_c_error)
{
    int result;

    if (umock_c_state != UMOCK_C_STATE_NOT_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_007: [ umock_c_init when umock is already initialized shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umock_c: umock_c already initialized.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_LIB_01_144: [ Out of the box umock_c shall support the following types through the header umocktypes_c.h: ]*/
        /* Codes_SRS_UMOCK_C_LIB_01_028: [**char**] */
        /* Codes_SRS_UMOCK_C_LIB_01_029 : [**unsigned char**] */
        /* Codes_SRS_UMOCK_C_LIB_01_030 : [**short**] */
        /* Codes_SRS_UMOCK_C_LIB_01_031 : [**unsigned short**] */
        /* Codes_SRS_UMOCK_C_LIB_01_032 : [**int**] */
        /* Codes_SRS_UMOCK_C_LIB_01_033 : [**unsigned int**] */
        /* Codes_SRS_UMOCK_C_LIB_01_034 : [**long**] */
        /* Codes_SRS_UMOCK_C_LIB_01_035 : [**unsigned long**] */
        /* Codes_SRS_UMOCK_C_LIB_01_036 : [**long long**] */
        /* Codes_SRS_UMOCK_C_LIB_01_037 : [**unsigned long long**] */
        /* Codes_SRS_UMOCK_C_LIB_01_038 : [**float**] */
        /* Codes_SRS_UMOCK_C_LIB_01_039 : [**double**] */
        /* Codes_SRS_UMOCK_C_LIB_01_040 : [**long double**] */
        /* Codes_SRS_UMOCK_C_LIB_01_041 : [**size_t**] */
        /* Codes_SRS_UMOCK_C_LIB_01_151: [ void\* ]*/
        /* Codes_SRS_UMOCK_C_LIB_01_152: [ const void\* ]*/
        /* Codes_SRS_UMOCK_C_01_023: [ umock_c_init shall initialize the umock types by calling umocktypes_init. ]*/
        if ((umocktypes_init() != 0) ||
            /* Codes_SRS_UMOCK_C_01_002: [ umock_c_init shall register the C naive types by calling umocktypes_c_register_types. ]*/
            (umocktypes_c_register_types() != 0))
        {
            /* Codes_SRS_UMOCK_C_01_005: [ If any of the calls fails, umock_c_init shall fail and return a non-zero value. ]*/
            UMOCK_LOG("umock_c: Could not register standard C types with umock_c.");
            result = __LINE__;
        }
        else
        {
            /* Codes_SRS_UMOCK_C_01_003: [ umock_c_init shall create a call recorder by calling umockcallrecorder_create. ]*/
            umock_call_recorder = umockcallrecorder_create();
            if (umock_call_recorder == NULL)
            {
                /* Codes_SRS_UMOCK_C_01_005: [ If any of the calls fails, umock_c_init shall fail and return a non-zero value. ]*/
                UMOCK_LOG("umock_c: Could not create the call recorder.");
                result = __LINE__;
            }
            else
            {
                /* Codes_SRS_UMOCK_C_01_024: [ on_umock_c_error shall be optional. ]*/
                /* Codes_SRS_UMOCK_C_01_006: [ The on_umock_c_error callback shall be stored to be used for later error callbacks. ]*/
                on_umock_c_error_function = on_umock_c_error;

                /* Codes_SRS_UMOCK_C_01_001: [umock_c_init shall initialize the umock library.] */
                umock_c_state = UMOCK_C_STATE_INITIALIZED;

                /* Codes_SRS_UMOCK_C_01_004: [ On success, umock_c_init shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

void umock_c_deinit(void)
{
    /* Codes_SRS_UMOCK_C_LIB_01_012: [If umock_c was not initialized, umock_c_deinit shall do nothing.] */
    /* Codes_SRS_UMOCK_C_01_010: [ If the module is not initialized, umock_c_deinit shall do nothing. ] */
    if (umock_c_state == UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_009: [ umock_c_deinit shall free the call recorder created in umock_c_init. ]*/
        umockcallrecorder_destroy(umock_call_recorder);
        umock_c_state = UMOCK_C_STATE_NOT_INITIALIZED;

        /* Codes_SRS_UMOCK_C_01_008: [ umock_c_deinit shall deinitialize the umock types by calling umocktypes_deinit. ]*/
        umocktypes_deinit();
    }
}

void umock_c_reset_all_calls(void)
{
    /* Codes_SRS_UMOCK_C_01_012: [ If the module is not initialized, umock_c_reset_all_calls shall do nothing. ]*/
    if (umock_c_state == UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_011: [ umock_c_reset_all_calls shall reset all calls by calling umockcallrecorder_reset_all_calls on the call recorder created in umock_c_init. ]*/
        if (umockcallrecorder_reset_all_calls(umock_call_recorder) != 0)
        {
            /* Codes_SRS_UMOCK_C_01_025: [ If the underlying umockcallrecorder_reset_all_calls fails, the on_umock_c_error callback shall be triggered with UMOCK_C_RESET_CALLS_ERROR. ]*/
            umock_c_indicate_error(UMOCK_C_RESET_CALLS_ERROR);
        }
    }
}

int umock_c_add_expected_call(UMOCKCALL_HANDLE mock_call)
{
    int result;

    if (umock_c_state != UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_020: [ If the module is not initialized, umock_c_add_expected_call shall return a non-zero value. ]*/
        UMOCK_LOG("umock_c: Cannot add an expected call, umock_c not initialized.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_01_019: [ umock_c_add_expected_call shall add an expected call by calling umockcallrecorder_add_expected_call on the call recorder created in umock_c_init. ]*/
        result = umockcallrecorder_add_expected_call(umock_call_recorder, mock_call);
    }

    return result;
}

int umock_c_add_actual_call(UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call)
{
    int result;

    if (umock_c_state != UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_022: [ If the module is not initialized, umock_c_add_actual_call shall return a non-zero value. ]*/
        UMOCK_LOG("umock_c: Cannot add an actual call, umock_c not initialized.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_01_021: [ umock_c_add_actual_call shall add an actual call by calling umockcallrecorder_add_actual_call on the call recorder created in umock_c_init. ]*/
        result = umockcallrecorder_add_actual_call(umock_call_recorder, mock_call, matched_call);
    }

    return result;
}

const char* umock_c_get_expected_calls(void)
{
    const char* result;

    if (umock_c_state != UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_016: [ If the module is not initialized, umock_c_get_expected_calls shall return NULL. ]*/
        UMOCK_LOG("umock_c: Cannot get the expected calls, umock_c not initialized.");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_01_015: [ umock_c_get_expected_calls shall return the string for the recorded expected calls by calling umockcallrecorder_get_expected_calls on the call recorder created in umock_c_init. ]*/
        result = umockcallrecorder_get_expected_calls(umock_call_recorder);
    }

    return result;
}

const char* umock_c_get_actual_calls(void)
{
    const char* result;

    if (umock_c_state != UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_014: [ If the module is not initialized, umock_c_get_actual_calls shall return NULL. ]*/
        UMOCK_LOG("umock_c: Cannot get the actual calls, umock_c not initialized.");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_01_013: [ umock_c_get_actual_calls shall return the string for the recorded actual calls by calling umockcallrecorder_get_actual_calls on the call recorder created in umock_c_init. ]*/
        result = umockcallrecorder_get_actual_calls(umock_call_recorder);
    }

    return result;
}

UMOCKCALL_HANDLE umock_c_get_last_expected_call(void)
{
    UMOCKCALL_HANDLE result;

    if (umock_c_state != UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_018: [ If the module is not initialized, umock_c_get_last_expected_call shall return NULL. ]*/
        UMOCK_LOG("umock_c: Cannot get the last expected call, umock_c not initialized.");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_01_017: [ umock_c_get_last_expected_call shall return the last expected call by calling umockcallrecorder_get_last_expected_call on the call recorder created in umock_c_init. ]*/
        result = umockcallrecorder_get_last_expected_call(umock_call_recorder);
    }

    return result;
}

void umock_c_indicate_error(UMOCK_C_ERROR_CODE error_code)
{
    if (on_umock_c_error_function != NULL)
    {
        on_umock_c_error_function(error_code);
    }
}

UMOCKCALLRECORDER_HANDLE umock_c_get_call_recorder(void)
{
    UMOCKCALLRECORDER_HANDLE result;

    if (umock_c_state != UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_027: [ If the module is not initialized, umock_c_get_call_recorder shall return NULL. ]*/
        UMOCK_LOG("umock_c_get_call_recorder: Cannot get the call recorder, umock_c not initialized.");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_01_026: [ umock_c_get_call_recorder shall return the handle to the currently used call recorder. ]*/
        result = umock_call_recorder;
    }

    return result;
}

int umock_c_set_call_recorder(UMOCKCALLRECORDER_HANDLE call_recorder)
{
    int result;

    if (umock_c_state != UMOCK_C_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_01_033: [ If the module is not initialized, umock_c_set_call_recorder shall return a non-zero value. ]*/
        UMOCK_LOG("umock_c_set_call_recorder: Cannot set the call recorder, umock_c not initialized.");
        result = __LINE__;
    }
    else if (call_recorder == NULL)
    {
        /* Codes_SRS_UMOCK_C_01_030: [ If call_recorder is NULL, umock_c_set_call_recorder shall return a non-zero value. ]*/
        UMOCK_LOG("umock_c_set_call_recorder: NULL call_recorder.");
        result = __LINE__;
    }
    else
    {
        UMOCKCALLRECORDER_HANDLE new_call_recorder;

        /* Codes_SRS_UMOCK_C_01_028: [ umock_c_set_call_recorder shall replace the currently used call recorder with the one identified by the call_recorder argument. ]*/
        /* Codes_SRS_UMOCK_C_01_031: [ umock_c_set_call_recorder shall make a copy of call_recorder by calling umockcallrecorder_clone and use the copy for future actions. ]*/
        new_call_recorder = umockcallrecorder_clone(call_recorder);
        if (new_call_recorder == NULL)
        {
            /* Codes_SRS_UMOCK_C_01_032: [ If umockcallrecorder_clone fails, umock_c_set_call_recorder shall return a non-zero value. ]*/
            UMOCK_LOG("umock_c_set_call_recorder: Failed cloning call recorder.");
            result = __LINE__;
        }
        else
        {
            /* Codes_SRS_UMOCK_C_01_034: [ The previously used call recorder shall be destroyed by calling umockcallrecorder_destroy. ]*/
            umockcallrecorder_destroy(umock_call_recorder);
            umock_call_recorder = new_call_recorder;

            /* Codes_SRS_UMOCK_C_01_029: [ On success, umock_c_set_call_recorder shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

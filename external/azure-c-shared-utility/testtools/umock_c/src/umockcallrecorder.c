// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdio.h>
#include <string.h>

#include "umockcallrecorder.h"
#include "umockcall.h"
#include "umocktypes.h"
#include "umockalloc.h"
#include "umock_log.h"

typedef struct UMOCK_EXPECTED_CALL_TAG
{
    UMOCKCALL_HANDLE umockcall;
    unsigned int is_matched : 1;
} UMOCK_EXPECTED_CALL;

typedef struct UMOCKCALLRECORDER_TAG
{
    size_t expected_call_count;
    UMOCK_EXPECTED_CALL* expected_calls;
    size_t actual_call_count;
    UMOCKCALL_HANDLE* actual_calls;
    char* expected_calls_string;
    char* actual_calls_string;
} UMOCKCALLRECORDER;

UMOCKCALLRECORDER_HANDLE umockcallrecorder_create(void)
{
    UMOCKCALLRECORDER_HANDLE result;

    /* Codes_SRS_UMOCKCALLRECORDER_01_001: [ umockcallrecorder_create shall create a new instance of a call recorder and return a non-NULL handle to it on success. ]*/
    result = (UMOCKCALLRECORDER_HANDLE)umockalloc_malloc(sizeof(UMOCKCALLRECORDER));
    /* Codes_SRS_UMOCKCALLRECORDER_01_002: [ If allocating memory for the call recorder fails, umockcallrecorder_create shall return NULL. ]*/
    if (result != NULL)
    {
        result->expected_call_count = 0;
        result->expected_calls = NULL;
        result->expected_calls_string = NULL;
        result->actual_call_count = 0;
        result->actual_calls = NULL;
        result->actual_calls_string = NULL;
    }

    return result;
}

void umockcallrecorder_destroy(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    /* Codes_SRS_UMOCKCALLRECORDER_01_004: [ If umock_call_recorder is NULL, umockcallrecorder_destroy shall do nothing. ]*/
    if (umock_call_recorder != NULL)
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_003: [ umockcallrecorder_destroy shall free the resources associated with a the call recorder identified by the umock_call_recorder argument. ]*/
        (void)umockcallrecorder_reset_all_calls(umock_call_recorder);
        umockalloc_free(umock_call_recorder->actual_calls_string);
        umockalloc_free(umock_call_recorder->expected_calls_string);
        umockalloc_free(umock_call_recorder);
    }
}

int umockcallrecorder_reset_all_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    int result;

    if (umock_call_recorder == NULL)
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_007: [ If umock_call_recorder is NULL, umockcallrecorder_reset_all_calls shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umockcallrecorder: Reset all calls failed: NULL umock_call_recorder.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_005: [ umockcallrecorder_reset_all_calls shall free all the expected and actual calls for the call recorder identified by umock_call_recorder. ]*/
        if (umock_call_recorder->expected_calls != NULL)
        {
            size_t i;
            for (i = 0; i < umock_call_recorder->expected_call_count; i++)
            {
                umockcall_destroy(umock_call_recorder->expected_calls[i].umockcall);
            }

            umockalloc_free(umock_call_recorder->expected_calls);
            umock_call_recorder->expected_calls = NULL;
        }
        umock_call_recorder->expected_call_count = 0;

        if (umock_call_recorder->actual_calls != NULL)
        {
            size_t i;
            for (i = 0; i < umock_call_recorder->actual_call_count; i++)
            {
                umockcall_destroy(umock_call_recorder->actual_calls[i]);
            }

            umockalloc_free(umock_call_recorder->actual_calls);
            umock_call_recorder->actual_calls = NULL;
        }
        umock_call_recorder->actual_call_count = 0;

        /* Codes_SRS_UMOCKCALLRECORDER_01_006: [ On success umockcallrecorder_reset_all_calls shall return 0. ]*/
        result = 0;
    }

    return result;
}

int umockcallrecorder_add_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call)
{
    int result;

    if ((umock_call_recorder == NULL) ||
        (mock_call == NULL))
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_012: [ If any of the arguments is NULL, umockcallrecorder_add_expected_call shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umockcallrecorder: Bad arguments in add expected call: umock_call_recorder = %p, mock_call = %p.",
            umock_call_recorder, mock_call);
        result = __LINE__;
    }
    else
    {
        UMOCK_EXPECTED_CALL* new_expected_calls = (UMOCK_EXPECTED_CALL*)umockalloc_realloc(umock_call_recorder->expected_calls, sizeof(UMOCK_EXPECTED_CALL) * (umock_call_recorder->expected_call_count + 1));
        if (new_expected_calls == NULL)
        {
            /* Codes_SRS_UMOCKCALLRECORDER_01_013: [ If allocating memory for the expected calls fails, umockcallrecorder_add_expected_call shall fail and return a non-zero value. ] */
            UMOCK_LOG("umockcallrecorder: Cannot allocate memory in add expected call.");
            result = __LINE__;
        }
        else
        {
            /* Codes_SRS_UMOCKCALLRECORDER_01_008: [ umockcallrecorder_add_expected_call shall add the mock_call call to the expected call list maintained by the call recorder identified by umock_call_recorder. ]*/
            umock_call_recorder->expected_calls = new_expected_calls;
            umock_call_recorder->expected_calls[umock_call_recorder->expected_call_count].umockcall = mock_call;
            umock_call_recorder->expected_calls[umock_call_recorder->expected_call_count++].is_matched = 0;

            /* Codes_SRS_UMOCKCALLRECORDER_01_009: [ On success umockcallrecorder_add_expected_call shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int umockcallrecorder_add_actual_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call)
{
    int result;

    if ((umock_call_recorder == NULL) ||
        (mock_call == NULL) ||
        (matched_call == NULL))
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_019: [ If any of the arguments is NULL, umockcallrecorder_add_actual_call shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umockcallrecorder: Bad arguments in add actual call: umock_call_recorder = %p, mock_call = %p, matched_call = %p.",
            umock_call_recorder, mock_call, matched_call);
        result = __LINE__;
    }
    else
    {
        size_t i;
        unsigned int is_error = 0;

        *matched_call = NULL;

        /* Codes_SRS_UMOCK_C_LIB_01_115: [ umock_c shall compare calls in order. ]*/
        for (i = 0; i < umock_call_recorder->expected_call_count; i++)
        {
            if (umock_call_recorder->expected_calls[i].is_matched == 0)
            {
                /* Codes_SRS_UMOCKCALLRECORDER_01_017: [ Comparing the calls shall be done by calling umockcall_are_equal. ]*/
                int are_equal_result = umockcall_are_equal(umock_call_recorder->expected_calls[i].umockcall, mock_call);
                if (are_equal_result == 1)
                {
                    /* Codes_SRS_UMOCKCALLRECORDER_01_016: [ If the call matches one of the expected calls, a handle to the matched call shall be filled into the matched_call argument. ]*/
                    *matched_call = umock_call_recorder->expected_calls[i].umockcall;
                    break;
                }
                /* Codes_SRS_UMOCKCALLRECORDER_01_021: [ If umockcall_are_equal fails, umockcallrecorder_add_actual_call shall fail and return a non-zero value. ]*/
                else if (are_equal_result != 0)
                {
                    is_error = 1;
                    break;
                }
                else
                {
                    i = umock_call_recorder->expected_call_count;
                    break;
                }
            }
        }

        if ((umock_call_recorder->actual_call_count == 0) && (i < umock_call_recorder->expected_call_count))
        {
            umock_call_recorder->expected_calls[i].is_matched = 1;
        }
        else
        {
            i = umock_call_recorder->expected_call_count;
        }

        if (is_error)
        {
            UMOCK_LOG("umockcallrecorder: Error in finding a matched call.");
            result = __LINE__;
        }
        else
        {
            if (i == umock_call_recorder->expected_call_count)
            {
                /* an unexpected call */
                /* Codes_SRS_UMOCKCALLRECORDER_01_014: [ umockcallrecorder_add_actual_call shall check whether the call mock_call matches any of the expected calls maintained by umock_call_recorder. ]*/
                UMOCKCALL_HANDLE* new_actual_calls = (UMOCKCALL_HANDLE*)umockalloc_realloc(umock_call_recorder->actual_calls, sizeof(UMOCKCALL_HANDLE) * (umock_call_recorder->actual_call_count + 1));
                if (new_actual_calls == NULL)
                {
                    UMOCK_LOG("umockcallrecorder: Cannot allocate memory for actual calls.");
                    result = __LINE__;
                }
                else
                {
                    umock_call_recorder->actual_calls = new_actual_calls;
                    umock_call_recorder->actual_calls[umock_call_recorder->actual_call_count++] = mock_call;

                    /* Codes_SRS_UMOCKCALLRECORDER_01_018: [ When no error is encountered, umockcallrecorder_add_actual_call shall return 0. ]*/
                    result = 0;
                }
            }
            else
            {
                umockcall_destroy(mock_call);

                /* Codes_SRS_UMOCKCALLRECORDER_01_018: [ When no error is encountered, umockcallrecorder_add_actual_call shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

const char* umockcallrecorder_get_expected_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    const char* result;

    if (umock_call_recorder == NULL)
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_029: [ If the umock_call_recorder is NULL, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
        UMOCK_LOG("umockcallrecorder: NULL umock_call_recorder in get expected calls.");
        result = NULL;
    }
    else
    {
        size_t i;
        char* new_expected_calls_string;
        size_t current_length = 0;

        for (i = 0; i < umock_call_recorder->expected_call_count; i++)
        {
            if (umock_call_recorder->expected_calls[i].is_matched == 0)
            {
                /* Codes_SRS_UMOCKCALLRECORDER_01_028: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
                char* stringified_call = umockcall_stringify(umock_call_recorder->expected_calls[i].umockcall);
                if (stringified_call == NULL)
                {
                    /* Codes_SRS_UMOCKCALLRECORDER_01_030: [ If umockcall_stringify fails, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
                    break;
                }
                else
                {
                    size_t stringified_call_length = strlen(stringified_call);
                    new_expected_calls_string = (char*)umockalloc_realloc(umock_call_recorder->expected_calls_string, current_length + stringified_call_length + 1);
                    if (new_expected_calls_string == NULL)
                    {
                        umockalloc_free(stringified_call);

                        /* Codes_SRS_UMOCKCALLRECORDER_01_031: [ If allocating memory for the resulting string fails, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
                        break;
                    }
                    else
                    {
                        umock_call_recorder->expected_calls_string = new_expected_calls_string;
                        (void)memcpy(umock_call_recorder->expected_calls_string + current_length, stringified_call, stringified_call_length + 1);
                        current_length += stringified_call_length;
                    }

                    umockalloc_free(stringified_call);
                }
            }
        }

        if (i < umock_call_recorder->expected_call_count)
        {
            result = NULL;
        }
        else
        {
            if (current_length == 0)
            {
                new_expected_calls_string = (char*)umockalloc_realloc(umock_call_recorder->expected_calls_string, 1);
                if (new_expected_calls_string == NULL)
                {
                    /* Codes_SRS_UMOCKCALLRECORDER_01_031: [ If allocating memory for the resulting string fails, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
                    UMOCK_LOG("umockcallrecorder: Cannot allocate memory for expected calls.");
                    result = NULL;
                }
                else
                {
                    umock_call_recorder->expected_calls_string = new_expected_calls_string;
                    umock_call_recorder->expected_calls_string[0] = '\0';

                    /* Codes_SRS_UMOCKCALLRECORDER_01_027: [ umockcallrecorder_get_expected_calls shall return a pointer to the string representation of all the expected calls. ]*/
                    result = umock_call_recorder->expected_calls_string;
                }
            }
            else
            {
                /* Codes_SRS_UMOCKCALLRECORDER_01_027: [ umockcallrecorder_get_expected_calls shall return a pointer to the string representation of all the expected calls. ]*/
                result = umock_call_recorder->expected_calls_string;
            }
        }
    }

    return result;
}

const char* umockcallrecorder_get_actual_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    const char* result;

    if (umock_call_recorder == NULL)
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_024: [ If the umock_call_recorder is NULL, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
        UMOCK_LOG("umockcallrecorder: NULL umock_call_recorder in get actual calls.");
        result = NULL;
    }
    else
    {
        size_t i;
        char* new_actual_calls_string;

        if (umock_call_recorder->actual_call_count == 0)
        {
            new_actual_calls_string = (char*)umockalloc_realloc(umock_call_recorder->actual_calls_string, 1);
            if (new_actual_calls_string == NULL)
            {
                /* Codes_SRS_UMOCKCALLRECORDER_01_026: [ If allocating memory for the resulting string fails, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
                UMOCK_LOG("umockcallrecorder: Cannot allocate memory for actual calls.");
                result = NULL;
            }
            else
            {
                umock_call_recorder->actual_calls_string = new_actual_calls_string;
                umock_call_recorder->actual_calls_string[0] = '\0';

                /* Codes_SRS_UMOCKCALLRECORDER_01_022: [ umockcallrecorder_get_actual_calls shall return a pointer to the string representation of all the actual calls. ]*/
                result = umock_call_recorder->actual_calls_string;
            }
        }
        else
        {
            size_t current_length = 0;

            for (i = 0; i < umock_call_recorder->actual_call_count; i++)
            {
                /* Codes_SRS_UMOCKCALLRECORDER_01_023: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
                char* stringified_call = umockcall_stringify(umock_call_recorder->actual_calls[i]);
                if (stringified_call == NULL)
                {
                    /* Codes_SRS_UMOCKCALLRECORDER_01_025: [ If umockcall_stringify fails, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
                    break;
                }
                else
                {
                    size_t stringified_call_length = strlen(stringified_call);
                    new_actual_calls_string = (char*)umockalloc_realloc(umock_call_recorder->actual_calls_string, current_length + stringified_call_length + 1);
                    if (new_actual_calls_string == NULL)
                    {
                        umockalloc_free(stringified_call);

                        /* Codes_SRS_UMOCKCALLRECORDER_01_026: [ If allocating memory for the resulting string fails, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
                        break;
                    }
                    else
                    {
                        umock_call_recorder->actual_calls_string = new_actual_calls_string;
                        (void)memcpy(umock_call_recorder->actual_calls_string + current_length, stringified_call, stringified_call_length + 1);
                        current_length += stringified_call_length;
                    }

                    umockalloc_free(stringified_call);
                }
            }

            if (i < umock_call_recorder->actual_call_count)
            {
                result = NULL;
            }
            else
            {
                /* Codes_SRS_UMOCKCALLRECORDER_01_022: [ umockcallrecorder_get_actual_calls shall return a pointer to the string representation of all the actual calls. ]*/
                result = umock_call_recorder->actual_calls_string;
            }
        }
    }

    return result;
}

UMOCKCALL_HANDLE umockcallrecorder_get_last_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    UMOCKCALL_HANDLE result;

    if (umock_call_recorder == NULL)
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_033: [ If umock_call_recorder is NULL, umockcallrecorder_get_last_expected_call shall fail and return NULL. ]*/
        UMOCK_LOG("umockcallrecorder: NULL umock_call_recorder in get last expected calls.");
        result = NULL;
    }
    else
    {
        if (umock_call_recorder->expected_call_count == 0)
        {
            /* Codes_SRS_UMOCKCALLRECORDER_01_034: [ If no expected call has been recorded for umock_call_recorder then umockcallrecorder_get_last_expected_call shall fail and return NULL. ]*/
            UMOCK_LOG("umockcallrecorder: No expected calls recorded.");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_UMOCKCALLRECORDER_01_032: [ umockcallrecorder_get_last_expected_call shall return the last expected call for the umock_call_recorder call recorder. ]*/
            result = umock_call_recorder->expected_calls[umock_call_recorder->expected_call_count - 1].umockcall;
        }
    }

    return result;
}

/* Codes_SRS_UMOCKCALLRECORDER_01_035: [ umockcallrecorder_clone shall clone a call recorder and return a handle to the newly cloned call recorder. ]*/
UMOCKCALLRECORDER_HANDLE umockcallrecorder_clone(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    UMOCKCALLRECORDER_HANDLE result;

    if (umock_call_recorder == NULL)
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_036: [ If the umock_call_recorder argument is NULL, umockcallrecorder_clone shall fail and return NULL. ]*/
        UMOCK_LOG("umockcallrecorder_clone: NULL umock_call_recorder.");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_037: [ If allocating memory for the new umock call recorder instance fails, umockcallrecorder_clone shall fail and return NULL. ]*/
        result = umockcallrecorder_create();
        if (result != NULL)
        {
            size_t i;

            result->expected_calls = (UMOCK_EXPECTED_CALL*)umockalloc_malloc(sizeof(UMOCK_EXPECTED_CALL) * umock_call_recorder->expected_call_count);
            if (result->expected_calls == NULL)
            {
                /* Codes_SRS_UMOCKCALLRECORDER_01_052: [ If allocating memory for the expected calls fails, umockcallrecorder_clone shall fail and return NULL. ]*/
                umockcallrecorder_destroy(result);
                result = NULL;
                UMOCK_LOG("umockcallrecorder: clone call recorder failed - cannot allocate expected_calls.");
            }
            else
            {
                for (i = 0; i < umock_call_recorder->expected_call_count; i++)
                {
                    result->expected_calls[i].umockcall = NULL;
                }

                /* Codes_SRS_UMOCKCALLRECORDER_01_038: [ umockcallrecorder_clone shall clone all the expected calls. ]*/
                for (i = 0; i < umock_call_recorder->expected_call_count; i++)
                {
                    result->expected_calls[i].is_matched = umock_call_recorder->expected_calls[i].is_matched;

                    /* Codes_SRS_UMOCKCALLRECORDER_01_039: [ Each expected call shall be cloned by calling umockcall_clone. ]*/
                    result->expected_calls[i].umockcall = umockcall_clone(umock_call_recorder->expected_calls[i].umockcall);
                    if (result->expected_calls[i].umockcall == NULL)
                    {
                        break;
                    }
                }

                if (i < umock_call_recorder->expected_call_count)
                {
                    size_t j;

                    /* Codes_SRS_UMOCKCALLRECORDER_01_040: [ If cloning an expected call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
                    for (j = 0; j < i; j++)
                    {
                        umockcall_destroy(result->expected_calls[j].umockcall);
                        result->expected_calls[j].umockcall = NULL;
                    }

                    umockcallrecorder_destroy(result);
                    result = NULL;
                    UMOCK_LOG("umockcallrecorder: clone call recorder failed - cannot clone all expected calls.");
                }
                else
                {
                    result->expected_call_count = umock_call_recorder->expected_call_count;

                    result->actual_calls = (UMOCKCALL_HANDLE*)umockalloc_malloc(sizeof(UMOCKCALL_HANDLE) * umock_call_recorder->actual_call_count);
                    if (result->actual_calls == NULL)
                    {
                        /* Codes_SRS_UMOCKCALLRECORDER_01_053: [ If allocating memory for the actual calls fails, umockcallrecorder_clone shall fail and return NULL. ]*/
                        umockcallrecorder_destroy(result);
                        result = NULL;
                        UMOCK_LOG("umockcallrecorder: clone call recorder failed - cannot allocate expected_calls.");
                    }
                    else
                    {
                        for (i = 0; i < umock_call_recorder->actual_call_count; i++)
                        {
                            result->actual_calls[i] = NULL;
                        }

                        /* Codes_SRS_UMOCKCALLRECORDER_01_041: [ umockcallrecorder_clone shall clone all the actual calls. ]*/
                        for (i = 0; i < umock_call_recorder->actual_call_count; i++)
                        {
                            /* Codes_SRS_UMOCKCALLRECORDER_01_042: [ Each actual call shall be cloned by calling umockcall_clone. ]*/
                            result->actual_calls[i] = umockcall_clone(umock_call_recorder->actual_calls[i]);
                            if (result->actual_calls[i] == NULL)
                            {
                                break;
                            }
                        }

                        if (i < umock_call_recorder->actual_call_count)
                        {
                            size_t j;

                            /* Codes_SRS_UMOCKCALLRECORDER_01_043: [ If cloning an actual call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
                            for (j = 0; j < i; j++)
                            {
                                umockcall_destroy(result->actual_calls[j]);
                                result->actual_calls[j] = NULL;
                            }

                            umockcallrecorder_destroy(result);
                            result = NULL;
                            UMOCK_LOG("umockcallrecorder: clone call recorder failed - cannot clone all actual calls.");
                        }
                        else
                        {
                            result->actual_call_count = umock_call_recorder->actual_call_count;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int umockcallrecorder_get_expected_call_count(UMOCKCALLRECORDER_HANDLE umock_call_recorder, size_t* expected_call_count)
{
    int result;

    if ((umock_call_recorder == NULL) ||
        (expected_call_count == NULL))
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_046: [ If any of the arguments is NULL, umockcallrecorder_get_expected_call_count shall return a non-zero value. ]*/
        result = __LINE__;
        UMOCK_LOG("umockcallrecorder_get_expected_call_count: Invalid arguments, umock_call_recorder = %p, expected_call_count = %p",
            umock_call_recorder, expected_call_count);
    }
    else
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_044: [ umockcallrecorder_get_expected_call_count shall return in the expected_call_count argument the number of expected calls associated with the call recorder. ]*/
        *expected_call_count = umock_call_recorder->expected_call_count;
        /* Codes_SRS_UMOCKCALLRECORDER_01_045: [ On success umockcallrecorder_get_expected_call_count shall return 0. ]*/
        result = 0;
    }

    return result;
}

int umockcallrecorder_fail_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, size_t index)
{
    int result;

    /* Codes_SRS_UMOCKCALLRECORDER_01_049: [ If umock_call_recorder is NULL, umockcallrecorder_fail_call shall return a non-zero value. ]*/
    if ((umock_call_recorder == NULL) ||
        /* Codes_SRS_UMOCKCALLRECORDER_01_050: [ If index is invalid, umockcallrecorder_fail_call shall return a non-zero value. ]*/
        (index >= umock_call_recorder->expected_call_count))
    {
        result = __LINE__;
        UMOCK_LOG("umockcallrecorder_fail_call: NULL Invalid arguments, umock_call_recorder = %p, index = %zu",
            umock_call_recorder, index);
    }
    else
    {
        /* Codes_SRS_UMOCKCALLRECORDER_01_047: [ umockcallrecorder_fail_call shall mark an expected call as to be failed by calling umockcall_set_fail_call with a 1 value for fail_call. ]*/
        if (umockcall_set_fail_call(umock_call_recorder->expected_calls[index].umockcall, 1) != 0)
        {
            /* Codes_SRS_UMOCKCALLRECORDER_01_051: [ If umockcall_set_fail_call fails, umockcallrecorder_fail_call shall return a non-zero value. ]*/
            result = __LINE__;
            UMOCK_LOG("umockcallrecorder_fail_call: umockcall_set_fail_call failed.");
        }
        else
        {
            /* Codes_SRS_UMOCKCALLRECORDER_01_048: [ On success, umockcallrecorder_fail_call shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

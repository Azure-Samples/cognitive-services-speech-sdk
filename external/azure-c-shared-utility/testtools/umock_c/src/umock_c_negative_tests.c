// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stddef.h>
#include "umock_c_negative_tests.h"
#include "umock_c.h"
#include "umockcall.h"
#include "umockcallrecorder.h"
#include "umock_log.h"

static UMOCKCALLRECORDER_HANDLE snapshot_call_recorder;
typedef enum UMOCK_C_NEGATIVE_TESTS_STATE_TAG
{
    UMOCK_C_NEGATIVE_TESTS_STATE_NOT_INITIALIZED,
    UMOCK_C_NEGATIVE_TESTS_STATE_INITIALIZED
} UMOCK_C_NEGATIVE_TESTS_STATE;

static UMOCK_C_NEGATIVE_TESTS_STATE umock_c_negative_tests_state = UMOCK_C_NEGATIVE_TESTS_STATE_NOT_INITIALIZED;

int umock_c_negative_tests_init(void)
{
    int result;

    if (umock_c_negative_tests_state != UMOCK_C_NEGATIVE_TESTS_STATE_NOT_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_005: [ If the module has already been initialized, umock_c_negative_tests_init shall return a non-zero value. ]*/
        UMOCK_LOG("umock_c_negative_tests_init: Already initialized.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_004: [ On success, umock_c_negative_tests_init shall return 0. ]*/
        umock_c_negative_tests_state = UMOCK_C_NEGATIVE_TESTS_STATE_INITIALIZED;
        result = 0;
    }

    return result;
}

void umock_c_negative_tests_deinit(void)
{
    /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_003: [ If the module was not previously initialized, umock_c_negative_tests_deinit shall do nothing. ]*/
    if (umock_c_negative_tests_state == UMOCK_C_NEGATIVE_TESTS_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_002: [ umock_c_negative_tests_deinit shall free all resources associated with the negative tests module. ]*/
        if (snapshot_call_recorder != NULL)
        {
            umockcallrecorder_destroy(snapshot_call_recorder);
            snapshot_call_recorder = NULL;
        }

        umock_c_negative_tests_state = UMOCK_C_NEGATIVE_TESTS_STATE_NOT_INITIALIZED;
    }
}

/* Codes_SRS_UMOCK_C_LIB_01_167: [ umock_c_negative_tests_snapshot shall take a snapshot of the current setup of expected calls (a.k.a happy path). ]*/
/* Codes_SRS_UMOCK_C_LIB_01_168: [ If umock_c_negative_tests_snapshot is called without the module being initialized, it shall do nothing. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_169: [ All errors shall be reported by calling the umock_c on error function. ]*/
/* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_006: [ umock_c_negative_tests_snapshot shall make a copy of the current call recorder for umock_c with all its recorded calls. ]*/
void umock_c_negative_tests_snapshot(void)
{
    if (umock_c_negative_tests_state != UMOCK_C_NEGATIVE_TESTS_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_015: [ If the module was not previously initialized, umock_c_negative_tests_snapshot shall do nothing. ]*/
        UMOCK_LOG("umock_c_negative_tests_snapshot: Not initialized.");
    }
    else
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_007: [ The current call recorder shall be obtained by calling umock_c_get_call_recorder. ]*/
        UMOCKCALLRECORDER_HANDLE current_call_recorder = umock_c_get_call_recorder();
        if (current_call_recorder == NULL)
        {
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_009: [ If getting the call recorder fails, umock_c_negative_tests_snapshot shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
            UMOCK_LOG("umock_c_negative_tests_snapshot: Error getting the current call recorder.");
            umock_c_indicate_error(UMOCK_C_ERROR);
        }
        else
        {
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_008: [ The copy of the recorder shall be made by calling umockcallrecorder_clone. ]*/
            UMOCKCALLRECORDER_HANDLE new_call_recorder = umockcallrecorder_clone(current_call_recorder);
            if (new_call_recorder == NULL)
            {
                /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_010: [ If copying the call recorder fails, umock_c_negative_tests_snapshot shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
                UMOCK_LOG("umock_c_negative_tests_snapshot: Error cloning the call recorder.");
                umock_c_indicate_error(UMOCK_C_ERROR);
            }
            else
            {
                if (snapshot_call_recorder != NULL)
                {
                    umockcallrecorder_destroy(snapshot_call_recorder);
                }

                snapshot_call_recorder = new_call_recorder;
            }
        }
    }
}

/* Codes_SRS_UMOCK_C_LIB_01_170: [ umock_c_negative_tests_reset shall bring umock_c expected and actual calls to the state recorded when umock_c_negative_tests_snapshot was called. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_171: [ If umock_c_negative_tests_reset is called without the module being initialized, it shall do nothing. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_172: [ All errors shall be reported by calling the umock_c on error function. ]*/
void umock_c_negative_tests_reset(void)
{
    if (umock_c_negative_tests_state != UMOCK_C_NEGATIVE_TESTS_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_016: [ If the module was not previously initialized, umock_c_negative_tests_reset shall do nothing. ]*/
        UMOCK_LOG("umock_c_negative_tests_reset: Not initialized.");
    }
    else
    {
        if (snapshot_call_recorder == NULL)
        {
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_012: [ If no call has been made to umock_c_negative_tests_snapshot, umock_c_negative_tests_reset shall indicate a failure via the umock error callback with error code UMOCK_C_ERROR. ]*/
            UMOCK_LOG("umock_c_negative_tests_reset: No snapshot was done, use umock_c_negative_tests_snapshot.");
            umock_c_indicate_error(UMOCK_C_ERROR);
        }
        else
        {
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_011: [ umock_c_negative_tests_reset shall reset the call recorder used by umock_c to the call recorder stored in umock_c_negative_tests_reset. ]*/
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_013: [ The reset shall be done by calling umock_c_set_call_recorder and passing the call recorder stored in umock_c_negative_tests_reset as argument. ]*/
            if (umock_c_set_call_recorder(snapshot_call_recorder) != 0)
            {
                /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_014: [ If umock_c_set_call_recorder fails, umock_c_negative_tests_reset shall indicate a failure via the umock error callback with error code UMOCK_C_ERROR. ]*/
                UMOCK_LOG("umock_c_negative_tests_reset: Failed setting the call recorder to the snapshot one.");
                umock_c_indicate_error(UMOCK_C_ERROR);
            }
        }
    }
}

/* Codes_SRS_UMOCK_C_LIB_01_173: [ umock_c_negative_tests_fail_call shall instruct the negative tests module to fail a specific call. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_174: [ If umock_c_negative_tests_fail_call is called without the module being initialized, it shall do nothing. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_175: [ All errors shall be reported by calling the umock_c on error function. ]*/
void umock_c_negative_tests_fail_call(size_t index)
{
    if (umock_c_negative_tests_state != UMOCK_C_NEGATIVE_TESTS_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_020: [ If the module was not previously initialized, umock_c_negative_tests_fail_call shall do nothing. ]*/
        UMOCK_LOG("umock_c_negative_tests_fail_call: Not initialized.");
    }
    else
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_017: [ umock_c_negative_tests_fail_call shall call umockcallrecorder_fail_call on the currently used call recorder used by umock_c. ]*/
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_018: [ The currently used recorder shall be obtained by calling umock_c_get_call_recorder. ]*/
        UMOCKCALLRECORDER_HANDLE call_recorder = umock_c_get_call_recorder();
        if (call_recorder == NULL)
        {
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_019: [ If umock_c_get_call_recorder fails, umock_c_negative_tests_fail_call shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
            UMOCK_LOG("umock_c_negative_tests_fail_call: Cannot get call recorder.");
            umock_c_indicate_error(UMOCK_C_ERROR);
        }
        else
        {
            if (umockcallrecorder_fail_call(call_recorder, index) != 0)
            {
                /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_025: [ If failing the call by calling umockcallrecorder_fail_call fails, umock_c_negative_tests_fail_call shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
                UMOCK_LOG("umock_c_negative_tests_fail_call: Cannot get call recorder.");
                umock_c_indicate_error(UMOCK_C_ERROR);
            }
        }
    }
}

/* Codes_SRS_UMOCK_C_LIB_01_176: [ umock_c_negative_tests_call_count shall provide the number of expected calls, so that the test code can iterate through all negative cases. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_177: [ If umock_c_negative_tests_fail_call is called without the module being initialized, it shall return 0. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_178: [ All errors shall be reported by calling the umock_c on error function. ]*/
size_t umock_c_negative_tests_call_count(void)
{
    size_t result;

    if (umock_c_negative_tests_state != UMOCK_C_NEGATIVE_TESTS_STATE_INITIALIZED)
    {
        /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_024: [ If the module was not previously initialized, umock_c_negative_tests_call_count shall return 0. ]*/
        UMOCK_LOG("umock_c_negative_tests_call_count: Not initialized.");
        result = 0;
    }
    else
    {
        if (snapshot_call_recorder == NULL)
        {
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_022: [ If no call has been made to umock_c_negative_tests_snapshot, umock_c_negative_tests_fail_call shall return 0 and indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
            UMOCK_LOG("umock_c_negative_tests_call_count: No snapshot, use umock_c_negative_tests_snapshot.");
            umock_c_indicate_error(UMOCK_C_ERROR);
            result = 0;
        }
        else
        {
            /* Codes_SRS_UMOCK_C_NEGATIVE_TESTS_01_021: [ umock_c_negative_tests_call_count shall return the count of expected calls for the current snapshot call recorder by calling umockcallrecorder_get_expected_call_count. ]*/
            if (umockcallrecorder_get_expected_call_count(snapshot_call_recorder, &result) != 0)
            {
                UMOCK_LOG("umock_c_negative_tests_call_count: Getting the expected call count from the recorder failed.");
                umock_c_indicate_error(UMOCK_C_ERROR);
                result = 0;
            }
        }
    }

    return result;
}

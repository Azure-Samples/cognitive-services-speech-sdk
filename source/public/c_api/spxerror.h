//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// spxerror.h: Public API definitions for global SPX errors and realted #define macros
//

#pragma once

typedef unsigned int SPXHR;

#define SPX_NOERROR                 0x00000000
#define SPX_INIT_HR(hr)             SPXHR hr = SPX_NOERROR

#define SPX_SUCCEEDED(x)            ((x & 0x80000000) == 0)
#define SPX_FAILED(x)               (!SPX_SUCCEEDED(x))   

#define __SPX_ERRCODE_SUCCEEDED(x)  (0x000f5000 | (x & 0x0fff))
#define __SPX_ERRCODE_FAILED(x)     (0x800f5000 | (x & 0x0fff))


/*** SPXERR_NOT_IMPL
*   The function is not implemented.
*/
#define SPXERR_NOT_IMPL             __SPX_ERRCODE_FAILED(0xfff)

/*** SPXERR_UNINITIALIZED
*   The object has not been properly initialized.
*/
#define SPXERR_UNINITIALIZED        __SPX_ERRCODE_FAILED(0x001)

/*** SPXERR_ALREADY_INITIALIZED
*   The object has already been initialized.
*/
#define SPXERR_ALREADY_INITIALIZED  __SPX_ERRCODE_FAILED(0x002)

/*** SPXERR_UNHANDLED_EXCEPTION
*   An unnhandled exception was detected.
*/
#define SPXERR_UNHANDLED_EXCEPTION  __SPX_ERRCODE_FAILED(0x003)

/*** SPXERR_NOT_FOUND
*   The object or property was not found.
*/
#define SPXERR_NOT_FOUND            __SPX_ERRCODE_FAILED(0x004)

/*** SPXERR_INVALID_ARG
*   One or more arguments are not valid.
*/
#define SPXERR_INVALID_ARG          __SPX_ERRCODE_FAILED(0x004)

/*** SPXERR_TIMEOUT
*   The specified timeout value has ellapsed.
*/
#define SPXERR_TIMEOUT              __SPX_ERRCODE_FAILED(0x005)

/*** SPXERR_ALREADY_IN_PROGRESS
*   The asynchronous operation is already in progress.
*/
#define SPXERR_ALREADY_IN_PROGRESS  __SPX_ERRCODE_FAILED(0x006)

/*** SPXERR_FILE_OPEN_FAILED
*   The attempt to open the file failed.
*/
#define SPXERR_FILE_OPEN_FAILED     __SPX_ERRCODE_FAILED(0x007)

/*** SPXERR_UNEXPECTED_EOF
*   The end of the file was reached unexpectedly.
*/
#define SPXERR_UNEXPECTED_EOF       __SPX_ERRCODE_FAILED(0x008)

/*** SPXERR_INVALID_HEADER
*   One or more arguments are not valid.
*/
#define SPXERR_INVALID_HEADER       __SPX_ERRCODE_FAILED(0x009)

/*** SPXERR_AUDIO_IS_PUMPING
*   The requested operation cannot be performed while audio is pumping
*/
#define SPXERR_AUDIO_IS_PUMPING     __SPX_ERRCODE_FAILED(0x00a)

/*** SPXERR_UNSUPPORTED_FORMAT
*   The requested operation cannot be performed while audio is pumping
*/
#define SPXERR_UNSUPPORTED_FORMAT   __SPX_ERRCODE_FAILED(0x00b)

/*** SPXERR_ABORT
*   Operation aborted.
*/
#define SPXERR_ABORT                __SPX_ERRCODE_FAILED(0x00c)

/*** SPXERR_MIC_NOT_AVAILABLE
*   Microphone is not available.
*/
#define SPXERR_MIC_NOT_AVAILABLE    __SPX_ERRCODE_FAILED(0x00d)

/*** SPXERR_INVALID_STATE
*   An invalid state was encountered.
*/
#define SPXERR_INVALID_STATE        __SPX_ERRCODE_FAILED(0x00e)

/*** SPXERR_UUID_CREATE_FAILED
*   Attempting to create a UUID failed.
*/
#define SPXERR_UUID_CREATE_FAILED   __SPX_ERRCODE_FAILED(0x00f)

/*** SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION
*   An unexpected session state transition was encountered when setting the session audio format. Valid transitions are:
*   - StartingPump --> ProcessingAudio (at the beginning of stream)
*   - StoppingPump --> WaitingForAdapterDone (at the end of stream)
*   - ProcessingAudio --> WaitingForAdapterDone (when the stream runs out of data)
*   NOTE: All other state transitions are invalid
*/
#define SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION __SPX_ERRCODE_FAILED(0x010)

/*** SPXERR_PROCESS_AUDIO_INVALID_STATE
*   An unexpected session state was encountered in while processing audio. Valid states to encounter are:
*   - ProcessingAudio: We're allowed to process audio while in this state
*   - StoppingPump: We're allowed to be called to process audio, but we'll ignore the data passed in while we're attempting to stop the pump
*   NOTE: All other states are invalid while processing audio.
*/
#define SPXERR_PROCESS_AUDIO_INVALID_STATE __SPX_ERRCODE_FAILED(0x011)

/*** SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION
*   An unexpected state transition was encountered while attempting to start recognizing. Valid state transitions are:
*   - Idle --> StartingPump
*   NOTE: All other state transitions are invalid when attempting to start recognizing
*/
#define SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION __SPX_ERRCODE_FAILED(0x012)

/*** SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE
*   An unexpected error was encountered when trying to create an internal object.
*/
#define SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE  __SPX_ERRCODE_FAILED(0x013)

/*** SPXERR_MIC_ERROR
*   An error in the audio-capturing system.
*/
#define SPXERR_MIC_ERROR            __SPX_ERRCODE_FAILED(0x014)

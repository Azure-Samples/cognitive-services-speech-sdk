//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
/// \file spxerror.h
/// \brief Public API definitions for global SPX errors and related preprocessor macros.

#pragma once

#include <stdint.h>

/// <summary>
/// Type definition for SPX result codes.
/// </summary>
typedef uintptr_t SPXHR;

/// <summary>
/// Default result code indicating no error.
/// </summary>
#define SPX_NOERROR                 0

/// <summary>
/// Declare and initialize result code variable.
/// </summary>
#define SPX_INIT_HR(hr)             SPXHR hr = SPX_NOERROR; \
                                    (void)(hr)

/// <summary>
/// Check if result code indicates success.
/// </summary>
#define SPX_SUCCEEDED(x)            ((x) == SPX_NOERROR)

/// <summary>
/// Check if result code indicates error.
/// </summary>
#define SPX_FAILED(x)               (!SPX_SUCCEEDED(x))

#define __SPX_ERRCODE_FAILED(x)     (x)

/// <summary>
/// The function is not implemented.
/// </summary>
#define SPXERR_NOT_IMPL             __SPX_ERRCODE_FAILED(0xfff)

/// <summary>
/// The object has not been properly initialized.
/// </summary>
#define SPXERR_UNINITIALIZED        __SPX_ERRCODE_FAILED(0x001)

/// <summary>
/// The object has already been initialized.
/// </summary>
#define SPXERR_ALREADY_INITIALIZED  __SPX_ERRCODE_FAILED(0x002)

/// <summary>
/// An unhandled exception was detected.
/// </summary>
#define SPXERR_UNHANDLED_EXCEPTION  __SPX_ERRCODE_FAILED(0x003)

/// <summary>
/// The object or property was not found.
/// </summary>
#define SPXERR_NOT_FOUND            __SPX_ERRCODE_FAILED(0x004)

/// <summary>
/// One or more arguments are not valid.
/// </summary>
#define SPXERR_INVALID_ARG          __SPX_ERRCODE_FAILED(0x005)

/// <summary>
/// The specified timeout value has elapsed.
/// </summary>
#define SPXERR_TIMEOUT              __SPX_ERRCODE_FAILED(0x006)

/// <summary>
/// The asynchronous operation is already in progress.
/// </summary>
#define SPXERR_ALREADY_IN_PROGRESS  __SPX_ERRCODE_FAILED(0x007)

/// <summary>
/// The attempt to open the file failed.
/// </summary>
#define SPXERR_FILE_OPEN_FAILED     __SPX_ERRCODE_FAILED(0x008)

/// <summary>
/// The end of the file was reached unexpectedly.
/// </summary>
#define SPXERR_UNEXPECTED_EOF       __SPX_ERRCODE_FAILED(0x009)

/// <summary>
/// Invalid audio header encountered.
/// </summary>
#define SPXERR_INVALID_HEADER       __SPX_ERRCODE_FAILED(0x00a)

/// <summary>
/// The requested operation cannot be performed while audio is pumping
/// </summary>
#define SPXERR_AUDIO_IS_PUMPING     __SPX_ERRCODE_FAILED(0x00b)

/// <summary>
/// Unsupported audio format.
/// </summary>
#define SPXERR_UNSUPPORTED_FORMAT   __SPX_ERRCODE_FAILED(0x00c)

/// <summary>
/// Operation aborted.
/// </summary>
#define SPXERR_ABORT                __SPX_ERRCODE_FAILED(0x00d)

/// <summary>
/// Microphone is not available.
/// </summary>
#define SPXERR_MIC_NOT_AVAILABLE    __SPX_ERRCODE_FAILED(0x00e)

/// <summary>
/// An invalid state was encountered.
/// </summary>
#define SPXERR_INVALID_STATE        __SPX_ERRCODE_FAILED(0x00f)

/// <summary>
/// Attempting to create a UUID failed.
/// </summary>
#define SPXERR_UUID_CREATE_FAILED   __SPX_ERRCODE_FAILED(0x010)

/// <summary>
/// An unexpected session state transition was encountered when setting the session audio format.
/// </summary>
/// <remarks>
/// Valid transitions are:
/// * WaitForPumpSetFormatStart --> ProcessingAudio (at the beginning of stream)
/// * StoppingPump --> WaitForAdapterCompletedSetFormatStop (at the end of stream)
/// * ProcessingAudio --> WaitForAdapterCompletedSetFormatStop (when the stream runs out of data)
/// All other state transitions are invalid.
/// </remarks>
#define SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION __SPX_ERRCODE_FAILED(0x011)

/// <summary>
/// An unexpected session state was encountered in while processing audio.
/// </summary>
/// <remarks>
/// Valid states to encounter are:
/// * ProcessingAudio: We're allowed to process audio while in this state.
/// * StoppingPump: We're allowed to be called to process audio, but we'll ignore the data passed in while we're attempting to stop the pump.
/// All other states are invalid while processing audio.
/// </remarks>
#define SPXERR_PROCESS_AUDIO_INVALID_STATE __SPX_ERRCODE_FAILED(0x012)

/// <summary>
/// An unexpected state transition was encountered while attempting to start recognizing.
/// </summary>
/// <remarks>
/// A valid transition is:
/// * Idle --> WaitForPumpSetFormatStart
/// All other state transitions are invalid when attempting to start recognizing
/// </remarks>
#define SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION __SPX_ERRCODE_FAILED(0x013)

/// <summary>
/// An unexpected error was encountered when trying to create an internal object.
/// </summary>
#define SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE  __SPX_ERRCODE_FAILED(0x014)

/// <summary>
/// An error in the audio-capturing system.
/// </summary>
#define SPXERR_MIC_ERROR            __SPX_ERRCODE_FAILED(0x015)

/// <summary>
/// The requested operation cannot be performed; there is no audio input.
/// </summary>
#define SPXERR_NO_AUDIO_INPUT       __SPX_ERRCODE_FAILED(0x016)

/// <summary>
/// An unexpected error was encountered when trying to access the USP site.
/// </summary>
#define SPXERR_UNEXPECTED_USP_SITE_FAILURE  __SPX_ERRCODE_FAILED(0x017)

/// <summary>
/// An unexpected error was encountered when trying to access the Unidec site.
/// </summary>
#define SPXERR_UNEXPECTED_UNIDEC_SITE_FAILURE  __SPX_ERRCODE_FAILED(0x018)

/// <summary>
/// The buffer is too small.
/// </summary>
#define SPXERR_BUFFER_TOO_SMALL  __SPX_ERRCODE_FAILED(0x019)

/// <summary>
/// A method failed to allocate memory.
/// </summary>
#define SPXERR_OUT_OF_MEMORY  __SPX_ERRCODE_FAILED(0x01A)

/// <summary>
/// An unexpected runtime error occurred.
/// </summary>
#define SPXERR_RUNTIME_ERROR  __SPX_ERRCODE_FAILED(0x01B)

/// <summary>
/// The url specified is invalid.
/// </summary>
#define SPXERR_INVALID_URL  __SPX_ERRCODE_FAILED(0x01C)

/// <summary>
/// The region specified is invalid or missing.
/// </summary>
#define SPXERR_INVALID_REGION  __SPX_ERRCODE_FAILED(0x01D)


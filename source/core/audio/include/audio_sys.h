// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef AUDIO_SYS_H
#define AUDIO_SYS_H

#include <stdint.h>

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility_strings_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_RESULT_VALUES     \
AUDIO_RESULT_OK,                \
AUDIO_RESULT_INVALID_ARG,       \
AUDIO_RESULT_INVALID_STATE,     \
AUDIO_RESULT_ERROR              \

/** @brief Enumeration specifying the possible return values for the APIs in
*          this module.
*/
DEFINE_ENUM(AUDIO_RESULT, AUDIO_RESULT_VALUES)

#define AUDIO_STATE_VALUES  \
    AUDIO_STATE_STARTING,   \
    AUDIO_STATE_RUNNING,    \
    AUDIO_STATE_PAUSED,     \
    AUDIO_STATE_STOPPING, /* indicates the state when the stop signal is sent, the audio will stop after finishing the queued buffer.*/   \
    AUDIO_STATE_STOPPED,    \
    AUDIO_STATE_CLOSED      \

DEFINE_ENUM(AUDIO_STATE, AUDIO_STATE_VALUES)

#define AUDIO_ERROR_VALUES  \
    AUDIO_ERROR_NONE,       \
    AUDIO_ERROR_XRUN,       \
    AUDIO_ERROR_BUFF_FULL,  \
    AUDIO_ERROR_SND_PREPARE,\
    AUDIO_ERROR_BUSY_LOOP   \

DEFINE_ENUM(AUDIO_ERROR, AUDIO_ERROR_VALUES)

#define AUDIO_DATA_FLOW_VALUES  \
    AUDIO_CAPTURE,                \
    AUDIO_RENDER                  \

DEFINE_ENUM(AUDIO_DATA_FLOW, AUDIO_DATA_FLOW_VALUES)

typedef struct AUDIO_SYS_DATA_TAG* AUDIO_SYS_HANDLE;

typedef void* AUDIO_BUFFER;

typedef struct _AUDIO_SETTINGS
{
    uint16_t  wFormatTag;
    uint16_t  nChannels;
    uint32_t  nSamplesPerSec;
    uint32_t  nAvgBytesPerSec;
    uint16_t  nBlockAlign;
    uint16_t  wBitsPerSample;
    STRING_HANDLE hDeviceName;
    AUDIO_DATA_FLOW eDataFlow;
} AUDIO_SETTINGS;

typedef struct _AUDIO_WAVEFORMAT
{
    uint16_t wFormatTag;        /* format type */
    uint16_t nChannels;         /* number of channels (i.e. mono, stereo...) */
    uint32_t nSamplesPerSec;    /* sample rate */
    uint32_t nAvgBytesPerSec;   /* for buffer estimation */
    uint16_t nBlockAlign;       /* block size of data */
    uint16_t wBitsPerSample;    /* Number of bits per sample of mono data */
} AUDIO_WAVEFORMAT;

typedef AUDIO_SETTINGS* AUDIO_SETTINGS_HANDLE;

typedef void(*ON_AUDIOERROR_CALLBACK)(void* pContext, AUDIO_ERROR error);
typedef void(*ON_AUDIOOUTPUT_STATE_CALLBACK)(void* pContext, AUDIO_STATE state);
typedef void(*ON_AUDIOINPUT_STATE_CALLBACK)(void* pContext, AUDIO_STATE state);
typedef int(*AUDIOINPUT_WRITE)(void* pContext, uint8_t* pBuffer, uint32_t size);
typedef void(*AUDIOCOMPLETE_CALLBACK)(void* pContext);
typedef void(*AUDIO_BUFFERUNDERRUN_CALLBACK)(void* pContext);

// create a new AUDIO_SETTINGS instance
AUDIO_SETTINGS_HANDLE audio_format_create();
// destroy a AUDIO_SETTINGS instance
void audio_format_destroy(AUDIO_SETTINGS_HANDLE handle);

extern AUDIO_SYS_HANDLE audio_create_with_parameters(AUDIO_SETTINGS_HANDLE format);
extern void audio_destroy(AUDIO_SYS_HANDLE handle);
AUDIO_RESULT audio_setcallbacks(AUDIO_SYS_HANDLE              handle,
                                ON_AUDIOOUTPUT_STATE_CALLBACK output_cb,
                                void*                         output_ctx,
                                ON_AUDIOINPUT_STATE_CALLBACK  input_cb,
                                void*                         input_ctx,
                                AUDIOINPUT_WRITE              audio_write_cb,
                                void*                         audio_write_ctx,
                                ON_AUDIOERROR_CALLBACK        error_cb,
                                void*                         error_ctx);

#define AUDIO_OPTION_DEVICENAME "devicename"
#define AUDIO_OPTION_INPUT_FRAME_COUNT "buff_frame_count"
#define AUDIO_OPTION_NUMBER_CHANNELS "channels"

AUDIO_RESULT audio_set_options(AUDIO_SYS_HANDLE handle, const char* optionName, const void* value);

extern AUDIO_RESULT  audio_output_startasync(
    AUDIO_SYS_HANDLE handle,
    const AUDIO_WAVEFORMAT* format,
    AUDIOINPUT_WRITE pfnReadCallback,
    AUDIOCOMPLETE_CALLBACK pfnComplete,
    AUDIO_BUFFERUNDERRUN_CALLBACK pfnBufferUnderRun,
    void* pContext);
extern AUDIO_RESULT  audio_output_pause(AUDIO_SYS_HANDLE handle);
extern AUDIO_RESULT  audio_output_restart(AUDIO_SYS_HANDLE handle);
extern STRING_HANDLE audio_output_get_name(AUDIO_SYS_HANDLE handle);
extern AUDIO_RESULT  audio_output_stop(AUDIO_SYS_HANDLE handle);

extern AUDIO_RESULT  audio_playwavfile(AUDIO_SYS_HANDLE handle, const char* audioFile);

extern AUDIO_RESULT  audio_input_start(AUDIO_SYS_HANDLE handle);
extern AUDIO_RESULT  audio_input_stop(AUDIO_SYS_HANDLE handle);

extern AUDIO_RESULT  audio_output_set_volume(AUDIO_SYS_HANDLE handle, long volume);
extern AUDIO_RESULT  audio_output_get_volume(AUDIO_SYS_HANDLE handle, long* volume);

// retrieve the detailed name of the audio interface, if available.
extern STRING_HANDLE get_input_device_nice_name(AUDIO_SYS_HANDLE);

#ifdef __cplusplus
}

#endif /* __cplusplus */

#endif /* AUDIO_SYS_H */

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include "azure_c_shared_utility/audio_sys.h"
#include "azure_c_shared_utility/iot_logging.h"
#include <windows.h>

#define MAGIC_TAG_RIFF      0x46464952
#define MAGIC_TAG_WAVE      0x45564157
#define MAGIC_TAG_data      0x61746164
#define MAGIC_TAG_fmt       0x20746d66

#define AUDIO_CHANNELS_MONO     1
#define AUDIO_CHANNELS_STEREO   2
#define AUDIO_SAMPLE_RATE       44100//16000
#define AUDIO_BITS              16
//#define AUDIO_BLOCK_ALIGN       (AUDIO_CHANNELS_MONO * (AUDIO_BITS >> 3))
//#define AUDIO_BYTE_RATE         (AUDIO_SAMPLE_RATE * AUDIO_BLOCK_ALIGN)

typedef struct AUDIO_SYS_DATA_TAG
{
    ON_AUDIOOUTPUT_STATE_CALLBACK output_state_cb;
    ON_AUDIOINPUT_STATE_CALLBACK input_state_cb;
    void* user_outputctx;
    void* user_inputctx;
    //BUFFER_HANDLE                   mhBuffer;
    uint16_t channels;
    uint32_t sampleRate;
    uint16_t bitsPerSample;
    bool waveDataDirty;
} AUDIO_SYS_DATA;

static AUDIO_RESULT write_audio_stream(AUDIO_SYS_DATA* audioData, const unsigned char* data, size_t length, const AUDIOOUTPUT_WAVEFORMAT* pFormat)
{
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    return result;
}

int open_wave_data(AUDIO_SYS_DATA* audioData)
{
    int result = 0;
    return result;
}

AUDIO_SYS_HANDLE audio_create()
{
    AUDIO_SYS_DATA* result;

    result = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA) );
    if (result != NULL)
    {
        memset(result, 0, sizeof(AUDIO_SYS_DATA));
    }
    return result;
}

void audio_destroy(AUDIO_SYS_HANDLE handle)
{
    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
        free(audioData);
    }
}

AUDIO_RESULT audio_setcallbacks(AUDIO_SYS_HANDLE handle, ON_AUDIOOUTPUT_STATE_CALLBACK output_cb, void* output_ctx, ON_AUDIOINPUT_STATE_CALLBACK input_cb, void* input_ctx)
{
    AUDIO_RESULT result;
    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
        audioData->input_state_cb = input_cb;
        audioData->user_inputctx = input_ctx;
        audioData->output_state_cb = output_cb;
        audioData->user_outputctx = output_ctx;
        result = AUDIO_RESULT_OK;
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_output_start(AUDIO_SYS_HANDLE handle, const unsigned char* audioBuffer, size_t audioLen)
{
    AUDIO_RESULT result = AUDIO_RESULT_ERROR;
    if (handle != NULL)
    {
        const unsigned char* responseCtx = audioBuffer;
        if (audioData->waveDataDirty)
        {
            if (open_wave_data(audioData) != 0)
            {
                result = AUDIO_RESULT_ERROR;
            }
        }
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_output_pause(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result;
    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
        result = AUDIO_RESULT_OK;
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_input_start(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result;
    if (handle != NULL)
    {
        result = AUDIO_RESULT_OK;
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_input_pause(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result;
    if (handle != NULL)
    {
        result = AUDIO_RESULT_OK;
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_set_options(AUDIO_SYS_HANDLE handle, const char* optionName, const void* value)
{
    AUDIO_RESULT result;
    if (handle != NULL && optionName != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;

        if (strcmp("channels", optionName) == 0)
        {
            int channel = *((int*)value);
            if (channel == 1 || channel == 2)
            {
                audioData->channels = channel;
                audioData->waveDataDirty = true;
                result = AUDIO_RESULT_OK;
            }
            else
            {
                result = AUDIO_RESULT_INVALID_ARG;
            }
        }
        else
        {
            result = AUDIO_RESULT_INVALID_ARG;
        }
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT  audio_output_set_volume(AUDIO_SYS_HANDLE handle, long volume)
{
    (void)volume;
    return AUDIO_RESULT_ERROR;
}

AUDIO_RESULT  audio_output_get_volume(AUDIO_SYS_HANDLE handle, long* volume)
{
    (void)volume;
    return AUDIO_RESULT_ERROR;
}

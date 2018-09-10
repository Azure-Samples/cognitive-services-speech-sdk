//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_audio_stream_format.cpp: Public API definitions for audio stream format related C methods and types

#include "stdafx.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) audio_stream_format_is_handle_valid(SPXAUDIOSTREAMFORMATHANDLE hformat)
{
    return Handle_IsValid<SPXAUDIOSTREAMFORMATHANDLE, SPXWAVEFORMATEX>(hformat);
}

SPXAPI audio_stream_format_create_from_default_input(SPXAUDIOSTREAMFORMATHANDLE* hformat)
{
    return audio_stream_format_create_from_waveformat_pcm(hformat, 16000, 16, 1);
}

SPXAPI audio_stream_format_create_from_waveformat_pcm(SPXAUDIOSTREAMFORMATHANDLE* hformat, uint32_t samplesPerSecond, uint8_t bitsPerSample, uint8_t channels)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *hformat = SPXHANDLE_INVALID;

        auto format = SpxAllocWAVEFORMATEX(sizeof(SPXWAVEFORMATEX));
        format->wFormatTag = WAVE_FORMAT_PCM;
        format->nChannels = channels;
        format->nSamplesPerSec = samplesPerSecond;
        format->nAvgBytesPerSec = samplesPerSecond * (bitsPerSample / 8) * channels;
        format->nBlockAlign = (channels * bitsPerSample) / 8;
        format->wBitsPerSample = bitsPerSample;
        format->cbSize = 0;

        *hformat = CSpxSharedPtrHandleTableManager::TrackHandle<SPXWAVEFORMATEX, SPXAUDIOSTREAMFORMATHANDLE>(format);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_stream_format_release(SPXAUDIOSTREAMFORMATHANDLE hformat)
{
    return Handle_Close<SPXAUDIOSTREAMFORMATHANDLE, SPXWAVEFORMATEX>(hformat);
}

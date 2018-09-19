//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_audio_stream_format.h: Public API declarations for audio stream format related C methods and types
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) audio_stream_format_is_handle_valid(SPXAUDIOSTREAMFORMATHANDLE hformat);
SPXAPI audio_stream_format_create_from_default_input(SPXAUDIOSTREAMFORMATHANDLE* hformat);
SPXAPI audio_stream_format_create_from_waveformat_pcm(SPXAUDIOSTREAMFORMATHANDLE* hformat, uint32_t samplesPerSecond, uint8_t bitsPerSample, uint8_t channels);
SPXAPI audio_stream_format_release(SPXAUDIOSTREAMFORMATHANDLE hformat);

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_audio_stream.h: Public API declarations for audio stream related C methods and types
//

#pragma once
#include <speechapi_c_common.h>

// audio_stream
SPXAPI_(bool) audio_stream_is_handle_valid(SPXAUDIOSTREAMHANDLE haudioStream);
SPXAPI audio_stream_create_push_audio_input_stream(SPXAUDIOSTREAMHANDLE* haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat);
SPXAPI audio_stream_create_pull_audio_input_stream(SPXAUDIOSTREAMHANDLE* haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat);
SPXAPI audio_stream_release(SPXAUDIOSTREAMHANDLE haudioStream);

// pull_audio_input_stream
typedef int (*CUSTOM_AUDIO_PULL_STREAM_READ_CALLBACK)(void* pvContext, uint8_t* buffer, uint32_t size);
typedef void (*CUSTOM_AUDIO_PULL_STREAM_CLOSE_CALLBACK)(void* pvContext);
SPXAPI pull_audio_input_stream_set_callbacks(SPXAUDIOSTREAMHANDLE haudioStream, void* pvContext, CUSTOM_AUDIO_PULL_STREAM_READ_CALLBACK readCallback, CUSTOM_AUDIO_PULL_STREAM_CLOSE_CALLBACK closeCallback);

// push_audio_input_stream
SPXAPI push_audio_input_stream_write(SPXAUDIOSTREAMHANDLE haudioStream, uint8_t* buffer, uint32_t size);
SPXAPI push_audio_input_stream_close(SPXAUDIOSTREAMHANDLE haudioStream);

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_audio_stream_format.h: Public API declarations for audio stream format related C methods and types
//

#pragma once
#include <speechapi_c_common.h>

/// <summary>
/// Defines supported audio stream container format.
/// Changed in version 1.4.0.
/// </summary>
enum Audio_Stream_Container_Format
{
    /// <summary>
    /// Stream ContainerFormat definition for OGG OPUS.
    /// </summary>
    StreamFormat_Ogg_Opus = 0x101,

    /// <summary>
    /// Stream ContainerFormat definition for MP3.
    /// </summary>
    StreamFormat_Mp3 = 0x102,

    /// <summary>
    /// Stream ContainerFormat definition for FLAC. Not supported yet.
    /// </summary>
    StreamFormat_Flac = 0x103
};

typedef enum Audio_Stream_Container_Format Audio_Stream_Container_Format;

SPXAPI_(bool) audio_stream_format_is_handle_valid(SPXAUDIOSTREAMFORMATHANDLE hformat);
SPXAPI audio_stream_format_create_from_default_input(SPXAUDIOSTREAMFORMATHANDLE* hformat);
SPXAPI audio_stream_format_create_from_waveformat_pcm(SPXAUDIOSTREAMFORMATHANDLE* hformat, uint32_t samplesPerSecond, uint8_t bitsPerSample, uint8_t channels);
SPXAPI audio_stream_format_create_from_default_output(SPXAUDIOSTREAMFORMATHANDLE* hformat);
SPXAPI audio_stream_format_create_from_compressed_format(SPXAUDIOSTREAMFORMATHANDLE* hformat, Audio_Stream_Container_Format compressedFormat);
SPXAPI audio_stream_format_release(SPXAUDIOSTREAMFORMATHANDLE hformat);

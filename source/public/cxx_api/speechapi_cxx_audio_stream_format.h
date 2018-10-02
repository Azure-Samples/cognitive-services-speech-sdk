//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_audio_stream_format.h: Public API declarations for AudioStreamFormat and related C++ classes
//

#pragma once
#include <functional>
#include <memory>
#include <string>
#include <spxdebug.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_smart_handle.h>
#include <speechapi_c_audio_stream_format.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Audio {

/// <summary>
/// Class to represent the audio stream format used for custom audio input configurations.
/// </summary>
class AudioStreamFormat
{
public:

    /// <summary>
    /// Destructor, does nothing.
    /// </summary>
    virtual ~AudioStreamFormat() {}

    /// <summary>
    /// Internal operator used to get underlying handle value.
    /// </summary>
    explicit operator SPXAUDIOSTREAMFORMATHANDLE() const { return m_hformat.get(); }

    /// <summary>
    /// Creates an audio stream format object representing the default audio stream format (16Khz 16bit mono PCM).
    /// </summary>
    /// <returns>A shared pointer to AudioStreamFormat</returns>
    static std::shared_ptr<AudioStreamFormat> GetDefaultInputFormat()
    {
        SPXAUDIOSTREAMFORMATHANDLE hformat = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_stream_format_create_from_default_input(&hformat));

        auto format = new AudioStreamFormat(hformat);
        return std::shared_ptr<AudioStreamFormat>(format);
    }

    /// <summary>
    /// Creates an audio stream format object with the specified PCM waveformat characteristics.
    /// </summary>
    /// <remarks>
    /// Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
    /// </remarks>
    /// <param name="samplesPerSecond">Samples per second.</param>
    /// <param name="bitsPerSample">Bits per second.</param>
    /// <param name="channels">1.</param>
    /// <returns>A shared pointer to AudioStreamFormat</returns>
    static std::shared_ptr<AudioStreamFormat> GetWaveFormatPCM(uint32_t samplesPerSecond, uint8_t bitsPerSample = 16, uint8_t channels = 1)
    {
        SPXAUDIOSTREAMFORMATHANDLE hformat = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_stream_format_create_from_waveformat_pcm(&hformat, samplesPerSecond, bitsPerSample, channels));

        auto format = new AudioStreamFormat(hformat);
        return std::shared_ptr<AudioStreamFormat>(format);
    }


protected:

    /*! \cond PROTECTED */

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit AudioStreamFormat(SPXAUDIOSTREAMFORMATHANDLE hformat) : m_hformat(hformat) { }

    /*! \endcond */

private:

    DISABLE_COPY_AND_MOVE(AudioStreamFormat);

    /// <summary>
    /// Internal member variable that holds the smart handle.
    /// </summary>
    SmartHandle<SPXAUDIOSTREAMFORMATHANDLE, &audio_stream_format_release> m_hformat;
};


} } } } // Microsoft::CognitiveServices::Speech::Audio

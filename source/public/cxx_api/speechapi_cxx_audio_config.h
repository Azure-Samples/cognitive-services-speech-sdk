//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_audio_config.h: Public API declarations for AudioConfig C++ class
//

#pragma once
#include <functional>
#include <memory>
#include <string>
#include <spxdebug.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_smart_handle.h>
#include <speechapi_cxx_audio_stream_format.h>
#include <speechapi_cxx_audio_stream.h>
#include <speechapi_c_audio_config.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Audio {


/// <summary>
/// Represents specific audio configuration, such as microphone, file, or custom audio streams
/// </summary>
class AudioConfig
{
public:

    /// <summary>
    /// Internal operator used to get underlying handle value.
    /// </summary>
    explicit operator SPXAUDIOCONFIGHANDLE() const { return m_haudioConfig.get(); }

    /// <summary>
    /// Creates an AudioConfig object representing the default microphone on the system.
    /// </summary>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromDefaultMicrophoneInput()
    {
        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_input_from_default_microphone(&haudioConfig));

        auto config = new AudioConfig(haudioConfig);
        return std::shared_ptr<AudioConfig>(config);
    }

    /// <summary>
    /// Creates an AudioConfig object representing the specified file.
    /// </summary>
    /// <param name="fileName">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromWavFileInput(const SPXSTRING& fileName)
    {
        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_input_from_wav_file_name(&haudioConfig, Utils::ToUTF8(fileName).c_str()));

        auto config = new AudioConfig(haudioConfig);
        return std::shared_ptr<AudioConfig>(config);
    }

    /// <summary>
    /// Creates an AudioConfig object representing the specified stream.
    /// </summary>
    /// <param name="stream">Specifies the custom audio input stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromStreamInput(std::shared_ptr<AudioInputStream> stream)
    {
        SPX_IFTRUE_THROW_HR(stream == nullptr, SPXERR_INVALID_ARG);

        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_input_from_stream(&haudioConfig, GetStreamHandle(stream)));

        auto config = new AudioConfig(haudioConfig);
        return std::shared_ptr<AudioConfig>(config);
    }

protected:

    /*! \cond PROTECTED */

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit AudioConfig(SPXAUDIOCONFIGHANDLE haudioConfig) : m_haudioConfig(haudioConfig) { }

    /// <summary>
    /// Internal helper method to get the audio stream format handle.
    /// </summary>
    static SPXAUDIOSTREAMHANDLE GetStreamHandle(std::shared_ptr<AudioInputStream> stream) { return (SPXAUDIOSTREAMHANDLE)(*stream.get()); }

    /*! \endcond */

private:

    DISABLE_COPY_AND_MOVE(AudioConfig);

    /// <summary>
    /// Internal member variable that holds the smart handle.
    /// </summary>
    SmartHandle<SPXAUDIOCONFIGHANDLE, &audio_config_release> m_haudioConfig;
    std::shared_ptr<AudioInputStream> m_stream;
};


} } } } // Microsoft::CognitiveServices::Speech::Audio

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
    /// <returns>A handle.</returns>
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
    /// Creates an AudioConfig object representing a specific microphone on the system.
    /// Added in version 1.3.0.
    /// </summary>
    /// <param name="deviceName">Specifies the device name. Please refer to <a href="https://aka.ms/csspeech/microphone-selection">this page</a> on how to retrieve platform-specific microphone names.</param>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromMicrophoneInput(const SPXSTRING& deviceName)
    {
        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_input_from_a_microphone(&haudioConfig, Utils::ToUTF8(deviceName).c_str()));

        auto config = new AudioConfig(haudioConfig);
        return std::shared_ptr<AudioConfig>(config);
    }

    /// <summary>
    /// Creates an AudioConfig object representing the specified file.
    /// </summary>
    /// <param name="fileName">Specifies the audio input file.</param>
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
    /// <param name="stream">Specifies the custom audio input stream.</param>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromStreamInput(std::shared_ptr<AudioInputStream> stream)
    {
        SPX_IFTRUE_THROW_HR(stream == nullptr, SPXERR_INVALID_ARG);

        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_input_from_stream(&haudioConfig, GetStreamHandle(stream)));

        auto config = new AudioConfig(haudioConfig);
        return std::shared_ptr<AudioConfig>(config);
    }

    /// <summary>
    /// Creates an AudioConfig object representing the default audio output device (speaker) on the system.
    /// Added in version 1.4.0
    /// </summary>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromDefaultSpeakerOutput()
    {
        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_output_from_default_speaker(&haudioConfig));

        auto config = new AudioConfig(haudioConfig);
        return std::shared_ptr<AudioConfig>(config);
    }

    /// <summary>
    /// Creates an AudioConfig object representing the specified file for audio output.
    /// Added in version 1.4.0
    /// </summary>
    /// <param name="fileName">Specifies the audio output file. The parent directory must already exist.</param>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromWavFileOutput(const SPXSTRING& fileName)
    {
        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_output_from_wav_file_name(&haudioConfig, Utils::ToUTF8(fileName).c_str()));

        auto config = new AudioConfig(haudioConfig);
        return std::shared_ptr<AudioConfig>(config);
    }

    /// <summary>
    /// Creates an AudioConfig object representing the specified output stream.
    /// Added in version 1.4.0
    /// </summary>
    /// <param name="stream">Specifies the custom audio output stream.</param>
    /// <returns>A shared pointer to the AudioConfig object</returns>
    static std::shared_ptr<AudioConfig> FromStreamOutput(std::shared_ptr<AudioOutputStream> stream)
    {
        SPX_IFTRUE_THROW_HR(stream == nullptr, SPXERR_INVALID_ARG);

        SPXAUDIOCONFIGHANDLE haudioConfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(audio_config_create_audio_output_from_stream(&haudioConfig, GetOutputStreamHandle(stream)));

        auto config = new AudioConfig(haudioConfig);
        config->m_outputStream = stream;
        return std::shared_ptr<AudioConfig>(config);
    }

    /// Sets a property value by name.
    /// </summary>
    /// <param name="name">The property name.</param>
    /// <param name="value">The property value.</param>
    void SetProperty(const SPXSTRING& name, const SPXSTRING& value)
    {
        property_bag_set_string(m_propertybag, -1, Utils::ToUTF8(name).c_str(), Utils::ToUTF8(value).c_str());
    }

    /// <summary>
    /// Sets a property value by ID.
    /// </summary>
    /// <param name="id">The property id.</param>
    /// <param name="value">The property value.</param>
    void SetProperty(PropertyId id, const SPXSTRING& value)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(id), nullptr, Utils::ToUTF8(value).c_str());
    }

    /// <summary>
    /// Gets a property value by name.
    /// </summary>
    /// <param name="name">The parameter name.</param>
    /// <returns>The property value.</returns>
    SPXSTRING GetProperty(const SPXSTRING& name) const
    {
        const char* value = property_bag_get_string(m_propertybag, -1, Utils::ToUTF8(name).c_str(), "");
        return Utils::ToSPXString(Utils::CopyAndFreePropertyString(value));
    }

    /// <summary>
    /// Gets a property value by ID.
    /// </summary>
    /// <param name="id">The parameter id.</param>
    /// <returns>The property value.</returns>
    SPXSTRING GetProperty(PropertyId id) const
    {
        const char* value = property_bag_get_string(m_propertybag, static_cast<int>(id), nullptr, "");
        return Utils::ToSPXString(Utils::CopyAndFreePropertyString(value));
    }

    /// <summary>
    /// Destructs the object.
    /// </summary>
    virtual ~AudioConfig()
    {
        property_bag_release(m_propertybag);
    }

protected:

    /*! \cond PROTECTED */

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit AudioConfig(SPXAUDIOCONFIGHANDLE haudioConfig)
        : m_haudioConfig(haudioConfig)
    {
        SPX_THROW_ON_FAIL(audio_config_get_property_bag(m_haudioConfig, &m_propertybag));
    }

    /// <summary>
    /// Internal helper method to get the audio stream format handle.
    /// </summary>
    static SPXAUDIOSTREAMHANDLE GetStreamHandle(std::shared_ptr<AudioInputStream> stream) { return (SPXAUDIOSTREAMHANDLE)(*stream.get()); }

    /// <summary>
    /// Internal helper method to get the audio output stream format handle.
    /// </summary>
    static SPXAUDIOSTREAMHANDLE GetOutputStreamHandle(std::shared_ptr<AudioOutputStream> stream) { return (SPXAUDIOSTREAMHANDLE)(*stream.get()); }

    /*! \endcond */

private:

    DISABLE_COPY_AND_MOVE(AudioConfig);

    /// <summary>
    /// Internal member variable that holds the smart handle.
    /// </summary>
    SmartHandle<SPXAUDIOCONFIGHANDLE, &audio_config_release> m_haudioConfig;

    /// <summary>
    /// Internal member variable that holds the properties of the audio config
    /// </summary>
    SPXPROPERTYBAGHANDLE m_propertybag;

    std::shared_ptr<AudioInputStream> m_stream;
    std::shared_ptr<AudioOutputStream> m_outputStream;
};


} } } } // Microsoft::CognitiveServices::Speech::Audio

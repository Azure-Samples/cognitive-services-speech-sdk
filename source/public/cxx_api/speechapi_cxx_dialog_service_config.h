//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
#pragma once

#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_speech_config.h>
#include <speechapi_cxx_enums.h>
#include <speechapi_c_property_bag.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Dialog {

/// <summary>
/// Class that defines configurations for the dialog service connector object.
/// </summary>
class DialogServiceConfig final: public SpeechConfig
{
    /*! \cond PROTECTED */
    inline explicit DialogServiceConfig(SPXSPEECHCONFIGHANDLE h_config) : SpeechConfig{ h_config }
    {
    }
    /*! \endcond */
public:
    /// <summary>
    /// Creates a dialog service config instance with the specified bot secret.
    /// </summary>
    /// <param name="secretKey">Speech channel bot secret key.</param>
    /// <param name="subscription">Subscription key associated with the bot</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    /// <returns>A shared pointer to the new dialog service config.</returns>
    inline static std::shared_ptr<DialogServiceConfig> FromBotSecret(const SPXSTRING& secretKey, const SPXSTRING& subscription, const SPXSTRING& region)
    {
        SPXSPEECHCONFIGHANDLE h_config = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(dialog_service_config_from_bot_secret(&h_config, Utils::ToUTF8(secretKey).c_str(), Utils::ToUTF8(subscription).c_str(), Utils::ToUTF8(region).c_str()));
        return std::shared_ptr<DialogServiceConfig>{ new DialogServiceConfig(h_config) };
    }

    /// <summary>
    /// Creates a dialog service config instance with the specified Speech Commands application id.
    /// </summary>
    /// <param name="appId">Speech Commands application id.</param>
    /// <param name="subscription">Subscription key associated with the dialog.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    /// <returns>A shared pointer to the new dialog service config.</returns>
    inline static std::shared_ptr<DialogServiceConfig> FromSpeechCommandsAppId(const SPXSTRING& appId, const SPXSTRING& subscription, const SPXSTRING& region)
    {
        SPXSPEECHCONFIGHANDLE h_config = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(dialog_service_config_from_speech_commands_app_id(&h_config, Utils::ToUTF8(appId).c_str(), Utils::ToUTF8(subscription).c_str(), Utils::ToUTF8(region).c_str()));
        return std::shared_ptr<DialogServiceConfig>{ new DialogServiceConfig(h_config) };
    }

    /// <summary>
    /// Sets the corresponding backend application identifier.
    /// </summary>
    /// <param name="applicationId">Application identifier.</param>
    inline void SetApplicationId(const SPXSTRING& applicationId)
    {
        SetProperty(PropertyId::Conversation_ApplicationId, applicationId);
    }

    /// <summary>
    /// Gets the application identifier.
    /// </summary>
    /// <returns>Speech Channel Secret Key.</returns>
    inline SPXSTRING GetApplicationId() const
    {
        return GetProperty(PropertyId::Conversation_ApplicationId);
    }

    /// <summary>
    /// Sets the connection initial silence timeout.
    /// </summary>
    /// <param name="connectionInitialSilenceTimeout">Connection initial silence timeout.</param>
    inline void SetInitialSilenceTimeout(const SPXSTRING& connectionInitialSilenceTimeout)
    {
        SetProperty(PropertyId::Conversation_Initial_Silence_Timeout, connectionInitialSilenceTimeout);
    }

    /// <summary>
    /// Gets the connection initial silence timeout.
    /// </summary>
    /// <returns>Initial silence timeout.</returns>
    inline SPXSTRING GetInitialSilenceTimeout() const
    {
        return GetProperty(PropertyId::Conversation_Initial_Silence_Timeout);
    }

    /// <summary>
    /// Sets the audio format that is returned by text to speech during a reply from the backing dialog.
    /// </summary>
    /// <param name="audioFormat">The audio format to return.</param>
    inline void SetTextToSpeechAudioFormat(const SPXSTRING& audioFormat)
    {
        SetProperty(PropertyId::SpeechServiceConnection_SynthOutputFormat, audioFormat);
    }

    /// <summary>
    /// Gets the audio format that is returned by text to speech during a reply from the backing dialog.
    /// </summary>
    /// <returns>The audio format.</returns>
    inline SPXSTRING GetTextToSpeechAudioFormat() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_SynthOutputFormat);
    }

};
} } } }

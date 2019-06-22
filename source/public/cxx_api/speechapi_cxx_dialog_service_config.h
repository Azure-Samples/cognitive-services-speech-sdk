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
    /// Creates a dialog service config instance with the specified task dialog application id.
    /// </summary>
    /// <param name="appId">Task dialog application id.</param>
    /// <param name="subscription">Subscription key associated with the dialog.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    /// <returns>A shared pointer to the new dialog service config.</returns>
    inline static std::shared_ptr<DialogServiceConfig> FromTaskDialogAppId(const SPXSTRING& appId, const SPXSTRING& subscription, const SPXSTRING& region)
    {
        SPXSPEECHCONFIGHANDLE h_config = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(dialog_service_config_from_task_dialog_app_id(&h_config, Utils::ToUTF8(appId).c_str(), Utils::ToUTF8(subscription).c_str(), Utils::ToUTF8(region).c_str()));
        return std::shared_ptr<DialogServiceConfig>{ new DialogServiceConfig(h_config) };
    }

    /// <summary>
    /// Sets the Speech Channel Secret Key.
    /// </summary>
    /// <param name="secretKey">Speech Channel Secret Key.</param>
    inline void SetSecretKey(const SPXSTRING& secretKey)
    {
        SetProperty(PropertyId::Conversation_Secret_Key, secretKey);
    }

    /// <summary>
    /// Gets the Speech Channel Secret Key.
    /// </summary>
    /// <returns>Speech Channel Secret Key.</returns>
    inline SPXSTRING GetSecretKey() const
    {
        return GetProperty(PropertyId::Conversation_Secret_Key);
    }

    /// <summary>
    /// Sets the Task Dialog Application Id.
    /// </summary>
    /// <param name="appId">Task Dialog Application Id.</param>
    inline void SetTaskDialogAppId(const SPXSTRING& appId)
    {
        SetProperty(PropertyId::Conversation_TaskDialogAppId, appId);
    }

    /// <summary>
    /// Gets the Task Dialog Application Id.
    /// </summary>
    /// <returns>Task Dialog Application Id.</returns>
    inline SPXSTRING GetTaskDialogAppId() const
    {
        return GetProperty(PropertyId::Conversation_TaskDialogAppId);
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

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
/// Class that defines configurations for speech bot connector
/// </summary>
class BotConnectorConfig final: public SpeechConfig
{
    /*! \cond PROTECTED */
    inline explicit BotConnectorConfig(SPXSPEECHCONFIGHANDLE h_config) : SpeechConfig{ h_config }
    {
    }
    /*! \endcond */
public:
    /// <summary>
    /// Creates and instance of the bot connector config with the specified bot id.
    /// </summary>
    /// <param name="secretKey">Speech channel secret key.</param>
    /// <param name="subscription">Subscription key associated with the bot</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    /// <returns>A shared pointer to the new bot connector config.</returns>
    inline static std::shared_ptr<BotConnectorConfig> FromSecretKey(const SPXSTRING& secretKey, const SPXSTRING& subscription, const SPXSTRING& region)
    {
        SPXSPEECHCONFIGHANDLE h_config = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(bot_connector_config_from_secret_key(&h_config, Utils::ToUTF8(secretKey).c_str(), Utils::ToUTF8(subscription).c_str(), Utils::ToUTF8(region).c_str()));
        return std::shared_ptr<BotConnectorConfig>{ new BotConnectorConfig(h_config) };
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
    /// Sets the Bot Connection initial silence timeout.
    /// </summary>
    /// <param name="botConnectionInitialSilenceTimeout">Bot Connection initial silence timeout.</param>
    inline void SetBotInitialSilenceTimeout(const SPXSTRING& botConnectionInitialSilenceTimeout)
    {
        SetProperty(PropertyId::Conversation_Initial_Silence_Timeout, botConnectionInitialSilenceTimeout);
    }

    /// <summary>
    /// Gets the Bot initial silence timeout.
    /// </summary>
    /// <returns>Bot initial silence timeout.</returns>
    inline SPXSTRING GetBotInitialSilenceTimeout() const
    {
        return GetProperty(PropertyId::Conversation_Initial_Silence_Timeout);
    }

    /// <summary>
    /// Sets the audio format that is returned by text to speech during a bot reply.
    /// </summary>
    /// <param name="audioFormat">The audio format to return.</param>
    inline void SetTextToSpeechAudioFormat(const SPXSTRING& audioFormat)
    {
        SetProperty(PropertyId::SpeechServiceConnection_SynthOutputFormat, audioFormat);
    }

    /// <summary>
    /// Gets the audio format that is returned by text to speech during a bot reply.
    /// </summary>
    /// <returns>The audio format.</returns>
    inline SPXSTRING GetTextToSpeechAudioFormat() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_SynthOutputFormat);
    }

};
} } } }

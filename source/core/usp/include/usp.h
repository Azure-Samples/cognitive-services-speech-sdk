//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp.h: the header file includes types, methods and error code exposed by usplib
//

#pragma once

#include <memory>
#include <functional>
#include <string>

#include "uspmessages.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

/**
* The Callbacks type represents an application-defined structure used to register callbacks for USP events.
* The callbacks are invoked during the processing of the request, an application should spend as little time as possible
* in the callback function.
*/
struct Callbacks
{
    /**
     * A callback function that will be invoked when a speech.startDetected message is received from service.
    */
    virtual void OnSpeechStartDetected(const SpeechStartDetectedMsg&) {}

    /**
    * A callback function that will be invoked when a speech.endDetected message is received from service.
    */
    virtual void OnSpeechEndDetected(const SpeechEndDetectedMsg&) {}

    /**
    * A callback function that will be invoked when a speech.hypothesis message is received from service.
    */
    virtual void OnSpeechHypothesis(const SpeechHypothesisMsg&) {}

    /**
    * A callback function that will be invoked when a speech.phrase message is received from service.
    */
    virtual void OnSpeechPhrase(const SpeechPhraseMsg&) {}

    /**
    * A callback function that will be invoked when a speech.fragment message is received from service.
    */
    virtual void OnSpeechFragment(const SpeechFragmentMsg&) {}

    /**
    * A callback function that will be invoked when a turn.start message is received from service.
    */
    virtual void OnTurnStart(const TurnStartMsg&) {}

    /**
    * A callback function that will be invoked when a turn.end message is received from service.
    */
    virtual void OnTurnEnd(const TurnEndMsg&) {}

    /**
    * A callback function that will be invoked when an error occurs in handling communication with service.
    */
    virtual void OnError(const std::string& /*error*/) {}

    /**
    * A callback function that will be invoked when the first chunk in an audio stream is received from the service.
    */
    virtual void OnAudioStreamStart(const AudioStreamStartMsg&) {}

    /**
    * A callback function that will be invoked when a translation.hypothesis message is received from service.
    */
    virtual void OnTranslationHypothesis(const TranslationHypothesisMsg&) {}

    /**
    * A callback function that will be invoked when a translation.phrase message is received from service.
    */
    virtual void OnTranslationPhrase(const TranslationPhraseMsg&) {}

    /**
    * A callback function that will be invoked when a translation.synthesis message is received from service.
    */
    virtual void OnTranslationSynthesis(const TranslationSynthesisMsg&) {}

    /**
    * A callback function that will be invoked when a translation.synthesis.end message is received from service.
    */
    virtual void OnTranslationSynthesisEnd(const TranslationSynthesisEndMsg&) {}

    /**
    * A callback function that will be invoked when a message having a path defined by user is received from service.
    */
    virtual void OnUserMessage(const UserMsg&) {}
};

using CallbacksPtr = std::shared_ptr<Callbacks>;

enum class EndpointType { Speech, Intent, Translation, CDSDK };

enum class RecognitionMode : unsigned int { Interactive = 0, Conversation = 1, Dictation = 2 };

enum class OutputFormat : unsigned int { Simple = 0, Detailed = 1 };

enum class AuthenticationType { SubscriptionKey, AuthorizationToken, SearchDelegationRPSToken };

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

class Client;

class Connection
{
public:
    /**
    * Sends data to the USP client.
    * @param buffer The buffer contains data to be sent.
    * @param size The amount of data in bytes to be sent.
    */
    void WriteAudio(const uint8_t* buffer, size_t size);

    /**
    * Finalizes sending audio data to indicate the end of audio.
    * @return A UspResult indicating success or error.
    */
    void FlushAudio();

    /**
    * Sends a user defined message.
    * @param messagePath The path of the user-defined message.
    * @param buffer The message payload.
    * @param size The length of the message in bytes.
    */
    void SendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size);

    /**
    * Closes the USP connection.
    */
    ~Connection();

private:

    friend class Client;

    Connection(const Client&);

    class Impl;

    std::shared_ptr<Impl> m_impl;
};


using ConnectionPtr = std::unique_ptr<Connection>;

class Client
{
    static constexpr auto s_defaultLanguage = "en-us";

public:
    /**
    * Creates a USP client.
    * @param callbacks The struct defines callback functions that will be invoked when various USP events occur.
    * @param type  The speech service to be used, Speech, Intent, Translation, and etc.
    */
    Client(CallbacksPtr callbacks, EndpointType endpoint):
        m_callbacks(callbacks),
        m_endpoint(endpoint),
        m_recoMode(RecognitionMode::Interactive),
        m_outputFormat(OutputFormat::Simple),
        m_language(s_defaultLanguage),
        m_authType(AuthenticationType::SubscriptionKey)
    {
    }

    /**
    * Sets the region of the service endpoint.
    */
    Client& SetRegion(const std::string& region)
    {
        m_region = region;
        return *this;
    }

    /**
    * Sets the URL of the service endpoint. It should contain the host name, resoure path and all query parameters needed.
    */
    Client& SetEndpointUrl(const std::string& endpointUrl) 
    {
        m_customEndpointUrl = endpointUrl;
        return *this;
    }

    /**
    * Sets the speech service type.
    */
    Client& SetEndpointType(EndpointType type) 
    {
        m_endpoint = type;
        return *this;
    }

    /**
    * Sets the recognition mode, e.g. Interactive, Conversation, Dictation.
    */
    Client& SetRecognitionMode(RecognitionMode mode) 
    {
        m_recoMode = mode;
        return *this;
    }

    /**
    * Sets authentication parameters.
    * @param authType The type of authentication to be used.
    * @param authData The authentication data for the specified authentication type.
    */
    Client& SetAuthentication(AuthenticationType authType, const std::string& authData)
    {
        m_authType = authType;
        m_authData = authData;
        return *this;
    }

    /**
    * Sets the source audio language, which must be one of the supported languages specified using 
    * an IETF language tag BCP 47 (https://en.wikipedia.org/wiki/IETF_language_tag).
    */
    Client& SetLanguage(const std::string& language) 
    {
        m_language = language;
        return *this;
    }

    /**
    * Sets the output format, can be either Simple or Detailed.
    */
    Client& SetOutputFormat(OutputFormat format) 
    {
        m_outputFormat = format;
        return *this;
    }

    /**
    * Sets the custom speech model id.
    */
    Client& SetModelId(const std::string& modelId) 
    {
        m_modelId = modelId;
        return *this;
    }

    /**
    * Sets the source language of translation.
    */
    Client& SetTranslationSourceLanguage(const std::string& lang)
    {
        m_translationSourceLanguage = lang;
        return *this;
    }

    /**
    * Sets the target languages of translation.
    */
    Client& SetTranslationTargetLanguages(const std::string& langs)
    {
        m_translationTargetLanguages = langs;
        return *this;
    }

    /**
    * Sets the voice that is desired in translation.
    */
    Client& SetTranslationVoice(const std::string& voice)
    {
        m_translationVoice = voice;
        return *this;
    }

    /**
    * Sets the language understanding region.
    */
   Client& SetIntentRegion(const std::string& region)
   {
       m_intentRegion = region;
       return *this;
   }

    /**
    * Establishes connection to the service.
    */
    ConnectionPtr Connect();

    // TODO: add other getters if/as needed.
    const EndpointType& GetEndpointType()
    {
        return m_endpoint;
    }

    Client(const Client&) = default;
    ~Client() = default;

private:

    friend class Connection::Impl;

    CallbacksPtr m_callbacks;

    EndpointType m_endpoint;
    RecognitionMode m_recoMode;
    std::string m_customEndpointUrl;
    std::string m_region;

    OutputFormat m_outputFormat;
    std::string m_language;
    std::string m_modelId;

    std::string m_translationSourceLanguage;
    std::string m_translationTargetLanguages;
    std::string m_translationVoice;

    std::string m_intentRegion;

    AuthenticationType m_authType;
    std::string m_authData;

};

}}}}

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
#include "ispxinterfaces.h"
#include "audio_chunk.h"
#include "exception.h"

struct ProxyServerInfo;

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
    * transport - indicates if the error was origniated on transport, not USP layer.
    */
    virtual void OnError(bool /*transport*/, ErrorCode /*errorCode*/, const std::string& /*errorMessage*/) {}

    /**
    * A callback function that will be invoked when a translation.hypothesis message is received from service.
    */
    virtual void OnTranslationHypothesis(const TranslationHypothesisMsg&) {}

    /**
    * A callback function that will be invoked when a translation.phrase message is received from service.
    */
    virtual void OnTranslationPhrase(const TranslationPhraseMsg&) {}

    /**
    * A callback function that will be invoked when an audio output chunk message is received from service.
    */
    virtual void OnAudioOutputChunk(const AudioOutputChunkMsg&) {}

    /**
    * A callback function that will be invoked when a message having a path defined by user is received from service.
    */
    virtual void OnUserMessage(const UserMsg&) {}

    /**
     * A callback function that will be invoked when the connection to service is established.
    */
    virtual void OnConnected() {}

    /**
    * A callback function that will be invoked when the connection to service is lost.
    */
    virtual void OnDisconnected() {}
};

using CallbacksPtr = std::shared_ptr<Callbacks>;

enum class EndpointType { Speech, Intent, Translation, CDSDK, Bot };

enum class RecognitionMode : unsigned int { Interactive = 0, Conversation = 1, Dictation = 2 };

enum class OutputFormat : unsigned int { Simple = 0, Detailed = 1 };

enum class AuthenticationType { SubscriptionKey, AuthorizationToken, SearchDelegationRPSToken, SIZE_AUTHENTICATION_TYPE };

enum class MessageType { Config, Context, Agent };

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

class Client;

class Connection
{
public:
    /**
    * Sends data to the USP client.
    * @param audioChunk The audio chunk to be sent.
    */
    void WriteAudio(const Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk);

    /**
    * Finalizes sending audio data to indicate the end of audio.
    */
    void FlushAudio();

    /**
    * Sends a user defined message.
    * @param messagePath The path of the user-defined message.
    * @param buffer The message payload.
    * @param size The length of the message in bytes.
    * @param messageType The type of message to be sent. The USP currently has three different types of messages:
    *  1. Config:  Config messages are not associated with a specific request. Some endpoints require that a config message is
    *              sent as the first message after opening the connection.
    *  2. Context: Client applications can send a context message at any time before sending the first audio chunk for a request.
    *              You must send at most one context message for each turn.
    *  3. Agent:   Agent messages are meant for communicating with a back end agent.
    *              Each message represents the start of a new request.
    */
    void SendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size, MessageType messageType);

    /**
    * Writes the latency value into telemetry data.
    * @param latencyInTicks The latency value.
    * @param isPhraseLatency If it is true, the latency is for phrase result. Otherwise it is for hypothesis result.
    */
    void WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency);

    /**
     * Returns true if the status is connected.
     */
    bool IsConnected();

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
    * @param endpoint The speech service to be used, Speech, Intent, Translation, and etc.
    * @param connectionId Connection id, that will be passed to the service in the X-ConnectionId header and can be used for diagnostics.
    * @param threadService Thread service.
    */
    Client(CallbacksPtr callbacks, EndpointType endpoint, const std::wstring& connectionId, const std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService>& threadService):
        m_callbacks(callbacks),
        m_endpoint(endpoint),
        m_recoMode(RecognitionMode::Interactive),
        m_outputFormat(OutputFormat::Simple),
        m_language(s_defaultLanguage),
        m_authData((size_t)AuthenticationType::SIZE_AUTHENTICATION_TYPE,""),
        m_connectionId(connectionId),
        m_threadService(threadService)
    {
    }

    /**
    * Sets the audio response format that will be passed to the service in the X-Output-AudioCodec header.
    * More info can be found here: https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/rest-apis
    */
    Client& SetAudioResponseFormat(const std::string& format)
    {
        m_audioResponseFormat = format;
        return *this;
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
    * Sets the URL of the service endpoint. It should contain the host name, resource path and all query parameters needed.
    */
    Client& SetEndpointUrl(const std::string& endpointUrl)
    {
        m_customEndpointUrl = endpointUrl;
        return *this;
    }

    /**
    * Sets the proxy server information, which is used to configure the connection to go through a proxy server.
    */
    Client& SetProxyServerInfo(const char* proxyHost, int proxyPort, const char* proxyUsername = nullptr, const char* proxyPassword = nullptr);

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
    Client& SetAuthentication(const std::vector<std::string>& authData)
    {
        if (authData.size() != (size_t)AuthenticationType::SIZE_AUTHENTICATION_TYPE)
        {
           ::Microsoft::CognitiveServices::Speech::Impl::ThrowLogicError("Incorrect authentication configuration: array size mismatch.");
        }

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

     /**
     * Get the endpoint type.
     */
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

     std::shared_ptr<ProxyServerInfo> m_proxyServerInfo;

     OutputFormat m_outputFormat;
     std::string m_language;
     std::string m_modelId;

     std::string m_translationSourceLanguage;
     std::string m_translationTargetLanguages;
     std::string m_translationVoice;

     std::string m_intentRegion;

     std::vector<std::string> m_authData;

     std::wstring m_connectionId;

     std::string m_audioResponseFormat;

     std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> m_threadService;
};

}}}}

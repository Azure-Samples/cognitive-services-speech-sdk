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
#include <unordered_map>
#include <array>

#include <platform_init.h>
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
     * A callback function that will be invoked for all USP messages received from the service.
    */
    virtual void OnMessageReceived(const RawMsg&) {}

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
    * A callback function that will be invoked when a speech.keyword message is received from service.
    */
    virtual void OnSpeechKeywordDetected(const SpeechKeywordDetectedMsg&) {}

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
    * A callback function that will be invoked when a turn.start unrelated to speech is received from service.
    */
    virtual void OnMessageStart(const TurnStartMsg&) {}

    /**
    * A callback function that will be invoked when a turn.end message unrelated to speech is received from service.
    */
    virtual void OnMessageEnd(const TurnEndMsg&) {}

    /**
    * A callback function that will be invoked when an error occurs in handling communication with service.
    * transport - indicates if the error was originated on transport, not USP layer.
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
    * A callback function that will be invoked when an audio output metadata message is received from service.
    */
    virtual void OnAudioOutputMetadata(const AudioOutputMetadataMsg&) {}

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

enum class EndpointType { Speech, Intent, Translation, Dialog, ConversationTranscriptionService, SpeechSynthesis };

enum class RecognitionMode : unsigned int { Interactive = 0, Conversation = 1, Dictation = 2 };

enum class OutputFormat : unsigned int { Simple = 0, Detailed = 1 };

enum class AuthenticationType: size_t { SubscriptionKey = 0, AuthorizationToken, SearchDelegationRPSToken, DialogApplicationId, ConversationToken, SIZE_AUTHENTICATION_TYPE };

enum class MessageType { Config, Context, Agent, AgentContext, SpeechEvent, Event, Ssml, Unknown};

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
    * @param requestId The request ID for this turn.
    */
    void SendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size, MessageType messageType, const std::string& requestId="", bool binary=false);

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
    * Returns the URL used for connection.
    */
    std::string GetConnectionUrl();

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
public:
    /**
     * Creates a USP client.
     * @param callbacks The struct defines callback functions that will be invoked when various USP events occur.
     * @param endpointType The speech service to be used, Speech, Intent, Translation, and etc.
     * @param connectionId Connection id, that will be passed to the service in the X-ConnectionId header and can be used for diagnostics.
     * @param threadService Thread service.
     */
    Client(CallbacksPtr callbacks, EndpointType endpointType, const std::wstring& connectionId, const std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService>& threadService):
        m_callbacks(callbacks),
        m_endpointType(endpointType),
        m_recoMode(RecognitionMode::Interactive),
        m_authData{},
        m_connectionId(connectionId),
        m_threadService(threadService)
    {
    }

    /**
     * Sets the audio response format that will be passed to the service in the X-Output-AudioCodec header.
     * More info can be found here: https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-apis
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
     * Sets the language understanding region.
     */
    Client& SetIntentRegion(const std::string& region)
    {
        m_intentRegion = region;
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
     * Sets the URL of the service host. It should contain "protocol://host:port" where ":port" is optional.
     */
    Client& SetHostUrl(const std::string& hostUrl)
    {
        m_customHostUrl = hostUrl;
        return *this;
    }

    /**
     * Sets the query parameters provided by users.
     */
    Client& SetUserDefinedQueryParameters(const std::string& queryParameters)
    {
        m_userDefinedQueryParameters = queryParameters;
        return *this;
    }

    /**
    * Sets the proxy server information, which is used to configure the connection to go through a proxy server.
    */
    Client& SetProxyServerInfo(const char* proxyHost, int proxyPort, const char* proxyUsername, const char* proxyPassword);

    /**
     * When using OpenSSL only: sets a single trusted cert, optionally w/o CRL checks.
     * This is meant to be used in a firewall setting with potential lack of
     * CRLs (particularly on the leaf).
     * @param trustedCert the certificate to trust (PEM format)
     * @param disable_crl_check whether to also disable CRL checks
     * @return Client reference
     */
    Client& SetSingleTrustedCert(const std::string& trustedCert, bool disable_crl_check)
    {
        m_trustedCert = trustedCert;
        m_disable_crl_check = disable_crl_check;
        return *this;
    }

    /**
     * Sets the speech service type.
     */
    Client& SetEndpointType(EndpointType type)
    {
        m_endpointType = type;
        return *this;
    }

    /**
     * Get the endpoint type.
     */
    const EndpointType& GetEndpointType()
    {
        return m_endpointType;
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
    Client& SetAuthentication(const std::array<std::string, static_cast<size_t>(AuthenticationType::SIZE_AUTHENTICATION_TYPE)>& authData)
    {
        std::copy_n(authData.cbegin(), authData.size(), m_authData.begin());
        return *this;
    }

    /**
     * Sets query parameters
     */
    Client& SetQueryParameter(const std::string& name, const std::string& value);

    /**
     * Sets the polling interval the client will use.
     */
    Client& SetPollingIntervalms(const std::uint32_t pollingInterval)
    {
        m_pollingIntervalms = pollingInterval;
        return *this;
    }

    enum class DialogBackend
    {
        NotSet = 0,
        BotFramework,
        CustomCommands
    };
    /**
     * Sets the dialog backend to connect to.
     */
    Client& SetDialogBackend(DialogBackend dialogBackend)
    {
        m_dialogBackend = dialogBackend;
        return *this;
    }


     /**
     * Establishes connection to the service.
     */
    ConnectionPtr Connect();

    Client(const Client&) = default;
    ~Client() = default;

private:

    friend class Connection::Impl;

    CallbacksPtr m_callbacks;

    EndpointType m_endpointType;
    RecognitionMode m_recoMode;
    std::string m_customEndpointUrl;
    std::string m_customHostUrl;
    std::string m_region;
    std::string m_userDefinedQueryParameters;
    std::unordered_map<std::string, std::string> m_queryParameters;

    std::shared_ptr<ProxyServerInfo> m_proxyServerInfo;

    std::string m_trustedCert;
    bool m_disable_crl_check;

    std::string m_intentRegion;

    std::array<std::string, static_cast<size_t>(AuthenticationType::SIZE_AUTHENTICATION_TYPE)> m_authData;

    std::wstring m_connectionId;

    std::string m_audioResponseFormat;

    std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> m_threadService;

    std::uint32_t m_pollingIntervalms = 10;

    DialogBackend m_dialogBackend = DialogBackend::NotSet;
};

class endpoint
{
public:

    static const char* protocol;

    static const char* outputFormatQueryParam;
    static const char* langQueryParam;
    static const char* deploymentIdQueryParam;
    static const char* profanityQueryParam;
    static const char* initialSilenceTimeoutQueryParam;
    static const char* endSilenceTimeoutQueryParam;
    static const char* stableIntermediateThresholdQueryParam;
    static const char* storeAudioQueryParam;
    static const char* wordLevelTimestampsQueryParam;

    static const char* outputFormatSimple;
    static const char* outputFormatDetailed;

    static const char* postProcessingTrueText;

    static const char* profanityMasked;
    static const char* profanityRemoved;
    static const char* profanityRaw;

    class unifiedspeech
    {
    public:

        static const char* hostnameSuffix;
        static const char* pathPrefix;
        static const char* pathSuffix;

        static const char* postprocessingQueryParam;
        static const char* lidEnabledQueryParam;

        static const std::vector<std::string> queryParameters;
    };

    class azurecnspeech
    {
    public:

        static const char* hostnameSuffix;
    };

    class azurecntranslation
    {
    public:

        static const char* hostnameSuffix;
    };

    class azurecnspeechsynthesis
    {
    public:

        static const char* hostnameSuffix;
    };

    class translation
    {
    public:

        static const char* hostnameSuffix;
        static const char* path;

        static const char* fromQueryParam;
        static const char* toQueryParam;
        static const char* voiceQueryParam;
        static const char* featuresQueryParam;
        static const char* stableTranslationQueryParam;

        static const char* requireVoice;

        static const std::vector<std::string> queryParameters;
    };

    class luis
    {
    public:

        static const char* hostname;
        static const char* pathPrefix;
        static const char* pathSuffix;

        static const std::vector<std::string> queryParameters;
    };

    class dialog
    {
    public:

        static const char* url;

        class resourcePath
        {
        public:

            static const char* botFramework;
            static const char* customCommands;
        };

        static const char* suffix;

        class version
        {
        public:

            static const char* botFramework;
            static const char* customCommands;
        };

        static const char* customVoiceDeploymentIdsQueryParam;

        static const std::vector<std::string> queryParameters;
    };

    class conversationTranscriber
    {
    public:

        static const char* hostname;
        static const char* pathPrefix1;
        static const char* pathPrefix2;
        static const char* pathSuffixMultiAudio;

        static const std::vector<std::string> queryParameters;
    };

    class speechSynthesis
    {
    public:

        static const char* hostnameSuffix;
        static const char* path;

        static const std::vector<std::string> queryParameters;
    };
};

}}}}

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

enum class EndpointType { Speech, Intent, Translation, Dialog, ConversationTranscriptionService };

enum class RecognitionMode : unsigned int { Interactive = 0, Conversation = 1, Dictation = 2 };

enum class OutputFormat : unsigned int { Simple = 0, Detailed = 1 };

enum class AuthenticationType: size_t { SubscriptionKey = 0, AuthorizationToken, SearchDelegationRPSToken, DialogApplicationId, SIZE_AUTHENTICATION_TYPE };

enum class MessageType { Config, Context, Agent, AgentContext, SpeechEvent, Event};

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
};

extern void PlatformInit(const char* proxyHost, int proxyPort, const char* proxyUsername, const char* proxyPassword);

namespace endpoint
{
    const std::string protocol = "wss://";

    const std::string outputFormatQueryParam = "format=";
    const std::string langQueryParam = "language=";
    const std::string deploymentIdQueryParam = "cid=";
    const std::string profanityQueryParam = "profanity=";
    const std::string initialSilenceTimeoutQueryParam = "initialSilenceTimeoutMs=";
    const std::string endSilenceTimeoutQueryParam = "endSilenceTimeoutMs=";
    const std::string stableIntermediateThresholdQueryParam = "stableIntermediateThreshold=";
    const std::string storeAudioQueryParam = "storeAudio=";
    const std::string wordLevelTimestampsQueryParam = "wordLevelTimestamps=";

    const std::string outputFormatSimple = "simple";
    const std::string outputFormatDetailed = "detailed";

    const std::string postProcessingTrueText = "TrueText";

    const std::string profanityMasked = "masked";
    const std::string profanityRemoved = "removed";
    const std::string profanityRaw = "raw";

    namespace unifiedspeech
    {
        const std::string hostnameSuffix = ".stt.speech.microsoft.com";
        const std::string pathPrefix = "/speech/recognition/";
        const std::string pathSuffix = "/cognitiveservices/v1";

        const std::string postprocessingQueryParam = "postprocessing=";

        const std::vector<std::string> queryParameters = {
            langQueryParam,
            deploymentIdQueryParam,
            initialSilenceTimeoutQueryParam,
            endSilenceTimeoutQueryParam,
            storeAudioQueryParam,

            outputFormatQueryParam,
            wordLevelTimestampsQueryParam,
            profanityQueryParam,
            stableIntermediateThresholdQueryParam,
            postprocessingQueryParam

        };
    }

    namespace translation
    {
        const std::string hostnameSuffix = ".s2s.speech.microsoft.com";
        const std::string path = "/speech/translation/cognitiveservices/v1";

        const std::string fromQueryParam = "from=";
        const std::string toQueryParam = "to=";
        const std::string voiceQueryParam = "voice=";
        const std::string featuresQueryParam = "features=";
        const std::string stableTranslationQueryParam = "stableTranslation=";

        const std::string requireVoice = "texttospeech";

        const std::vector<std::string> queryParameters = {
            fromQueryParam,
            toQueryParam,
            voiceQueryParam,

            deploymentIdQueryParam,
            initialSilenceTimeoutQueryParam,
            endSilenceTimeoutQueryParam,
            storeAudioQueryParam,

            outputFormatQueryParam,
            wordLevelTimestampsQueryParam,
            profanityQueryParam,
            stableIntermediateThresholdQueryParam,

            stableTranslationQueryParam
        };
    }

    namespace luis
    {
        const std::string hostname = "speech.platform.bing.com";
        const std::string pathPrefix1 = "/speech/";
        const std::string pathPrefix2 = "/recognition/";
        const std::string pathSuffix = "/cognitiveservices/v1";

        const std::vector<std::string> queryParameters = { langQueryParam, outputFormatQueryParam };
    }

    namespace dialog
    {
        const std::string url = ".convai.speech.microsoft.com/api/v3";

        const std::vector<std::string> queryParameters = { langQueryParam };
    }

    namespace conversationTranscriber
    {
        const std::vector<std::string> queryParameters = { langQueryParam, outputFormatQueryParam };
        const std::string hostname = ".cts.speech.microsoft.com";
        const std::string pathPrefix1 = "transcribe.";
        const std::string pathPrefix2 = "/speech/recognition";
        const std::string pathSuffixMultiAudio = "/multiaudio";
    }
}
}}}}

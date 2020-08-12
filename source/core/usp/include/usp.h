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
#include "usp_message.h"
#include "ispxinterfaces.h"
#include "exception.h"
#include "site_helpers.h"
#include "interface_helpers.h"
#include <object_with_site_init_impl.h>

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

class ISpxUspCallbacks :
    public Impl::ISpxInterfaceBaseFor<ISpxUspCallbacks>,
    public Callbacks
{
};

class CSpxUspCallbackWrapper final :
    public Impl::ISpxObjectWithSiteInitImpl<ISpxUspCallbacks>,
    public ISpxUspCallbacks
{
public:

    CSpxUspCallbackWrapper() = default;
    ~CSpxUspCallbackWrapper() = default;

    SPX_INTERFACE_MAP_BEGIN()
        using namespace Impl;
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxUspCallbacks)
    SPX_INTERFACE_MAP_END()

    // --- ISpxUspCallbacks (overrides)
    inline void OnMessageReceived(const USP::RawMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnMessageReceived(m); }); }
    inline void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechStartDetected(m); }); }
    inline void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechEndDetected(m); }); }
    inline void OnSpeechHypothesis(const USP::SpeechHypothesisMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechHypothesis(m); }); }
    inline void OnSpeechKeywordDetected(const USP::SpeechKeywordDetectedMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechKeywordDetected(m); }); }
    inline void OnSpeechPhrase(const USP::SpeechPhraseMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechPhrase(m); }); }
    inline void OnSpeechFragment(const USP::SpeechFragmentMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechFragment(m); }); }
    inline void OnTurnStart(const USP::TurnStartMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTurnStart(m); }); }
    inline void OnTurnEnd(const USP::TurnEndMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTurnEnd(m); }); }
    inline void OnMessageStart(const USP::TurnStartMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnMessageStart(m); }); }
    inline void OnMessageEnd(const USP::TurnEndMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnMessageEnd(m); }); }
    inline void OnError(bool transport, USP::ErrorCode errorCode, const std::string& errorMessage) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnError(transport, errorCode, errorMessage); }); }
    inline void OnTranslationHypothesis(const USP::TranslationHypothesisMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationHypothesis(m); }); }
    inline void OnTranslationPhrase(const USP::TranslationPhraseMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationPhrase(m); }); }
    inline void OnAudioOutputChunk(const USP::AudioOutputChunkMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnAudioOutputChunk(m); }); }
    inline void OnAudioOutputMetadata(const USP::AudioOutputMetadataMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnAudioOutputMetadata(m); }); }
    inline void OnUserMessage(const USP::UserMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnUserMessage(m); }); }
    inline void OnConnected() final { InvokeOnSite([](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnConnected(); }); }
    inline void OnDisconnected() final { InvokeOnSite([](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnDisconnected(); }); }


private:
    DISABLE_COPY_AND_MOVE(CSpxUspCallbackWrapper);
};

using CallbacksPtr = std::shared_ptr<Callbacks>;

enum class EndpointType { Speech, Intent, Translation, Dialog, ConversationTranscriptionService, SpeechSynthesis };

enum class RecognitionMode : unsigned int { Interactive = 0, Conversation = 1, Dictation = 2 };

enum class OutputFormat : unsigned int { Simple = 0, Detailed = 1 };

enum class AuthenticationType: size_t { SubscriptionKey = 0, AuthorizationToken, SearchDelegationRPSToken, DialogApplicationId, ConversationToken, SIZE_AUTHENTICATION_TYPE };

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
    * @param message The message to send.
    */
    void SendMessage(std::unique_ptr<USP::Message> message);

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
        m_userDefinedHttpHeaders{},
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
     * Sets HTTP header values.
     * @param headers header values passed in by end users.
     */
    Client& SetUserDefinedHttpHeaders(const std::unordered_map<std::string, std::string>& headers)
    {
        // In case, a new key has a duplicate in the existing keys, overwrites the existing one.
        for (const auto& newHeader : headers)
        {
            m_userDefinedHttpHeaders[newHeader.first] = newHeader.second;
        }

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

    std::unordered_map<std::string, std::string> m_userDefinedHttpHeaders;
    std::wstring m_connectionId;

    std::string m_audioResponseFormat;

    std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> m_threadService;

    std::uint32_t m_pollingIntervalms = 10;

    DialogBackend m_dialogBackend = DialogBackend::NotSet;
};

constexpr const char queryParameterDelim = '&';

struct endpoint
{
    static constexpr const char * protocol = "wss://";

    static constexpr const char * outputFormatQueryParam = "format=";
    static constexpr const char * langQueryParam = "language=";
    static constexpr const char * deploymentIdQueryParam = "cid=";
    static constexpr const char * profanityQueryParam = "profanity=";
    static constexpr const char * initialSilenceTimeoutQueryParam = "initialSilenceTimeoutMs=";
    static constexpr const char * endSilenceTimeoutQueryParam = "endSilenceTimeoutMs=";
    static constexpr const char * stableIntermediateThresholdQueryParam = "stableIntermediateThreshold=";
    static constexpr const char * storeAudioQueryParam = "storeAudio=";
    static constexpr const char * wordLevelTimestampsQueryParam = "wordLevelTimestamps=";

    static constexpr const char * outputFormatSimple = "simple";
    static constexpr const char * outputFormatDetailed = "detailed";

    static constexpr const char * postProcessingTrueText = "TrueText";

    static constexpr const char * profanityMasked = "masked";
    static constexpr const char * profanityRemoved = "removed";
    static constexpr const char * profanityRaw = "raw";

    struct unifiedspeech
    {
        static constexpr const char * hostnameSuffix = ".stt.speech.microsoft.com";
        static constexpr const char * pathPrefix = "/speech/recognition/";
        static constexpr const char * pathSuffix = "/cognitiveservices/v1";

        static constexpr const char * postprocessingQueryParam = "postprocessing=";
        static constexpr const char * lidEnabledQueryParam = "lidEnabled=";

        static constexpr std::array<const char *, 11> queryParameters{{
            langQueryParam,
            deploymentIdQueryParam,
            initialSilenceTimeoutQueryParam,
            endSilenceTimeoutQueryParam,
            storeAudioQueryParam,
            outputFormatQueryParam,
            wordLevelTimestampsQueryParam,
            profanityQueryParam,
            stableIntermediateThresholdQueryParam,
            postprocessingQueryParam,
            lidEnabledQueryParam
        }};
    };

    struct azurecnspeech
    {
        static constexpr const char * hostnameSuffix = ".stt.speech.azure.cn";
    };

    struct azurecntranslation
    {
        static constexpr const char * hostnameSuffix = ".s2s.speech.azure.cn";
    };

    struct  azurecnspeechsynthesis
    {
        static constexpr const char * hostnameSuffix = ".tts.speech.azure.cn";
    };

    struct translation
    {
        static constexpr const char * hostnameSuffix = ".s2s.speech.microsoft.com";
        static constexpr const char * path = "/speech/translation/cognitiveservices/v1";

        static constexpr const char * fromQueryParam = "from=";
        static constexpr const char * toQueryParam = "to=";
        static constexpr const char * voiceQueryParam = "voice=";
        static constexpr const char * featuresQueryParam = "features=";
        static constexpr const char * stableTranslationQueryParam = "stableTranslation=";

        static constexpr const char * requireVoice = "texttospeech";

        static constexpr std::array<const char *, 12> queryParameters{{
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
        }};
    };

    struct luis
    {
        static constexpr const char * hostname = ".sr.speech.microsoft.com";
        static constexpr const char * pathPrefix = "/speech/recognition/";
        static constexpr const char * pathSuffix = "/cognitiveservices/v1";

        static constexpr std::array<const char *, 2> queryParameters{{
            langQueryParam,
            outputFormatQueryParam
        }};
    };

    struct dialog
    {
        static constexpr const char * url = ".convai.speech.microsoft.com";

        struct resourcePath
        {
            static constexpr const char * botFramework = "";
            static constexpr const char * customCommands = "/commands";
        };

        static constexpr const char * suffix = "/api";

        struct version
        {
            static constexpr const char * botFramework = "/v3";
            static constexpr const char * customCommands = "/v1";
        };

        static constexpr const char * botIdQueryParam = "botId=";
        static constexpr const char * customVoiceDeploymentIdsQueryParam = "voiceDeploymentId=";

        struct customCommands
        {
            static constexpr std::array<const char *, 2> queryParameters{{
                langQueryParam,
                customVoiceDeploymentIdsQueryParam
            }};
        };

        struct botFramework
        {
            static constexpr std::array<const char *, 3> queryParameters{{
                langQueryParam,
                customVoiceDeploymentIdsQueryParam,
                botIdQueryParam
            }};
        };
    };

    struct conversationTranscriber
    {
        static constexpr const char * hostname = ".cts.speech.microsoft.com";
        static constexpr const char * pathPrefix1 = "transcribe.";
        static constexpr const char * pathPrefix2 = "/speech/recognition";
        static constexpr const char * pathSuffixMultiAudio = "/multiaudio";

        static constexpr std::array<const char *, 2> queryParameters{{
            langQueryParam,
            outputFormatQueryParam
        }};
    };

    struct speechSynthesis
    {
        static constexpr const char * hostnameSuffix = ".tts.speech.microsoft.com";
        static constexpr const char * path = "/cognitiveservices/websocket/v1";

        static constexpr std::array<const char *, 0> queryParameters{};
    };
};

}}}}

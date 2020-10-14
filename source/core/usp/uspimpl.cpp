//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspimpl.cpp: implementation of the USP library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"

#include <assert.h>
#include <inttypes.h>

#include "azure_c_shared_utility_xlogging_wrapper.h"
#include "azure_c_shared_utility_httpheaders_wrapper.h"
#include "azure_c_shared_utility_platform_wrapper.h"
#include "azure_c_shared_utility_urlencode_wrapper.h"
#include "azure_c_shared_utility_http_proxy_io_wrapper.h"

#include "error_info.h"

#include "uspcommon.h"
#include "uspinternal.h"

#include "usp_metrics.h"

#include "exception.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include "string_utils.h"
#include "guid_utils.h"
#include "http_utils.h"
#include "endpoint_utils.h"

#include "json.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;

uint64_t telemetry_gettime()
{
    auto now = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

/* TODO: This won't be needed when we move to c++17 */
constexpr std::array<const char *, 11> endpoint::unifiedspeech::queryParameters;
constexpr std::array<const char *, 12> endpoint::translation::queryParameters;
constexpr std::array<const char *, 2> endpoint::luis::queryParameters;
constexpr std::array<const char*, 2> endpoint::dialog::customCommands::queryParameters;
constexpr std::array<const char *, 4> endpoint::dialog::botFramework::queryParameters;
constexpr std::array<const char *, 2> endpoint::conversationTranscriber::queryParameters;
constexpr std::array<const char *, 0> endpoint::speechSynthesis::queryParameters;

constexpr auto HEADER_PATH = "Path";

const char* path::speechHypothesis = "speech.hypothesis";
const char* path::speechPhrase = "speech.phrase";
const char* path::speechFragment = "speech.fragment";
const char* path::speechKeyword = "speech.keyword";
const char* path::turnStart = "turn.start";
const char* path::turnEnd = "turn.end";
const char* path::speechStartDetected = "speech.startDetected";
const char* path::speechEndDetected = "speech.endDetected";
const char* path::translationHypothesis = "translation.hypothesis";
const char* path::translationPhrase = "translation.phrase";
const char* path::translationSynthesis = "translation.synthesis";
const char* path::translationSynthesisEnd = "translation.synthesis.end";
const char* path::translationResponse = "translation.response";
const char* path::audio = "audio";
const char* path::audioMetaData = "audio.metadata";
const char* path::audioStart = "audio.start";
const char* path::audioEnd = "audio.end";

const char* json_properties::offset = "Offset";
const char* json_properties::duration = "Duration";
const char* json_properties::status = "Status";
const char* json_properties::text = "Text";
const char* json_properties::recoStatus = "RecognitionStatus";
const char* json_properties::displayText = "DisplayText";
const char* json_properties::context = "context";
const char* json_properties::tag = "serviceTag";
const char* json_properties::speaker = "SpeakerId";
const char* json_properties::id = "ID";
const char* json_properties::nbest = "NBest";
const char* json_properties::confidence = "Confidence";
const char* json_properties::display = "Display";

const char* json_properties::translation = "Translation";
const char* json_properties::translationStatus = "TranslationStatus";
const char* json_properties::failureReason = "FailureReason";
const char* json_properties::translations = "Translations";
const char* json_properties::synthesisStatus = "SynthesisStatus";
const char* json_properties::lang = "Language";
const char* json_properties::translationLanguage = "TranslationLanguage";

const char* json_properties::metadata = "Metadata";
const char* json_properties::type = "Type";
const char* json_properties::data = "Data";
const char* json_properties::wordBoundary = "WordBoundary";
const char* json_properties::lowerText = "text";

const char* json_properties::primaryLanguage = "PrimaryLanguage";
const char* json_properties::speechHypothesis = "SpeechHypothesis";
const char* json_properties::speechPhrase = "SpeechPhrase";

constexpr auto azurecnRegion = "china";

enum class NetworkType
{
    Default,
    AzureCN
};

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;

template <class T>
static void throw_if_null(const T* ptr, const string& name)
{
    if (ptr == NULL)
    {
        ThrowInvalidArgumentException("The argument '" + name +"' is null."); \
    }
}

inline bool contains(const string& content, const string& name)
{
    return (content.find(name) != string::npos) ? true : false;
}

template<size_t N>
void BuildQueryParameters(const array<const char *, N>& parameterList, const unordered_map<string, string>& valueMap, bool isCustomEndpoint, ostringstream& oss)
{
    for (auto queryParameterName : parameterList)
    {
        if (!isCustomEndpoint || !contains(oss.str(), queryParameterName))
        {
            auto entry = valueMap.find(queryParameterName);
            if (entry != valueMap.end() && !entry->second.empty())
            {
                oss << queryParameterDelim << queryParameterName << entry->second;
            }
        }
    }
}

/// <summary>
/// Helper method to read a value from constant the USP headers unordered map and return a default
/// empty string instead of an exception. Using [] would require a non-constant header, and .at
/// throws exceptions for missing keys
/// </summary>
/// <param name="headers">The headers to retrieve the value from</param>
/// <param name="key">The key to retrieve</param>
/// <returns>The value of that header, or an empty string if the key is not found</returns>
static std::string TryGet(const UspHeaders& headers, const char * key)
{
    auto iter = headers.find(key);
    if (iter == headers.end())
    {
        return std::string{};
    }
    else
    {
        return iter->second;
    }
}

constexpr std::array<const char *, 3> g_recoModeStrings{{ "interactive", "conversation", "dictation" }};

// This is called from telemetry_flush, invoked on a worker thread in turn-end.
void Connection::Impl::OnTelemetryData(std::string&& data, const std::string& requestId)
{
    if (m_transport != nullptr)
    {
        m_transport->SendTelemetryData(std::move(data), requestId);
    }
}

Connection::Impl::Impl(const Client& config)
    : m_config(config),
    m_valid(false),
    m_connected(false),
    m_speechContextMessageAllowed(true),
    m_audioOffset(0),
    m_creationTime(telemetry_gettime()),
    m_turnUsingHeaders(false)
{
    if (m_config.m_proxyServerInfo != nullptr)
    {
        PlatformInit(m_config.m_proxyServerInfo->host.c_str(),
            m_config.m_proxyServerInfo->port,
            m_config.m_proxyServerInfo->username.c_str(),
            m_config.m_proxyServerInfo->password.c_str());
    }
    else
    {
        PlatformInit(nullptr, 0, nullptr, nullptr);
    }

    m_threadService = m_config.m_threadService;
}

uint64_t Connection::Impl::getTimestamp()
{
    return telemetry_gettime() - m_creationTime;
}

void Connection::Impl::Invoke(function<void(CallbacksPtr)> callback)
{
    auto callbacks = m_config.m_callbacks;
    if (callbacks == nullptr || !m_valid)
    {
        return;
    }

    callback(callbacks);
}

void Connection::Impl::Shutdown()
{
    m_config.m_callbacks = nullptr;
    m_connected = false;
    m_valid = false;
}

bool Connection::Impl::IsConnected()
{
    return m_connected;
}

std::string Connection::Impl::GetConnectionUrl()
{
    return m_connectionUrl;
}

NetworkType GetNetworkType(std::string region)
{
    if (region.find(azurecnRegion) == 0)
    {
        return NetworkType::AzureCN;
    }
    else
    {
        return NetworkType::Default;
    }
}

std::string GetHostNameSuffix(std::string region, EndpointType endpointType)
{
    std::string result = "";
    NetworkType networkType = GetNetworkType(region);

    switch (endpointType)
    {
    case EndpointType::Speech:
        switch (networkType)
        {
        case NetworkType::AzureCN:
            result = region + endpoint::azurecnspeech::hostnameSuffix;
            break;
        default:
            result = region + endpoint::unifiedspeech::hostnameSuffix;
            break;
        }
        break;
    case EndpointType::Translation:
        switch (networkType)
        {
        case NetworkType::AzureCN:
            result = region + endpoint::azurecntranslation::hostnameSuffix;
            break;
        default:
            result = region + endpoint::translation::hostnameSuffix;
            break;
        }
        break;
    case EndpointType::SpeechSynthesis:
        switch (networkType)
        {
        case NetworkType::AzureCN:
            result = region + endpoint::azurecnspeechsynthesis::hostnameSuffix;
            break;
        default:
            result = region + endpoint::speechSynthesis::hostnameSuffix;
            break;
        }
        break;
    default:
        break;
    }
    return result;
}

string Connection::Impl::ConstructConnectionUrl() const
{
    auto recoMode = static_cast<underlying_type_t<RecognitionMode>>(m_config.m_recoMode);
    auto region = m_config.m_region;
    auto intentRegion = m_config.m_intentRegion;
    ostringstream oss;
    bool customEndpoint = false;
    bool customHost = false;

    // Using customized endpoint if it is defined.
    if (!m_config.m_customEndpointUrl.empty())
    {
        // Check for invalid use of auth token service endpoint

        bool isTokenServiceEndpoint = false;
        std::string endpointRegion;

        std::tie(isTokenServiceEndpoint, endpointRegion) =
            EndpointUtils::IsTokenServiceEndpoint(m_config.m_customEndpointUrl);

        if (isTokenServiceEndpoint)
        {
            if (!endpointRegion.empty())
            {
                // Use only the region, defaults for the rest
                region = endpointRegion;

                // As of this writing, there are more speech regions than intent
                // regions and they cannot be perfectly matched, so this doesn't
                // work right 100% of the time which cannot be helped.
                intentRegion = region;
            }
        }
        else
        {
            customEndpoint = true;
        }
    }

    if (customEndpoint)
    {
        oss << m_config.m_customEndpointUrl;
    }
    else if (!m_config.m_customHostUrl.empty())
    {
        // Parse the custom host address
        Url url = HttpUtils::ParseUrl(m_config.m_customHostUrl);

        if (!url.path.empty())
        {
            ThrowInvalidArgumentException("Resource path is not allowed in the host URI.");
        }

        if (!url.query.empty())
        {
            ThrowInvalidArgumentException("Query parameters are not allowed in the host URI.");
        }

        oss << HttpUtils::SchemePrefix(url.scheme) << url.host << ':' << url.port;
        customHost = true;
    }
    else
    {
        oss << endpoint::protocol;
    }

    if (!customEndpoint) // standard endpoint, or custom host with standard path
    {
        switch (m_config.m_endpointType)
        {
        case EndpointType::Speech:
            if (!customHost)
            {
                oss << GetHostNameSuffix(region, EndpointType::Speech);
            }
            oss << endpoint::unifiedspeech::pathPrefix
                << g_recoModeStrings[recoMode]
                << endpoint::unifiedspeech::pathSuffix;
            break;

        case EndpointType::Translation:
            if (!customHost)
            {
                oss << GetHostNameSuffix(region, EndpointType::Translation);
            }
            oss << endpoint::translation::path;
            break;

        case EndpointType::Intent:
            if (!customHost)
            {
                oss << intentRegion
                    << endpoint::luis::hostname;
            }
            if (m_config.m_recoMode != USP::RecognitionMode::Interactive)
            {
                ThrowInvalidArgumentException("Invalid reco mode for intent recognition.");
            }
            oss << endpoint::luis::pathPrefix
                << g_recoModeStrings[recoMode]
                << endpoint::luis::pathSuffix;
            break;

        case EndpointType::Dialog:
            /* This will generate an url of the form wss://<region>.convai.speech.microsoft.com/<backend>/api/<version> */
            if (!customHost)
            {
                oss << region
                    << endpoint::dialog::url;
            }
            { // must be enclosed in a block to avoid compiler errors about jumping past a declaration with an initializer
                const char* resource = nullptr;
                const char* version = nullptr;
                if (m_config.m_dialogBackend == Client::DialogBackend::BotFramework)
                {
                    resource = endpoint::dialog::resourcePath::botFramework;
                    version = endpoint::dialog::version::botFramework;
                }
                else if (m_config.m_dialogBackend == Client::DialogBackend::CustomCommands)
                {
                    resource = endpoint::dialog::resourcePath::customCommands;
                    version = endpoint::dialog::version::customCommands;
                }
                else
                {
                    ThrowInvalidArgumentException("Invalid dialog backend.");
                }

                oss << resource
                    << endpoint::dialog::suffix
                    << version;
            }
            break;

        case EndpointType::ConversationTranscriptionService:
            if (!customHost)
            {
                oss << endpoint::conversationTranscriber::pathPrefix1
                    << region
                    << endpoint::conversationTranscriber::hostname;
            }
            oss << endpoint::conversationTranscriber::pathPrefix2
                << endpoint::conversationTranscriber::pathSuffixMultiAudio;
            break;

        case EndpointType::SpeechSynthesis:
            if (!customHost)
            {
                oss << GetHostNameSuffix(region, EndpointType::SpeechSynthesis);
            }
            oss << endpoint::speechSynthesis::path;
            break;

        default:
            ThrowInvalidArgumentException("Unknown endpoint type.");
        }
    }

    // Appends user defined query parameters first.
    if (!m_config.m_userDefinedQueryParameters.empty())
    {
        oss << queryParameterDelim << m_config.m_userDefinedQueryParameters;
        customEndpoint = true;
    }
    // Note the use of customEndpoint in the code below for building query params
    // - customEndpoint true:  endpoint is specified by FromEndpoint, or m_config.m_userDefinedQueryParameters is non-empty
    // - customEndpoint false: endpoint is not specified by FromEndpoint and m_config.m_userDefinedQueryParameters is empty

    switch (m_config.m_endpointType)
    {
    case EndpointType::ConversationTranscriptionService:
        BuildQueryParameters(endpoint::unifiedspeech::queryParameters, m_config.m_queryParameters, customEndpoint, oss);
        break;

    case EndpointType::Speech:
        BuildQueryParameters(endpoint::unifiedspeech::queryParameters, m_config.m_queryParameters, customEndpoint, oss);
        break;

    case EndpointType::Intent:
        BuildQueryParameters(endpoint::luis::queryParameters, m_config.m_queryParameters, customEndpoint, oss);
        break;

    case EndpointType::Translation:
        for (auto queryParameterName : endpoint::translation::queryParameters)
        {
            if (!customEndpoint || !contains(oss.str(), queryParameterName))
            {
                auto entry = m_config.m_queryParameters.find(queryParameterName);
                if (entry != m_config.m_queryParameters.end() && !entry->second.empty())
                {
                    // Need to use separate parameter for each target language.
                    if (strcmp(queryParameterName, endpoint::translation::toQueryParam) == 0)
                    {
                        auto langVector = PAL::split(entry->second, CommaDelim);
                        for (auto item : langVector)
                        {
                            oss << queryParameterDelim << endpoint::translation::toQueryParam << HttpUtils::UrlEscape(item);
                        }
                    }
                    // Voice need 2 query parameters.
                    else if (strcmp(queryParameterName, endpoint::translation::voiceQueryParam) == 0)
                    {
                        oss << queryParameterDelim << endpoint::translation::featuresQueryParam << endpoint::translation::requireVoice;
                        oss << queryParameterDelim << endpoint::translation::voiceQueryParam << HttpUtils::UrlEscape(entry->second);
                    }
                    else
                    {
                        oss << queryParameterDelim << queryParameterName << entry->second;
                    }
                }
            }
        }
        break;

    case EndpointType::Dialog:
        if (m_config.m_dialogBackend == Client::DialogBackend::BotFramework)
        {
            BuildQueryParameters(endpoint::dialog::botFramework::queryParameters, m_config.m_queryParameters, customEndpoint, oss);
        }
        else if (m_config.m_dialogBackend == Client::DialogBackend::CustomCommands)
        {
            BuildQueryParameters(endpoint::dialog::customCommands::queryParameters, m_config.m_queryParameters, customEndpoint, oss);
        }
        break;

    case EndpointType::SpeechSynthesis:
        BuildQueryParameters(endpoint::speechSynthesis::queryParameters, m_config.m_queryParameters, customEndpoint, oss);
        break;
    }

    auto urlStr = oss.str();
    auto firstQueryDelim = urlStr.find_first_of("?&");
    if (firstQueryDelim != string::npos)
    {
        urlStr[firstQueryDelim] = '?';
    }

    return urlStr;
}



void Connection::Impl::Connect()
{
    LogInfo("%s: entering...", __FUNCTION__);
    if (m_transport != nullptr || m_valid)
    {
        ThrowLogicError("USP connection already created.");
    }

    HttpEndpointInfo endpoint;

    if (!m_config.m_audioResponseFormat.empty())
    {
        endpoint.SetHeader(headers::audioResponseFormat, m_config.m_audioResponseFormat);
    }

    // Set authentication headers.
    auto& authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::SubscriptionKey)];
    if (!authStr.empty())
    {
        LogInfo("Adding subscription key headers");
        endpoint.SetHeader(headers::ocpApimSubscriptionKey, authStr);
    }
    authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::AuthorizationToken)];
    if (!authStr.empty())
    {
        LogInfo("Adding authorization token headers");
        auto token = "Bearer " + authStr;
        endpoint.SetHeader(headers::authorization, token);
    }
    authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::SearchDelegationRPSToken)];
    if (!authStr.empty())
    {
        LogInfo("Adding search delegation RPS token.");
        endpoint.SetHeader(headers::searchDelegationRPSToken, authStr);
    }
    authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::DialogApplicationId)];
    if (!authStr.empty())
    {
        const char* headerName = m_config.m_dialogBackend == Client::DialogBackend::BotFramework ? headers::dialogDLSSecret :
                                 m_config.m_dialogBackend == Client::DialogBackend::CustomCommands ? headers::dialogCommandsAppId :
                                 nullptr;

        if (headerName != nullptr)
        {
            LogInfo("Adding Dialog auth header.");
            endpoint.SetHeader(headerName, authStr);
        }
    }
    authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::ConversationToken)];
    if (!authStr.empty())
    {
        LogInfo("Adding conversation token.");
        endpoint.SetHeader(headers::capitoConversationToken, authStr);
    }

    if (m_config.m_endpointType == EndpointType::Dialog)
    {
        auto& region = m_config.m_region;
        if (!region.empty())
        {
            LogInfo("Adding region header");
            endpoint.SetHeader(headers::region, region);
        }
    }

    for (const auto& header : m_config.m_userDefinedHttpHeaders)
    {
        LogInfo("Set a user defined HTTP header '%s':'%s'", header.first.c_str(), header.second.c_str());
        endpoint.SetHeader(header.first, header.second);
    }

    // TODO ralphe: should probably update the ConstructConnectionUrl to set the scheme, host, path and query parameters
    //              directly on the endpoint instance
    m_connectionUrl = ConstructConnectionUrl();
    endpoint.EndpointUrl(m_connectionUrl);
    m_connectionUrl = endpoint.EndpointUrl();
    LogInfo("connectionUrl=%s", m_connectionUrl.c_str());

    m_telemetry = make_unique<Telemetry>(
        [weak = std::weak_ptr<Connection::Impl>(shared_from_this())](std::string&& data, const std::string& requestId)
        {
            auto instance = weak.lock();
            if (instance != nullptr)
            {
                instance->OnTelemetryData(std::move(data), requestId);
            }
        });

    if (m_telemetry == nullptr)
    {
        ThrowRuntimeError("Failed to create telemetry instance.");
    }

    string connectionId = PAL::ToString(m_config.m_connectionId);

    // Log the device uuid
    MetricsDeviceStartup(*m_telemetry, connectionId, PAL::DeviceUuid());

#ifdef SPEECHSDK_USE_OPENSSL
    if (!m_config.m_trustedCert.empty())
    {
        endpoint
            .DisableDefaultVerifyPaths(true)
            .SingleTrustedCertificate(m_config.m_trustedCert, m_config.m_disable_crl_check);
    }
#endif

    m_transport = UspWebSocket::Create(
        m_threadService,
        ISpxThreadService::Affinity::Background,
        std::chrono::milliseconds{ m_config.m_pollingIntervalms },
        *(m_telemetry.get()));

    std::shared_ptr<USP::Connection::Impl> shared(shared_from_this());

    m_transport->OnConnected.add(shared, &Connection::Impl::OnTransportOpened);
    m_transport->OnDisconnected.add(shared, &Connection::Impl::OnTransportClosed);
    m_transport->OnError.add(shared, &Connection::Impl::OnTransportError);
    m_transport->OnUspBinaryData.add(shared, &Connection::Impl::OnTransportBinaryData);
    m_transport->OnUspTextData.add(shared, &Connection::Impl::OnTransportTextData);

    // set the connection ID header and protocol
    endpoint
        .SetHeader("X-ConnectionId", connectionId)
        .AddWebSocketProtocol("USP");

    // open the web socket connection
    m_valid = true;
    m_transport->Connect(endpoint, connectionId);
}

string Connection::Impl::CreateRequestId()
{
    auto requestId = PAL::ToString(PAL::CreateGuidWithoutDashes());

    LogInfo("RequestId: '%s'", requestId.c_str());
    RegisterRequestId(requestId);

    return requestId;
}

void Connection::Impl::RegisterRequestId(const string& requestId)
{
    MetricsTransportRequestId(m_telemetry.get(), requestId.c_str());
    m_activeRequestIds.insert(requestId);
}

void Connection::Impl::QueueMessage(std::unique_ptr<USP::Message> message)
{
    if (message->Path().empty())
    {
        ThrowInvalidArgumentException("The path is null or empty.");
    }

    if (!m_valid || m_transport == nullptr)
    {
        return;
    }

    // According to USP protocol, speech.context must be sent before any audio in a turn, and
    // only one speech.context message is allowed in the same turn.
    if (message->MessageType() == MessageType::Context)
    {
        // QueueMessage() is serialized by ThreadService, no lock is needed.
        if (!m_speechContextMessageAllowed)
        {
            ThrowLogicError("Error trying to send a context message while in the middle of a speech turn.");
        }
        else
        {
            // Only one speech.context in the same turn.
            m_speechContextMessageAllowed = false;
        }
    }

    std::string usedRequestId = message->RequestId();
    if (usedRequestId.empty())
    {
        usedRequestId = UpdateRequestId(message->MessageType(), message->IsBinary());
    }
    else if (message->Path() == "synthesis.context")
    {
        // TODO ralphe: This logic feels like a hack and should be cleaned up. If we want to
        //              set the request ID we should be explicit about it e.g. call a
        //              SetRequestId(...) method
        m_speechRequestId = usedRequestId;
    }

    message->RequestId(usedRequestId);

    if (m_transport)
    {
        m_transport->SendData(std::move(message));
    }
}

string Connection::Impl::UpdateRequestId(const MessageType messageType, bool binary)
{
    // The config message does not require a X-RequestId header, because this message is not associated with a particular request.
    // Other messages, such as speech.event, speech.context etc need a request id.
    string requestId;

    switch (messageType)
    {
    case MessageType::Config:
        break;

    case MessageType::AgentContext:
        requestId = CreateRequestId();
        m_speechRequestId = requestId;
        break;

    case MessageType::Context:
        if (m_config.m_endpointType == EndpointType::ConversationTranscriptionService)
        {
            // For ConversationTranscription, speech.event might be sent before speech.context, so m_speechRequestId might already be set.
            // After the Bug 1784130 is fixed, the following code should be removed, i.e. instead of creating requestId, a LogicError should be thrown,
            if (m_speechRequestId.empty())
            {
                m_speechRequestId = CreateRequestId();
            }
        }
        else
        {
            // For other services, speech.event must be associated to an ongoing speech turn. Only speech.context or audio can kick-off a new turn.
            // And speech.context must be sent before audio, so m_speechRequestId must be empty at this time.
            if (!m_speechRequestId.empty())
            {
                ThrowLogicError("Speech.Context must be the first message in a turn, and m_speechRequestId must be empty.");
            }
            m_speechRequestId = CreateRequestId();
        }
        requestId = m_speechRequestId;
        break;

    case MessageType::SpeechEvent:
        //Bug 1784130: according to USP, SpeechEvent is associated with the current speech turn. So m_speechRequestId must be non-empty.
        //However, the current conversation transcriber will send speech.event either before a turn (before audio/speech.context)
        //or outside a turn (after turn.end).
        if (m_config.m_endpointType == EndpointType::ConversationTranscriptionService)
        {
            if (m_speechRequestId.empty())
            {
                m_speechRequestId = CreateRequestId();
            }
        }
        if (m_speechRequestId.empty())
        {
            ThrowLogicError("Speech.event must be associated to the current speech turn, so m_speechRequestId must be non-empty.");
        }
        requestId = m_speechRequestId;
        break;

    case MessageType::Event:
        requestId = CreateRequestId();
        break;

    case MessageType::Agent:
        requestId = CreateRequestId();
        break;

    case MessageType::Ssml:
        if (m_speechRequestId.empty())
        {
            ThrowLogicError("Request ID is required for speech.synthesis request, so m_speechRequestId must be non-empty when sending SSML request.");
        }

        requestId = m_speechRequestId;
        break;

    default:
        // Unknown messages that are binary will get an existing request ID if one is available.
        if (binary)
        {
            if (m_speechRequestId.empty())
            {
                m_speechRequestId = CreateRequestId();
            }

            requestId = m_speechRequestId;
        }
        else
        {
            requestId = CreateRequestId();
        }
        break;
    }

    LogInfo("Create requestId %s for messageType %d", requestId.c_str(), static_cast<int>(messageType));
    return requestId;
}

void Connection::Impl::QueueAudioSegment(const Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk)
{
    auto size = audioChunk->size;
    if (size == 0)
    {
        QueueAudioEnd();
        return;
    }

    LogInfo("TS:%" PRIu64 ", Write %" PRIu32 " bytes audio data.", getTimestamp(), size);

    throw_if_null(audioChunk->data.get(), "data");

    if (!m_valid)
    {
        return;
    }

    // after sending audio message, no speech.context is allowed in the same turn.
    if (m_speechContextMessageAllowed)
    {
        m_speechContextMessageAllowed = false;
    }

    MetricsAudioStreamData(size);

    bool newTurn = m_audioOffset == 0;
    if (newTurn)
    {
        // The service uses the first audio message that contains a unique request identifier to signal the start of a new request/response cycle or turn.
        // After receiving an audio message with a new request identifier, the service discards any queued or unsent messages
        // that are associated with any previous turn.
        m_speechRequestId = m_speechRequestId.empty() ? CreateRequestId() : m_speechRequestId;
        LogInfo("The current speech request id is %s", m_speechRequestId.c_str());

        MetricsAudioStreamInit();
        MetricsAudioStart(*m_telemetry, m_speechRequestId);
    }

    if (m_transport)
    {
        m_transport->SendAudioData(path::audio, audioChunk, m_speechRequestId, newTurn);
    }

    m_audioOffset += size;
}

void Connection::Impl::QueueAudioEnd()
{
    LogInfo("TS:%" PRIu64 ", Flush audio buffer.", getTimestamp());

    if (!m_valid || m_audioOffset == 0)
    {
        return;
    }

    // no speech.context is allowed after audio.end
    if (m_speechContextMessageAllowed)
    {
        m_speechContextMessageAllowed = false;
    }

    std::exception_ptr ex;
    try
    {
        if (m_transport)
        {
            m_transport->SendAudioData(path::audio, std::make_shared<DataChunk>(nullptr, 0), m_speechRequestId);
        }
    }
    catch (const std::exception& innerEx)
    {
        ex = std::make_exception_ptr(ExceptionWithCallStack{ std::string("QueueAudioEnd failed with an exception: ") + innerEx.what(), SPXERR_RUNTIME_ERROR });
    }
    catch (...)
    {
        ex = std::make_exception_ptr(ExceptionWithCallStack{ "QueueAudioEnd failed with an unknown throwable", SPXERR_RUNTIME_ERROR });
    }

    m_audioOffset = 0;
    MetricsAudioStreamFlush();
    MetricsAudioEnd(*m_telemetry, m_speechRequestId);

    if (ex)
    {
        std::rethrow_exception(ex);
    }
}

void Connection::Impl::WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency)
{
    if (m_valid)
    {
        MetricsResultLatency(*m_telemetry, m_speechRequestId, latencyInTicks, isPhraseLatency);
    }
    else
    {
        SPX_TRACE_ERROR("%s: m_valid is false.", __FUNCTION__);
    }
}

// Callback for transport opened
void Connection::Impl::OnTransportOpened()
{
    if (m_connected)
    {
        LogError("TS:%" PRIu64 ", connection:0x%x is already connected!!!", getTimestamp(), this);
    }

    assert(m_connected == false);
    m_connected = true;
    LogInfo("TS:%" PRIu64 ", OnConnected: connection:0x%x", getTimestamp(), this);
    Invoke([&](auto callbacks) {
        callbacks->OnConnected();
    });
}

// Callback for transport closed
void Connection::Impl::OnTransportClosed(WebSocketDisconnectReason reason, const std::string& details, bool serverRequested)
{
    if (m_connected)
    {
        m_connected = false;
        LogInfo("TS:%" PRIu64 ", OnDisconnected: connection:0x%x, Reason: %d, Server Requested: %d, Details: %s",
            getTimestamp(), this, reason, serverRequested, details.c_str());
        
        auto error = ErrorInfo::FromWebSocket(serverRequested ? WebSocketError::REMOTE_CLOSED : WebSocketError::UNKNOWN, reason, details);
        auto callbacks = m_config.m_callbacks;

        Invoke([&error](auto callbacks) {
            callbacks->OnDisconnected(error);
        });
    }
}

// Callback for transport errors
void Connection::Impl::OnTransportError(const std::shared_ptr<ISpxErrorInformation>& error)
{
    const auto errorCode = error != nullptr ? error->GetCancellationCode() : CancellationErrorCode::NoError;
    const auto& errorMessage = error != nullptr ? error->GetDetails() : "";

    LogInfo("TS:%" PRIu64 ", TransportError: connection:0x%x, code=%d, string=%s",
        getTimestamp(), this, errorCode, errorMessage.c_str());

    if (m_connected)
    {
        m_connected = false;
        LogInfo("TS:%" PRIu64 ", OnDisconnected: connection:0x%x", getTimestamp(), this);
        Invoke([&error](auto callbacks) {
            callbacks->OnDisconnected(error);
        });
    }

    Invoke([&error](auto callbacks) {
        callbacks->OnError(error);
    });

    m_valid = false;
}

static RecognitionStatus ToRecognitionStatus(const string& str)
{
    if (0 == str.compare("Success")) return  RecognitionStatus::Success;
    if (0 == str.compare("NoMatch")) return  RecognitionStatus::NoMatch;
    if (0 == str.compare("InitialSilenceTimeout")) return  RecognitionStatus::InitialSilenceTimeout;
    if (0 == str.compare("BabbleTimeout")) return RecognitionStatus::InitialBabbleTimeout;
    if (0 == str.compare("Error")) return RecognitionStatus::Error;
    if (0 == str.compare("EndOfDictation")) return RecognitionStatus::EndOfDictation;
    if (0 == str.compare("TooManyRequests")) return RecognitionStatus::TooManyRequests;
    if (0 == str.compare("BadRequest")) return RecognitionStatus::BadRequest;
    if (0 == str.compare("Forbidden")) return RecognitionStatus::Forbidden;
    if (0 == str.compare("ServiceUnavailable")) return RecognitionStatus::ServiceUnavailable;

    PROTOCOL_VIOLATION("Unknown RecognitionStatus: %s", str.c_str());
    return RecognitionStatus::InvalidMessage;
}

// Function to convert keyword verification status string to KeywordVerificationStatus enum.
static KeywordVerificationStatus ToKeywordVerificationStatus(const string& str)
{
    if (0 == str.compare("Accepted")) return KeywordVerificationStatus::Accepted;
    if (0 == str.compare("Rejected")) return KeywordVerificationStatus::Rejected;

    PROTOCOL_VIOLATION("Unknown KeywordVerificationStatus: %s", str.c_str());
    return KeywordVerificationStatus::InvalidMessage;
}

static TranslationStatus ToTranslationStatus(const string& str)
{
    if (0 == str.compare("Success")) return  TranslationStatus::Success;
    if (0 == str.compare("Error")) return  TranslationStatus::Error;

    PROTOCOL_VIOLATION("Unknown TranslationStatus: %s", str.c_str());
    return TranslationStatus::InvalidMessage;
}

static ConfidenceLevel ToConfidenceLevel(const string& str)
{
    if (0 == str.compare("Unknown")) return  ConfidenceLevel::Unknown;
    if (0 == str.compare("Low")) return  ConfidenceLevel::Low;
    if (0 == str.compare("Medium")) return  ConfidenceLevel::Medium;
    if (0 == str.compare("High")) return  ConfidenceLevel::High;
    PROTOCOL_VIOLATION("Invalid ConfidenceLevel: %s", str.c_str());
    return ConfidenceLevel::InvalidMessage;
}

static string RetrievePrimaryLanguage(const nlohmann::json& json, string messagePath)
{
    string language;
    auto primaryLanguageJson = json.find(json_properties::primaryLanguage);
    if (primaryLanguageJson != json.end())
    {
        language = primaryLanguageJson->at(json_properties::lang).get<string>();
        if (language.empty())
        {
            PROTOCOL_VIOLATION("Invalid %s message, with primaryLanguage section but no language value. json = %s.", messagePath.c_str(), primaryLanguageJson->dump().c_str());
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("Got language %s from %s message.", language.c_str(), messagePath.c_str());
        }
    }

    return language;
}

static SpeechHypothesisMsg RetrieveSpeechResult(const nlohmann::json& json)
{
    auto offset = json.at(json_properties::offset).get<OffsetType>();
    auto duration = json.at(json_properties::duration).get<DurationType>();
    auto textObj = json.find(json_properties::text);
    string text;
    if (textObj != json.end())
    {
        text = json.at(json_properties::text).get<string>();
    }
    string language = RetrievePrimaryLanguage(json, "speech.hypothesis");
    return SpeechHypothesisMsg(PAL::ToWString(json.dump()), offset, duration, PAL::ToWString(text), L"", L"", move(language));
}

static TranslationResult RetrieveTranslationResult(const nlohmann::json& translationJson, bool expectStatus)
{
    TranslationResult result;
    if (expectStatus)
    {
        auto status = translationJson.find(json_properties::translationStatus);
        if (status != translationJson.end())
        {
            result.translationStatus = ToTranslationStatus(status->get<string>());
        }
        else
        {
            PROTOCOL_VIOLATION("No TranslationStatus is provided. Json: %s", translationJson.dump().c_str());
            result.translationStatus = TranslationStatus::InvalidMessage;
            result.failureReason = L"Status is missing in the protocol message. Response text:" + PAL::ToWString(translationJson.dump());
        }

        auto failure = translationJson.find(json_properties::failureReason);
        if (failure != translationJson.end())
        {
            result.failureReason += PAL::ToWString(failure->get<string>());
        }
    }

    if (expectStatus && result.translationStatus != TranslationStatus::Success)
    {
        return result;
    }
    else
    {
        auto translations = translationJson.at(json_properties::translations);
        for (const auto& object : translations)
        {
            auto lang = object.at(json_properties::lang).get<string>();
            std::string txt;
            if (object.find(json_properties::text) != object.end())
            {
                txt = object.at(json_properties::text).get<string>();
            }
            else if (object.find(json_properties::displayText) != object.end())
            {
                txt = object.at(json_properties::displayText).get<string>();
            }

            if (lang.empty() && txt.empty())
            {
                PROTOCOL_VIOLATION("empty language and text field in translations text. lang=%s, text=%s. json=%s", lang.c_str(), txt.c_str(), object.dump().c_str());
                continue;
            }

            result.translations[PAL::ToWString(lang)] = PAL::ToWString(txt);
        }

        if (!result.translations.size())
        {
            PROTOCOL_VIOLATION("No Translations text block in the message. Response text:", translationJson.dump().c_str());
        }
        return result;
    }
}

std::string GetHeadersAsString(const UspHeaders& headers)
{
    std::ostringstream oss;
    for (const auto& entry : headers)
    {
        // NOTE: for consistency with the previous implementation, I left the extra new line at the beginning
        //       of the headers string
        oss << "\r\n";
        oss << entry.first << ": " << entry.second;
    }

    return oss.str();
}

// Callback for data available on transport
void Connection::Impl::OnTransportData(bool isBinary, const UspHeaders& headers, const unsigned char* buffer, size_t bufferSize)
{
    if (buffer == nullptr)
    {
        return;
    }

    bool messageHadContinuationHeader = false;

    auto path = TryGet(headers, HEADER_PATH);
    if (path.empty())
    {
        PROTOCOL_VIOLATION("response missing '%s' header", HEADER_PATH);
        return;
    }

    try
    {
        // TODO ralphe: Should we update the message received to take the parsed unordered map of headers instead?
        Invoke([&](auto callbacks)
        {
            callbacks->OnMessageReceived({ GetHeadersAsString(headers), path, buffer, (uint32_t)bufferSize, isBinary });
        });
    }
    catch (const std::exception& ex)
    {
        // shouldn't fail because of a bad callback
        LogError("OnMessageReceived callback failed with exception: '%s'", ex.what());
    }

    string requestId = TryGet(headers, headers::requestId);
    if (!m_activeRequestIds.count(requestId))
    {
        if (requestId.empty() || path != path::turnStart)
        {
            PROTOCOL_VIOLATION("Unexpected request id '%s', Path: %s", requestId.c_str(), path.c_str());
            MetricsUnexpectedRequestId(requestId);
            return;
        }
        else
        {
            LogInfo("Service originated request received with requestId: %s", requestId.c_str());
            RegisterRequestId(requestId);
        }
    }

    std::string contentType;
    if (bufferSize != 0)
    {
        contentType = TryGet(headers, headers::contentType);
        if (contentType.empty())
        {
            LogInfo("response '%s' contains body with no content-type", path.c_str());
        }
        else
        {
            LogInfo("Response Message: content type: %s.", contentType.c_str());
        }
    }

    MetricsReceivedMessage(*m_telemetry, requestId, path);

    auto headerOffset = TryGet(headers, headers::continuationOffset);
    if (!headerOffset.empty())
    {
        Invoke([&](auto callbacks) { callbacks->OnAcknowledgedAudio(stoull(headerOffset)); });
        messageHadContinuationHeader = true;
    }

    auto token = TryGet(headers, headers::continuationToken);
    if (!token.empty())
    {
        Invoke([&](auto callbacks) { callbacks->OnToken(token); });
        messageHadContinuationHeader = true;
    }

    LogInfo("TS:%" PRIu64 " Response Message: path: %s, size: %zu.", getTimestamp(), path.c_str(), bufferSize);

    if (isBinary)
    {
        if (path == path::translationSynthesis || path == path::audio)
        {
            // streamId is optional
            auto streamId = TryGet(headers, headers::streamId);

            AudioOutputChunkMsg msg;
            if (path == path::audio && m_streamIdLangMap.size() > 0)
            {
                SPX_DBG_TRACE_VERBOSE("m_streamIdLangMap has data, will FillLanguageForAudioOutputChunkMsg");
                FillLanguageForAudioOutputChunkMsg(streamId, path, msg);
            }
            msg.requestId = requestId;
            msg.audioBuffer = (uint8_t *)buffer;
            msg.audioLength = bufferSize;

            Invoke([&](auto callbacks) { callbacks->OnAudioOutputChunk(msg); });
        }
        else
        {
            PROTOCOL_VIOLATION("Binary frame received with unexpected path: %s", path.c_str());
        }
    }
    else
    {
        auto json = (bufferSize > 0) ? nlohmann::json::parse(buffer, buffer + bufferSize) : nlohmann::json();
        if (path ==  path::speechStartDetected || path == path::speechEndDetected)
        {
            auto offsetObj = json[json_properties::offset];
            // For whatever reason, offset is sometimes missing on the end detected message.
            auto offset = offsetObj.is_null() ? 0 : offsetObj.get<OffsetType>();

            if (path == path::speechStartDetected)
            {
                Invoke([&](auto callbacks) { callbacks->OnSpeechStartDetected({PAL::ToWString(json.dump()), offset}); });
            }
            else
            {
               // Not sure this doesn't belong here....
               // connection->Invoke([&] { callbacks->OnAcknowledgedAudio(offset); });
                Invoke([&](auto callbacks) { callbacks->OnSpeechEndDetected({PAL::ToWString(json.dump()), offset}); });
            }
        }
        else if (path == path::turnStart)
        {
            auto tag = json[json_properties::context][json_properties::tag].get<string>();

            m_turnUsingHeaders = messageHadContinuationHeader;

            if (requestId == m_speechRequestId)
            {
                /* We know this request id is related to a speech turn */
                Invoke([&](auto callbacks) { callbacks->OnTurnStart({ PAL::ToWString(json.dump()), tag, requestId, m_turnUsingHeaders }); });
            }
            else
            {
                /* We know this request id is a server initiated request */
                Invoke([&](auto callbacks) { callbacks->OnMessageStart({ PAL::ToWString(json.dump()), tag, requestId, m_turnUsingHeaders }); });
            }
        }
        else if (path == path::turnEnd)
        {
            {
                m_activeRequestIds.erase(requestId);
                SPX_DBG_TRACE_VERBOSE("Got turn end, clear m_streamIdLangMap.");
                m_streamIdLangMap.clear();
            }

            // flush the telemetry before invoking the onTurnEnd callback.
            // TODO: 1164154
            m_telemetry->Flush(requestId);

            if (requestId == m_speechRequestId)
            {
                m_speechRequestId.clear();
                m_speechContextMessageAllowed = true;
                Invoke([&](auto callbacks) { callbacks->OnTurnEnd({ requestId }); });
            }
            else
            {
                Invoke([&](auto callbacks) { callbacks->OnMessageEnd({ requestId }); });
            }
        }
        else if (path == path::speechKeyword)
        {
            auto status = ToKeywordVerificationStatus(json[json_properties::status].get<string>());
            auto offsetObj = json[json_properties::offset];
            auto offset = offsetObj.is_null()? 0 : offsetObj.get<OffsetType>();
            auto durationObj = json[json_properties::duration];
            auto duration = durationObj.is_null()? 0 : durationObj.get<DurationType>();
            auto textObj = json[json_properties::text];
            auto text = textObj.is_null()? "" : textObj.get<string>();

            Invoke([&](auto callbacks) {
                callbacks->OnSpeechKeywordDetected({PAL::ToWString(json.dump()),
                                                    offset,
                                                    duration,
                                                    status,
                                                    PAL::ToWString(text)});
            });
        }
        else if (path == path::speechHypothesis || path == path::speechFragment)
        {
            auto offset = json[json_properties::offset].get<OffsetType>();
            auto duration = json[json_properties::duration].get<DurationType>();
            auto text = json[json_properties::text].get<string>();
            std::string speaker;
            if (json.find(json_properties::speaker) != json.end())
            {
                speaker = json[json_properties::speaker].get<string>();
            }
            std::string id;
            if (json.find(json_properties::id) != json.end())
            {
                id = json[json_properties::id].get<string>();
            }
            auto language = RetrievePrimaryLanguage(json, path);
            if (path == path::speechHypothesis)
            {
                Invoke([&](auto callbacks) {
                    callbacks->OnSpeechHypothesis({PAL::ToWString(json.dump()),
                                                   offset,
                                                   duration,
                                                   PAL::ToWString(text),
                                                   PAL::ToWString(speaker),
                                                   PAL::ToWString(id),
                                                   move(language)});
                });
            }
            else
            {
                Invoke([&](auto callbacks) {
                    callbacks->OnSpeechFragment({ PAL::ToWString(json.dump()),
                                                 offset,
                                                 duration,
                                                 PAL::ToWString(text),
                                                 PAL::ToWString(speaker),
                                                 PAL::ToWString(id),
                                                 move(language)});
                });
            }
        }
        else if (path == path::speechPhrase)
        {
            SpeechPhraseMsg result = RetrieveSpeechPhraseResult(json);
            if (isErrorRecognitionStatus(result.recognitionStatus))
            {
                InvokeRecognitionErrorCallback(result.recognitionStatus, json.dump());
            }
            else
            {
                Invoke([&](auto callbacks) { callbacks->OnSpeechPhrase(result); });
            }

            if (!m_turnUsingHeaders)
            {
                // Tell our site we're done with the audio.
                Invoke([&](auto callbacks) { callbacks->OnAcknowledgedAudio(result.offset + result.duration); });
            }
        }
        else if (path == path::translationHypothesis)
        {
            auto speechResult = RetrieveSpeechResult(json);
            auto translationResult = RetrieveTranslationResult(json[json_properties::translation], false);
            // TranslationStatus is always success for translation.hypothesis
            translationResult.translationStatus = TranslationStatus::Success;

            Invoke([&](auto callbacks) {
                callbacks->OnTranslationHypothesis({move(speechResult.json),
                                                    speechResult.offset,
                                                    speechResult.duration,
                                                    move(speechResult.text),
                                                    move(translationResult)});
            });
        }
        else if (path == path::translationPhrase)
        {
            auto status = ToRecognitionStatus(json.at(json_properties::recoStatus));
            if (isErrorRecognitionStatus(status))
            {
                // There is an error in speech recognition, fire an error event.
                InvokeRecognitionErrorCallback(status, json.dump());
            }
            else
            {
                auto speechResult = RetrieveSpeechResult(json);
                TranslationResult translationResult;
                if (status == RecognitionStatus::Success)
                {
                    translationResult = RetrieveTranslationResult(json[json_properties::translation], true);
                }
                else
                {
                    translationResult.translationStatus = TranslationStatus::Success;
                }
                // There is no speech recognition error, we fire a translation phrase event.
                Invoke([&](auto callbacks) {
                    callbacks->OnTranslationPhrase({move(speechResult.json),
                                                    speechResult.offset,
                                                    speechResult.duration,
                                                    move(speechResult.text),
                                                    move(translationResult),
                                                    status});
                });

                if (!m_turnUsingHeaders)
                {
                    // Tell our site we're done with the audio.
                    Invoke([&](auto callbacks) { callbacks->OnAcknowledgedAudio(speechResult.offset + speechResult.duration); });
                }
            }
        }
        else if (path == path::translationSynthesisEnd)
        {
            string failureReason;
            bool synthesisSuccess = false;

            auto statusHandle = json.find(json_properties::synthesisStatus);
            if (statusHandle != json.end())
            {
                auto synthesisStatus = statusHandle->get<string>();
                if (synthesisStatus != "Success" && synthesisStatus != "Error")
                {
                    PROTOCOL_VIOLATION("Invalid synthesis status in synthesis.end message. Json=%s", json.dump().c_str());
                    failureReason = "Invalid synthesis status in synthesis.end message.";
                }
                synthesisSuccess = synthesisStatus == "Success";
            }
            else
            {
                PROTOCOL_VIOLATION("No synthesis status in synthesis.end message. Json=%s", json.dump().c_str());
                failureReason = "No synthesis status in synthesis.end message.";
            }

            auto failureHandle = json.find(json_properties::failureReason);
            if (failureHandle != json.end())
            {
                if (synthesisSuccess)
                {
                    PROTOCOL_VIOLATION("FailureReason should be empty if SynthesisStatus is success. Json=%s", json.dump().c_str());
                }
                failureReason = failureReason + failureHandle->get<string>();
            }

            if (synthesisSuccess)
            {
                AudioOutputChunkMsg msg;
                msg.audioBuffer = NULL;
                msg.audioLength = 0;
                msg.requestId = requestId;
                Invoke([&](auto callbacks) { callbacks->OnAudioOutputChunk(msg); });
            }
            else
            {
                auto error = ErrorInfo::FromExplicitError(CancellationErrorCode::ServiceError, failureReason);
                Invoke([&](auto callbacks) { callbacks->OnError(error); });
            }
        }
        else if (path == path::translationResponse)
        {
            if (json.find(json_properties::speechHypothesis) != json.end())
            {
                SPX_DBG_TRACE_INFO("Got translation response for %s.", json_properties::speechHypothesis);
                auto speechHypothesisJson = json[json_properties::speechHypothesis];
                auto speechHypothesisMsg = RetrieveSpeechResult(speechHypothesisJson);
                auto translationResult = RetrieveTranslationResult(json, true);
                if (translationResult.translationStatus != TranslationStatus::Success)
                {
                    PROTOCOL_VIOLATION("Translation result for hypothesis should always have success status, otherwise, service shouldn't have sent it. Json=%s", json.dump().c_str());
                }
                else
                {
                    Invoke([&](auto callbacks) {
                        callbacks->OnTranslationHypothesis({PAL::ToWString(json.dump()),
                                                            speechHypothesisMsg.offset,
                                                            speechHypothesisMsg.duration,
                                                            move(speechHypothesisMsg.text),
                                                            move(translationResult),
                                                            move(speechHypothesisMsg.language)});
                    });
                }
            }
            else if (json.find(json_properties::speechPhrase) != json.end())
            {
                SPX_DBG_TRACE_INFO("Got translation response for %s.", json_properties::speechPhrase);
                auto speechPhraseJson = json[json_properties::speechPhrase];
                auto status = ToRecognitionStatus(speechPhraseJson.at(json_properties::recoStatus));
                if (isErrorRecognitionStatus(status))
                {
                    // There is an error in speech recognition, fire an error event.
                    InvokeRecognitionErrorCallback(status, json.dump());
                }
                else
                {
                    auto speechPhraseMsg = RetrieveSpeechPhraseResult(speechPhraseJson);
                    TranslationResult translationResult;
                    if (status == RecognitionStatus::Success)
                    {
                        translationResult = RetrieveTranslationResult(json, true);
                    }
                    else
                    {
                        translationResult.translationStatus = TranslationStatus::Success;
                    }
                    // There is no speech recognition error, we fire a translation phrase event.
                    Invoke([&](auto callbacks) {
                        callbacks->OnTranslationPhrase({PAL::ToWString(json.dump()),
                                                        speechPhraseMsg.offset,
                                                        speechPhraseMsg.duration,
                                                        move(speechPhraseMsg.displayText),
                                                        move(translationResult),
                                                        status,
                                                        move(speechPhraseMsg.language),
                                                        speechPhraseMsg.languageDetectionConfidence});
                    });


                    if (!m_turnUsingHeaders)
                    {
                        // Tell our site we're done with the audio.
                        Invoke([&](auto callbacks) { callbacks->OnAcknowledgedAudio(speechPhraseMsg.offset + speechPhraseMsg.duration); });
                    }
                }
            }
            else
            {
                 PROTOCOL_VIOLATION("Invalid translation response, no %s or %s data. Json=%s",
                     json_properties::speechPhrase,
                     json_properties::speechHypothesis,
                     json.dump().c_str());
            }
        }
        else if (path == path::audioMetaData)
        {
            AudioOutputMetadataMsg msg;
            msg.requestId = requestId;
            msg.size = bufferSize;

            auto metadatas = json[json_properties::metadata];
            if (metadatas.is_array())
            {
                auto metadataIterator = metadatas.begin();
                while (metadataIterator != metadatas.end())
                {
                    auto metadata = metadataIterator.value();
                    auto metadataType = metadata[json_properties::type];
                    if (metadataType.get<std::string>() == json_properties::wordBoundary)
                    {
                        AudioOutputMetadata audioOutputMetadata;
                        audioOutputMetadata.type = json_properties::wordBoundary;

                        auto metadataData = metadata[json_properties::data];
                        if (metadataData.is_structured())
                        {
                            auto offset = metadataData[json_properties::offset];
                            audioOutputMetadata.textBoundary.audioOffset = offset.get<OffsetType>();

                            auto text = metadataData[json_properties::lowerText];
                            if (text.is_structured())
                            {
                                audioOutputMetadata.textBoundary.text = PAL::ToWString(text[json_properties::text].get<string>());
                            }
                        }

                        msg.metadatas.emplace_back(audioOutputMetadata);
                    }

                    metadataIterator++;
                }
            }

            Invoke([&](auto callbacks) { callbacks->OnAudioOutputMetadata(msg); });
        }
        else if (path == path::audioStart)
        {
            auto streamId = TryGet(headers, headers::streamId);
            if (streamId.empty())
            {
                 PROTOCOL_VIOLATION("No stream id in %s header", path.c_str());
            }
            else if (json.find(json_properties::translationLanguage) == json.end())
            {
                // TODO: in future, audio.start may be used in other scenario,  this validation logic need to update accordingly.
                PROTOCOL_VIOLATION("Cannot find TranslationLanguage in audio.start message. Json=%s", json.dump().c_str());
            }
            else
            {
                auto language = json[json_properties::translationLanguage].get<string>();
                LogInfo("Got streamId %s to language %s map.  current m_streamIdLangMap size = %d", streamId.c_str(), language.c_str(), m_streamIdLangMap.size());
                // This is a protection logic to avoid memory usage increases due to service error.
                // So far we haven't any scenario that needs more than 10 synthesising languages
                // TODO: remove this protection code after the new service endpoint is stable
                const int MaxLanguages = 10;
                if (m_streamIdLangMap.size() > MaxLanguages)
                {
                    PROTOCOL_VIOLATION("We have got more than % audio.start messages for different languages, service is sending too many such messages.", MaxLanguages);
                }
                else
                {
                    m_streamIdLangMap[streamId] = language;
                }
            }
        }
        else if (path == path::audioEnd)
        {
            string failureReason;
            auto status = json[json_properties::status].get<string>();
            if (status != "Success" && status != "Error")
            {
                failureReason = "Invalid status in audio.end message.";
                PROTOCOL_VIOLATION("%s Json=%s", failureReason.c_str(), json.dump().c_str());
            }
            bool isSuccess = status == "Success";
            if (isSuccess)
            {
                AudioOutputChunkMsg msg;
                msg.audioBuffer = NULL;
                msg.audioLength = 0;
                msg.requestId = requestId;
                auto streamId = TryGet(headers, headers::streamId);
                FillLanguageForAudioOutputChunkMsg(streamId, path, msg);
                Invoke([&](auto callbacks) { callbacks->OnAudioOutputChunk(msg); });
            }
            else
            {
                failureReason = failureReason + json[json_properties::failureReason].get<string>();
                auto error = ErrorInfo::FromExplicitError(CancellationErrorCode::ServiceError, failureReason);
                Invoke([&](auto callbacks) { callbacks->OnError(error); });
            }
        }
        else
        {
            Invoke([&](auto callbacks) {
                callbacks->OnUserMessage({path,
                                          contentType,
                                          requestId,
                                          buffer,
                                          bufferSize});
            });
        }
    }
}

void Connection::Impl::InvokeRecognitionErrorCallback(RecognitionStatus status, const string& response)
{
    auto callbacks = m_config.m_callbacks;
    auto error = ErrorInfo::FromRecognitionStatus(status, response);

    this->Invoke([&](auto callbacks) { callbacks->OnError(error); });
}

SpeechPhraseMsg Connection::Impl::RetrieveSpeechPhraseResult(const nlohmann::json& json)
{
    SpeechPhraseMsg result;
    result.json = PAL::ToWString(json.dump());
    result.offset = json[json_properties::offset].get<OffsetType>();
    result.duration = json[json_properties::duration].get<DurationType>();
    result.recognitionStatus = ToRecognitionStatus(json[json_properties::recoStatus].get<string>());
    if (json.find(json_properties::speaker) != json.end())
    {
        result.speaker = PAL::ToWString(json[json_properties::speaker].get<string>());
    }
    if (json.find(json_properties::id) != json.end())
    {
        result.id = PAL::ToWString(json[json_properties::id].get<string>());
    }
    if (result.recognitionStatus == RecognitionStatus::Success)
    {
        if (json.find(json_properties::displayText) != json.end())
        {
            // The DisplayText field will be present only if the RecognitionStatus field has the value Success.
            // and the format output is simple
            result.displayText = PAL::ToWString(json[json_properties::displayText].get<string>());
        }
        else // Detailed
        {
            // The service returns sorted n-best results and the first result is the best one.
            // Use the first one as the default text, irrespective of the confidence value.
            auto phrases = json.at(json_properties::nbest);
            if (!phrases.empty())
            {
                result.displayText = PAL::ToWString(phrases[0].at(json_properties::display).get<string>());
            }
        }
    }

    auto primaryLanguageJson = json.find(json_properties::primaryLanguage);
    if (primaryLanguageJson != json.end())
    {
        result.language = primaryLanguageJson->at(json_properties::lang).get<string>();
        auto confidence = primaryLanguageJson->at(json_properties::confidence).get<string>();
        if (result.language.empty() || confidence.empty())
        {
            PROTOCOL_VIOLATION("Invalid language detection response. language = %s and confidence = %s should both have values. Json = %s",
                result.language.c_str(),
                confidence.c_str(),
                primaryLanguageJson->dump().c_str());
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("Got language %s and confidence %s from speech phrase message.", result.language.c_str(), confidence.c_str());
            result.languageDetectionConfidence = ToConfidenceLevel(confidence);
        }
    }
    return result;
}

bool Connection::Impl::isErrorRecognitionStatus(RecognitionStatus status)
{
    switch (status)
    {
    case RecognitionStatus::Success:
    case RecognitionStatus::InitialSilenceTimeout:
    case RecognitionStatus::InitialBabbleTimeout:
    case RecognitionStatus::NoMatch:
    case RecognitionStatus::EndOfDictation:
        return false;
    default:
        return true;
    }
}

void Connection::Impl::FillLanguageForAudioOutputChunkMsg(const std::string& streamId, const std::string& messagePath, AudioOutputChunkMsg& msg)
{
    if (streamId.empty())
    {
        PROTOCOL_VIOLATION("%s message is received but doesn't have streamId in header.", messagePath.c_str());
    }
    else if (m_streamIdLangMap.count(streamId) == 0)
    {
        PROTOCOL_VIOLATION(
            "%s message is received but cannot find streamId %s from streamId to language map, may be caused by audio.start message not being received before this message.",
            messagePath.c_str(),
            streamId.c_str());
    }
    else
    {
        msg.language = m_streamIdLangMap.at(streamId);
        if (messagePath == path::audioEnd)
        {
            SPX_DBG_TRACE_VERBOSE("Got audio end, remove %s from m_streamIdLangMap.", streamId.c_str());
            m_streamIdLangMap.erase(streamId);
        }
    }
}

}}}}

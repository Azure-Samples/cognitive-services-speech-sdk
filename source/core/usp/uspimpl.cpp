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

#include "uspcommon.h"
#include "uspinternal.h"

#include "transport.h"
#include "dnscache.h"
#include "metrics.h"

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

const char* endpoint::protocol = "wss://";

const char* endpoint::outputFormatQueryParam = "format=";
const char* endpoint::langQueryParam = "language=";
const char* endpoint::deploymentIdQueryParam = "cid=";
const char* endpoint::profanityQueryParam = "profanity=";
const char* endpoint::initialSilenceTimeoutQueryParam = "initialSilenceTimeoutMs=";
const char* endpoint::endSilenceTimeoutQueryParam = "endSilenceTimeoutMs=";
const char* endpoint::stableIntermediateThresholdQueryParam = "stableIntermediateThreshold=";
const char* endpoint::storeAudioQueryParam = "storeAudio=";
const char* endpoint::wordLevelTimestampsQueryParam = "wordLevelTimestamps=";

const char* endpoint::outputFormatSimple = "simple";
const char* endpoint::outputFormatDetailed = "detailed";

const char* endpoint::postProcessingTrueText = "TrueText";

const char* endpoint::profanityMasked = "masked";
const char* endpoint::profanityRemoved = "removed";
const char* endpoint::profanityRaw = "raw";

const char* endpoint::unifiedspeech::hostnameSuffix = ".stt.speech.microsoft.com";
const char* endpoint::unifiedspeech::pathPrefix = "/speech/recognition/";
const char* endpoint::unifiedspeech::pathSuffix = "/cognitiveservices/v1";

const char* endpoint::unifiedspeech::postprocessingQueryParam = "postprocessing=";
const char* endpoint::unifiedspeech::lidEnabledQueryParam = "lidEnabled=";

const char* endpoint::azurecnspeech::hostnameSuffix = ".stt.speech.azure.cn";
const char* endpoint::azurecntranslation::hostnameSuffix = ".s2s.speech.azure.cn";
const char* endpoint::azurecnspeechsynthesis::hostnameSuffix = ".tts.speech.azure.cn";
const char* azurecnRegion = "china";

enum class NetworkType
{
    Default,
    AzureCN
};

const std::vector<std::string> endpoint::unifiedspeech::queryParameters = {

    endpoint::langQueryParam,
    endpoint::deploymentIdQueryParam,
    endpoint::initialSilenceTimeoutQueryParam,
    endpoint::endSilenceTimeoutQueryParam,
    endpoint::storeAudioQueryParam,

    endpoint::outputFormatQueryParam,
    endpoint::wordLevelTimestampsQueryParam,
    endpoint::profanityQueryParam,
    endpoint::stableIntermediateThresholdQueryParam,
    endpoint::unifiedspeech::postprocessingQueryParam,

    lidEnabledQueryParam
};

const char* endpoint::translation::hostnameSuffix = ".s2s.speech.microsoft.com";
const char* endpoint::translation::path = "/speech/translation/cognitiveservices/v1";

const char* endpoint::translation::fromQueryParam = "from=";
const char* endpoint::translation::toQueryParam = "to=";
const char* endpoint::translation::voiceQueryParam = "voice=";
const char* endpoint::translation::featuresQueryParam = "features=";
const char* endpoint::translation::stableTranslationQueryParam = "stableTranslation=";

const char* endpoint::translation::requireVoice = "texttospeech";

const std::vector<std::string> endpoint::translation::queryParameters = {

    endpoint::translation::fromQueryParam,
    endpoint::translation::toQueryParam,
    endpoint::translation::voiceQueryParam,

    endpoint::deploymentIdQueryParam,
    endpoint::initialSilenceTimeoutQueryParam,
    endpoint::endSilenceTimeoutQueryParam,
    endpoint::storeAudioQueryParam,

    endpoint::outputFormatQueryParam,
    endpoint::wordLevelTimestampsQueryParam,
    endpoint::profanityQueryParam,
    endpoint::stableIntermediateThresholdQueryParam,

    endpoint::translation::stableTranslationQueryParam
};

const char* endpoint::luis::hostname = ".sr.speech.microsoft.com";
const char* endpoint::luis::pathPrefix = "/speech/recognition/";
const char* endpoint::luis::pathSuffix = "/cognitiveservices/v1";

const std::vector<std::string> endpoint::luis::queryParameters = {
    endpoint::langQueryParam,
    endpoint::outputFormatQueryParam
};

const char* endpoint::dialog::url = ".convai.speech.microsoft.com";

const char* endpoint::dialog::resourcePath::botFramework = "";
const char* endpoint::dialog::resourcePath::customCommands = "/commands";

const char* endpoint::dialog::suffix = "/api";

const char* endpoint::dialog::version::botFramework = "/v3";
const char* endpoint::dialog::version::customCommands = "/v1";

const char* endpoint::dialog::customVoiceDeploymentIdsQueryParam = "voiceDeploymentId=";

const std::vector<std::string> endpoint::dialog::queryParameters = {
    endpoint::langQueryParam,
    endpoint::dialog::customVoiceDeploymentIdsQueryParam
};

const char* endpoint::conversationTranscriber::hostname = ".cts.speech.microsoft.com";
const char* endpoint::conversationTranscriber::pathPrefix1 = "transcribe.";
const char* endpoint::conversationTranscriber::pathPrefix2 = "/speech/recognition";
const char* endpoint::conversationTranscriber::pathSuffixMultiAudio = "/multiaudio";

const std::vector<std::string> endpoint::conversationTranscriber::queryParameters = {
    endpoint::langQueryParam,
    endpoint::outputFormatQueryParam
};

const char* endpoint::speechSynthesis::hostnameSuffix = ".tts.speech.microsoft.com";
const char* endpoint::speechSynthesis::path = "/cognitiveservices/websocket/v1";

const std::vector<std::string> endpoint::speechSynthesis::queryParameters = { };


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

const string g_recoModeStrings[] = { "interactive", "conversation", "dictation" };

// This is called from telemetry_flush, invoked on a worker thread in turn-end.
void Connection::Impl::OnTelemetryData(const uint8_t* buffer, size_t bytesToWrite, void *context, const char *requestId)
{
    Connection::Impl *connection = (Connection::Impl*)context;
    TransportWriteTelemetry(connection->m_transport.get(), buffer, bytesToWrite, requestId);
}


Connection::Impl::Impl(const Client& config)
    : m_config(config),
    m_valid(false),
    m_connected(false),
    m_speechContextMessageAllowed(true),
    m_audioOffset(0),
    m_creationTime(telemetry_gettime())
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

void Connection::Impl::Invoke(function<void()> callback)
{
    if (!m_valid)
    {
        return;
    }

    callback();
}

void Connection::Impl::ScheduleWork()
{
    // Reschedule to make sure we do not reenter the transport
    // from one of its callbacks.
    auto keepAlive = shared_from_this();
    packaged_task<void()> task([keepAlive]() { DoWork(keepAlive); });
    m_threadService->ExecuteAsync(move(task));
}

void Connection::Impl::WorkLoop(shared_ptr<Connection::Impl> ptr)
{
    packaged_task<void()> task([ptr]()
    {
        if (!ptr->m_valid)
            return;

        DoWork(ptr);

        packaged_task<void()> task([ptr]() { WorkLoop(ptr); });
        ptr->m_threadService->ExecuteAsync(move(task), chrono::milliseconds(ptr->m_config.m_pollingIntervalms));

    });

    ptr->m_threadService->ExecuteAsync(move(task));
}

void Connection::Impl::DoWork(weak_ptr<Connection::Impl> ptr)
{
    try
    {
        auto connection = ptr.lock();
        if (connection == nullptr)
        {
            return;
        }

        if (!connection->m_valid)
        {
            return;
        }

        auto callbacks = connection->m_config.m_callbacks;

        try
        {
            TransportDoWork(connection->m_transport.get());
        }
        catch (const exception& e)
        {
            connection->Invoke([&] { callbacks->OnError(false, ErrorCode::RuntimeError, e.what()); });
        }
        catch (...)
        {
            connection->Invoke([&] { callbacks->OnError(false, ErrorCode::RuntimeError, "Unhandled exception in the USP layer."); });
        }
    }
    catch (const exception& ex)
    {
        (void)ex; // release builds
        LogError("%s Unexpected Exception %s. Thread terminated", __FUNCTION__, ex.what());
    }
    catch (...)
    {
        LogError("%s Unexpected Exception. Thread terminated", __FUNCTION__);
    }
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

string Connection::Impl::EncodeParameterString(const string& parameter) const
{
    STRING_HANDLE encodedHandle = URL_EncodeString(parameter.c_str());
    string encodedStr(STRING_c_str(encodedHandle));
    STRING_delete(encodedHandle);
    return encodedStr;
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
                    if (queryParameterName == endpoint::translation::toQueryParam)
                    {
                        auto langVector = PAL::split(entry->second, CommaDelim);
                        for (auto item : langVector)
                        {
                            oss << queryParameterDelim << endpoint::translation::toQueryParam << EncodeParameterString(item);
                        }
                    }
                    // Voice need 2 query parameters.
                    else if (queryParameterName == endpoint::translation::voiceQueryParam)
                    {
                        oss << queryParameterDelim << endpoint::translation::featuresQueryParam << endpoint::translation::requireVoice;
                        oss << queryParameterDelim << endpoint::translation::voiceQueryParam << EncodeParameterString(entry->second);
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
        BuildQueryParameters(endpoint::dialog::queryParameters, m_config.m_queryParameters, customEndpoint, oss);
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

void Connection::Impl::BuildQueryParameters(const vector<string>& parameterList, const unordered_map<string, string>& valueMap, bool isCustomEndpoint, ostringstream& oss) const
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

void Connection::Impl::Connect()
{
    LogInfo("%s: entering...", __FUNCTION__);
    if (m_transport != nullptr || m_valid)
    {
        ThrowLogicError("USP connection already created.");
    }

    using HeadersPtr = deleted_unique_ptr<remove_pointer<HTTP_HEADERS_HANDLE>::type>;

    HeadersPtr connectionHeaders(HTTPHeaders_Alloc(), HTTPHeaders_Free);

    if (connectionHeaders == nullptr)
    {
        ThrowRuntimeError("Failed to create connection headers.");
    }

    auto headersPtr = connectionHeaders.get();

    if (!m_config.m_audioResponseFormat.empty())
    {
        HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::audioResponseFormat, m_config.m_audioResponseFormat.c_str());
    }

    // Set authentication headers.
    auto& authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::SubscriptionKey)];
    if (!authStr.empty())
    {
        LogInfo("Adding subscription key headers");
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::ocpApimSubscriptionKey, authStr.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set authentication using subscription key.");
        }
    }
    authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::AuthorizationToken)];
    if (!authStr.empty())
    {
        LogInfo("Adding authorization token headers");
        auto token = "Bearer " + authStr;
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::authorization, token.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set authentication using authorization token.");
        }
    }
    authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::SearchDelegationRPSToken)];
    if (!authStr.empty())
    {
        LogInfo("Adding search delegation RPS token.");
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::searchDelegationRPSToken, authStr.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set authentication using Search-DelegationRPSToken.");
        }
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
            if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headerName, authStr.c_str()) != 0)
            {
                ThrowRuntimeError("Failed to set authentication using Dialog auth header.");
            }
        }
    }
    authStr = m_config.m_authData[static_cast<size_t>(AuthenticationType::ConversationToken)];
    if (!authStr.empty())
    {
        LogInfo("Adding conversation token.");
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::capitoConversationToken, authStr.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set the conversation token header.");
        }
    }

    if (m_config.m_endpointType == EndpointType::Dialog)
    {
        auto& region = m_config.m_region;
        if (!region.empty())
        {
            LogInfo("Adding region header");
            if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::region, region.c_str()) != 0)
            {
                ThrowRuntimeError("Failed to set region.");
            }
        }
    }

    m_connectionUrl = ConstructConnectionUrl();
    LogInfo("connectionUrl=%s", m_connectionUrl.c_str());

    m_telemetry = make_unique<Telemetry>(Connection::Impl::OnTelemetryData, this);
    if (m_telemetry == nullptr)
    {
        ThrowRuntimeError("Failed to create telemetry instance.");
    }

    string connectionId = PAL::ToString(m_config.m_connectionId);

    // Log the device uuid
    MetricsDeviceStartup(*m_telemetry, connectionId, PAL::DeviceUuid());

    bool disable_default_verify_paths = false;
    const char *trustedCert = nullptr;
    bool disable_crl_check = false;

#ifdef SPEECHSDK_USE_OPENSSL
    if (!m_config.m_trustedCert.empty())
    {
        disable_default_verify_paths = true;
        trustedCert = m_config.m_trustedCert.c_str();
        disable_crl_check = m_config.m_disable_crl_check;
    }
#endif

    m_transport = TransportRequestCreate(
        m_connectionUrl,
        this,
        m_telemetry.get(),
        headersPtr,
        connectionId,
        m_config.m_proxyServerInfo.get(),
        disable_default_verify_paths,
        trustedCert,
        disable_crl_check);

    if (m_transport == nullptr)
    {
        ThrowRuntimeError("Failed to create transport request.");
    }

#ifdef __linux__
    m_dnsCache = DnsCachePtr(DnsCacheCreate(), DnsCacheDestroy);
    if (!m_dnsCache)
    {
        ThrowRuntimeError("Failed to create DNS cache.");
    }
#else
    m_dnsCache = nullptr;
#endif

    TransportSetDnsCache(m_transport.get(), m_dnsCache.get());
    TransportSetCallbacks(m_transport.get(), OnTransportError, OnTransportData, OnTransportOpened, OnTransportClosed);

    m_valid = true;
    WorkLoop(shared_from_this());
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

void Connection::Impl::QueueMessage(const string& path, const uint8_t *data, size_t size, MessageType messageType, const string& requestId, bool binary)
{
    throw_if_null(data, "message payload is null");

    if (path.empty())
    {
        ThrowInvalidArgumentException("The path is null or empty.");
    }

    if (m_valid)
    {
        // According to USP protocol, speech.context must be sent before any audio in a turn, and
        // only one speech.context message is allowed in the same turn.
        if (messageType == MessageType::Context)
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
        if (!requestId.empty() && path == "synthesis.context")
        {
            m_speechRequestId = requestId;
        }
        auto usedRequestId = requestId.empty() ? UpdateRequestId(messageType) : requestId;
        (void)TransportMessageWrite(m_transport.get(), path.c_str(), data, size, usedRequestId.c_str(), binary);
    }

    ScheduleWork();
}

string Connection::Impl::UpdateRequestId(const MessageType messageType)
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
            // For other services, speech.event must be associated to an ongoing speech trun. Only speech.context or audio can kick-off a new turn.
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
        requestId = CreateRequestId();
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

    int ret = 0;

    if (m_audioOffset == 0)
    {
        // The service uses the first audio message that contains a unique request identifier to signal the start of a new request/response cycle or turn.
        // After receiving an audio message with a new request identifier, the service discards any queued or unsent messages
        // that are associated with any previous turn.
        m_speechRequestId = m_speechRequestId.empty() ? CreateRequestId() : m_speechRequestId;
        LogInfo("The current speech request id is %s", m_speechRequestId.c_str());

        MetricsAudioStreamInit();
        MetricsAudioStart(*m_telemetry, m_speechRequestId);

        ret = TransportStreamPrepare(m_transport.get());
        if (ret != 0)
        {
            ThrowRuntimeError("TransportStreamPrepare failed. error=" + to_string(ret));
        }
    }

    ret = TransportStreamWrite(m_transport.get(), path::audio, audioChunk, m_speechRequestId.c_str());
    if (ret != 0)
    {
        ThrowRuntimeError("TransportStreamWrite failed. error=" + to_string(ret));
    }

    m_audioOffset += size;
    ScheduleWork();
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

    auto ret = TransportStreamFlush(m_transport.get(), path::audio, m_speechRequestId.c_str());

    m_audioOffset = 0;
    MetricsAudioStreamFlush();
    MetricsAudioEnd(*m_telemetry, m_speechRequestId);

    if (ret != 0)
    {
        ThrowRuntimeError("Returns failure, reason: TransportStreamFlush returned " + to_string(ret));
    }

    ScheduleWork();
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
void Connection::Impl::OnTransportOpened(void* context)
{
    Connection::Impl *connection = static_cast<Connection::Impl*>(context);
    if (connection == nullptr)
    {
        ThrowRuntimeError("Invalid USP connection.");
    }
    if (connection->m_connected)
    {
        LogError("TS:%" PRIu64 ", connection:0x%x is already connected!!!", connection->getTimestamp(), connection);
    }
    assert(connection->m_connected == false);
    connection->m_connected = true;
    LogInfo("TS:%" PRIu64 ", OnConnected: connection:0x%x", connection->getTimestamp(), connection);
    auto callbacks = connection->m_config.m_callbacks;
    connection->Invoke([&] {
            callbacks->OnConnected();
    });
}

// Callback for transport closed
void Connection::Impl::OnTransportClosed(void* context)
{
    Connection::Impl *connection = static_cast<Connection::Impl*>(context);
    if (connection == nullptr)
    {
        ThrowRuntimeError("Invalid USP connection.");
    }
    if (connection->m_connected)
    {
        connection->m_connected = false;
        LogInfo("TS:%" PRIu64 ", OnDisconnected: connection:0x%x", connection->getTimestamp(), connection);
        auto callbacks = connection->m_config.m_callbacks;
        connection->Invoke([&] {
            callbacks->OnDisconnected();
        });
    }
}

// Callback for transport errors
void Connection::Impl::OnTransportError(TransportErrorInfo* errorInfo, void* context)
{
    throw_if_null(context, "context");

    Connection::Impl *connection = static_cast<Connection::Impl*>(context);

    auto errorStr = (errorInfo->errorString != nullptr) ? errorInfo->errorString : "";
    LogInfo("TS:%" PRIu64 ", TransportError: connection:0x%x, reason=%d, code=%d [0x%08x], string=%s",
        connection->getTimestamp(), connection, errorInfo->reason, errorInfo->errorCode, errorInfo->errorCode, errorStr);

    auto callbacks = connection->m_config.m_callbacks;
    USP::ErrorCode uspErrorCode = ErrorCode::ConnectionError;
    string errorMessage;
    auto errorCodeInString = to_string(errorInfo->errorCode);

    switch (errorInfo->reason)
    {
    case TRANSPORT_ERROR_REMOTE_CLOSED:
        uspErrorCode = ErrorCode::ConnectionError;
        errorMessage = "Connection was closed by the remote host. Error code: " + errorCodeInString + ". Error details: " + errorStr;
        break;

    case TRANSPORT_ERROR_CONNECTION_FAILURE:
        uspErrorCode = ErrorCode::ConnectionError;
        errorMessage = "Connection failed (no connection to the remote host). Internal error: " + errorCodeInString
            + ". Error details: " + errorStr
            + ". Please check network connection, firewall setting, and the region name used to create speech factory.";
        break;

    case TRANSPORT_ERROR_WEBSOCKET_UPGRADE:
        switch (errorInfo->errorCode)
        {
        case HTTP_BADREQUEST:
            uspErrorCode = ErrorCode::BadRequest;
            errorMessage = "WebSocket Upgrade failed with a bad request (400). Please check the language name and endpoint id (if used) are correctly associated with the provided subscription key.";
            break;
        case HTTP_UNAUTHORIZED:
            uspErrorCode = ErrorCode::AuthenticationError;
            errorMessage = "WebSocket Upgrade failed with an authentication error (401). Please check for correct subscription key (or authorization token) and region name.";
            break;
        case HTTP_FORBIDDEN:
            uspErrorCode = ErrorCode::AuthenticationError;
            errorMessage = "WebSocket Upgrade failed with an authentication error (403). Please check for correct subscription key (or authorization token) and region name.";
            break;
        case HTTP_TOO_MANY_REQUESTS:
            uspErrorCode = ErrorCode::TooManyRequests;
            errorMessage = "WebSocket Upgrade failed with too many requests error (429). Please check for correct subscription key (or authorization token) and region name.";
            break;
        case HTTP_MOVED:
        case HTTP_PERM_REDIRECT:
            uspErrorCode = ErrorCode::ServiceRetirectPermanent;
            errorMessage = errorInfo->errorString != NULL ? errorInfo->errorString : "Redirect Location Unknown";
            break;
        case HTTP_TEMP_REDIRECT:
            uspErrorCode = ErrorCode::ServiceRedirectTemprary;
            errorMessage = errorInfo->errorString != NULL ? errorInfo->errorString : "Redirect Location Unknown";
            break;
        default:
            uspErrorCode = ErrorCode::ConnectionError;
            errorMessage = "WebSocket Upgrade failed with HTTP status code: " + errorCodeInString;
            break;
        }
        break;

    case TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME:
        uspErrorCode = ErrorCode::ConnectionError;
        errorMessage = "Failure while sending a frame over the WebSocket connection. Internal error: " + errorCodeInString
            + ". Error details: " + errorStr;
        break;

    case TRANSPORT_ERROR_WEBSOCKET_ERROR:
        uspErrorCode = ErrorCode::ConnectionError;
        errorMessage = "WebSocket operation failed. Internal error: " + errorCodeInString
            + ". Error details: " + errorStr;
        break;

    case TRANSPORT_ERROR_DNS_FAILURE:
        uspErrorCode = ErrorCode::ConnectionError;
        errorMessage = "DNS connection failed (the remote host did not respond). Internal error: " + errorCodeInString;
        break;

    default:
    case TRANSPORT_ERROR_UNKNOWN:
        uspErrorCode = ErrorCode::ConnectionError;
        errorMessage = "Unknown transport error.";
        break;
    }

    if (connection->m_connected)
    {
        connection->m_connected = false;
        LogInfo("TS:%" PRIu64 ", OnDisconnected: connection:0x%x", connection->getTimestamp(), connection);
        connection->Invoke([&] {
            callbacks->OnDisconnected();
        });
    }

    assert(!errorMessage.empty());
    connection->Invoke([&] { callbacks->OnError(true, uspErrorCode, errorMessage); });

    connection->m_valid = false;
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

std::string GetHeadersAsString(HTTP_HEADERS_HANDLE responseHeader)
{
    size_t headersCount;

    if (HTTPHeaders_GetHeaderCount(responseHeader, &headersCount) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_GetHeaderCount failed.");
        return std::string{};
    }

    std::ostringstream oss;
    for (size_t i = 0; i < headersCount; i++)
    {
        char *temp;
        oss << "\r\n";
        if (HTTPHeaders_GetHeader(responseHeader, i, &temp) == HTTP_HEADERS_OK)
        {
            oss << temp;
            free(temp);
        }
        else
        {
            LogError("HTTPHeaders_GetHeader failed");
            return std::string{};
        }
    }

    return oss.str();
}

// Callback for data available on transport
void Connection::Impl::OnTransportData(TransportResponse *response, void *context)
{
    throw_if_null(context, "context");
    if (response == nullptr)
    {
        return;
    }

    Connection::Impl *connection = static_cast<Connection::Impl *>(context);

    if (response->frameType == FRAME_TYPE_UNKNOWN)
    {
        LogError("Response frame type is unknown.");
        return;
    }

    if (response->responseHeader == NULL)
    {
        LogError("ResponseHeader is NULL.");
        return;
    }

    auto path = HTTPHeaders_FindHeaderValue(response->responseHeader, KEYWORD_PATH);
    if (path == NULL)
    {
        PROTOCOL_VIOLATION("response missing '%s' header", KEYWORD_PATH);
        return;
    }

    auto callbacks = connection->m_config.m_callbacks;
    if (response->frameType == FRAME_TYPE_TEXT || response->frameType == FRAME_TYPE_BINARY)
    {
        connection->Invoke([&] { callbacks->OnMessageReceived({
            GetHeadersAsString(response->responseHeader), path,
            response->buffer, (uint32_t)response->bufferSize,
            response->frameType == FRAME_TYPE_BINARY }); });
    }

    string requestId = HTTPHeaders_FindHeaderValue(response->responseHeader, headers::requestId);
    if (!connection->m_activeRequestIds.count(requestId))
    {
        if (requestId.empty() || strncmp(path, path::turnStart, strlen(path::turnStart)) != 0)
        {
            PROTOCOL_VIOLATION("Unexpected request id '%s', Path: %s", requestId.c_str(), path);
            MetricsUnexpectedRequestId(requestId);
            return;
        }
        else
        {
            LogInfo("Service originated request received with requestId: %s", requestId.c_str());
            connection->RegisterRequestId(requestId);
        }
    }

    const char *contentType = NULL;
    if (response->bufferSize != 0)
    {
        contentType = HTTPHeaders_FindHeaderValue(response->responseHeader, headers::contentType);
        if (contentType == NULL)
        {
            LogInfo("response '%s' contains body with no content-type", path);
        }
        else
        {
            LogInfo("Response Message: content type: %s.", contentType);
        }
    }

    MetricsReceivedMessage(*connection->m_telemetry, requestId, path);

    LogInfo("TS:%" PRIu64 " Response Message: path: %s, size: %zu.", connection->getTimestamp(), path, response->bufferSize);

    if (response->frameType == FRAME_TYPE_BINARY)
    {
        if (strncmp(path, path::translationSynthesis, strlen(path::translationSynthesis)) == 0 ||
            strncmp(path, path::audio, strlen(path::audio)) == 0)
        {
            // streamId is optional
            auto streamId = HTTPHeaders_FindHeaderValue(response->responseHeader, headers::streamId);

            AudioOutputChunkMsg msg;
            if (strncmp(path, path::audio, strlen(path::audio)) == 0 && connection->m_streamIdLangMap.size() > 0)
            {
                SPX_DBG_TRACE_VERBOSE("m_streamIdLangMap has data, will FillLanguageForAudioOutputChunkMsg");
                connection->FillLanguageForAudioOutputChunkMsg(streamId, path, msg);
            }
            msg.requestId = requestId;
            msg.audioBuffer = (uint8_t *)response->buffer;
            msg.audioLength = response->bufferSize;
            connection->Invoke([&] { callbacks->OnAudioOutputChunk(msg); });
        }
        else
        {
            PROTOCOL_VIOLATION("Binary frame received with unexpected path: %s", path);
        }
    }
    else if (response->frameType == FRAME_TYPE_TEXT)
    {
        auto json = (response->bufferSize > 0) ? nlohmann::json::parse(response->buffer, response->buffer + response->bufferSize) : nlohmann::json();
        if (strncmp(path, path::speechStartDetected, strlen(path::speechStartDetected)) == 0 ||
            strncmp(path, path::speechEndDetected, strlen(path::speechEndDetected)) == 0)
        {
            auto offsetObj = json[json_properties::offset];
            // For whatever reason, offset is sometimes missing on the end detected message.
            auto offset = offsetObj.is_null() ? 0 : offsetObj.get<OffsetType>();

            if (strncmp(path, path::speechStartDetected, strlen(path::speechStartDetected)) == 0)
            {
                connection->Invoke([&] { callbacks->OnSpeechStartDetected({PAL::ToWString(json.dump()), offset}); });
            }
            else
            {
                connection->Invoke([&] { callbacks->OnSpeechEndDetected({PAL::ToWString(json.dump()), offset}); });
            }
        }
        else if (strncmp(path, path::turnStart, strlen(path::turnStart)) == 0)
        {
            auto tag = json[json_properties::context][json_properties::tag].get<string>();
            if (requestId == connection->m_speechRequestId)
            {
                /* We know this request id is related to a speech turn */
                connection->Invoke([&] { callbacks->OnTurnStart({ PAL::ToWString(json.dump()), tag, requestId }); });
            }
            else
            {
                /* We know this request id is a server initiated request */
                connection->Invoke([&] { callbacks->OnMessageStart({ PAL::ToWString(json.dump()), tag, requestId }); });
            }
        }
        else if (strncmp(path, path::turnEnd, strlen(path::turnEnd)) == 0)
        {
            {
                connection->m_activeRequestIds.erase(requestId);
                SPX_DBG_TRACE_VERBOSE("Got turn end, clear m_streamIdLangMap.");
                connection->m_streamIdLangMap.clear();
            }

            // flush the telemetry before invoking the onTurnEnd callback.
            // TODO: 1164154
            connection->m_telemetry->Flush(requestId);

            if (requestId == connection->m_speechRequestId)
            {
                connection->m_speechRequestId.clear();
                connection->m_speechContextMessageAllowed = true;
                connection->Invoke([&] { callbacks->OnTurnEnd({ requestId }); });
            }
            else
            {
                connection->Invoke([&] { callbacks->OnMessageEnd({ requestId }); });
            }
        }
        else if (strncmp(path, path::speechKeyword, strlen(path::speechKeyword)) == 0)
        {
            auto status = ToKeywordVerificationStatus(json[json_properties::status].get<string>());
            auto offsetObj = json[json_properties::offset];
            auto offset = offsetObj.is_null()? 0 : offsetObj.get<OffsetType>();
            auto durationObj = json[json_properties::duration];
            auto duration = durationObj.is_null()? 0 : durationObj.get<DurationType>();
            auto textObj = json[json_properties::text];
            auto text = textObj.is_null()? "" : textObj.get<string>();

            connection->Invoke([&] {
                callbacks->OnSpeechKeywordDetected({PAL::ToWString(json.dump()),
                                            offset,
                                            duration,
                                            status,
                                            PAL::ToWString(text) });
            });
        }
        else if (strncmp(path, path::speechHypothesis, strlen(path::speechHypothesis)) == 0 ||
                 strncmp(path, path::speechFragment, strlen(path::speechFragment)) == 0)
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
            if (strncmp(path, path::speechHypothesis, strlen(path::speechHypothesis)) == 0)
            {
                connection->Invoke([&] {
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
                connection->Invoke([&] {
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
        else if (strncmp(path, path::speechPhrase, strlen(path::speechPhrase)) == 0)
        {
            SpeechPhraseMsg result = connection->RetrieveSpeechPhraseResult(json);
            if (connection->isErrorRecognitionStatus(result.recognitionStatus))
            {
                connection->InvokeRecognitionErrorCallback(result.recognitionStatus, json.dump());
            }
            else
            {
                connection->Invoke([&] { callbacks->OnSpeechPhrase(result); });
            }
        }
        else if (strncmp(path, path::translationHypothesis, strlen(path::translationHypothesis)) == 0)
        {
            auto speechResult = RetrieveSpeechResult(json);
            auto translationResult = RetrieveTranslationResult(json[json_properties::translation], false);
            // TranslationStatus is always success for translation.hypothesis
            translationResult.translationStatus = TranslationStatus::Success;

            connection->Invoke([&] {
                callbacks->OnTranslationHypothesis({move(speechResult.json),
                                                    speechResult.offset,
                                                    speechResult.duration,
                                                    move(speechResult.text),
                                                    move(translationResult)});
            });
        }
        else if (strncmp(path, path::translationPhrase, strlen(path::translationPhrase)) == 0)
        {
            auto status = ToRecognitionStatus(json.at(json_properties::recoStatus));
            if (connection->isErrorRecognitionStatus(status))
            {
                // There is an error in speech recognition, fire an error event.
                connection->InvokeRecognitionErrorCallback(status, json.dump());
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
                connection->Invoke([&] {
                    callbacks->OnTranslationPhrase({move(speechResult.json),
                                                    speechResult.offset,
                                                    speechResult.duration,
                                                    move(speechResult.text),
                                                    move(translationResult),
                                                    status});
                });
            }
        }
        else if (strncmp(path, path::translationSynthesisEnd, strlen(path::translationSynthesisEnd)) == 0)
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
                connection->Invoke([&] { callbacks->OnAudioOutputChunk(msg); });
            }
            else
            {
                connection->Invoke([&] { callbacks->OnError(false, ErrorCode::ServiceError, failureReason.c_str()); });
            }
        }
        else if (strncmp(path, path::translationResponse, strlen(path::translationResponse)) == 0)
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
                    connection->Invoke([&] {
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
                if (connection->isErrorRecognitionStatus(status))
                {
                    // There is an error in speech recognition, fire an error event.
                    connection->InvokeRecognitionErrorCallback(status, json.dump());
                }
                else
                {
                    auto speechPhraseMsg = connection->RetrieveSpeechPhraseResult(speechPhraseJson);
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
                    connection->Invoke([&] {
                        callbacks->OnTranslationPhrase({PAL::ToWString(json.dump()),
                                                        speechPhraseMsg.offset,
                                                        speechPhraseMsg.duration,
                                                        move(speechPhraseMsg.displayText),
                                                        move(translationResult),
                                                        status,
                                                        move(speechPhraseMsg.language),
                                                        speechPhraseMsg.languageDetectionConfidence});
                    });
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
        else if (strncmp(path, path::audioMetaData, strlen(path::audioMetaData)) == 0)
        {
            AudioOutputMetadataMsg msg;
            msg.requestId = requestId;
            msg.size = response->bufferSize;

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

            connection->Invoke([&] { callbacks->OnAudioOutputMetadata(msg); });
        }
        else if (strncmp(path, path::audioStart, strlen(path::audioStart)) == 0)
        {
            auto streamId = HTTPHeaders_FindHeaderValue(response->responseHeader, headers::streamId);
            if (streamId == nullptr)
            {
                 PROTOCOL_VIOLATION("No stream id in %s header", path);
            }
            else if (json.find(json_properties::translationLanguage) == json.end())
            {
                // TODO: in future, audio.start may be used in other scenario,  this validation logic need to update accordingly.
                PROTOCOL_VIOLATION("Cannot find TranslationLanguage in audio.start message. Json=%s", json.dump().c_str());
            }
            else
            {
                auto language = json[json_properties::translationLanguage].get<string>();
                LogInfo("Got streamId %s to language %s map.  current m_streamIdLangMap size = %d", streamId, language.c_str(), connection->m_streamIdLangMap.size());
                // This is a protection logic to avoid memory usage increases due to service error.
                // So far we haven't any scenario that needs more than 10 synthesising languages
                // TODO: remove this protection code after the new service endpoint is stable
                const int MaxLanguages = 10;
                if (connection->m_streamIdLangMap.size() > MaxLanguages)
                {
                    PROTOCOL_VIOLATION("We have got more than % audio.start messages for different languges, service is sending too many such messages.", MaxLanguages);
                }
                else
                {
                    connection->m_streamIdLangMap[streamId] = language;
                }
            }
        }
        else if (strncmp(path, path::audioEnd, strlen(path::audioEnd)) == 0)
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
                auto streamId = HTTPHeaders_FindHeaderValue(response->responseHeader, headers::streamId);
                connection->FillLanguageForAudioOutputChunkMsg(streamId, path, msg);
                connection->Invoke([&] { callbacks->OnAudioOutputChunk(msg); });
            }
            else
            {
                failureReason = failureReason + json[json_properties::failureReason].get<string>();
                connection->Invoke([&] { callbacks->OnError(false, ErrorCode::ServiceError, failureReason.c_str()); });
            }
        }
        else
        {
            connection->Invoke([&] {
                callbacks->OnUserMessage({string(path),
                                          string(contentType == nullptr ? "" : contentType),
                                          requestId,
                                          response->buffer,
                                          response->bufferSize});
            });
        }
    }
}

void Connection::Impl::InvokeRecognitionErrorCallback(RecognitionStatus status, const string& response)
{
    auto callbacks = m_config.m_callbacks;
    string msg;
    ErrorCode error = ErrorCode::ServiceError;

    switch (status)
    {
    case RecognitionStatus::Error:
        msg = "The speech recognition service encountered an internal error and could not continue. Response text:" + response;
        error = ErrorCode::ServiceError;
        break;
    case RecognitionStatus::TooManyRequests:
        msg = "The number of parallel requests exceeded the number of allowed concurrent transcriptions. Response text:" + response;
        error = ErrorCode::TooManyRequests;
        break;
    case RecognitionStatus::BadRequest:
        msg = "Invalid parameter or unsupported audio format in the request. Response text:" + response;
        error = ErrorCode::BadRequest;
        break;
    case RecognitionStatus::Forbidden:
        msg = "The recognizer is using a free subscription that ran out of quota. Response text:" + response;
        error = ErrorCode::Forbidden;
        break;
    case RecognitionStatus::ServiceUnavailable:
        msg = "The service is currently unavailable. Response text:" + response;
        error = ErrorCode::ServiceUnavailable;
        break;
    case RecognitionStatus::InvalidMessage:
        msg = "Invalid response. Response text:" + response;
        error = ErrorCode::ServiceError;
        break;
    case RecognitionStatus::Success:
    case RecognitionStatus::EndOfDictation:
    case RecognitionStatus::InitialSilenceTimeout:
    case RecognitionStatus::InitialBabbleTimeout:
    case RecognitionStatus::NoMatch:
        msg = "Runtime Error: invoke error callback for non-error recognition status. Response text:" + response;
        error = ErrorCode::RuntimeError;
        break;
    default:
        msg = "Runtime Error: invalid recognition status. Response text:" + response;
        error = ErrorCode::RuntimeError;
        break;
    }

    this->Invoke([&] { callbacks->OnError(false, error, msg.c_str()); });
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

void Connection::Impl::FillLanguageForAudioOutputChunkMsg(const char* streamId, const std::string& messagePath, AudioOutputChunkMsg& msg)
{
    if (streamId == nullptr)
    {
        PROTOCOL_VIOLATION("%s message is recieved but doesn't have streamId in header.", messagePath.c_str());
    }
    else if (m_streamIdLangMap.count(streamId) == 0)
    {
        PROTOCOL_VIOLATION(
            "%s message is recieved but cannot find streamId %s from streamId to language map, may be caused by audio.start message not being receieved before this message.",
            messagePath.c_str(),
            streamId);
    }
    else
    {
        msg.language = m_streamIdLangMap.at(streamId);
        if (messagePath == path::audioEnd)
        {
            SPX_DBG_TRACE_VERBOSE("Got audio end, remove %s from m_streamIdLangMap.", streamId);
            m_streamIdLangMap.erase(streamId);
        }
    }
}

void PlatformInit(const char* proxyHost, int proxyPort, const char* proxyUsername, const char* proxyPassword)
{
    static once_flag initOnce;

    call_once(initOnce, [&] {
        if (platform_init() != 0) {
            ThrowRuntimeError("Failed to initialize platform (azure-c-shared)");
        }

        // Set proxy if needed.
        if (proxyHost != nullptr && *proxyHost != '\0')
        {
            if (proxyPort <= 0)
            {
                ThrowRuntimeError("Invalid port of the proxy server.");
            }
            string hostAndPort = proxyHost + string(":") + to_string(proxyPort);
            string userNameAndPassword;
            if (proxyUsername != nullptr)
            {
                if (proxyPassword == nullptr)
                {
                    ThrowRuntimeError("Invalid password of the proxy service. It should not be null if user name is specified");
                }
                userNameAndPassword = proxyUsername + string(":") + proxyPassword;
            }
            platform_set_http_proxy(hostAndPort.c_str(), userNameAndPassword.empty() ? nullptr : userNameAndPassword.c_str());
        }
    });
}

}}}}

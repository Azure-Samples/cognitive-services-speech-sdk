//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspimpl.c: implementation of the USP library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include <vector>
#include <set>

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

// Todo(1126805) url builder + auth interfaces

const std::string g_recoModeStrings[] = { "interactive", "conversation", "dictation" };
const std::string g_outFormatStrings[] = { "simple", "detailed" };


// TODO: remove this as soon as transport.c is re-written in cpp.
extern "C" {
    const char* g_keywordContentType = headers::contentType;
}


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
    m_audioOffset(0),
    m_creationTime(telemetry_gettime())
{
    static once_flag initOnce;

    call_once(initOnce, [this] {
        if (platform_init() != 0) {
            ThrowRuntimeError("Failed to initialize platform (azure-c-shared)");
        }

        // Set proxy if needed.
        if (m_config.m_proxyServerInfo != nullptr)
        {
            auto proxy = m_config.m_proxyServerInfo;
            if (proxy->host == nullptr || *proxy->host == '\0' || proxy->port <= 0)
            {
                ThrowRuntimeError("Invalid host name or port of the proxy server.");
            }
            string hostAndPort = proxy->host + string(":") + to_string(proxy->port);
            string userNameAndPassword;
            if (proxy->username != nullptr)
            {
                if (proxy->password == nullptr)
                {
                    ThrowRuntimeError("Invalid password of the proxy service. It should not be null if user name is specified");
                }
                userNameAndPassword = proxy->username + string(":") + proxy->password;
            }
            platform_set_http_proxy(hostAndPort.c_str(), userNameAndPassword.empty() ? nullptr : userNameAndPassword.c_str());
        }
    });

    m_threadService = m_config.m_threadService;
    Validate();
}

uint64_t Connection::Impl::getTimestamp()
{
    return telemetry_gettime() - m_creationTime;
}

void Connection::Impl::Invoke(std::function<void()> callback)
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
    std::packaged_task<void()> task([keepAlive]() { DoWork(keepAlive); });
    m_threadService->ExecuteAsync(std::move(task));
}

void Connection::Impl::WorkLoop(shared_ptr<Connection::Impl> ptr)
{
    std::packaged_task<void()> task([ptr]()
    {
        if (!ptr->m_valid)
            return;

        DoWork(ptr);

        std::packaged_task<void()> task([ptr]() { WorkLoop(ptr); });
        ptr->m_threadService->ExecuteAsync(std::move(task), std::chrono::milliseconds(100));
    });

    ptr->m_threadService->ExecuteAsync(std::move(task));
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
    catch (const std::exception& ex)
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

void Connection::Impl::Validate()
{
    if (m_config.m_authData.empty())
    {
        ThrowInvalidArgumentException("No valid authentication mechanism was specified.");
    }
}

string Connection::Impl::EncodeParameterString(const string& parameter) const
{
    STRING_HANDLE encodedHandle = URL_EncodeString(parameter.c_str());
    string encodedStr(STRING_c_str(encodedHandle));
    STRING_delete(encodedHandle);
    return encodedStr;
}

string Connection::Impl::ConstructConnectionUrl() const
{
    auto recoMode = static_cast<underlying_type_t<RecognitionMode>>(m_config.m_recoMode);
    ostringstream oss;
    bool customEndpoint = false;

    // Using customized endpoint if it is defined.
    if (!m_config.m_customEndpointUrl.empty())
    {
        oss << m_config.m_customEndpointUrl;
        customEndpoint = true;
    }
    else
    {
        oss << endpoint::protocol;
        switch (m_config.m_endpoint)
        {
        case EndpointType::Speech:
            oss << m_config.m_region
                << endpoint::unifiedspeech::hostnameSuffix
                << endpoint::unifiedspeech::pathPrefix
                << g_recoModeStrings[recoMode]
                << endpoint::unifiedspeech::pathSuffix;
            break;
        case EndpointType::Translation:
            oss << m_config.m_region
                << endpoint::translation::hostnameSuffix
                << endpoint::translation::path;
            break;
        case EndpointType::Intent:
            if (m_config.m_recoMode != USP::RecognitionMode::Interactive)
            {
                ThrowInvalidArgumentException("Invalid reco mode for intent recognition.");
            }
            oss << endpoint::luis::hostname
                << endpoint::luis::pathPrefix1
                << m_config.m_intentRegion
                << endpoint::luis::pathPrefix2
                << g_recoModeStrings[recoMode]
                << endpoint::luis::pathSuffix;
            break;
        case EndpointType::CDSDK:
            oss << endpoint::CDSDK::url;
            break;
        default:
            ThrowInvalidArgumentException("Unknown endpoint type.");
        }
    }

    // The first query parameter.
    auto format = static_cast<underlying_type_t<OutputFormat>>(m_config.m_outputFormat);
    if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::outputFormatQueryParam))
    {
        auto delim = (customEndpoint && (oss.str().find('?') != string::npos)) ? '&' : '?';
        oss << delim << endpoint::unifiedspeech::outputFormatQueryParam << g_outFormatStrings[format];
    }

    // Todo: use libcurl or another library to encode the url as whole, instead of each parameter.
    switch (m_config.m_endpoint)
    {
    case EndpointType::Speech:
        if (!m_config.m_modelId.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::deploymentIdQueryParam))
            {
                oss << '&' << endpoint::unifiedspeech::deploymentIdQueryParam << m_config.m_modelId;
            }
        }
        else if (!m_config.m_language.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::langQueryParam))
            {
                oss << '&' << endpoint::unifiedspeech::langQueryParam << m_config.m_language;
            }
        }
        break;
    case EndpointType::Intent:
        if (!m_config.m_language.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::langQueryParam))
            {
                oss << '&' << endpoint::unifiedspeech::langQueryParam << m_config.m_language;
            }
        }
        break;
    case EndpointType::Translation:
        if (!m_config.m_modelId.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::deploymentIdQueryParam))
            {
                oss << '&' << endpoint::unifiedspeech::deploymentIdQueryParam << m_config.m_modelId;
            }
        }
        else if (!m_config.m_translationSourceLanguage.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::translation::from))
            {
                oss << '&' << endpoint::translation::from << EncodeParameterString(m_config.m_translationSourceLanguage);
            }
        }

        if (!customEndpoint || !contains(oss.str(), endpoint::translation::to))
        {
            size_t start = 0;
            auto delim = ',';
            size_t end = m_config.m_translationTargetLanguages.find_first_of(delim);
            while (end != string::npos)
            {
                oss << '&' << endpoint::translation::to << EncodeParameterString(m_config.m_translationTargetLanguages.substr(start, end - start));
                start = end + 1;
                end = m_config.m_translationTargetLanguages.find_first_of(delim, start);
            }
            oss << '&' << endpoint::translation::to << EncodeParameterString(m_config.m_translationTargetLanguages.substr(start, end));
        }

        if (!m_config.m_translationVoice.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::translation::voice))
            {
                oss << '&' << endpoint::translation::features << endpoint::translation::requireVoice;
                oss << '&' << endpoint::translation::voice << EncodeParameterString(m_config.m_translationVoice);
            }
        }
        break;
    case EndpointType::CDSDK:
        // no query parameter needed.
        break;
    }

    return oss.str();
}

void Connection::Impl::Connect()
{
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

    assert(!m_config.m_authData.empty());

    switch (m_config.m_authType)
    {
    case AuthenticationType::SubscriptionKey:
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::ocpApimSubscriptionKey, m_config.m_authData.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set authentication using subscription key.");
        }
        break;

    case AuthenticationType::AuthorizationToken:
        {
            ostringstream oss;
            oss << "Bearer " << m_config.m_authData;
            auto token = oss.str();
            if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::authorization, token.c_str()) != 0)
            {
                ThrowRuntimeError("Failed to set authentication using authorization token.");
            }
        }
        break;

    // TODO(1126805): url builder + auth interfaces
    case AuthenticationType::SearchDelegationRPSToken:
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::searchDelegationRPSToken, m_config.m_authData.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set authentication using Search-DelegationRPSToken.");
        }
        break;

    default:
        ThrowRuntimeError("Unsupported authentication type");
    }

    auto connectionUrl = ConstructConnectionUrl();
    LogInfo("connectionUrl=%s", connectionUrl.c_str());

    m_telemetry = std::make_unique<Telemetry>(Connection::Impl::OnTelemetryData, this);
    if (m_telemetry == nullptr)
    {
        ThrowRuntimeError("Failed to create telemetry instance.");
    }

    std::string connectionId = PAL::ToString(m_config.m_connectionId);

    // Log the device uuid
    MetricsDeviceStartup(*m_telemetry, connectionId, PAL::DeviceUuid());

    m_transport = TransportPtr(TransportRequestCreate(connectionUrl.c_str(), this, m_telemetry.get(), headersPtr, connectionId.c_str(), m_config.m_proxyServerInfo.get()), TransportRequestDestroy);

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
    MetricsTransportRequestId(m_telemetry.get(), requestId.c_str());

    m_activeRequestIds.insert(requestId);

    return requestId;
}

void Connection::Impl::QueueMessage(const string& path, const uint8_t *data, size_t size, MessageType messageType)
{
    throw_if_null(data, "data");

    if (path.empty())
    {
        ThrowInvalidArgumentException("The path is null or empty.");
    }

    if (m_valid)
    {
        // If the service receives multiple context messages for a single turn,
        // the service will close the WebSocket connection with an error.
        if (messageType == MessageType::Context && !m_speechRequestId.empty())
        {
            ThrowLogicError("Error trying to send a context message while in the middle of a speech turn.");
        }

        // The config message does not require a X-RequestId header, because this message is not associated with a particular request.
        string requestId;
        if (messageType != MessageType::Config)
        {
           requestId = CreateRequestId();
           m_speechRequestId = (messageType == MessageType::Context) ? requestId : m_speechRequestId;
        }

        (void)TransportMessageWrite(m_transport.get(), path.c_str(), data, size, requestId.c_str());
    }

    ScheduleWork();
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

    MetricsAudioStreamData(size);

    int ret = 0;

    if (m_audioOffset == 0)
    {
        // The service uses the first audio message that contains a unique request identifier to signal the start of a new request/response cycle or turn.
        // After receiving an audio message with a new request identifier, the service discards any queued or unsent messages
        // that are associated with any previous turn.
        m_speechRequestId = m_speechRequestId.empty() ? CreateRequestId() : m_speechRequestId;
        MetricsAudioStreamInit();
        MetricsAudioStart(*m_telemetry, m_speechRequestId);

        ret = TransportStreamPrepare(m_transport.get(), "/audio");
        if (ret != 0)
        {
            ThrowRuntimeError("TransportStreamPrepare failed. error=" + to_string(ret));
        }
    }

    ret = TransportStreamWrite(m_transport.get(), audioChunk, m_speechRequestId.c_str());
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
    auto ret = TransportStreamFlush(m_transport.get(), m_speechRequestId.c_str());

    m_audioOffset = 0;
    MetricsAudioStreamFlush();
    MetricsAudioEnd(*m_telemetry, m_speechRequestId);

    if (ret != 0)
    {
        ThrowRuntimeError("Returns failure, reason: TransportStreamFlush returned " + to_string(ret));
    }

    ScheduleWork();
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

static TranslationStatus ToTranslationStatus(const string& str)
{
    if (0 == str.compare("Success")) return  TranslationStatus::Success;
    if (0 == str.compare("Error")) return  TranslationStatus::Error;

    PROTOCOL_VIOLATION("Unknown TranslationStatus: %s", str.c_str());
    return TranslationStatus::InvalidMessage;
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
    return SpeechHypothesisMsg(PAL::ToWString(json.dump()), offset, duration, PAL::ToWString(text));
}

static TranslationResult RetrieveTranslationResult(const nlohmann::json& json, bool expectStatus)
{
    auto translation = json[json_properties::translation];

    TranslationResult result;
    if (expectStatus)
    {
        auto status = translation.find(json_properties::translationStatus);
        if (status != translation.end())
        {
            result.translationStatus = ToTranslationStatus(status->get<string>());
        }
        else
        {
            PROTOCOL_VIOLATION("No TranslationStatus is provided. Json: %s", translation.dump().c_str());
            result.translationStatus = TranslationStatus::InvalidMessage;
            result.failureReason = L"Status is missing in the protocol message. Response text:" + PAL::ToWString(json.dump());
        }

        auto failure = translation.find(json_properties::translationFailureReason);
        if (failure != translation.end())
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
        auto translations = translation.at(json_properties::translations);
        for (const auto& object : translations)
        {
            auto lang = object.at(json_properties::lang).get<string>();
            auto txt = object.at(json_properties::text).get<string>();
            if (lang.empty() && txt.empty())
            {
                PROTOCOL_VIOLATION("emtpy language and text field in translations text. lang=%s, text=%s.", lang.c_str(), txt.c_str());
                continue;
            }

            result.translations[PAL::ToWString(lang)] = PAL::ToWString(txt);
        }

        if (!result.translations.size())
        {
            PROTOCOL_VIOLATION("No Translations text block in the message. Response text:", json.dump().c_str());
        }
        return result;
    }
}

// Callback for data available on tranport
void Connection::Impl::OnTransportData(TransportResponse *response, void *context)
{
    throw_if_null(context, "context");

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

    string requestId = HTTPHeaders_FindHeaderValue(response->responseHeader, headers::requestId);
    if (requestId.empty() || !connection->m_activeRequestIds.count(requestId))
    {
        PROTOCOL_VIOLATION("Empty or unexpected request id '%s', Path: %s", requestId.c_str(), path);
        MetricsUnexpectedRequestId(requestId);
        return;
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

    string pathStr(path);
    auto callbacks = connection->m_config.m_callbacks;

    if (response->frameType == FRAME_TYPE_BINARY)
    {
        if (pathStr == path::translationSynthesis || pathStr == path::audio)
        {
            // streamId is optional
            auto streamId = HTTPHeaders_FindHeaderValue(response->responseHeader, headers::streamId);

            AudioOutputChunkMsg msg;
            msg.streamId = streamId == nullptr ? -1 : atoi(streamId);
            msg.audioBuffer = (uint8_t *)response->buffer;
            msg.audioLength = response->bufferSize;
            connection->Invoke([&] { callbacks->OnAudioOutputChunk(msg); });
        }
        else
        {
            PROTOCOL_VIOLATION("Binary frame received with unexpected path: %s", pathStr.c_str());
        }
    }
    else if (response->frameType == FRAME_TYPE_TEXT)
    {
        auto json = (response->bufferSize > 0) ? nlohmann::json::parse(response->buffer, response->buffer + response->bufferSize) : nlohmann::json();
        if (pathStr == path::speechStartDetected || path == path::speechEndDetected)
        {
            auto offsetObj = json[json_properties::offset];
            // For whatever reason, offset is sometimes missing on the end detected message.
            auto offset = offsetObj.is_null() ? 0 : offsetObj.get<OffsetType>();

            if (path == path::speechStartDetected)
            {
                connection->Invoke([&] { callbacks->OnSpeechStartDetected({PAL::ToWString(json.dump()), offset}); });
            }
            else
            {
                connection->Invoke([&] { callbacks->OnSpeechEndDetected({PAL::ToWString(json.dump()), offset}); });
            }
        }
        else if (pathStr == path::turnStart)
        {
            auto tag = json[json_properties::context][json_properties::tag].get<string>();
            connection->Invoke([&] { callbacks->OnTurnStart({PAL::ToWString(json.dump()), tag}); });
        }
        else if (pathStr == path::turnEnd)
        {
            {
                if (requestId == connection->m_speechRequestId)
                {
                    connection->m_speechRequestId.clear();
                }
                connection->m_activeRequestIds.erase(requestId);
            }

            // flush the telemetry before invoking the onTurnEnd callback.
            // TODO: 1164154
            connection->m_telemetry->Flush(requestId);

            connection->Invoke([&] { callbacks->OnTurnEnd({}); });
        }
        else if (path == path::speechHypothesis || path == path::speechFragment)
        {
            auto offset = json[json_properties::offset].get<OffsetType>();
            auto duration = json[json_properties::duration].get<DurationType>();
            auto text = json[json_properties::text].get<string>();

            if (path == path::speechHypothesis)
            {
                connection->Invoke([&] {
                    callbacks->OnSpeechHypothesis({PAL::ToWString(json.dump()),
                                                   offset,
                                                   duration,
                                                   PAL::ToWString(text)});
                });
            }
            else
            {
                connection->Invoke([&] {
                    callbacks->OnSpeechFragment({PAL::ToWString(json.dump()),
                                                 offset,
                                                 duration,
                                                 PAL::ToWString(text)});
                });
            }
        }
        else if (path == path::speechPhrase)
        {
            SpeechPhraseMsg result;
            result.json = PAL::ToWString(json.dump());
            result.offset = json[json_properties::offset].get<OffsetType>();
            result.duration = json[json_properties::duration].get<DurationType>();
            result.recognitionStatus = ToRecognitionStatus(json[json_properties::recoStatus].get<string>());

            switch (result.recognitionStatus)
            {
            case RecognitionStatus::Success:
                if (json.find(json_properties::displayText) != json.end())
                {
                    // The DisplayText field will be present only if the RecognitionStatus field has the value Success.
                    // and the format output is simple
                    result.displayText = PAL::ToWString(json[json_properties::displayText].get<string>());
                }
                else // Detailed
                {
                    auto phrases = json.at(json_properties::nbest);

                    double confidence = 0;
                    for (const auto& object : phrases)
                    {
                        auto currentConfidence = object.at(json_properties::confidence).get<double>();
                        // Picking up the result with the highest confidence.
                        if (currentConfidence > confidence)
                        {
                            confidence = currentConfidence;
                            result.displayText = PAL::ToWString(object.at(json_properties::display).get<string>());
                        }
                    }
                }
                connection->Invoke([&] { callbacks->OnSpeechPhrase(result); });
                break;
            case RecognitionStatus::InitialSilenceTimeout:
            case RecognitionStatus::InitialBabbleTimeout:
            case RecognitionStatus::NoMatch:
            case RecognitionStatus::EndOfDictation:
                connection->Invoke([&] { callbacks->OnSpeechPhrase(result); });
                break;
            default:
                connection->InvokeRecognitionErrorCallback(result.recognitionStatus, json.dump());
                break;
            }
        }
        else if (path == path::translationHypothesis)
        {
            auto speechResult = RetrieveSpeechResult(json);
            auto translationResult = RetrieveTranslationResult(json, false);
            // TranslationStatus is always success for translation.hypothesis
            translationResult.translationStatus = TranslationStatus::Success;

            connection->Invoke([&] {
                callbacks->OnTranslationHypothesis({std::move(speechResult.json),
                                                    speechResult.offset,
                                                    speechResult.duration,
                                                    std::move(speechResult.text),
                                                    std::move(translationResult)});
            });
        }
        else if (path == path::translationPhrase)
        {
            auto status = ToRecognitionStatus(json.at(json_properties::recoStatus));
            auto speechResult = RetrieveSpeechResult(json);
            bool isErrorStatus = false;

            TranslationResult translationResult;
            switch (status)
            {
            case RecognitionStatus::Success:
                translationResult = RetrieveTranslationResult(json, true);
                break;
            case RecognitionStatus::InitialSilenceTimeout:
            case RecognitionStatus::InitialBabbleTimeout:
            case RecognitionStatus::NoMatch:
            case RecognitionStatus::EndOfDictation:
                translationResult.translationStatus = TranslationStatus::Success;
                break;
            default:
                isErrorStatus = true;
                break;
            }

            if (isErrorStatus)
            {
                // There is an error in speech recognition, fire an error event.
                connection->InvokeRecognitionErrorCallback(status, json.dump());
            }
            else
            {
                // There is no speech recognition error, we fire a translation phrase event.
                connection->Invoke([&] {
                    callbacks->OnTranslationPhrase({std::move(speechResult.json),
                                                    speechResult.offset,
                                                    speechResult.duration,
                                                    std::move(speechResult.text),
                                                    std::move(translationResult),
                                                    status});
                });
            }
        }
        else if (path == path::translationSynthesisEnd)
        {
            std::string failureReason;
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

            auto failureHandle = json.find(json_properties::translationFailureReason);
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
                msg.streamId = -1;
                msg.audioBuffer = NULL;
                msg.audioLength = 0;
                connection->Invoke([&] { callbacks->OnAudioOutputChunk(msg); });
            }
            else
            {
                connection->Invoke([&] { callbacks->OnError(false, ErrorCode::ServiceError, failureReason.c_str()); });
            }
        }
        else
        {
            connection->Invoke([&] {
                callbacks->OnUserMessage({pathStr,
                                          string(contentType == nullptr ? "" : contentType),
                                          response->buffer,
                                          response->bufferSize});
            });
        }
    }
}

void Connection::Impl::InvokeRecognitionErrorCallback(RecognitionStatus status, const std::string& response)
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

}}}}

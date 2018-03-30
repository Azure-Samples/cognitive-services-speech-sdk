//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspimpl.c: implementation of the USP library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <vector>
#include <set>

#include <assert.h>
#include <inttypes.h>

#include "azure_c_shared_utility_xlogging_wrapper.h"
#include "azure_c_shared_utility_httpheaders_wrapper.h"
#include "azure_c_shared_utility_platform_wrapper.h"
#include "azure_c_shared_utility_buffer_wrapper.h"

#include "uspcommon.h"
#include "uspinternal.h"

#include "iobuffer.h"
#include "transport.h"
#include "dnscache.h"
#include "metrics.h"

#ifdef __linux__
#include <unistd.h>
#endif

const char* g_keywordContentType = "Content-Type";
const char* g_messagePathSpeechHypothesis = "speech.hypothesis";
const char* g_messagePathSpeechPhrase = "speech.phrase";
const char* g_messagePathSpeechFragment = "speech.fragment";
const char* g_messagePathTurnStart = "turn.start";
const char* g_messagePathTurnEnd = "turn.end";
const char* g_messagePathSpeechStartDetected = "speech.startDetected";
const char* g_messagePathSpeechEndDetected = "speech.endDetected";
//Todo: Figure out what to do about user agent build hash and version number
const auto g_userAgent = "CortanaSDK (Windows;Win32;DeviceType=Near;SpeechClient=2.0.4)";

const auto g_requestHeaderUserAgent = "User-Agent";
const auto g_requestHeaderOcpApimSubscriptionKey = "Ocp-Apim-Subscription-Key";
const auto g_requestHeaderAuthorization = "Authorization";
const auto g_requestHeaderSearchDelegationRPSToken = "X-Search-DelegationRPSToken";
const auto g_requestHeaderAudioResponseFormat = "X-Output-AudioCodec";


uint64_t telemetry_gettime()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

namespace USP {

using namespace std;

// Todo: read from a configuration file.
const auto g_protocol = "wss://";
const auto g_bingSpeechHostname = "speech.platform.bing.com";
const auto g_CRISHostname = ".api.cris.ai";
const auto g_pathPrefix = "/speech/recognition/";
const auto g_pathSuffix = "/cognitiveservices/v1?";

// Todo(1126805) url builder + auth interfaces
const auto g_CDSDKEndpoint = "speech.platform.bing.com/cortana/api/v1?environment=Home&";

const auto g_langQueryParam = "language=";

const vector<string> g_recoModeStrings = { "interactive", "conversation", "dictation" };
const vector<string> g_outFormatStrings = { "format=simple", "format=detailed" };

const set<string> contentPaths = { g_messagePathTurnStart, g_messagePathSpeechHypothesis, g_messagePathSpeechPhrase, g_messagePathSpeechFragment };

// This is called from telemetry_flush, invoked on a worker thread in turn-end. 
void Connection::Impl::OnTelemetryData(const uint8_t* buffer, size_t bytesToWrite, void *context, const char *requestId)
{
    Connection::Impl *connection = (Connection::Impl*)context;
    TransportWriteTelemetry(connection->m_transport.get(), buffer, bytesToWrite, requestId);
}


Connection::Impl::Impl(const Client& config)
    : m_config(config),
    m_connected(false),
    m_audioOffset(0),
    m_creationTime(telemetry_gettime())
{
    static once_flag initOnce;

    call_once(initOnce, [] {
        if (platform_init() != 0) {
            throw runtime_error("Failed to initialize platform (azure-c-shared)");
        }
    });

    Validate();
}

uint64_t Connection::Impl::getTimestamp() 
{
    return telemetry_gettime() - m_creationTime;
}

void Connection::Impl::WorkThread(weak_ptr<Connection::Impl> ptr)
{
    auto connection = ptr.lock();
    connection->SignalConnected();
    connection.reset();

    while (true) 
    {
        connection = ptr.lock();
        if (connection == nullptr) 
        {
            // connection is destroyed, our work here is done.
            return;
        }
        unique_lock<recursive_mutex> lock(connection->m_mutex);
        TransportDoWork(connection->m_transport.get());
        if (!connection->m_connected) 
        {
            return;
        }
        connection->m_cv.wait_for(lock, chrono::milliseconds(200), [&] {return connection->m_haveWork; });
        connection->m_haveWork = false;
    }
}

void Connection::Impl::SignalWork() 
{
    m_haveWork = true;
    m_cv.notify_one();
}

void Connection::Impl::SignalConnected()
{
    unique_lock<recursive_mutex> lock(m_mutex);
    m_connected = true;
    m_cv.notify_one();
}


void Connection::Impl::Shutdown()
{
    unique_lock<recursive_mutex> lock(m_mutex);
    m_connected = false;
    SignalWork();
    lock.unlock();
}

void Connection::Impl::Validate()
{
    if (m_config.m_endpoint == EndpointType::Cris && !m_config.m_language.empty())
    {
        // TODO: make this a proper warning.
        LogInfo("WARNING: Language option for CRIS service is not yet supported and will probably be ignored.");
    }

    if (m_config.m_endpoint != EndpointType::Cris && !m_config.m_modelId.empty())
    {
        // TODO: make this a proper warning.
        LogInfo("WARNING: Modeld id option can only used in combination with a CRIS endpoint and will be ignored.");
    }

    if (m_config.m_endpoint == EndpointType::Custom && m_config.m_endpointUrl.empty())
    {
        throw invalid_argument("No valid endpoint was specified.");
    }

    if (m_config.m_authData.empty())
    {
        throw invalid_argument("No valid authentication mechanism was specified.");
    }
}

string Connection::Impl::ConstructConnectionUrl() 
{
    auto recoMode = static_cast<underlying_type_t<RecognitionMode>>(m_config.m_recoMode);

    ostringstream oss;
    oss << g_protocol;
    switch (m_config.m_endpoint) 
    {
    case EndpointType::BingSpeech:
        oss << g_bingSpeechHostname 
            << g_pathPrefix 
            << g_recoModeStrings[recoMode]
            << g_pathSuffix;
        break;
    case EndpointType::Cris:
        oss << m_config.m_modelId 
            << g_CRISHostname 
            << g_pathPrefix 
            << g_recoModeStrings[recoMode] 
            << g_pathSuffix;
        break;
    case EndpointType::CDSDK:
        oss << g_CDSDKEndpoint;
        break;
    case EndpointType::Custom:
        oss << m_config.m_endpointUrl;
        break;
    default:
        throw invalid_argument("Unknown endpoint type.");
    }
    
    auto format = static_cast<underlying_type_t<OutputFormat>>(m_config.m_outputFormat);
    oss << g_outFormatStrings[format];

    if (!m_config.m_language.empty()) 
    {
        oss << '&' << g_langQueryParam << m_config.m_language;
    }

    return oss.str();
}

void Connection::Impl::Connect()
{
    if (m_transport != nullptr || m_connected)
    {
        throw logic_error("USP connection already created.");
    }

    using HeadersPtr = deleted_unique_ptr<remove_pointer<HTTP_HEADERS_HANDLE>::type>;

    HeadersPtr connectionHeaders(HTTPHeaders_Alloc(), HTTPHeaders_Free);

    if (connectionHeaders == nullptr)
    {
        throw runtime_error("Failed to create connection headers.");
    }

    auto headersPtr = connectionHeaders.get();

    if (m_config.m_endpoint == EndpointType::CDSDK)
    {
        // TODO: MSFT: 1135317 Allow for configurable audio format
        HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, g_requestHeaderAudioResponseFormat, "riff-16khz-16bit-mono-pcm");
        HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, g_requestHeaderUserAgent, g_userAgent);
    }
    
    assert(!m_config.m_authData.empty());

    switch (m_config.m_authType)
    {
    case AuthenticationType::SubscriptionKey:
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, g_requestHeaderOcpApimSubscriptionKey, m_config.m_authData.c_str()) != 0)
        {
            throw runtime_error("Failed to set authentication using subscription key.");
        }
        break;

    case AuthenticationType::AuthorizationToken:
    {
        ostringstream oss;
        oss << "Bearer " << m_config.m_authData;
        auto token = oss.str();
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, g_requestHeaderAuthorization, token.c_str()) != 0)
        {
            throw runtime_error("Failed to set authentication using authorization token.");
        }
    }

    // TODO(1126805): url builder + auth interfaces
    case AuthenticationType::SearchDelegationRPSToken:
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, g_requestHeaderSearchDelegationRPSToken, m_config.m_authData.c_str()) != 0)
        {
            throw runtime_error("Failed to set authentication using Search-DelegationRPSToken.");
        }
        break;

    default:
        throw runtime_error(string("Unsupported authentication type"));
    }

    auto connectionUrl = ConstructConnectionUrl();

    m_telemetry = TelemetryPtr(telemetry_create(Connection::Impl::OnTelemetryData, this), telemetry_destroy);
    if (m_telemetry == nullptr)
    {
        throw runtime_error("Failed to create telemetry instance.");
    }

    m_transport = TransportPtr(TransportRequestCreate(connectionUrl.c_str(), this, m_telemetry.get(), headersPtr), TransportRequestDestroy);
    if (m_transport == nullptr)
    {
        throw runtime_error("Failed to create transport request.");
    }

    m_dnsCache = DnsCachePtr(DnsCacheCreate(), DnsCacheDestroy);
    if (!m_dnsCache)
    {
        throw runtime_error("Failed to create DNS cache.");
    }

    TransportSetDnsCache(m_transport.get(), m_dnsCache.get());
    TransportSetCallbacks(m_transport.get(), OnTransportError, OnTransportData);

    thread worker(&Connection::Impl::WorkThread, shared_from_this());
    unique_lock<recursive_mutex> lock(m_mutex);
    m_cv.wait(lock, [&] {return m_connected; });
    worker.detach();
}

void Connection::Impl::QueueMessage(const string& path, const uint8_t *data, size_t size)
{
    unique_lock<recursive_mutex> lock(m_mutex);

    USP_THROW_IF_ARGUMENT_NULL(data);

    if (path.empty())
    {
        throw invalid_argument("The path is null or empty.");
    }

    if (m_connected) 
    {
        (void)TransportMessageWrite(m_transport.get(), path.c_str(), data, size);
    }

    SignalWork();
}

void Connection::Impl::QueueAudioSegment(const uint8_t* data, size_t size)
{
    if (size == 0)
    {
        QueueAudioEnd();
        return;
    }

    unique_lock<recursive_mutex> lock(m_mutex);

    LogInfo("TS:%" PRIu64 ", Write %" PRIu32 " bytes audio data.", getTimestamp(), size);

    USP_THROW_IF_ARGUMENT_NULL(data);

    if (!m_connected)
    {
        return;
    }

    metrics_audiostream_data(size);

    int ret = 0;

    if (m_audioOffset == 0)
    {
        metrics_audiostream_init();
        metrics_audio_start(m_telemetry.get());

        ret = TransportStreamPrepare(m_transport.get(), "/audio");
        if (ret != 0)
        {
            throw runtime_error("TransportStreamPrepare failed. error=" + to_string(ret));
        }
    }

    ret = TransportStreamWrite(m_transport.get(), data, size);
    if (ret != 0)
    {
        throw runtime_error("TransportStreamWrite failed. error=" + to_string(ret));
    }

    m_audioOffset += size;
    SignalWork();
}

void Connection::Impl::QueueAudioEnd()
{
    unique_lock<recursive_mutex> lock(m_mutex);
    LogInfo("TS:%" PRIu64 ", Flush audio buffer.", getTimestamp());

    if (!m_connected || m_audioOffset == 0)
    {
        return;
    }

    auto ret = TransportStreamFlush(m_transport.get());

    m_audioOffset = 0;
    metrics_audiostream_flush();
    metrics_audio_end(m_telemetry.get());

    if (ret != 0)
    {
        throw runtime_error("Returns failure, reason: TransportStreamFlush returned " + to_string(ret));
    }
    SignalWork();
}

// Callback for transport errors
void Connection::Impl::OnTransportError(TransportHandle transportHandle, TransportError reason, void* context)
{
    (void)transportHandle;
    USP_THROW_IF_ARGUMENT_NULL(context);

    Connection::Impl *connection = static_cast<Connection::Impl*>(context);
    LogInfo("TS:%" PRIu64 ", TransportError: connection:0x%x, reason=%d.", connection->getTimestamp(), connection, reason);

    if (!connection->m_connected)
    {
        return;
    }

    auto& callbacks = connection->m_config.m_callbacks;
    switch (reason)
    {
    case TRANSPORT_ERROR_NONE:
        callbacks.OnError("Unknown transport error.");
        break;

    case TRANSPORT_ERROR_AUTHENTICATION:
        callbacks.OnError("Authentication error (401/403).");
        break;

    case TRANSPORT_ERROR_CONNECTION_FAILURE:
        callbacks.OnError("Connection failed (no connection to the remote host).");
        break;

    case TRANSPORT_ERROR_DNS_FAILURE:
        callbacks.OnError("Connection failed (the remote host did not respond).");
        break;

    case TRANSPORT_ERROR_REMOTECLOSED:
        callbacks.OnError("Connection was closed by the remote host.");
        break;

    default:
        throw runtime_error("Unknown TransportError enum value.");
        break;
    }
}

// Callback for data available on tranport
void Connection::Impl::OnTransportData(TransportHandle transportHandle, HTTP_HEADERS_HANDLE responseHeader, const unsigned char* buffer, size_t size, unsigned int errorCode, void* context)
{
    (void)transportHandle;
    USP_THROW_IF_ARGUMENT_NULL(context);

    Connection::Impl *connection = static_cast<Connection::Impl*>(context);


    if (errorCode != 0)
    {
        LogError("Response error %d.", errorCode);
        // TODO: Lower layers need appropriate signals
        return;
    }
    else if (responseHeader == NULL)
    {
        LogError("ResponseHeader is NULL.");
        return;
    }

    auto path = HTTPHeaders_FindHeaderValue(responseHeader, KEYWORD_PATH);
    if (path == NULL)
    {
        PROTOCOL_VIOLATION("response missing '%s' header", KEYWORD_PATH);
        return;
    }

    const char* contentType = NULL;
    if (size != 0)
    {
        contentType = HTTPHeaders_FindHeaderValue(responseHeader, g_keywordContentType);
        if (contentType == NULL)
        {
            PROTOCOL_VIOLATION("response '%s' contains body with no content-type", path);
            return;
        }
    }

    LogInfo("TS:%" PRIu64 " Response Message: path: %s, content type: %s, size: %zu.", connection->getTimestamp(), path, contentType, size);


    if (!connection->m_connected)
    {
        return;
    }

    string pathStr(path);
    auto& callbacks = connection->m_config.m_callbacks;

    if (pathStr == g_messagePathSpeechStartDetected)
    {
        SpeechStartDetectedMsg msg;
        callbacks.OnSpeechStartDetected(msg);
    }
    else if (pathStr == g_messagePathSpeechEndDetected)
    {
        SpeechEndDetectedMsg msg;
        callbacks.OnSpeechEndDetected(msg);
    }
    else if (pathStr == g_messagePathTurnEnd)
    {
        // flush the telemetry before invoking the onTurnEnd callback.
        // TODO: 1164154
        telemetry_flush(connection->m_telemetry.get());
        TransportCreateRequestId(connection->m_transport.get());

        TurnEndMsg msg;
        callbacks.OnTurnEnd(msg);
    }
    else if (contentPaths.find(pathStr) != contentPaths.end()) 
    {
        if (size == 0)
        {
            PROTOCOL_VIOLATION("response content size == %zu", size);
        }

        BUFFER_HANDLE responseContentHandle = BUFFER_create(
            (unsigned char*)buffer,
            size + 1);
        if (!responseContentHandle)
        {
            LogError("BUFFER_create failed.");
        }

        BUFFER_u_char(responseContentHandle)[size] = 0;

#ifdef _DEBUG
        LogInfo("TS:%" PRIu64 ", Content Message: path: %s, content type: %s, size: %zu, buffer: %s", 
            connection->getTimestamp(), path, contentType, size, (char *)BUFFER_u_char(responseContentHandle));
#endif
        (void)ContentDispatch((void*)&(callbacks), path, contentType, nullptr, responseContentHandle, size);

        BUFFER_delete(responseContentHandle);
    }
    else
    {
        connection->m_config.m_callbacks.OnUserMessage(pathStr, string(contentType), buffer, size);
    }
}

}

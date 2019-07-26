//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// transport.cpp: handling transport requests to the service.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdint.h>
#include <stddef.h>

#include <regex>
#include <assert.h>
#include <stdint.h>
#include <thread>
#include <chrono>

#include "stdafx.h"
#include "exception.h"
#include "metrics.h"
#include "transport.h"

#define COUNT_OF(a) (sizeof(a) / sizeof((a)[0]))

namespace usp = Microsoft::CognitiveServices::Speech::USP;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace usp;

// uncomment the line below to see all non-binary protocol messages in the log
// #define LOG_TEXT_MESSAGES

constexpr char g_keywordPTStimeStampName[] = "PTS";
constexpr char g_keywordSpeakerIdName[] = "SpeakerId";
constexpr char g_KeywordStreamId[]       = "X-StreamId";
constexpr char g_keywordRequestId[]      = "X-RequestId";
constexpr char g_keywordContentType[] = "Content-Type";
constexpr char g_messageHeader[]         = "%s:%s\r\nPath:%s\r\nContent-Type:application/json\r\n%s:%s\r\n\r\n";
constexpr char g_messageHeaderSsml[] = "%s:%s\r\nPath:%s\r\nContent-Type:application/ssml+xml\r\n%s:%s\r\n\r\n";
constexpr char g_messageHeaderWithoutRequestId[] = "%s:%s\r\nPath:%s\r\nContent-Type:application/json\r\n\r\n";

constexpr char g_wavheaderFormat[] = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n%s:%s\r\n";
// this is for audio and video data USP message
constexpr char g_requestFormat[]  = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n";
// compared to g_requestFormat2, has one more field PTStimestamp or userId
constexpr char g_requestFormat2[] = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n%s:%s\r\n";
// compared to g_requestFormat3, has two more fields: PTStimestamp and userId
constexpr char g_requestFormat3[] = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n";

constexpr char g_telemetryHeader[] = "%s:%s\r\nPath: telemetry\r\nContent-Type: application/json; charset=utf-8\r\n%s:%s\r\n\r\n";
constexpr char g_timeStampHeaderName[] = "X-Timestamp";
constexpr char g_audioWavName[] = "audio/x-wav";

static constexpr char g_RPSDelegationHeaderName[] = "X-Search-DelegationRPSToken";

#define WS_MESSAGE_HEADER_SIZE  2
#define WS_CONNECTION_TIME_MS   (570 * 1000) // 9.5 minutes

#define ANSWER_TIMEOUT_MS   15000

using namespace std::chrono_literals;

static void OnWSClosed(void* context);

/**
* The TransportRequest type represents a structure used for all transport
* related context.
*/
usp::TransportRequest::~TransportRequest()
{
    state = TransportState::TRANSPORT_STATE_DESTROYING;
    if (dnsCache != nullptr)
    {
        DnsCacheRemoveContextMatches(dnsCache, this);
    }

    if (ws.WSHandle != nullptr)
    {
        if (isOpen)
        {
            // starts the close handshake.
            int result;
            LogInfo("%s: start the close handshake.", __FUNCTION__);
            result = uws_client_close_handshake_async(ws.WSHandle, 1000 /*normal closure*/, "" /* not used */, OnWSClosed, this);
            if (result == 0)
            {
                // waits for close.
                const int max_n_retries = 100;
                int retries = 0;
                while (isOpen && retries++ < max_n_retries)
                {
                    LogInfo("%s: Continue to pump while waiting for close frame response (%d/%d).", __FUNCTION__, retries, max_n_retries);
                    uws_client_dowork(ws.WSHandle);
                    std::this_thread::sleep_for(10ms);
                }
                LogInfo("%s: retries %d. isOpen: %s", __FUNCTION__, retries, isOpen ? "true" : "false");
            }

            if (isOpen)
            {
                LogError("%s: force close websocket. (result=%d)", __FUNCTION__, result);
                // force to close.
                (void)uws_client_close_async(ws.WSHandle, OnWSClosed, this);

                // wait for force close.
                while (isOpen)
                {
                    uws_client_dowork(ws.WSHandle);
                    std::this_thread::sleep_for(10ms);
                }
                LogInfo("%s: isOpen: %s", __FUNCTION__, isOpen ? "true" : "false");
            }
        }
        LogInfo("%s: destroying uwsclient.", __FUNCTION__);
        uws_client_destroy(ws.WSHandle);
    }

    MetricsTransportClosed();

    if (headersHandle != nullptr)
    {
        HTTPHeaders_Free(headersHandle);
    }
}

/*
*  Helper function.
*/
int TransportCreateDataHeader(TransportHandle transportHandle, const char* requestId, char* buffer, size_t payloadSize, const std::string& psttimeStamp, const std::string& userId, bool wavHeader);


int ParseHttpHeaders(HTTP_HEADERS_HANDLE headersHandle, const unsigned char* buffer, size_t size)
{
    int ns, vs, offset;
    bool isDone;

    if (NULL == headersHandle)
    {
        return -1;
    }

    isDone = false;
    STRING_HANDLE name = NULL, value = NULL;
    for (ns = offset = vs = 0; offset < (int)size && !isDone; offset++)
    {
        switch (buffer[offset])
        {
        case ' ':
            break;
        case ':':
            // we only care about the first ':' separator, everything else is part of the value.
            if (!name)
            {
                name = STRING_from_byte_array(buffer + ns, (size_t)(offset - ns));
                vs = offset + 1;
            }
            break;
        case '\r':
            if (NULL != name)
            {
                value = STRING_from_byte_array(buffer + vs, (size_t)(offset - vs));
                HTTPHeaders_ReplaceHeaderNameValuePair(headersHandle, STRING_c_str(name), STRING_c_str(value));
                STRING_delete(name);
                STRING_delete(value);
                name = value = NULL;
            }
            else
            {
                isDone = true;
            }
            break;
        case '\n':
            vs = 0;
            ns = offset + 1;
            break;
        }
    }

    // skip the trailing '\n'
    if (isDone)
    {
        offset++;
    }

    return offset;
}


static std::string ConstructHeadersString(const std::string& prefix, HTTP_HEADERS_HANDLE httpHeadersHandle)
{
    size_t headersCount;

    if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_GetHeaderCount failed.");
        return std::string{};
    }

    std::ostringstream oss;
    oss << prefix;
    for (size_t i = 0; i < headersCount; i++)
    {
        char *temp;
        oss << "\r\n";
        if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &temp) == HTTP_HEADERS_OK)
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

static void SetTransportToClosed(TransportRequest* request)
{
    request->isOpen = false;
    request->state = TransportState::TRANSPORT_STATE_CLOSED;
}

// For websocket requests, the caller must ensure that the wsio instance is not
// open when this function is called.  uws_client_open_async will fail for an already-open
// wsio instance and we may get stuck retrying the open.
static void OnTransportError(TransportRequest* request, TransportErrorInfo* errorInfo)
{
    if (request->state == TransportState::TRANSPORT_STATE_RESETTING || request->state == TransportState::TRANSPORT_STATE_DESTROYING)
    {
        // don't propagate errors during the reset state, these errors are expected.
        request->isOpen = false;
        LogInfo("%s: request is in destroying or resetting state, return without invoking callback.", __FUNCTION__);
        return;
    }

    SetTransportToClosed(request);
    TransportErrorCallback callback = request->onTransportError;
    if (callback != NULL) {
        callback(errorInfo, request->context);
    }
}

DEFINE_ENUM_STRINGS(WS_ERROR, WS_ERROR_VALUES)

static void OnWSError(void* context, WS_ERROR errorCode)
{
    LogError("WS operation failed with error code=%d(%s)", errorCode, ENUM_TO_STRING(WS_ERROR, errorCode));

    TransportRequest* request = (TransportRequest*)context;
    if (request)
    {
        MetricsTransportDropped();
        TransportErrorInfo errorInfo;
        errorInfo.reason = TRANSPORT_ERROR_WEBSOCKET_ERROR;
        errorInfo.errorCode = errorCode;
        errorInfo.errorString = ENUM_TO_STRING(WS_ERROR, errorCode);
        OnTransportError(request, &errorInfo);
    }
}

static void OnWSPeerClosed(void* context, uint16_t* closeCode, const unsigned char* extraData, size_t extraDataLength)
{
    LogInfo("%s: context=%p", __FUNCTION__, context);
    TransportRequest* request = (TransportRequest*)context;
    if (request)
    {
        MetricsTransportDropped();
        TransportErrorInfo errorInfo;
        errorInfo.reason = TRANSPORT_ERROR_REMOTE_CLOSED;
        // WebSocket close code, if present (cf. https://tools.ietf.org/html/rfc6455#section-7.4.1):
        errorInfo.errorCode = closeCode != NULL ? *closeCode : -1;
        char* errorReason = nullptr;
        if (extraDataLength > 0)
        {
            errorReason = (char*)malloc(extraDataLength + 1);
            if (errorReason != nullptr)
            {
                strncpy_s(errorReason, extraDataLength + 1, (const char*)extraData, extraDataLength);
                errorReason[extraDataLength] = '\0';
            }
        }
        errorInfo.errorString = (const char*)errorReason;
        OnTransportError(request, &errorInfo); // technically, not an error.
        if (errorReason != nullptr)
        {
            free(errorReason);
        }
    }
}

static void OnWSClosed(void* context)
{
    LogInfo("%s: context=%p", __FUNCTION__, context);
    TransportRequest* request = (TransportRequest*)context;
    if (request)
    {
        if (request->state == TransportState::TRANSPORT_STATE_RESETTING)
        {
            // Re-open the connection.
            request->isOpen = false;
            request->state = TransportState::TRANSPORT_STATE_NETWORK_CHECK;
        }
        else if (request->state == TransportState::TRANSPORT_STATE_DESTROYING)
        {
            LogInfo("%s: request is in destroying state, ignore OnWSClosed().", __FUNCTION__);
            request->isOpen = false;
        }
        else
        {
            MetricsTransportClosed();
            SetTransportToClosed(request);
            TransportClosedCallback callback = request->onClosedCallback;
            if (callback != NULL)
            {
                callback(request->context);
            }
        }
    }
}

DEFINE_ENUM_STRINGS(WS_OPEN_RESULT, WS_OPEN_RESULT_VALUES)

static void OnWSOpened(void* context, WS_OPEN_RESULT_DETAILED open_result_detailed)
{
    TransportRequest* request = (TransportRequest*)context;
    WS_OPEN_RESULT open_result = open_result_detailed.result;
    char errorString[32];

    if (request == nullptr || request->state == TransportState::TRANSPORT_STATE_DESTROYING)
    {
        LogInfo("%s: request is null or in destroying state, ignore OnWSOpened()", __FUNCTION__);
        return;
    }
    request->isOpen = (open_result == WS_OPEN_OK);
    if (request->isOpen)
    {
        request->state = TransportState::TRANSPORT_STATE_CONNECTED;
        request->connectionTime = telemetry_gettime();
        LogInfo("Opening websocket completed. TransportRequest: 0x%x, wsio handle: 0x%x", request, request->ws.WSHandle);
        MetricsTransportConnected(*request->telemetry, request->connectionId);
        TransportOpenedCallback callback = request->onOpenedCallback;
        if (callback != NULL)
        {
            callback(request->context);
        }
    }
    else
    {
        // It is safe to transition to TRANSPORT_STATE_CLOSED because the
        // connection is not open.  We must be careful with this state for the
        // reasons described in OnTransportError.
        request->state = TransportState::TRANSPORT_STATE_CLOSED;
        MetricsTransportDropped();

        LogError("WS open operation failed with result=%d(%s), code=%d[0x%08x]",
            open_result,
            ENUM_TO_STRING(WS_OPEN_RESULT, open_result),
            open_result_detailed.code,
            open_result_detailed.code);

        TransportErrorCallback callback = request->onTransportError;
        if (callback)
        {
            TransportErrorInfo errorInfo;
            if (open_result == WS_OPEN_ERROR_BAD_RESPONSE_STATUS)
            {
                errorInfo.reason = TRANSPORT_ERROR_WEBSOCKET_UPGRADE;
                errorInfo.errorCode = open_result_detailed.code; // HTTP status
                errorInfo.errorString = NULL;
            }
            else
            {
                errorInfo.reason = TRANSPORT_ERROR_CONNECTION_FAILURE;
                errorInfo.errorCode = open_result_detailed.result;
                errorInfo.errorString = errorString;
                (void)snprintf(errorString, COUNT_OF(errorString) - 1, "%d",
                    open_result_detailed.code);
            }
            callback(&errorInfo, request->context);
        }
    }
}

static void OnWSFrameSent(void* context, WS_SEND_FRAME_RESULT sendResult)
{
    (void)sendResult;
    // TODO ok to ignore?

    TransportPacket *msg = reinterpret_cast<TransportPacket*>(context);
    if (msg->msgtype != METRIC_MESSAGE_TYPE_INVALID)
    {
        MetricsTransportStateEnd(msg->msgtype);
    }
    delete msg;
}

ResponseFrameType WSFrameEnumToResponseFrameEnum(WS_FRAME_TYPE frameType)
{
    switch (frameType)
    {
    case WS_FRAME_TYPE_TEXT:
        return FRAME_TYPE_TEXT;
    case WS_FRAME_TYPE_BINARY:
        return FRAME_TYPE_BINARY;
    default:
        return FRAME_TYPE_UNKNOWN;
    }
}

static void OnWSFrameReceived(void* context, unsigned char frame_type, const unsigned char* buffer, size_t size)
{
    TransportRequest*  request = (TransportRequest*)context;
    HTTP_HEADERS_HANDLE responseHeadersHandle;
    int offset;
    uint16_t headerSize;

    if (context)
    {
        if (request->state == TransportState::TRANSPORT_STATE_DESTROYING)
        {
            LogInfo("%s: request is in destroying state, ignore OnWSFrameReceived().", __FUNCTION__);

            return;
        }
        if (request->onRecvResponse)
        {
            responseHeadersHandle = HTTPHeaders_Alloc();
            if (!responseHeadersHandle)
            {
                return;
            }

            offset = -1;

            switch (frame_type)
            {
            case WS_FRAME_TYPE_TEXT:
#ifdef LOG_TEXT_MESSAGES
                LogInfo("Message received:%.*s", size, buffer);
#endif
                offset = ParseHttpHeaders(responseHeadersHandle, buffer, size);
                break;
            case WS_FRAME_TYPE_BINARY:
                if (size < 2)
                {
                    PROTOCOL_VIOLATION("unable to read binary message length%s", "");
                    goto Exit;
                }

                headerSize = (uint16_t)(buffer[0] << 8);
                headerSize |= (uint16_t)(buffer[1] << 0);
                offset = ParseHttpHeaders(responseHeadersHandle, buffer + 2, headerSize);
                if (offset >= 0)
                {
                    offset += 2;
                }
                break;
            }

            if (offset < 0)
            {
                PROTOCOL_VIOLATION("Unable to parse response headers%s", "");
                MetricsTransportParsingError();
                goto Exit;
            }

            TransportResponse response;
            response.frameType = WSFrameEnumToResponseFrameEnum(static_cast<WS_FRAME_TYPE>(frame_type));
            response.responseHeader = responseHeadersHandle;
            response.buffer = buffer + offset;
            response.bufferSize = size - offset;

            request->onRecvResponse(&response, request->context);

        Exit:
            HTTPHeaders_Free(responseHeadersHandle);
        }
    }
}

static void DnsComplete(DnsCacheHandle handle, int error, DNS_RESULT_HANDLE resultHandle, void *context)
{
    TransportRequest* request = (TransportRequest*)context;
    assert(request->state == TransportState::TRANSPORT_STATE_NETWORK_CHECKING);

    (void)handle;
    (void)resultHandle;

    MetricsTransportStateEnd(MetricMessageType::METRIC_TRANSPORT_STATE_DNS);

    if (NULL != request)
    {
        if (error != 0)
        {
            LogError("Network Check failed %d", error);

            // The network instance has not been connected yet so it is not
            // open, and it's safe to call OnTransportError.  We do the DNS
            // check before opening the connection.  We assert about this with
            // the TRANSPORT_STATE_NETWORK_CHECKING check above.
            TransportErrorInfo errorInfo;
            errorInfo.reason = TRANSPORT_ERROR_DNS_FAILURE;
            errorInfo.errorCode = error;
            errorInfo.errorString = NULL;
            OnTransportError(request, &errorInfo);
        }
        else
        {
            LogInfo("Network Check completed");
            request->state = TransportState::TRANSPORT_STATE_OPENING;
        }
    }
}

static int ProcessPacket(TransportRequest* request, std::unique_ptr<TransportPacket> packet)
{
    int err = 0;

    // re-stamp X-timestamp header value
    char timeString[TIME_STRING_MAX_SIZE] = "";
    int timeStringLength = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    int offset = sizeof(g_timeStampHeaderName);
    if (packet->wstype == WS_FRAME_TYPE_BINARY)
    {
        // Include the first 2 bytes for header length
        offset += 2;
    }
    memcpy(packet->buffer.get() + offset, timeString, timeStringLength);

    auto unmanaged_packet = packet.release();

    err = uws_client_send_frame_async(
        request->ws.WSHandle,
        unmanaged_packet->wstype == WS_FRAME_TYPE_TEXT ? static_cast<uint8_t>(WS_TEXT_FRAME) : static_cast<uint8_t>(WS_BINARY_FRAME),
        unmanaged_packet->buffer.get(),
        unmanaged_packet->length,
        true, // TODO: check this.
        OnWSFrameSent,
        unmanaged_packet);
    if (err)
    {
        LogError("WS transfer failed with %d", err);
    }
    return err;
}

static void WsioQueue(TransportRequest* request, std::unique_ptr<TransportPacket> packet)
{
    if (NULL == request)
    {
        return;
    }

    if (request->state == TransportState::TRANSPORT_STATE_CLOSED)
    {
        LogError("Trying to send on a previously closed socket");
        MetricsTransportInvalidStateError();
        return;
    }
#ifdef LOG_TEXT_MESSAGES
    if (packet->wstype == WS_TEXT_FRAME)
    {
        LogInfo("Message sent:\n>>>>>>>>>>\n%.*s\n>>>>>>>>>>", packet->length, packet->buffer.get());
    }
#endif
    request->queue.push(std::move(packet));
}

static std::unique_ptr<TransportPacket> PrepareTelemetryPayload(const uint8_t* eventBuffer, size_t eventBufferSize, const std::string& requestId)
{
    // serialize headers.
    size_t headerLen;

    size_t payloadSize = sizeof(g_telemetryHeader) +
        sizeof(g_keywordRequestId) +
        NO_DASH_UUID_LEN + // size of requestId
        sizeof(g_timeStampHeaderName) +
        TIME_STRING_MAX_SIZE +
        eventBufferSize;

    auto msg = std::make_unique<TransportPacket>(static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_TELEMETRY), static_cast<unsigned char>(WS_FRAME_TYPE_TEXT), payloadSize);

    char timeString[TIME_STRING_MAX_SIZE];
    int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    if (timeStringLen < 0)
    {
        ThrowRuntimeError("There was a problem getting time string");
    }

    // serialize the entire telemetry message.
    headerLen = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
        payloadSize,
        g_telemetryHeader,
        g_timeStampHeaderName,
        timeString,
        g_keywordRequestId,
        requestId.c_str());

    msg->length = headerLen;
    // body
    memcpy(msg->buffer.get() + msg->length, eventBuffer, eventBufferSize);
    msg->length += eventBufferSize;
    return msg;
}

void usp::TransportWriteTelemetry(TransportHandle handle, const uint8_t* buffer, size_t bytesToWrite, const char *requestId)
{
    if (NULL != handle)
    {
        auto msg = PrepareTelemetryPayload(buffer, bytesToWrite, requestId);
        WsioQueue(handle, std::move(msg));
    }
}

std::unique_ptr<TransportRequest> usp::TransportRequestCreate(const std::string& host, void* context, Telemetry* telemetry, HTTP_HEADERS_HANDLE connectionHeaders, const std::string& connectionId, const ProxyServerInfo* proxyInfo, const bool disableDefaultVerifyPaths, const char *trustedCert, const bool disableCrlCheck)
{
    constexpr auto protocol_wss = "wss";

    const std::regex hostRegex{ R"((wss?)://(([A-Za-z0-9-_.]+)(:([0-9]+))?)([/?].*))" };

    if (host.empty())
    {
        LogError("Received an empty host. Please provide a valid host.");
        return nullptr;
    }

    if (NO_DASH_UUID_LEN < connectionId.size() + 1)
    {
        LogError("Invalid size of connection Id. Please use a valid GUID with dashes removed.");
        return nullptr;
    }

    std::smatch hostMatch;
    if (!std::regex_match(host, hostMatch, hostRegex))
    {
        /* No match! */
        LogError("Invalid host. Please provide a valid host (ws|wss://hostname[:port][/resource][?query_string]).");
        return nullptr;
    }

    assert(hostMatch.size() == 7);

    const std::string protocol = hostMatch[1];
    const std::string hostName = hostMatch[3];
    const std::string portString = hostMatch[5];
    const std::string resource = hostMatch[6];

    const bool useSSL = protocol == protocol_wss;
    const int port = portString.empty() ? -1 : std::stoi(portString);
    const int defaultPort = useSSL ? 443 : 80;

    auto request = std::make_unique<TransportRequest>();
    if (nullptr == request)
    {
        return nullptr;
    }
    request->context = context;
    request->telemetry = telemetry;
    request->ws.config.port = port == -1 ? defaultPort : port;
    request->connectionId = connectionId;
    HTTPHeaders_ReplaceHeaderNameValuePair(connectionHeaders, "X-ConnectionId", request->connectionId.c_str());
    // HACKHACK: azure-c-shared doesn't allow to specify connection ("upgrade") headers, using this as a temporary
    // workaround until we migrate to something more decent.
    request->upgradeHeaders = ConstructHeadersString("USP", connectionHeaders);
    request->ws.config.protocol = request->upgradeHeaders.data();
    request->url = hostName;
    request->resourceName = resource;
    /* These are just views */
    request->ws.config.hostname = request->url.data();
    request->ws.config.resource_name = request->resourceName.data();

    if (proxyInfo != nullptr)
    {
        HTTP_PROXY_IO_CONFIG proxyConfig;
        proxyConfig.hostname = request->ws.config.hostname;
        proxyConfig.port = request->ws.config.port;
        request->proxyHost = proxyInfo->host;
        proxyConfig.proxy_hostname = request->proxyHost.c_str();
        proxyConfig.proxy_port = proxyInfo->port;
        request->proxyUser = proxyInfo->username;
        proxyConfig.username = request->proxyUser.c_str();
        request->proxyPassword = proxyInfo->password;
        proxyConfig.password = request->proxyPassword.c_str();
        const IO_INTERFACE_DESCRIPTION* underlyingIOInterface = http_proxy_io_get_interface_description();
        if (underlyingIOInterface == nullptr)
        {
            LogError("NULL proxy interface description");
            return nullptr;
        }
        void* underlyingIOParameters = &proxyConfig;

        TLSIO_CONFIG tlsioConfig;
        if (useSSL == true)
        {
            const IO_INTERFACE_DESCRIPTION* tlsioInterface = platform_get_default_tlsio();
            if (tlsioInterface == nullptr)
            {
                LogError("NULL TLSIO interface description");
                return nullptr;
            }
            else
            {
                tlsioConfig.hostname = request->ws.config.hostname;
                tlsioConfig.port = request->ws.config.port;
                tlsioConfig.underlying_io_interface = underlyingIOInterface;
                tlsioConfig.underlying_io_parameters = underlyingIOParameters;
                underlyingIOInterface = tlsioInterface;
                underlyingIOParameters = &tlsioConfig;
            }
        }
        request->ws.WSHandle = uws_client_create_with_io(underlyingIOInterface, underlyingIOParameters, request->ws.config.hostname, request->ws.config.port, request->ws.config.resource_name, (WS_PROTOCOL*)& request->ws.config.protocol, 1);
    }
    else
    {
        WSIO_CONFIG cfg = request->ws.config;
        WS_PROTOCOL wsProto;
        wsProto.protocol = cfg.protocol;
        request->ws.WSHandle = uws_client_create(cfg.hostname, cfg.port, cfg.resource_name, useSSL, &wsProto, 1);
    }

#ifdef SPEECHSDK_USE_OPENSSL
    int tls_version = OPTION_TLS_VERSION_1_2;
    uws_client_set_option(request->ws.WSHandle, OPTION_TLS_VERSION, &tls_version);

    uws_client_set_option(request->ws.WSHandle, OPTION_DISABLE_DEFAULT_VERIFY_PATHS, &disableDefaultVerifyPaths);
    if (trustedCert)
    {
        uws_client_set_option(request->ws.WSHandle, OPTION_TRUSTED_CERT, trustedCert);
    }

    // Note: layers above guarantee that CRL check can only be disabled when using a trusted cert.
    uws_client_set_option(request->ws.WSHandle, OPTION_DISABLE_CRL_CHECK, &disableCrlCheck);
#else
    UNUSED(trustedCert);
    UNUSED(disableCrlCheck);
    UNUSED(disableDefaultVerifyPaths);
#endif

    request->headersHandle = HTTPHeaders_Alloc();
    if (nullptr != request->headersHandle)
    {
        return request;
    }

    return nullptr;
}

static int TransportOpen(TransportRequest* request)
{
    if (!request->isOpen)
    {
        if (NULL == request->ws.WSHandle)
        {
            return -1;
        }
        else
        {
            LogInfo("Start to open websocket. TransportRequest: 0x%x, wsio handle: 0x%x", request, request->ws.WSHandle);
            MetricsTransportStart(*request->telemetry, request->connectionId);
            const int result = uws_client_open_async(request->ws.WSHandle,
                OnWSOpened, request,
                OnWSFrameReceived, request,
                OnWSPeerClosed, request,
                OnWSError, request);
            if (result)
            {
                LogError("uws_client_open_async failed with result %d", result);
                return -1;
            }
        }
    }

    return 0;
}


static int ReplaceHeaderNameValuePair(HTTP_HEADERS_HANDLE headers, const char *name, const char *value, int* valueChanged)
{
    const char* headerValue;
    *valueChanged = 0;

    headerValue = HTTPHeaders_FindHeaderValue(headers, name);
    if (NULL == headerValue || strcmp(headerValue, value))
    {
        *valueChanged = 1;
        return HTTPHeaders_ReplaceHeaderNameValuePair(headers, name, value);
    }

    return 0;
}

// Todo: deal with CogSvc token
// Return zero if the tokens in the headers are already up-to-date, and
// non-zero otherwise.
static int add_auth_headers(TokenStore tokenStore, HTTP_HEADERS_HANDLE headers, void* context)
{
    (void)context;

    int tokenChanged = 0;
    STRING_HANDLE accessToken = STRING_new();
    int ret = 0; //token_store_get_access_token(token_store, kCortanaScope, access_token);

    if (ret == 0 && STRING_length(accessToken) > 0)
    {
        ReplaceHeaderNameValuePair(headers, g_RPSDelegationHeaderName, STRING_c_str(accessToken), &tokenChanged);
    }
    else
    {
        LogError("cached bing token is not valid");
    }
    STRING_delete(accessToken);

    (void)tokenStore;
    return tokenChanged;
}

int usp::TransportRequestPrepare(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    int err = 0;
    if (NULL == request)
    {
        return -1;
    }

    if (request->tokenStore != NULL)
    {
        int tokenChanged = add_auth_headers(request->tokenStore, request->headersHandle, request->context);

        if ((telemetry_gettime() - request->connectionTime) >= WS_CONNECTION_TIME_MS)
        {
            tokenChanged = 1;
            LogInfo("forcing connection closed");
        }

        if (tokenChanged > 0 &&
            (request->state == TransportState::TRANSPORT_STATE_CONNECTED))
        {
            // The token is snapped at the time that we are connected.  Reset
            // our connection if the token changes to avoid using a stale token
            // with our requests.
            //
            // We only do this for websocket connections because HTTP
            // connections are short-lived.

            // Set the transport state before calling uws_client_close_async.
            request->state = TransportState::TRANSPORT_STATE_RESETTING;
            LogInfo("token changed, resetting connection");
            MetricsTransportReset();

            const int closeResult =
                uws_client_close_async(request->ws.WSHandle, OnWSClosed, request);
            if (closeResult)
            {
                // we failed to close the ws connection. Destroy it instead.
                // TODO is this complete? bubble up error?
            }
        }
    }

    if (request->state == TransportState::TRANSPORT_STATE_CLOSED)
    {
        request->state = TransportState::TRANSPORT_STATE_NETWORK_CHECK;
    }

    return err;
}

int usp::TransportMessageWrite(TransportHandle transportHandle, const std::string& path, const uint8_t* buffer, size_t bufferSize, const char* requestId)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    int ret;

    if (NULL == request)
    {
        return -1;
    }

    ret = usp::TransportRequestPrepare(request);
    if (ret)
    {
        return ret;
    }

    bool includeRequestId = requestId != NULL && requestId[0] != '\0';

    size_t payloadSize = sizeof(g_messageHeader) +
                         path.size() +
                         (includeRequestId ? sizeof(g_keywordRequestId) : 0) +
                         (includeRequestId ? NO_DASH_UUID_LEN : 0) +
                         sizeof(g_timeStampHeaderName) +
                         TIME_STRING_MAX_SIZE +
                         bufferSize;
    auto msg = std::make_unique<TransportPacket>(static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_DEVICECONTEXT), static_cast<unsigned char>(WS_FRAME_TYPE_TEXT), payloadSize);

    char timeString[TIME_STRING_MAX_SIZE];
    int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    if (timeStringLen < 0)
    {
        return -1;
    }

    // add headers
    if (includeRequestId)
    {
        if (0 == strcmp(path.data(), "ssml"))
        {
            msg->length = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
                                    payloadSize,
                                    g_messageHeaderSsml,
                                    g_timeStampHeaderName,
                                    timeString,
                                    path.c_str(),
                                    g_keywordRequestId,
                                    requestId);
        }
        else
        {
            msg->length = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
                                    payloadSize,
                                    g_messageHeader,
                                    g_timeStampHeaderName,
                                    timeString,
                                    path.c_str(),
                                    g_keywordRequestId,
                                    requestId);
        }
    }
    else
    {
        msg->length = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
                                payloadSize,
                                g_messageHeaderWithoutRequestId,
                                g_timeStampHeaderName,
                                timeString,
                                path.c_str());
    }

    // add body
    memcpy(msg->buffer.get() + msg->length, buffer, bufferSize);
    msg->length += bufferSize;

    WsioQueue(request, std::move(msg));
    return 0;
}

int usp::TransportStreamPrepare(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    int ret;

    if (NULL == request)
    {
        return -1;
    }

    request->streamId++;

    ret = usp::TransportRequestPrepare(request);
    return ret;
}

int usp::TransportStreamWrite(TransportHandle transportHandle, const std::string& path, const Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk, const char* requestId)
{
    size_t bufferSize = (size_t)audioChunk->size;
    auto buffer = audioChunk->data.get();
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request)
    {
        LogError("transportHandle is NULL.");
        return -1;
    }

    uint8_t msgtype = METRIC_MESSAGE_TYPE_INVALID;
    if (bufferSize == 0)
    {
        msgtype = static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_AUDIO_LAST);
        if (!request->ws.chunksent)
        {
            return 0;
        }

        request->ws.chunksent = false;
    }
    else if (!request->ws.chunksent)
    {
        if (bufferSize < 6)
        {
            LogError("Bad payload");
            return -1;
        }

        if (memcmp(buffer, "RIFF", 4) && memcmp(buffer, "#!SILK", 6))
        {
            return 0;
        }

        request->ws.chunksent = true;
        msgtype = static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_AUDIO_START);
    }

    std::string pstTimeStamp = audioChunk->capturedTime;
    std::string userId = audioChunk->userId;

    size_t payloadSize = sizeof(g_requestFormat3) +
        path.size() +
        sizeof(g_KeywordStreamId) +
        30 +
        sizeof(g_keywordRequestId) +
        sizeof(requestId) +
        sizeof(g_timeStampHeaderName) +
        TIME_STRING_MAX_SIZE +
        sizeof(g_keywordContentType) +
        sizeof(g_audioWavName) +
        pstTimeStamp.length() +
        userId.length() +
        bufferSize + 2; // 2 = header length

    auto msg = std::make_unique<TransportPacket>(msgtype, static_cast<unsigned char>(WS_FRAME_TYPE_BINARY), payloadSize);

    // fill the msg->buffer with the header content
    bool wavheader = audioChunk->isWavHeader;
    auto headerLen = TransportCreateDataHeader(request, requestId, reinterpret_cast<char *>(msg->buffer.get()), payloadSize, pstTimeStamp, userId, wavheader);
    if (headerLen < 0)
    {
        return -1;
    }
    // two bytes length
    msg->buffer[0] = (uint8_t)((headerLen >> 8) & 0xff);
    msg->buffer[1] = (uint8_t)((headerLen >> 0) & 0xff);
    msg->length = headerLen + WS_MESSAGE_HEADER_SIZE;

    // body
    std::memcpy(msg->buffer.get() + msg->length, buffer, bufferSize);
    msg->length += bufferSize;

    WsioQueue(request, std::move(msg));
    return 0;
}

int TransportCreateDataHeader(TransportHandle transportHandle, const char* requestId, char* buffer, size_t payloadSize, const std::string& psttimeStamp, const std::string& userId, bool wavheader)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request)
    {
        LogError("transportHandle is NULL.");
        return -1;
    }

    char timeString[TIME_STRING_MAX_SIZE];
    int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    if (timeStringLen < 0)
    {
        return -1;
    }
    size_t headerLen;
    std::string path = "audio";
    if (wavheader)
    {
        headerLen = sprintf_s(buffer + WS_MESSAGE_HEADER_SIZE,
            payloadSize - WS_MESSAGE_HEADER_SIZE,
            g_wavheaderFormat,
            g_timeStampHeaderName,
            timeString,
            path.c_str(),
            g_KeywordStreamId,
            request->streamId,
            g_keywordRequestId,
            requestId,
            g_keywordContentType,
            g_audioWavName);
    }
    else if (psttimeStamp.empty() && userId.empty())
    {
        headerLen = sprintf_s(buffer + WS_MESSAGE_HEADER_SIZE,
        payloadSize - WS_MESSAGE_HEADER_SIZE,
        g_requestFormat,  // "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n";
        g_timeStampHeaderName,  //"X-Timestamp";
            timeString,
        path.c_str(),
        g_KeywordStreamId,  //"X-StreamId";
        request->streamId,  // %d, streamId is uint32_t
        g_keywordRequestId, //"X-RequestId";
            requestId);
    }
    else if (!psttimeStamp.empty() && userId.empty())
    {
        headerLen = sprintf_s(buffer + WS_MESSAGE_HEADER_SIZE,
            payloadSize - WS_MESSAGE_HEADER_SIZE,
            g_requestFormat2,
            g_timeStampHeaderName,
            timeString,
            path.c_str(),
            g_KeywordStreamId,
            request->streamId,
            g_keywordRequestId,
            requestId,
            g_keywordPTStimeStampName,
            psttimeStamp.c_str());
    }
    else if (psttimeStamp.empty() && !userId.empty())
    {
        headerLen = sprintf_s(buffer + WS_MESSAGE_HEADER_SIZE,
            payloadSize - WS_MESSAGE_HEADER_SIZE,
            g_requestFormat2,
            g_timeStampHeaderName,
            timeString,
            path.c_str(),
            g_KeywordStreamId,
            request->streamId,
            g_keywordRequestId,
            requestId,
            g_keywordSpeakerIdName,
            userId.c_str());
    }
    // both are there
    else
    {
        headerLen = sprintf_s(buffer + WS_MESSAGE_HEADER_SIZE,
            payloadSize - WS_MESSAGE_HEADER_SIZE,
            g_requestFormat3,
            g_timeStampHeaderName,
            timeString,
            path.c_str(),
            g_KeywordStreamId,
            request->streamId,
            g_keywordRequestId,
            requestId,
            g_keywordSpeakerIdName,
            userId.c_str(),
            g_keywordPTStimeStampName,
            psttimeStamp.c_str());
    }

    return (int)headerLen;
}

int usp::TransportStreamFlush(TransportHandle transportHandle, const std::string& path, const char* requestId)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request)
    {
        LogError("transportHandle is null.");
        return -1;
    }
    if (NULL == requestId || requestId[0] == '\0')
    {
        LogError("requestId is null or empty.");
        return -1;
    }
    // No need to check whether WS is open, since it is possbile that
    // flush is called even before the WS connection is established, in
    // particular if the audio file is very short. Just append the zero-sized
    // buffer as indication of end-of-audio.
    return usp::TransportStreamWrite(request, path, std::make_shared<Microsoft::CognitiveServices::Speech::Impl::DataChunk>(nullptr, 0), requestId);
}

void usp::TransportDoWork(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;

    if (NULL == transportHandle)
    {
        return;
    }

    switch (request->state)
    {
    case TransportState::TRANSPORT_STATE_CLOSED:
        while (!request->queue.empty())
        {
            request->queue.pop();
        }
        break;

    case TransportState::TRANSPORT_STATE_RESETTING:
        // Do nothing -- we're waiting for WSOnCloseForReset to be invoked.
        break;

    case TransportState::TRANSPORT_STATE_DESTROYING:
        // Do nothing. We are waiting for closing the transport request.
        break;

    case TransportState::TRANSPORT_STATE_NETWORK_CHECKING:
        DnsCacheDoWork(request->dnsCache, request);
        return;

    case TransportState::TRANSPORT_STATE_NETWORK_CHECK:
        if (NULL == request->dnsCache)
        {
            // skip dns checks
            request->state = TransportState::TRANSPORT_STATE_OPENING;
        }
        else
        {
            request->state = TransportState::TRANSPORT_STATE_NETWORK_CHECKING;
            const char* host = request->ws.config.hostname;
            LogInfo("Start network check %s", host);
            MetricsTransportStateStart(MetricMessageType::METRIC_TRANSPORT_STATE_DNS);
            if (DnsCacheGetAddr(request->dnsCache, host, DnsComplete, request))
            {
                LogError("DnsCacheGetAddr failed");
                request->state = TransportState::TRANSPORT_STATE_OPENING;
            }
        }
        LogInfo("%s: open transport.", __FUNCTION__);
        if (TransportOpen(request))
        {
            request->state = TransportState::TRANSPORT_STATE_CLOSED;
            LogError("Failed to open transport");
            return;
        }
        return;

    case TransportState::TRANSPORT_STATE_OPENING:
        // wait for the "On open" callback
        break;

    case TransportState::TRANSPORT_STATE_CONNECTED:
        while (request->isOpen && !request->queue.empty())
        {
            auto packet = std::move(request->queue.front());
            request->queue.pop();

            if (packet->msgtype != METRIC_MESSAGE_TYPE_INVALID)
            {
                MetricsTransportStateStart(packet->msgtype);
            }
            int res = ProcessPacket(request, std::move(packet));
            if (res != 0)
            {
                TransportErrorInfo errorInfo;
                errorInfo.reason = TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME;
                errorInfo.errorCode = res;
                errorInfo.errorString = NULL;
                OnTransportError(request, &errorInfo);
            }
        }

        break;
    }

    uws_client_dowork(request->ws.WSHandle);
}

int usp::TransportSetCallbacks(TransportHandle transportHandle,
    TransportErrorCallback errorCallback,
    TransportResponseCallback recvCallback,
    TransportOpenedCallback openedCallback,
    TransportClosedCallback closedCallback)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request)
    {
        return -1;
    }

    request->onTransportError = errorCallback;
    request->onRecvResponse = recvCallback;
    request->onOpenedCallback = openedCallback;
    request->onClosedCallback = closedCallback;
    return 0;
}

int TransportSetTokenStore(TransportHandle transportHandle, TokenStore tokenStore)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request || NULL == tokenStore)
    {
        return -1;
    }

    request->tokenStore = tokenStore;

    return 0;
}

void usp::TransportSetDnsCache(TransportHandle transportHandle, DnsCacheHandle dnsCache)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    request->dnsCache = dnsCache;
}


//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// tranpsort.c: handling tranport requests to the service.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdint.h>
#include <stddef.h>

#include <assert.h>
#include <stdint.h>
#include <thread>
#include <chrono>

#include "azure_c_shared_utility_includes.h"

#include "metrics.h"
#include "transport.h"

#define COUNT_OF(a) (sizeof(a) / sizeof((a)[0]))

// uncomment the line below to see all non-binary protocol messages in the log
// #define LOG_TEXT_MESSAGES

const char g_KeywordStreamId[]       = "X-StreamId";
const char g_keywordRequestId[]      = "X-RequestId";
const char g_keywordWSS[]            = "wss://";
const char g_keywordWS[]             = "ws://";
const char g_messageHeader[]         = "%s:%s\r\nPath:%s\r\nContent-Type:application/json\r\n%s:%s\r\n\r\n";
const char g_messageHeaderWithoutRequestId[] = "%s:%s\r\nPath:%s\r\nContent-Type:application/json\r\n\r\n";

const char g_requestFormat[]  = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n";
const char g_telemetryHeader[] = "%s:%s\r\nPath: telemetry\r\nContent-Type: application/json; charset=utf-8\r\n%s:%s\r\n\r\n";
const char g_timeStampHeaderName[] = "X-Timestamp";

static const char g_RPSDelegationHeaderName[] = "X-Search-DelegationRPSToken";

#define WS_MESSAGE_HEADER_SIZE  2
#define WS_CONNECTION_TIME_MS   (570 * 1000) // 9.5 minutes

#define ANSWER_TIMEOUT_MS   15000

using namespace Microsoft::CognitiveServices::Speech::USP;

using namespace std::chrono_literals;

typedef struct _TransportPacket
{
    struct _TransportPacket*  next;
    uint8_t                   msgtype;
    unsigned char             wstype;
    size_t                    length;
    uint8_t                   buffer[1]; // must be last
} TransportPacket;

typedef enum _TRANSPORT_STATE
{
    TRANSPORT_STATE_CLOSED = 0,
    TRANSPORT_STATE_NETWORK_CHECK,
    TRANSPORT_STATE_NETWORK_CHECKING,
    TRANSPORT_STATE_OPENING,
    TRANSPORT_STATE_CONNECTED,
    TRANSPORT_STATE_RESETTING // needed for token-based auth (currently not used).
} TransportState;


/**
* The TransportRequest type represents a structure used for all transport
* related context.
*/
typedef struct _TransportRequest
{
    struct
    {
        UWS_CLIENT_HANDLE WSHandle;
        size_t pathLen;
        WSIO_CONFIG config;
        bool chunksent;
    } ws;
    const char*                  path;
    TransportResponseCallback    onRecvResponse;
    TransportErrorCallback       onTransportError;
    TransportOpenedCallback      onOpenedCallback;
    TransportClosedCallback      onClosedCallback;
    HTTP_HEADERS_HANDLE          headersHandle;
    bool                         isOpen;
    char*                        url;
    void*                        context;
    char                         connectionId[NO_DASH_UUID_LEN];
    uint32_t                     streamId;
    bool                         needAuthenticationHeader;
    TransportState               state;
    SINGLYLINKEDLIST_HANDLE      queue;
    DnsCacheHandle               dnsCache;
    uint64_t                     connectionTime;
    TokenStore                   tokenStore;
    Telemetry*                   telemetry;
} TransportRequest;

/*
* Converts a dashed GUID to a no-dash guid string.
*/
void GuidDToNFormat(char *guidString)
{
    char *dst = guidString;
    do
    {
        if (*guidString != '-')
        {
            *dst = *guidString;
            dst++;
        }

        guidString++;
    } while (dst[-1]);
}

int hexchar_to_int(char c)
{
    if ((c >= 'A') && c <= 'F')
    {
        return (int)(c - 'A') + 10;
    }
    else if ((c >= 'a') && c <= 'f')
    {
        return (int)(c - 'a') + 10;
    }
    else if ((c >= '0') && c <= '9')
    {
        return (int)(c - '0');
    }

    return 0;
}

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


// TODO: temporarily lifted this from httpapi_winhtt, remove after refactoring.
static char* ConstructHeadersString(HTTP_HEADERS_HANDLE httpHeadersHandle)
{
    char* result;
    size_t headersCount;

    if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = NULL;
        LogError("HTTPHeaders_GetHeaderCount failed.");
    }
    else
    {
        size_t i;

        /*the total size of all the headers is given by sumof(lengthof(everyheader)+2)*/
        size_t toAlloc = 0;
        for (i = 0; i < headersCount; i++)
        {
            char *temp;
            if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &temp) == HTTP_HEADERS_OK)
            {
                toAlloc += strlen(temp);
                toAlloc += 2;
                free(temp);
            }
            else
            {
                LogError("HTTPHeaders_GetHeader failed");
                break;
            }
        }

        if (i < headersCount)
        {
            result = NULL;
        }
        else
        {
            result = (char*)malloc(toAlloc * sizeof(char) + 1);

            if (result == NULL)
            {
                LogError("unable to malloc");
                /*let it be returned*/
            }
            else
            {
                result[0] = '\0';
                for (i = 0; i < headersCount; i++)
                {
                    char* temp;
                    if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &temp) != HTTP_HEADERS_OK)
                    {
                        LogError("unable to HTTPHeaders_GetHeader");
                        break;
                    }
                    else
                    {
                        (void)strcat_s(result, toAlloc+1, temp);
                        (void)strcat_s(result, toAlloc+1, "\r\n");
                        free(temp);
                    }
                }

                if (i < headersCount)
                {
                    free(result);
                    result = NULL;
                }
                else
                {
                    /*all is good*/
                }
            }
        }
    }

    return result;
}

static void SetTransportToClosed(TransportRequest* request)
{
    request->isOpen = false;
    request->state = TRANSPORT_STATE_CLOSED;
}

// For websocket requests, the caller must ensure that the wsio instance is not
// open when this function is called.  uws_client_open_async will fail for an already-open
// wsio instance and we may get stuck retrying the open.
static void OnTransportError(TransportRequest* request, TransportErrorInfo* errorInfo)
{
    if (request->state == TRANSPORT_STATE_RESETTING)
    {
        // don't propagate errors during the reset state, these errors are expected.
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
                strncpy(errorReason, (const char*)extraData, extraDataLength);
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
    TransportRequest* request = (TransportRequest*)context;
    if (request)
    {
        if (request->state == TRANSPORT_STATE_RESETTING)
        {
            // Re-open the connection.
            request->isOpen = false;
            request->state = TRANSPORT_STATE_NETWORK_CHECK;
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

    request->isOpen = (open_result == WS_OPEN_OK);
    if (request->isOpen)
    {
        request->state = TRANSPORT_STATE_CONNECTED;
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
        request->state = TRANSPORT_STATE_CLOSED;
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

    TransportPacket *msg = (TransportPacket*)context;
    // the first byte is the message type
    if (msg->msgtype != METRIC_MESSAGE_TYPE_INVALID)
    {
        MetricsTransportStateEnd(msg->msgtype);
    }
    free(msg);
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

    if (context && request->onRecvResponse)
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
            PROTOCOL_VIOLATION("Unable to parse response headers%s","");
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

static void DnsComplete(DnsCacheHandle handle, int error, DNS_RESULT_HANDLE resultHandle, void *context)
{
    TransportRequest* request = (TransportRequest*)context;
    assert(request->state == TRANSPORT_STATE_NETWORK_CHECKING);

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
            request->state = TRANSPORT_STATE_OPENING;
        }
    }
}

static int ProcessPacket(TransportRequest* request, TransportPacket* packet)
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
    memcpy((char*)packet->buffer + offset, timeString, timeStringLength);

    err = uws_client_send_frame_async(
        request->ws.WSHandle,
        packet->wstype == WS_FRAME_TYPE_TEXT ? static_cast<uint8_t>(WS_TEXT_FRAME) : static_cast<uint8_t>(WS_BINARY_FRAME),
        packet->buffer,
        packet->length,
        true, // TODO: check this.
        OnWSFrameSent,
        packet);
    if (err)
    {
        LogError("WS transfer failed with %d", err);
        free(packet);
    }
    return err;
}

static void WsioQueue(TransportRequest* request, TransportPacket* packet)
{
    if (NULL == request)
    {
        free(packet);
        return;
    }

    if (request->state == TRANSPORT_STATE_CLOSED)
    {
        LogError("Trying to send on a previously closed socket");
        MetricsTransportInvalidStateError();
        free(packet);
        return;
    }
#ifdef LOG_TEXT_MESSAGES
    if (packet->wstype == WS_TEXT_FRAME)
    {
        LogInfo("Message sent:\n>>>>>>>>>>\n%.*s\n>>>>>>>>>>", packet->length, packet->buffer);
    }
#endif
    (void)singlylinkedlist_add(request->queue, packet);
}

static void PrepareTelemetryPayload(TransportHandle request, const uint8_t* eventBuffer, size_t eventBufferSize, TransportPacket **pPacket, const char *requestId)
{
    (void)request;

    // serialize headers.
    size_t headerLen;

    size_t payloadSize = sizeof(g_telemetryHeader) +
        sizeof(g_keywordRequestId) +
        NO_DASH_UUID_LEN + // size of requestId
        sizeof(g_timeStampHeaderName) +
        TIME_STRING_MAX_SIZE +
        eventBufferSize;

    TransportPacket *msg = (TransportPacket*)malloc(sizeof(TransportPacket) + payloadSize);
    if (msg == nullptr)
    {
        // malloc failed, everything is lost
        return;
    }
    msg->msgtype = static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_TELEMETRY);
    msg->wstype = WS_FRAME_TYPE_TEXT;

    char timeString[TIME_STRING_MAX_SIZE];
    int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    if (timeStringLen < 0)
    {
        free(msg);
        return;
    }

    // serialize the entire telemetry message.
    headerLen = sprintf_s((char*)msg->buffer,
        payloadSize,
        g_telemetryHeader,
        g_timeStampHeaderName,
        timeString,
        g_keywordRequestId,
        requestId);

    msg->length = headerLen;
    // body
    memcpy(msg->buffer + msg->length, eventBuffer, eventBufferSize);
    msg->length += eventBufferSize;
    *pPacket = msg;
}

void TransportWriteTelemetry(TransportHandle handle, const uint8_t* buffer, size_t bytesToWrite, const char *requestId)
{
    if (NULL != handle)
    {
        TransportPacket *msg = NULL;
        PrepareTelemetryPayload(handle, buffer, bytesToWrite, &msg, requestId);
        WsioQueue(handle, msg);
    }
}

TransportHandle TransportRequestCreate(const char* host, void* context, Telemetry* telemetry, HTTP_HEADERS_HANDLE connectionHeaders, const char* connectionId, const ProxyServerInfo* proxyInfo)
{
    TransportRequest *request;
    int err = -1;
    const char* src;
    char* dst;
    int port = -1;
    const char* hostName = NULL;
    size_t hostOffset = 0;

    bool use_ssl = false;

    if (!host)
    {
        return NULL;
    }

    if (strstr(host, g_keywordWSS) == host)
    {
        hostOffset = strlen(g_keywordWSS);
    }
    else if(strstr(host, g_keywordWS) == host)
    {
        hostOffset = strlen(g_keywordWS);
    }
    else
    {
        // Only WebSockets-based transport
        return NULL;
    }

    // parse port number from host name if it is specified in the url

    hostName = host + hostOffset;
    const char* front = strchr(hostName, ':');
    if (front)
    {
        const char* back = strchr(front, '/');
        if (back == NULL)
        {
            back = front + strlen(front);
        }
        size_t length = back - front;
        char* portStr = (char*)malloc(length);
        if (portStr == NULL)
        {
            LogError("Failed to allocate memory for port string.");
            return NULL;
        }
        memcpy(portStr, ++front, length);
        portStr[length - 1] = '\0';
        port = atoi(portStr);
        free(portStr);
        if (port == 0)
        {
            LogError("Failed to parse port string.");
            return NULL;
        }
    }

    request = (TransportRequest*)malloc(sizeof(TransportRequest));
    if (NULL == request)
    {
        return NULL;
    }

    memset(request, 0, sizeof(TransportRequest));
    request->context = context;
    request->telemetry = telemetry;
    // disable by default, usually only websocket request will need it
    request->queue = singlylinkedlist_create();

    if (strstr(host, g_keywordWSS) == host)
    {
        request->ws.config.port = port == -1 ? 443 : port;
        use_ssl = true;
    }
    else if (strstr(host, g_keywordWS) == host)
    {
        request->ws.config.port = port == -1 ? 80 : port;
        use_ssl = false;
    }

    if (sizeof(request->connectionId) < strlen(connectionId) + 1)
    {
        free(request);
        LogError("Invalid size of connection Id. Please use a valid GUID with dashes removed.");
        return NULL;
    }

    strncpy(request->connectionId, connectionId, strlen(connectionId));
    HTTPHeaders_ReplaceHeaderNameValuePair(connectionHeaders, "X-ConnectionId", request->connectionId);
    char* headers = ConstructHeadersString(connectionHeaders);
    const char* proto = "USP\r\n";
    size_t len = strlen(proto) + strlen(headers) -2 /*(minus extra '\r\n')*/ + 1;
    char* str = (char*)malloc(len);
    if (str == NULL)
    {
        free(request);
        free(headers);
        LogError("Failed to allocate memory for connection headers string.");
        return NULL;
    }
    (void)snprintf(str, len, "%s%s", proto, headers);
    // HACKHACK: azure-c-shared doesn't allow to specify connection ("upgrade") headers, using this as a temporary
    // workaround until we migrate to something more decent.
    request->ws.config.protocol = str;

    len = strlen(host);
    request->url = (char*)malloc(len + 2); // 2=2 x NULL
    if (request->url != NULL)
    {
        host += strlen(use_ssl ? g_keywordWSS : g_keywordWS);
        dst = request->url;
        request->ws.config.hostname = dst;

        // split up host + path
        src = strchr(host, '/');
        if (NULL != src)
        {
            // Pull out port if it is there.
            const char *portSpec = strchr(host, ':');
            size_t copyLen = (portSpec == NULL ? src : portSpec) - host;
            if (copyLen <= len)
            {
                err = 0;
                memcpy(dst, host, copyLen);
                *(dst + copyLen) = 0;
                dst += (src - host) + 1;
                request->ws.config.resource_name = dst;
                strcpy_s(dst, len + 2 - (dst - request->url), src);

                if (proxyInfo != NULL)
                {
                    HTTP_PROXY_IO_CONFIG proxy_config;
                    proxy_config.hostname = request->ws.config.hostname;
                    proxy_config.port = request->ws.config.port;
                    proxy_config.proxy_hostname = proxyInfo->host;
                    proxy_config.proxy_port = proxyInfo->port;
                    proxy_config.username = proxyInfo->username;
                    proxy_config.password = proxyInfo->password;
                    const IO_INTERFACE_DESCRIPTION *underlying_io_interface = http_proxy_io_get_interface_description();
                    if (underlying_io_interface == NULL)
                    {
                        LogError("NULL proxy interface description");
                        free(request->url);
                        free(request);
                        free(headers);
                        free(str);
                        return NULL;
                    }
                    void *underlying_io_parameters = &proxy_config;

                    TLSIO_CONFIG tlsio_config;
                    if (use_ssl == true)
                    {
                        const IO_INTERFACE_DESCRIPTION *tlsio_interface = platform_get_default_tlsio();
                        if (tlsio_interface == NULL)
                        {
                            LogError("NULL TLSIO interface description");
                            free(request->url);
                            free(request);
                            free(headers);
                            free(str);
                            return NULL;
                        }
                        else
                        {
                            tlsio_config.hostname = request->ws.config.hostname;
                            tlsio_config.port = request->ws.config.port;
                            tlsio_config.underlying_io_interface = underlying_io_interface;
                            tlsio_config.underlying_io_parameters = underlying_io_parameters;
                            underlying_io_interface = tlsio_interface;
                            underlying_io_parameters = &tlsio_config;
                        }
                    }

                    request->ws.WSHandle = uws_client_create_with_io(underlying_io_interface, underlying_io_parameters, request->ws.config.hostname, request->ws.config.port, request->ws.config.resource_name, (WS_PROTOCOL *)&request->ws.config.protocol, 1);
                }
                else
                {
                    WSIO_CONFIG cfg = request->ws.config;
                    WS_PROTOCOL ws_proto;
                    ws_proto.protocol = cfg.protocol;
                    request->ws.WSHandle = uws_client_create(cfg.hostname, cfg.port, cfg.resource_name, use_ssl, &ws_proto, 1);
                }

#ifdef SPEECHSDK_USE_OPENSSL
                int tls_version = OPTION_TLS_VERSION_1_2;
                uws_client_set_option(request->ws.WSHandle, OPTION_TLS_VERSION, &tls_version);
#endif
            }
        }

        if (err)
        {
            free(request->url);
            free(request);
            LogError("Invalid URL");
            // Fall through will clear remaining objects and return NULL
        }
    }

    free(headers);
    free(str);

    if (!err)
    {
        request->headersHandle = HTTPHeaders_Alloc();
        if (NULL != request->headersHandle)
        {
            return request;
        }
    }

    return NULL;
}

void TransportRequestDestroy(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;

    if (!request)
    {
        return;
    }

    if (request->dnsCache)
    {
        DnsCacheRemoveContextMatches(request->dnsCache, request);
    }

    if (request->ws.WSHandle)
    {
        if (request->isOpen)
        {
            (void)uws_client_close_async(request->ws.WSHandle, OnWSClosed, request);
            // TODO ok to ignore return code?
            while (request->isOpen)
            {
                uws_client_dowork(request->ws.WSHandle);
                std::this_thread::sleep_for(100ms);
            }
        }

        uws_client_destroy(request->ws.WSHandle);
    }

    MetricsTransportClosed();

    if (request->url)
    {
        free(request->url);
    }

    if (request->headersHandle)
    {
        HTTPHeaders_Free(request->headersHandle);
    }

    if (request->queue)
    {
        for (LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(request->queue);
            list_item != NULL;
            list_item = singlylinkedlist_get_next_item(list_item))
        {
            free((TransportPacket*)singlylinkedlist_item_get_value(list_item));
        }

        singlylinkedlist_destroy(request->queue);
        request->queue = NULL;
    }

    free(request);
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

int TransportRequestPrepare(TransportHandle transportHandle)
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
            (request->state == TRANSPORT_STATE_CONNECTED))
        {
            // The token is snapped at the time that we are connected.  Reset
            // our connection if the token changes to avoid using a stale token
            // with our requests.
            //
            // We only do this for websocket connections because HTTP
            // connections are short-lived.

            // Set the transport state before calling uws_client_close_async.
            request->state = TRANSPORT_STATE_RESETTING;
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

    if (request->state == TRANSPORT_STATE_CLOSED)
    {
        request->state = TRANSPORT_STATE_NETWORK_CHECK;
    }

    return err;
}

int TransportMessageWrite(TransportHandle transportHandle, const char* path, const uint8_t* buffer, size_t bufferSize, const char* requestId)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    int ret;

    if (NULL == request)
    {
        return -1;
    }

    request->path = path;
    request->ws.pathLen = strlen(request->path);

    ret = TransportRequestPrepare(request);
    if (ret)
    {
        return ret;
    }

    bool includeRequestId = requestId != NULL && requestId[0] != '\0';

    size_t payloadSize = sizeof(g_messageHeader) +
                         (size_t)(request->ws.pathLen) +
                         (includeRequestId ? sizeof(g_keywordRequestId) : 0) +
                         (includeRequestId ? NO_DASH_UUID_LEN : 0) +
                         sizeof(g_timeStampHeaderName) +
                         TIME_STRING_MAX_SIZE +
                         bufferSize;
    TransportPacket *msg = (TransportPacket *)malloc(sizeof(TransportPacket) + payloadSize);
    if (msg == nullptr)
    {
        /* malloc failed, everything is lost */
        return -1;
    }

    msg->msgtype = static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_DEVICECONTEXT);
    msg->wstype = WS_FRAME_TYPE_TEXT;

    char timeString[TIME_STRING_MAX_SIZE];
    int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    if (timeStringLen < 0)
    {
        free(msg);
        return -1;
    }

    // add headers
    if (includeRequestId)
    {
        msg->length = sprintf_s((char *)msg->buffer,
                                payloadSize,
                                g_messageHeader,
                                g_timeStampHeaderName,
                                timeString,
                                request->path,
                                g_keywordRequestId,
                                requestId);
    }
    else
    {
        msg->length = sprintf_s((char *)msg->buffer,
                                payloadSize,
                                g_messageHeaderWithoutRequestId,
                                g_timeStampHeaderName,
                                timeString,
                                request->path);
    }

    // add body
    memcpy(msg->buffer + msg->length, buffer, bufferSize);
    msg->length += bufferSize;

    WsioQueue(request, msg);
    return 0;
}

int TransportStreamPrepare(TransportHandle transportHandle, const char* path)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    int ret;

    if (NULL == request)
    {
        return -1;
    }

    request->streamId++;

    if (NULL != path)
    {
        request->path = path + 1; // 1=remove "/"
        request->ws.pathLen = strlen(request->path);
    }

    ret = TransportRequestPrepare(request);
    return ret;
}

int TransportStreamWrite(TransportHandle transportHandle, const Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk, const char* requestId)
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

    size_t headerLen;
    size_t payloadSize = sizeof(g_requestFormat) +
        (size_t)request->ws.pathLen +
        sizeof(g_KeywordStreamId) +
        30 +
        sizeof(g_keywordRequestId) +
        sizeof(requestId) +
        sizeof(g_timeStampHeaderName) +
        TIME_STRING_MAX_SIZE +
        bufferSize + 2; // 2 = header length
    TransportPacket *msg = (TransportPacket*)malloc(sizeof(TransportPacket) + WS_MESSAGE_HEADER_SIZE + payloadSize);
    if (msg == nullptr)
    {
        /* malloc failed, everything is lost =( */
        return -1;
    }
    msg->msgtype = msgtype;
    msg->wstype = WS_FRAME_TYPE_BINARY;

    char timeString[TIME_STRING_MAX_SIZE];
    int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    if (timeStringLen < 0)
    {
        free(msg);
        return -1;
    }

    // add headers
    headerLen = sprintf_s((char*)msg->buffer + WS_MESSAGE_HEADER_SIZE,
        payloadSize,
        g_requestFormat,
        g_timeStampHeaderName,
        timeString,
        request->path,
        g_KeywordStreamId,
        request->streamId,
        g_keywordRequestId,
        requestId);

    // two byte length
    msg->buffer[0] = (uint8_t)((headerLen >> 8) & 0xff);
    msg->buffer[1] = (uint8_t)((headerLen >> 0) & 0xff);
    msg->length = WS_MESSAGE_HEADER_SIZE + headerLen;

    // body
    memcpy(msg->buffer + msg->length, buffer, bufferSize);
    msg->length += bufferSize;

    WsioQueue(request, msg);
    return 0;
}

int TransportStreamFlush(TransportHandle transportHandle, const char* requestId)
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
    return TransportStreamWrite(request, std::make_shared<Microsoft::CognitiveServices::Speech::Impl::DataChunk>(nullptr, 0), requestId);
}

void TransportDoWork(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    TransportPacket*  packet;
    LIST_ITEM_HANDLE list_item;

    if (NULL == transportHandle)
    {
        return;
    }

    switch (request->state)
    {
    case TRANSPORT_STATE_CLOSED:
        while (NULL != (list_item = singlylinkedlist_get_head_item(request->queue)))
        {
            free((TransportPacket*)singlylinkedlist_item_get_value(list_item));
            singlylinkedlist_remove(request->queue, list_item);
        }
        break;

    case TRANSPORT_STATE_RESETTING:
        // Do nothing -- we're waiting for WSOnCloseForReset to be invoked.
        break;

    case TRANSPORT_STATE_NETWORK_CHECKING:
        DnsCacheDoWork(request->dnsCache, request);
        return;

    case TRANSPORT_STATE_NETWORK_CHECK:
        if (NULL == request->dnsCache)
        {
            // skip dns checks
            request->state = TRANSPORT_STATE_OPENING;
        }
        else
        {
            request->state = TRANSPORT_STATE_NETWORK_CHECKING;
            const char* host = request->ws.config.hostname;
            LogInfo("Start network check %s", host);
            MetricsTransportStateStart(MetricMessageType::METRIC_TRANSPORT_STATE_DNS);
            if (DnsCacheGetAddr(request->dnsCache, host, DnsComplete, request))
            {
                LogError("DnsCacheGetAddr failed");
                request->state = TRANSPORT_STATE_OPENING;
            }
        }
        if (TransportOpen(request))
        {
            request->state = TRANSPORT_STATE_CLOSED;
            LogError("Failed to open transport");
            return;
        }
        return;

    case TRANSPORT_STATE_OPENING:
        // wait for the "On open" callback
        break;

    case TRANSPORT_STATE_CONNECTED:
        while (request->isOpen &&  NULL != (list_item = singlylinkedlist_get_head_item(request->queue)))
        {
            packet = (TransportPacket*)singlylinkedlist_item_get_value(list_item);
            if (packet->msgtype != METRIC_MESSAGE_TYPE_INVALID)
            {
                MetricsTransportStateStart(packet->msgtype);
            }
            int res = ProcessPacket(request, packet);
            if (res != 0)
            {
                TransportErrorInfo errorInfo;
                errorInfo.reason = TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME;
                errorInfo.errorCode = res;
                errorInfo.errorString = NULL;
                OnTransportError(request, &errorInfo);
            }
            singlylinkedlist_remove(request->queue, list_item);
        }

        break;
    }

    uws_client_dowork(request->ws.WSHandle);
}

int TransportSetCallbacks(TransportHandle transportHandle,
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

void TransportSetDnsCache(TransportHandle transportHandle, DnsCacheHandle dnsCache)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    request->dnsCache = dnsCache;
}

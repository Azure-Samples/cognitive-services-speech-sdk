//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// tranpsort.c: handling tranport requests to the service.
//

#include <assert.h>
#include <stdint.h>

#include "azure_c_shared_utility/uhttp.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/queue.h"
#include "azure_c_shared_utility/base64.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "uspinternal.h"

// uncomment the line below to see all non-binary protocol messages in the log
// #define LOG_TEXT_MESSAGES

#define ws   u1._ws
#define http u1._http

const char g_KeywordStreamId[]       = "X-StreamId";
const char g_keywordRequestId[]      = "X-RequestId";
const char g_keywordWSS[]            = "wss://";
const char g_keywordWS[]             = "ws://";
const char g_requestFormat[]  = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n";
const char g_telemetryHeader[] = "%s:%s\r\nPath: telemetry\r\nContent-Type: application/json; charset=utf-8\r\n%s:%s\r\n\r\n";
const char g_timeStampHeaderName[] = "X-Timestamp";

static const char g_RPSDelegationHeaderName[] = "X-Search-DelegationRPSToken";

#define WS_MESSAGE_HEADER_SIZE  2
#define WS_CONNECTION_TIME_MS   (570 * 1000) // 9.5 minutes

#define ANSWER_TIMEOUT_MS   15000

typedef struct _TransportStream
{
    int id;
    IOBUFFER* ioBuffer;
    size_t bufferSize;
    STRING_HANDLE contentType;
    void* context;
    struct _TransportStream* next;
} TransportStream;

typedef struct _TransportPacket
{
    struct _TransportPacket*  next;
    uint8_t                   msgtype;
    WSIO_MSG_TYPE             wstype;
    size_t                    length;
    uint8_t                   buffer[1]; // must be last
} TransportPacket;

static TransportStream* TransportCreateStream(TransportHandle transportHandle, int streamId, const char* contentType, void* context);
static TransportStream* TransportGetStream(TransportHandle transportHandle, int streamId);

typedef enum _TRANSPORT_STATE
{
    TRANSPORT_STATE_CLOSED = 0,
    TRANSPORT_STATE_NETWORK_CHECK,
    TRANSPORT_STATE_NETWORK_CHECKING,
    TRANSPORT_STATE_OPENING,
    TRANSPORT_STATE_CONNECTED,
    TRANSPORT_STATE_SENT, // only used for HTTP
                          // The request is being closed and will be re-opened.
    TRANSPORT_STATE_RESETTING
} TransportState;


/**
* The TransportRequest type represents a structure used to for all transport
* related context.
*/
typedef struct _TransportRequest
{
    union
    {
        struct
        {
            HTTP_CLIENT_HANDLE      requestHandle;
            TLSIO_CONFIG            tlsIoConfig;
        } _http;

        struct
        {
            CONCRETE_IO_HANDLE      WSHandle;
            size_t                  pathLen;
            WSIO_CONFIG             config;
            bool                    chunksent;
            TransportStream*        streamHead;
            BUFFER_HANDLE           metricData;
            size_t                  metricOffset;
            LOCK_HANDLE             metricLock;
        } _ws;
    } u1;

    const char*                  path;
    TransportResponseCallback    onRecvResponse;
    TransportErrorCallback       onTransportError;
    HTTP_HEADERS_HANDLE          headersHandle;
    BUFFER_HANDLE                responseContentHandle;
    bool                         isWS;
    bool                         isCompleted;
    bool                         isOpen;
    char*                        url;
    void*                        context;
    char                         requestId[37];
    char                         connectionId[37];
    uint8_t                      binaryRequestId[16];
    uint32_t                     streamId;
    bool                         needAuthenticationHeader;
    TransportState               state;
    LIST_HANDLE                  queue;
    DnsCacheHandle               dnsCache;
    uint64_t                     connectionTime;
    TokenStore                   tokenStore;
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
    int ns, ne, vs, offset;
    bool isDone;

    if (NULL == headersHandle)
    {
        return -1;
    }

    isDone = false;
    STRING_HANDLE name = NULL, value = NULL;
    for (ns = ne = offset = vs = 0; offset < (int)size && !isDone; offset++)
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
            ne = vs = 0;
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

static void InvalidateRequestId(TransportRequest* request)
{
    memset(request->requestId, 0, sizeof(request->requestId) / sizeof(request->requestId[0]));
    memset(request->binaryRequestId, 0, sizeof(request->binaryRequestId) / sizeof(request->binaryRequestId[0]));
}

// For websocket requests, the caller must ensure that the wsio instance is not
// open when this function is called.  wsio_open will fail for an already-open
// wsio instance and we may get stuck retrying the open.
static void OnTransportError(TransportRequest* request, TransportError err)
{
    request->isCompleted = true;
    request->isOpen = false;

    // don't propogate errors during the reset state, these errors are expected.
    if (request->state != TRANSPORT_STATE_RESETTING)
    {
        request->state = TRANSPORT_STATE_CLOSED;
        InvalidateRequestId(request);
        if (request->onTransportError != NULL) {
            request->onTransportError(request, err, request->context);
        }
    }
}

static void OnHttpErrorCallback(HTTP_CLIENT_HANDLE handle, void* callbackContext)
{
    TransportRequest* request = (TransportRequest*)callbackContext;
    OnTransportError(request, TRANSPORT_ERROR_NONE);
    http_client_close(handle);
}

static void DnsComplete(DnsCacheHandle handle, int error, DNS_RESULT_HANDLE resultHandle, void *context)
{
    TransportRequest* request = (TransportRequest*)context;
    assert(request->state == TRANSPORT_STATE_NETWORK_CHECKING);

    (void)handle;
    (void)resultHandle;

    metrics_transport_state_end(METRIC_TRANSPORT_STATE_DNS);

    if (NULL != request)
    {
        if (error != 0)
        {
            LogError("Network Check failed %d", error);

            // The network instance has not been connected yet so it is not
            // open, and it's safe to call OnTransportError.  We do the DNS
            // check before opening the connection.  We assert about this with
            // the TRANSPORT_STATE_NETWORK_CHECKING check above.
            OnTransportError(request, TRANSPORT_ERROR_DNS_FAILURE);
        }
        else
        {
            LogInfo("Network Check completed");
            request->state = TRANSPORT_STATE_OPENING;
        }
    }
}

static void OnConnectedCallback(HTTP_CLIENT_HANDLE handle, void* callbackContext)
{
    TransportRequest* request = (TransportRequest*)callbackContext;
    (void)handle;
    request->state = TRANSPORT_STATE_CONNECTED;
}

static void ChunkedMessageRecv(HTTP_CLIENT_HANDLE handle, void* callbackContext, const unsigned char* content, size_t contentLen, unsigned int statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, int isReplyComplete)
{
    TransportRequest* request = (TransportRequest*)callbackContext;
    (void)handle;
    const char* mime;

    if (contentLen)
    {
        if (!request->responseContentHandle)
        {
            request->responseContentHandle = BUFFER_create(content, contentLen);
        }
        else
        {
            size_t size = BUFFER_length(request->responseContentHandle);
            BUFFER_enlarge(request->responseContentHandle, contentLen);
            memcpy(BUFFER_u_char(request->responseContentHandle) + size, content, contentLen);
        }
    }

    if (isReplyComplete)
    {
        request->isCompleted = true;

        if (request->onRecvResponse)
        {
            request->onRecvResponse(request, responseHeadersHandle, BUFFER_u_char(request->responseContentHandle), BUFFER_length(request->responseContentHandle), statusCode, request->context);
        }
        else if (statusCode == 200)
        {
            mime = HTTPHeaders_FindHeaderValue(responseHeadersHandle, g_keywordContentType);
            if (NULL != mime)
            {
                ContentDispatch(request->context, NULL /* path */, mime, 0, request->responseContentHandle, USE_BUFFER_SIZE);
            }
        }

        request->state = TRANSPORT_STATE_CLOSED;

        if (request->responseContentHandle)
        {
            BUFFER_delete(request->responseContentHandle);
            request->responseContentHandle = NULL;
        }
    }
}

static void WsioOnOpened(void* context, IO_OPEN_RESULT open_result)
{
    TransportRequest* request = (TransportRequest*)context;
    TransportError err;
    int status;

    request->isOpen = (open_result == IO_OPEN_OK);
    if (open_result == IO_OPEN_OK)
    {
        request->state = TRANSPORT_STATE_CONNECTED;
        request->connectionTime = telemetry_gettime();
        LogInfo("Transport websocket connected");
        metrics_transport_connected(request->connectionId);
    }
    else
    {
        // It is safe to transition to TRANSPORT_STATE_CLOSED because the
        // connection is not open.  We must be careful with this state for the
        // reasons described in OnTransportError.
        request->state = TRANSPORT_STATE_CLOSED;
        metrics_transport_dropped();
        request->isCompleted = true;

        status = wsio_gethttpstatus(request->ws.WSHandle); 
        LogError("WSIO open failed %d %d", status, open_result);
        metrics_transport_error(request->connectionId, status);

        switch (status)
        {
        case 0: // no http response, assume general connection failure
            err = TRANSPORT_ERROR_CONNECTION_FAILURE;
            break;

        case 401:
        case 403:
            err = TRANSPORT_ERROR_AUTHENTICATION;
            break;

        default:
            // TODO: Add socket level detection
            err = TRANSPORT_ERROR_NONE;
            break;
        }

        if (request->onTransportError)
        {
            request->onTransportError((TransportHandle)request, err, request->context);
        }
    }
}

static int OnStreamChunk(TransportStream* stream, const uint8_t* buffer, size_t bufferSize)
{
    if (NULL == stream)
    {
        return -1;
    }

    if (NULL == stream->ioBuffer)
    {
        stream->ioBuffer = IoBufferNew();
        if (NULL == stream->ioBuffer)
        {
            LogError("IoBufferNew failed");
            return -1;
        }
    }

    if (NULL == stream->contentType)
    {
        PROTOCOL_VIOLATION("Content-Type not defined");
        return -1;
    }

    if (0 == stream->bufferSize)
    {
        LogInfo("First chunk for %s", STRING_c_str(stream->contentType));
        metrics_tts_first_chunk();
        ContentDispatch(
            stream->context,
            NULL,  // path
            STRING_c_str(stream->contentType),
            stream->ioBuffer,
            NULL,
            stream->bufferSize);
    }
    else if (0 == bufferSize)
    {
        LogInfo("Last chunk for %s", STRING_c_str(stream->contentType));
        metrics_tts_last_chunk();
    }

    if (bufferSize != 0)
    {
        IoBufferWrite(stream->ioBuffer, buffer, 0, (int)bufferSize);
        stream->bufferSize += bufferSize;
    }
    else if (stream->bufferSize > 0 && NULL != stream->contentType)
    {
        IoBufferWrite(stream->ioBuffer, 0, 0, 0); // mark buffer completed
        IoBufferDelete(stream->ioBuffer);
        stream->ioBuffer = 0;
        stream->bufferSize = 0;
    }

    return 0;
}

static void WsioOnBytesRecv(void* context, const unsigned char* buffer, size_t size, WSIO_MSG_TYPE message_type)
{
    TransportRequest*  request = (TransportRequest*)context;
    HTTP_HEADERS_HANDLE responseHeadersHandle;
    int offset;
    uint16_t headerSize;
    const char* value;
    TransportStream* stream;
    int streamId;

    if (context && request->onRecvResponse)
    {
        responseHeadersHandle = HTTPHeaders_Alloc();
        if (!responseHeadersHandle)
        {
            return;
        }

        offset = -1;

        switch (message_type)
        {
        case WSIO_MSG_TYPE_TEXT:
#ifdef LOG_TEXT_MESSAGES
            LogInfo("Message received:\n<<<<<<<<<<\n%.*s\n<<<<<<<<<<", size, buffer);
#endif
            offset = ParseHttpHeaders(responseHeadersHandle, buffer, size);
            break;
        case WSIO_MSG_TYPE_BINARY:
            if (size < 2)
            {
                PROTOCOL_VIOLATION("unable to read binary message length");
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
            PROTOCOL_VIOLATION("Unable to parse response headers");
            metrics_transport_parsingerror();
            goto Exit;
        }

        value = HTTPHeaders_FindHeaderValue(responseHeadersHandle, g_keywordRequestId);
        if (!value || strcmp(value, request->requestId))
        {
            PROTOCOL_VIOLATION("Unexpected request id '%s', expected '%s' Path: %s", value, request->requestId,
                HTTPHeaders_FindHeaderValue(responseHeadersHandle, KEYWORD_PATH));
            metrics_unexpected_requestid(value);
            goto Exit;
        }

        value = HTTPHeaders_FindHeaderValue(responseHeadersHandle, KEYWORD_PATH);
        if (value)
        {
            metrics_received_message(value);
        }

        // optional StreamId header
        value = HTTPHeaders_FindHeaderValue(responseHeadersHandle, g_KeywordStreamId);
        if (NULL != value)
        {
            streamId = atoi(value);
            stream = TransportGetStream((TransportHandle)request, streamId);
            if (NULL == stream)
            {
                stream = TransportCreateStream(
                    (TransportHandle)request, 
                    streamId, 
                    HTTPHeaders_FindHeaderValue(responseHeadersHandle, g_keywordContentType),
                    request->context);
                if (NULL == stream)
                {
                    LogError("alloc TransportStream failed");
                    goto Exit;
                }
            }

            (void)OnStreamChunk(stream, buffer + offset, size - offset);
        }
        else
        {
            request->onRecvResponse((TransportHandle)request, responseHeadersHandle, buffer + offset, size - offset, 0, request->context);
        }

    Exit:
        HTTPHeaders_Free(responseHeadersHandle);

    }
}

// WSIO sets the wsio instance to an error state before calling this.  It is
// safe to call wsio_open.
//
// SRS_WSIO_01_121: When on_underlying_ws_error is called while the IO is OPEN
// the wsio instance shall be set to ERROR and an error shall be indicated via
// the on_io_error callback passed to wsio_open.
static void WsioOnError(void* context)
{
    TransportRequest* request = (TransportRequest*)context;
    if (context)
    {
        metrics_transport_dropped();
        OnTransportError(request, TRANSPORT_ERROR_REMOTECLOSED);
    }
}

static void WsioOnSendComplete(void* context, IO_SEND_RESULT sendResult)
{
    (void)sendResult;
    TransportPacket *msg = (TransportPacket*)context;
    // the first byte is the message type
    if (msg->msgtype != METRIC_MESSAGE_TYPE_INVALID)
    {
        metrics_transport_state_end(msg->msgtype);
    }
    free(msg);
}

static void WsioQueue(TransportRequest* request, TransportPacket* packet)
{
    if (request->isWS  &&  request->requestId[0] == '\0')
    {
        LogError("Trying to send on a previously closed socket");
        metrics_transport_invalidstateerror();
        free(packet);
        return;
    }
#ifdef LOG_TEXT_MESSAGES
    if (packet->wstype == WSIO_MSG_TYPE_TEXT)
    {
        LogInfo("Message sent:\n>>>>>>>>>>\n%.*s\n>>>>>>>>>>", packet->length, packet->buffer);
    }
#endif
    (void)queue_enqueue(request->queue, packet);
}

static void WSOnClose(void* context)
{
    TransportRequest* request = (TransportRequest*)context;
    request->isOpen = false;
    request->state = TRANSPORT_STATE_CLOSED;
}

static void PrepareTelemetryPayload(void *context, const uint8_t* eventBuffer, size_t eventBufferSize, TransportPacket **pPacket, const char *requestId)
{
    TransportRequest *request = (TransportRequest*)context;

    // serialize headers.
    size_t headerLen;

    size_t payloadSize = sizeof(g_telemetryHeader) +
        sizeof(g_keywordRequestId) +
        sizeof(request->requestId) +
        sizeof(g_timeStampHeaderName) +
        TIME_STRING_MAX_SIZE +
        eventBufferSize;

    TransportPacket *msg = (TransportPacket*)malloc(sizeof(TransportPacket) + payloadSize);
    msg->msgtype = METRIC_MESSAGE_TYPE_TELEMETRY;
    msg->wstype = WSIO_MSG_TYPE_TEXT;

    char timeString[TIME_STRING_MAX_SIZE];
    int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
    if (timeStringLen < 0)
    {
        return;
    }

    // serialize the entire telemetry message.
    headerLen = sprintf_s((char*)msg->buffer,
        payloadSize,
        g_telemetryHeader,
        g_timeStampHeaderName,
        timeString,
        g_keywordRequestId,
        IS_STRING_NULL_OR_EMPTY(requestId) ? request->requestId : requestId);

    msg->length = headerLen;
    // body
    memcpy(msg->buffer + msg->length, eventBuffer, eventBufferSize);
    msg->length += eventBufferSize;
    *pPacket = msg;
}

static void WSOnTelemetryData(const uint8_t* buffer, size_t bytesToWrite, void *context, const char *requestId)
{
    TransportRequest *request = (TransportRequest*)context;
    TransportPacket *msg = NULL;
    PrepareTelemetryPayload(context, buffer, bytesToWrite, &msg, requestId);

    WsioQueue(request, msg);
}

static void TransportCreateConnectionId(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (transportHandle && request->isWS)
    {
        UniqueId_Generate(request->connectionId, sizeof(request->connectionId));
        GuidDToNFormat(request->connectionId);
    }
}

void TransportCreateRequestId(TransportHandle transportHandle)
{
    TransportStream*  stream;
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (transportHandle && request->isWS)
    {
        UniqueId_Generate(request->requestId, sizeof(request->requestId));
        GuidDToNFormat(request->requestId);
        for (int i = 0; request->requestId[i]; i += 2)
        {
            request->binaryRequestId[i / 2] = (uint8_t)
                ((uint8_t)hexchar_to_int(request->requestId[i]) << 4) |
                ((uint8_t)hexchar_to_int(request->requestId[i + 1]));
        }
        LogInfo("SessionId: '%s'", request->requestId);
        metrics_transport_requestid(request->requestId);

        // There is at most one active request at a time -- associate telemetry
        // events with this request ID.
        telemetry_set_context_string("request_id", request->requestId);

        for (stream = request->ws.streamHead; stream != NULL; stream = stream->next)
        {
            stream->bufferSize = 0;
            if (stream->ioBuffer)
            {
                IoBufferReset(stream->ioBuffer);
            }
        }
    }
}

TransportHandle TransportRequestCreate(const char* host, void* context)
{
    TransportRequest *request;
    int err = -1;
    const char* src;
    char* dst;
    size_t l;
    int val;

    if (!host)
    {
        return NULL;
    }

    request = (TransportRequest*)malloc(sizeof(TransportRequest));
    if (NULL == request)
    {
        return NULL;
    }

    memset(request, 0, sizeof(TransportRequest));
    request->context = context;
    // disable by default, usually only websocket request will need it
    request->queue = list_create();

    if (strstr(host, g_keywordWSS) == host)
    {
        request->isWS = true;
        request->ws.config.port = 443;
        request->ws.config.use_ssl = 2;
    }
    else if(strstr(host, g_keywordWS) == host)
    {
        request->isWS = true;
        request->ws.config.port = 80;
        request->ws.config.use_ssl = 0;
    }
    else
    {
        request->isWS = false;
    }

    if (request->isWS)
    {
        request->ws.metricLock = Lock_Init();
        telemetry_setcallbacks(WSOnTelemetryData, request);
        TransportCreateRequestId(request);

        l = strlen(host);
        request->url = (char*)malloc(l + 2); // 2=2 x NULL
        if (request->url != NULL)
        {
            if (request->ws.config.use_ssl)
            {
                host += (sizeof(g_keywordWSS) - 1);
            }
            else
            {
                host += (sizeof(g_keywordWS) - 1);
            }

            request->ws.config.host = dst = request->url;

            // split up host + path
            src = strchr(host, '/');
            if (NULL != src)
            {
                err = 0;
                memcpy(dst, host, src - host);
                dst += (src - host);
                *dst = 0; dst++;
                request->ws.config.relative_path = dst;
                strcpy_s(dst, l + 2 - (dst - request->url), src);
                request->ws.WSHandle = wsio_create(&request->ws.config);

                // override the system default when there is no TCP activity.
                // this prevents long system timeouts in the range of minutes
                // to detect that the network went down.
                val = ANSWER_TIMEOUT_MS;
                wsio_setoption(request->ws.WSHandle, "idletimeout", &val);
            }
        }
    }
    else
    {
        request->http.tlsIoConfig.hostname = host;
        request->http.tlsIoConfig.port = 443;
        request->http.requestHandle = http_client_create(NULL, OnConnectedCallback, OnHttpErrorCallback, ChunkedMessageRecv, request);
        if (NULL != request->http.requestHandle)
        {
            err = 0;
        }
    }

    if (!err)
    {
        request->headersHandle = HTTPHeaders_Alloc();
        if (NULL != request->headersHandle)
        {
            return request;
        }
    }

    free(request);

    return NULL;
}

void TransportRequestDestroy(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    TransportStream*  stream;
    TransportStream*  streamNext = NULL;

    if (!request)
    {
        return;
    }

    if (request->dnsCache)
    {
        DnsCacheRemoveContextMatches(request->dnsCache, request);
    }

    if (request->isWS)
    {
        telemetry_setcallbacks(NULL, NULL);
        if (request->ws.WSHandle)
        {
            if (request->isOpen)
            {
                (void)wsio_close(request->ws.WSHandle, WSOnClose, request);
                while (request->isOpen)
                {
                    wsio_dowork(request->ws.WSHandle);
                }
            }

            wsio_destroy(request->ws.WSHandle);
        }

        for (stream = request->ws.streamHead; NULL != stream; stream = streamNext)
        {
            streamNext = stream->next;

            if (stream->ioBuffer)
            {
                IoBufferDelete(stream->ioBuffer);
            }

            if (stream->contentType)
            {
                STRING_delete(stream->contentType);
            }

            free(stream);
        }

        metrics_transport_closed();

        if (request->ws.metricData)
        {
            BUFFER_delete(request->ws.metricData);
            request->ws.metricData = NULL;
        }

        if (request->ws.metricLock)
        {
            Lock_Deinit(request->ws.metricLock);
        }
    }
    else
    {
        if (request->http.requestHandle)
        {
            if (request->isOpen)
            {
                http_client_close(request->http.requestHandle);
            }

            http_client_destroy(request->http.requestHandle);
        }
    }

    InvalidateRequestId(request);

    if (request->url)
    {
        free(request->url);
    }

    if (request->headersHandle)
    {
        HTTPHeaders_Free(request->headersHandle);
    }

    if (request->responseContentHandle)
    {
        BUFFER_delete(request->responseContentHandle);
        request->responseContentHandle = NULL;
    }

    if (request->queue)
    {
        list_destroy(request->queue);
        request->queue = NULL;
    }

    free(request);
}

int
TransportRequestExecute(TransportHandle transportHandle, const char* path, uint8_t* buffer, size_t length)
{
    TransportRequest *request = (TransportRequest*)transportHandle;
    TransportPacket  *packet = NULL;
    if (!request)
    {
        return -1;
    }

    if (0 != length)
    {
        packet = (TransportPacket*)malloc(sizeof(TransportPacket) + length);
        if (NULL == packet)
        {
            return -1;
        }

        packet->length = length;
        memcpy(packet->buffer, buffer, length);
    }

    if (TransportRequestPrepare(request))
    {
        if (NULL != packet) free(packet);
        return -1;
    }

    request->path = path;

    if (NULL != packet)
    {
        WsioQueue(request, packet);
    }

    return 0;
}

static void AddMUID(TransportRequest* request)
{
#define GUID_BYTE_SIZE  16
    const uint8_t* pUI8;
    BUFFER_HANDLE buffer;
    char cookieBuf[5 + (GUID_BYTE_SIZE * 2) + 1]; // 5='MUID='.  Guid value is a 'N' formatted

    pUI8 = (uint8_t*)GetCdpDeviceThumbprint();
    if (pUI8 && *pUI8)
    {
        buffer = Base64_Decoder((char*)pUI8);
        if (buffer)
        {
            if (BUFFER_length(buffer) >= GUID_BYTE_SIZE)
            {
                pUI8 = BUFFER_u_char(buffer);
                sprintf_s(
                    cookieBuf, sizeof(cookieBuf),
                    "MUID=%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",
                    *(uint32_t*)(pUI8 + 0), 
                    *(uint16_t*)(pUI8 + 4), *(uint16_t*)(pUI8 + 6),
                    pUI8[8],  pUI8[9],  pUI8[10], pUI8[11], 
                    pUI8[12], pUI8[13], pUI8[14], pUI8[15]);

                TransportRequestAddRequestHeader(request, "Cookie", cookieBuf);
            }
            BUFFER_delete(buffer);
        }
    }
}

static int TransportOpen(TransportRequest* request)
{
    if (!request->isOpen)
    {
        if (!request->isWS)
        {
            if (http_client_open(request->http.requestHandle, request->http.tlsIoConfig.hostname))
            {
                LogInfo("http_client_open failed");
                return -1;
            }

            // HTTP only opens once the request has been started.
            request->isOpen = true;
        }
        else
        {
            if (NULL == request->ws.WSHandle)
            {
                return -1;
            }
            else
            {
                TransportCreateConnectionId(request);
                TransportRequestAddRequestHeader(request, "X-ConnectionId", request->connectionId);
                AddMUID(request);
                metrics_transport_start(request->connectionId);
                const int result = wsio_open(request->ws.WSHandle,
                    WsioOnOpened,
                    request,
                    WsioOnBytesRecv,
                    request,
                    WsioOnError,
                    request);
                if (result)
                {
                    LogError("wsio_open failed with result %d", result);
                    return -1;
                }
            }
        }

        request->state = TRANSPORT_STATE_CONNECTED;
    }
    
    return 0;
}

static void WSOnCloseForReset(void* context)
{
    TransportRequest* request = (TransportRequest*)context;
    request->isOpen = false;

    if (request->state == TRANSPORT_STATE_RESETTING)
    {
        // Re-open the connection.
        request->state = TRANSPORT_STATE_NETWORK_CHECK;
    }
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
    if (NULL == request || 
        NULL == request->http.requestHandle)
    {
        return -1;
    }

    if (request->responseContentHandle)
    {
        BUFFER_delete(request->responseContentHandle);
        request->responseContentHandle = NULL;
    }
    if (request->tokenStore != NULL)
    {
        int tokenChanged = add_auth_headers(request->tokenStore, request->headersHandle, request->context);

        if ((telemetry_gettime() - request->connectionTime) >= WS_CONNECTION_TIME_MS)
        {
            tokenChanged = 1;
            LogInfo("forcing connection closed");
        }

        if (tokenChanged > 0 && request->isWS &&
            (request->state == TRANSPORT_STATE_CONNECTED))
        {
            // The token is snapped at the time that we are connected.  Reset
            // our connection if the token changes to avoid using a stale token
            // with our requests.
            //
            // We only do this for websocket connections because HTTP
            // connections are short-lived.

            // Set the transport state before calling wsio_close because
            // wsio_close may call the complete callback before returning.
            request->state = TRANSPORT_STATE_RESETTING;
            LogInfo("token changed, resetting connection");
            metrics_transport_reset();

            const int closeResult =
                wsio_close(request->ws.WSHandle, WSOnCloseForReset, request);
            if (closeResult)
            {
                // We don't know how to recover from this state.  Crash the
                // process so that we start over.
                abort();
            }
        }
    }

    request->isCompleted = false;
    if (request->state == TRANSPORT_STATE_CLOSED)
    {
        request->state = TRANSPORT_STATE_NETWORK_CHECK;
    }

    return err;
}

int TransportRequestAddRequestHeader(TransportHandle transportHandle, const char *name, const char *value)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request || NULL == name  || NULL == value)
    {
        return -1;
    }

    return HTTPHeaders_ReplaceHeaderNameValuePair(request->headersHandle, name, value);
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

    if (request->isWS)
    {
        if (NULL != path)
        {
            request->path = path + 1; // 1=remove "/"
            request->ws.pathLen = strlen(request->path);
        }

        ret = wsio_setoption(request->ws.WSHandle, "connectionheaders", request->headersHandle);
        if (ret)
        {
            LogInfo("ERROR settings connectionheaders\n");
        }
    }

    ret = TransportRequestPrepare(request);
    if (ret)
    {
        return ret;
    }

    if (request->isWS)
    {
        // Todo: send Speech.config message?
        /*STRING_HANDLE turnContext =
            skill_serialize_context(request->context, !pszPath);*/
    }
    else
    {
        // Todo: do we support http?
        ret = http_client_start_chunk_request(request->http.requestHandle, HTTP_CLIENT_REQUEST_POST, path, request->headersHandle);
    }

    return ret;
}

int TransportStreamWrite(TransportHandle transportHandle, const uint8_t* buffer, size_t bufferSize)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request)
    {
        return -1;
    }

    if (request->isWS)
    {
        uint8_t msgtype = METRIC_MESSAGE_TYPE_INVALID;
        if (bufferSize == 0)
        {
            msgtype = METRIC_MESSAGE_TYPE_AUDIO_LAST;
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
            msgtype = METRIC_MESSAGE_TYPE_AUDIO_START;
        }

        size_t headerLen;
        size_t payloadSize = sizeof(g_requestFormat) + 
            (size_t)request->ws.pathLen + 
            sizeof(g_KeywordStreamId) + 
            30 + 
            sizeof(g_keywordRequestId) + 
            sizeof(request->requestId) + 
            sizeof(g_timeStampHeaderName) +
            TIME_STRING_MAX_SIZE +
            bufferSize + 2; // 2 = header length
        TransportPacket *msg = (TransportPacket*)malloc(sizeof(TransportPacket) + WS_MESSAGE_HEADER_SIZE + payloadSize);
        msg->msgtype = msgtype;
        msg->wstype = WSIO_MSG_TYPE_BINARY;

        char timeString[TIME_STRING_MAX_SIZE];
        int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
        if (timeStringLen < 0)
        {
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
            request->requestId);

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
    else
    {
        return http_client_send_chunk_request(request->http.requestHandle, buffer, bufferSize);
    }
}

int TransportStreamFlush(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request)
    {
        return -1;
    }

    if (request->isWS)
    {
        if (!request->isOpen)
        {
            return -1;
        }

        return TransportStreamWrite(request, NULL, 0);
    }
    else
    {
        return http_client_end_chunk_request(request->http.requestHandle);
    }
}

int TransportDoWork(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    TransportPacket*  packet;

    if (NULL == transportHandle)
    {
        return -1;
    }

    switch (request->state)
    {
    case TRANSPORT_STATE_CLOSED:
        while (NULL != (packet = (TransportPacket*)queue_dequeue(request->queue)))
        {
            free(packet);
        }
        break;

    case TRANSPORT_STATE_RESETTING:
        // Do nothing -- we're waiting for WSOnCloseForReset to be invoked.
        break;

    case TRANSPORT_STATE_NETWORK_CHECKING:
        DnsCacheDoWork(request->dnsCache, request);
        return 1;

    case TRANSPORT_STATE_SENT:
        // do nothing
        break;

    case TRANSPORT_STATE_NETWORK_CHECK:
        if (NULL == request->dnsCache)
        {
            // skip dns checks
            request->state = TRANSPORT_STATE_OPENING;
        }
        else
        {
            request->state = TRANSPORT_STATE_NETWORK_CHECKING;
            const char* host = request->isWS ? request->ws.config.host : request->http.tlsIoConfig.hostname;
            LogInfo("Start network check %s", host);
            metrics_transport_state_start(METRIC_TRANSPORT_STATE_DNS);
            if (DnsCacheGetAddr(request->dnsCache, host, DnsComplete, request))
            {
                LogError("DnsCacheGetAddr failed");
                request->state = TRANSPORT_STATE_OPENING;
            }
        }
        return 1;

    case TRANSPORT_STATE_OPENING:
        if (TransportOpen(request))
        {
            return 0;
        }
        return 1;

    case TRANSPORT_STATE_CONNECTED:
        if (!request->isWS)
        {
            // HTTP
            packet = (TransportPacket*)queue_peek(request->queue);
            request->state = TRANSPORT_STATE_SENT;

            if (http_client_execute_request(
                request->http.requestHandle,
                packet ? HTTP_CLIENT_REQUEST_POST : HTTP_CLIENT_REQUEST_GET,
                request->path,
                request->headersHandle,
                packet ? packet->buffer : NULL,
                packet ? packet->length : 0))
            {
                LogError("http_client_execute_request");
                request->state = TRANSPORT_STATE_CLOSED;
            }
        }
        else
        {
            while (request->isOpen &&  NULL != (packet = (TransportPacket*)queue_dequeue(request->queue)))
            {
                if (packet->msgtype != METRIC_MESSAGE_TYPE_INVALID)
                {
                    metrics_transport_state_start(packet->msgtype);
                }

                // re-stamp X-timestamp header value
                char timeString[TIME_STRING_MAX_SIZE] = "";
                int timeStringLength = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
                int offset = sizeof(g_timeStampHeaderName);
                if (packet->wstype == WSIO_MSG_TYPE_BINARY)
                {
                    // Include the first 2 bytes for header length
                    offset += 2;
                }
                memcpy((char*)packet->buffer + offset, timeString, timeStringLength);

                const int result = wsio_send(
                    request->ws.WSHandle,
                    packet->buffer,
                    packet->length,
                    packet->wstype,
                    WsioOnSendComplete,
                    packet);
                if (result)
                {
                    LogError("Failed in wsio_send with result %d", result);
                }
            }
        }
        break;
    }

    if (request->isWS)
    {
        wsio_dowork(request->ws.WSHandle);
    }
    else if (request->state == TRANSPORT_STATE_SENT)
    {
        http_client_dowork(request->http.requestHandle);
    }
    return request->isCompleted ? 0 : 1;
}

int TransportSetCallbacks(TransportHandle transportHandle, TransportErrorCallback errorCallback, TransportResponseCallback recvCallback)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    if (NULL == request)
    {
        return -1;
    }

    request->onTransportError = errorCallback;
    request->onRecvResponse = recvCallback;
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

static TransportStream* TransportGetStream(TransportHandle transportHandle, int streamId)
{
    TransportStream* stream;
    TransportRequest* request = (TransportRequest*)transportHandle;

    if (NULL == request || !request->isWS)
    {
        return NULL;
    }

    for (stream = request->ws.streamHead; stream != NULL; stream = stream->next)
    {
        if (stream->id == streamId)
        {
            return stream;
        }
    }

    return NULL;
}

static TransportStream* TransportCreateStream(TransportHandle transportHandle, int streamId, const char* contentType, void* context)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    TransportStream* stream;

    if (NULL == request || !request->isWS || !contentType)
    {
        return NULL;
    }

    for (stream = request->ws.streamHead; stream != NULL; stream = stream->next)
    {
        if (stream->bufferSize == 0)
        {
            break;
        }
    }

    // stream wasn't found, prepare a new one.
    if (NULL == stream)
    {
        // there wasn't a free stream, alloc a new one
        stream = (TransportStream*)malloc(sizeof(TransportStream));
        if (NULL == stream)
        {
            LogError("alloc TransportStream failed");
            return NULL;
        }

        memset(stream, 0, sizeof(TransportStream));

        // attach the stream to the list
        stream->next = request->ws.streamHead;
        request->ws.streamHead = stream;
    }

    stream->id = streamId;
    stream->context = context;

    // copy the content type
    if (NULL == stream->contentType)
    {
        stream->contentType = STRING_construct(contentType);
    }
    else if (NULL == stream->contentType)
    {
        STRING_copy(stream->contentType, contentType);
    }

    return stream;
}

void TransportSetDnsCache(TransportHandle transportHandle, DnsCacheHandle dnsCache)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    request->dnsCache = dnsCache;
}

const char* TransportGetRequestId(TransportHandle transportHandle)
{
    TransportRequest* request = (TransportRequest*)transportHandle;
    return request->requestId;
}

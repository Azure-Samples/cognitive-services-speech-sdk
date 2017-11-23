// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "private-iot-cortana-sdk.h"
#include "azure_c_shared_utility/uhttp.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/uniqueid.h"
#include <assert.h>

// uncomment the line below to see all non-binary protocol messages in the log
// #define LOG_TEXT_MESSAGES

#define ws   u1._ws
#define http u1._http

const char kStreamId[]       = "X-StreamId";
const char kRequestId[]      = "X-RequestId";
const char kWSS[]            = "wss://";
const char kWS[]             = "ws://";
const char kRequestFormat[]  = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n";
const char kContextHeader[]  = "%s:%s\r\nPath:%s\r\nContent-Type:application/json\r\n%s:%s\r\n\r\n%s";
const char kContextMessage[] = "speech.agent.context";
const char kAgentMessage[]   = "agent";
const char kTelemetryHeader[] = "%s:%s\r\nPath: telemetry\r\nContent-Type: application/json; charset=utf-8\r\n%s:%s\r\n\r\n";
const char kTimeStampHeaderName[] = "X-Timestamp";

static const char kRPSDelegationHeaderName[] = "X-Search-DelegationRPSToken";
static const char kCortanaScope[] = "Bing.Cortana";

#define WS_MESSAGE_HEADER_SIZE  2
#define WS_CONNECTION_TIME_MS   (570 * 1000) // 9.5 minutes

typedef struct _TRANSPORT_STREAM
{
    int                       id;
    IOBUFFER*                 pIoBuffer;
    size_t                    BufferSize;
    STRING_HANDLE             hContentType;
    void*                     pContext;
    struct _TRANSPORT_STREAM* pNext;
} TRANSPORT_STREAM;

typedef struct _TRANSPORT_PACKET
{
    struct _TRANSPORT_PACKET* next;
    uint8_t                   msgtype;
    WSIO_MSG_TYPE             wstype;
    size_t                    length;
    uint8_t                   buffer[1]; // must be last
} TRANSPORT_PACKET;

static TRANSPORT_STREAM* transport_createstream(
    TRANSPORT_HANDLE hTransport,
    int              nStreamId,
    const char*      pszContentType,
    void*            pContext);
static TRANSPORT_STREAM* transport_getstream(
    TRANSPORT_HANDLE hTransport,
    int              nStreamId);

// converts a dashed GUID to a no-dash guid string.
void GuidDToNFormat(char *pszGuidString)
{
    char *dst = pszGuidString;
    do
    {
        if (*pszGuidString != '-')
        {
            *dst = *pszGuidString;
            dst++;
        }

        pszGuidString++;
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

int HTTPHeaders_ParseHeaders(
    HTTP_HEADERS_HANDLE  hHeaders,
    const unsigned char* buffer, 
    size_t               size)
{
    int ns, ne, vs, offset;
    bool bDone;

    if (NULL == hHeaders)
    {
        return -1;
    }

    bDone = false;
    STRING_HANDLE hName = NULL, hValue = NULL;
    for (ns = ne = offset = vs = 0; offset < (int)size && !bDone; offset++)
    {
        switch (buffer[offset])
        {
        case ' ':
            break;
        case ':':
            // we only care about the first ':' separator, everything else is part of the value.
            if (!hName)
            {
                hName = STRING_from_byte_array(buffer + ns, (size_t)(offset - ns));
                vs = offset + 1;
            }
            break;
        case '\r':
            if (NULL != hName)
            {
                hValue = STRING_from_byte_array(buffer + vs, (size_t)(offset - vs));
                HTTPHeaders_ReplaceHeaderNameValuePair(hHeaders, STRING_c_str(hName), STRING_c_str(hValue));
                STRING_delete(hName);
                STRING_delete(hValue);
                hName = hValue = NULL;
            }
            else
            {
                bDone = true;
            }
            break;
        case '\n':
            ne = vs = 0;
            ns = offset + 1;
            break;
        }
    }

    // skip the trailing '\n' 
    if (bDone)
    {
        offset++;
    }

    return offset;
}

static void invalidate_request_id(
    TRANSPORT_REQUEST* pRequest
)
{
    memset(pRequest->requestId, 0, sizeof(pRequest->requestId) / sizeof(pRequest->requestId[0]));
    memset(pRequest->bRequestId, 0, sizeof(pRequest->bRequestId) / sizeof(pRequest->bRequestId[0]));
}

// For websocket requests, the caller must ensure that the wsio instance is not
// open when this function is called.  wsio_open will fail for an already-open
// wsio instance and we may get stuck retrying the open.
static void OnTransportError(TRANSPORT_REQUEST* pRequest, enum transport_error err)
{
    pRequest->fCompleted = true;
    pRequest->fOpen = false;

    // don't propogate errors during the reset state, these errors are expected.
    if (pRequest->state != TRANSPORT_STATE_RESETTING)
    {
        pRequest->state = TRANSPORT_STATE_CLOSED;
        invalidate_request_id(pRequest);
        if (pRequest->OnTransportError != NULL) {
            pRequest->OnTransportError(pRequest, err, pRequest->pContext);
        }
    }
}

static void OnErrorCallback(
    HTTP_CLIENT_HANDLE handle, 
    void* callbackCtx)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)callbackCtx;
    OnTransportError(pRequest, transport_error_none);
    http_client_close(handle);
}

static void DnsComplete(
    DNS_CACHE_HANDLE handle, 
    int error, 
    DNS_RESULT_HANDLE hResult, 
    void *pContext)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)pContext;
    assert(pRequest->state == TRANSPORT_STATE_NETWORK_CHECKING);

    (void)handle;
    (void)hResult;

    metrics_transport_state_end(METRIC_TRANSPORT_STATE_DNS);

    if (NULL != pRequest)
    {
        if (error != 0)
        {
            LogError("Network Check failed %d", error);

            // The network instance has not been connected yet so it is not
            // open, and it's safe to call OnTransportError.  We do the DNS
            // check before opening the connection.  We assert about this with
            // the TRANSPORT_STATE_NETWORK_CHECKING check above.
            OnTransportError(pRequest, transport_error_dns_failure);
        }
        else
        {
            LogInfo("Network Check completed");
            pRequest->state = TRANSPORT_STATE_OPENING;
        }
    }
}

static void OnConnectedCallback(
    HTTP_CLIENT_HANDLE handle, 
    void* callbackCtx)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)callbackCtx;
    (void)handle;
    pRequest->state = TRANSPORT_STATE_CONNECTED;
}

static void ChunkedMessageRecv(
    HTTP_CLIENT_HANDLE handle,
    void* callback_ctx,
    const unsigned char* content,
    size_t contentLen,
    unsigned int statusCode,
    HTTP_HEADERS_HANDLE hResponseHeaders,
    int bReplyComplete)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)callback_ctx;
    (void)handle;
    const char* pszMime;

    if (contentLen)
    {
        if (!pRequest->hResponseContent)
        {
            pRequest->hResponseContent = BUFFER_create(content, contentLen);
        }
        else
        {
            size_t size = BUFFER_length(pRequest->hResponseContent);
            BUFFER_enlarge(pRequest->hResponseContent, contentLen);
            memcpy(BUFFER_u_char(pRequest->hResponseContent) + size, content, contentLen);
        }
    }

    if (bReplyComplete)
    {
        pRequest->fCompleted = true;

        if (pRequest->OnRecvCallback)
        {
            pRequest->OnRecvCallback(
                pRequest,
                hResponseHeaders,
                BUFFER_u_char(pRequest->hResponseContent),
                BUFFER_length(pRequest->hResponseContent),
                statusCode,
                pRequest->pContext);
        }
        else if (statusCode == 200)
        {
            pszMime = HTTPHeaders_FindHeaderValue(hResponseHeaders, kContent_Type);
            if (NULL != pszMime)
            {
                if (CORTANA_ERROR_PENDING == Content_Dispatch(
                    pRequest->pContext,
                    NULL,  // path
                    pszMime,
					0,
                    pRequest->hResponseContent,
                    USE_BUFFER_SIZE))
                {
                    // Content_Dispatch has taken ownership of the buffer.
                    pRequest->hResponseContent = NULL;
                }
            }
        }

        pRequest->state = TRANSPORT_STATE_CLOSED;

        if (pRequest->hResponseContent)
        {
            BUFFER_delete(pRequest->hResponseContent);
            pRequest->hResponseContent = NULL;
        }
    }
}

static void WSIO_OnOpened(void* context, IO_OPEN_RESULT open_result)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)context;
    enum transport_error err;
    int status;

    pRequest->fOpen = (open_result == IO_OPEN_OK);
    if (open_result == IO_OPEN_OK)
    {
        pRequest->state = TRANSPORT_STATE_CONNECTED;
        pRequest->nConnectionTime = cortana_gettickcount();
        LogInfo("Transport websocket connected");
        metrics_transport_connected(pRequest->connectionId);
    }
    else
    {
        // It is safe to transition to TRANSPORT_STATE_CLOSED because the
        // connection is not open.  We must be careful with this state for the
        // reasons described in OnTransportError.
        pRequest->state = TRANSPORT_STATE_CLOSED;
        metrics_transport_dropped();
        pRequest->fCompleted = true;

        status = wsio_gethttpstatus(pRequest->ws.hWS); 
        LogError("WSIO open failed %d %d", status, open_result);
        metrics_transport_error(pRequest->connectionId, status);

        switch (status)
        {
        case 0: // no http response, assume general connection failure
            err = transport_error_connection_failure;
            break;

        case 401:
        case 403:
            err = transport_error_authentication;
            break;

        default:
            // TODO: Add socket level detection
            err = transport_error_none;
            break;
        }

        if (pRequest->OnTransportError)
        {
            pRequest->OnTransportError(
                (TRANSPORT_HANDLE)pRequest, 
                err,
                pRequest->pContext);
        }
    }
}

static int OnStreamChunk(
    TRANSPORT_STREAM*   pStream,
    const uint8_t*      pBuffer,
    size_t              bufferSize)
{
    if (NULL == pStream)
    {
        return -1;
    }

    if (NULL == pStream->pIoBuffer)
    {
        pStream->pIoBuffer = IOBUFFER_NEW();
        if (NULL == pStream->pIoBuffer)
        {
            LogError("IOBUFFER_NEW failed");
            return -1;
        }
    }

    if (NULL == pStream->hContentType)
    {
        PROTOCOL_VIOLATION("Content-Type not defined");
        return -1;
    }

    if (0 == pStream->BufferSize)
    {
        LogInfo("First chunk for %s", STRING_c_str(pStream->hContentType));
        metrics_tts_first_chunk();
        Content_Dispatch(
            pStream->pContext,
            NULL,  // path
            STRING_c_str(pStream->hContentType),
            pStream->pIoBuffer,
            NULL,
            pStream->BufferSize);
    }
    else if (0 == bufferSize)
    {
        LogInfo("Last chunk for %s", STRING_c_str(pStream->hContentType));
        metrics_tts_last_chunk();
    }

    if (bufferSize != 0)
    {
        IOBUFFER_Write(pStream->pIoBuffer, pBuffer, 0, (int)bufferSize);
        pStream->BufferSize += bufferSize;
    }
    else if (pStream->BufferSize > 0 &&
        NULL != pStream->hContentType)
    {
        IOBUFFER_Write(pStream->pIoBuffer, 0, 0, 0); // mark buffer completed
        IOBUFFER_DELETE(pStream->pIoBuffer);
        pStream->pIoBuffer = 0;
        pStream->BufferSize = 0;
    }

    return 0;
}

static void WSIO_OnBytesRecv(void* context, const unsigned char* buffer, size_t size, WSIO_MSG_TYPE message_type)
{
    TRANSPORT_REQUEST*  pRequest = (TRANSPORT_REQUEST*)context;
    HTTP_HEADERS_HANDLE hResponseHeader;
    int                 offset;
    uint16_t            headerSize;
    const char*         pszValue;
    TRANSPORT_STREAM*   pStream;
    int                 nStreamId;

    if (context && pRequest->OnRecvCallback)
    {
        hResponseHeader = HTTPHeaders_Alloc();
        if (!hResponseHeader)
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
            offset = HTTPHeaders_ParseHeaders(hResponseHeader, buffer, size);
            break;
        case WSIO_MSG_TYPE_BINARY:
            if (size < 2)
            {
                PROTOCOL_VIOLATION("unable to read binary message length");
                goto Exit;
            }

            headerSize = (uint16_t)(buffer[0] << 8);
            headerSize |= (uint16_t)(buffer[1] << 0);
            offset = HTTPHeaders_ParseHeaders(hResponseHeader, buffer + 2, headerSize);
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

        pszValue = HTTPHeaders_FindHeaderValue(hResponseHeader, kRequestId);
        if (!pszValue || strcmp(pszValue, pRequest->requestId))
        {
            PROTOCOL_VIOLATION("Unexpected request id '%s', expected '%s' Path: %s", pszValue, pRequest->requestId,
                HTTPHeaders_FindHeaderValue(hResponseHeader, kPath));
            metrics_unexpected_requestid(pszValue);
            goto Exit;
        }

		pszValue = HTTPHeaders_FindHeaderValue(hResponseHeader, kPath);
		if (pszValue)
		{
			metrics_received_message(pszValue);
		}

        // optional StreamId header
        pszValue = HTTPHeaders_FindHeaderValue(hResponseHeader, kStreamId);
        if (NULL != pszValue)
        {
            nStreamId = atoi(pszValue);
            pStream = transport_getstream((TRANSPORT_HANDLE)pRequest, nStreamId);
            if (NULL == pStream)
            {
                pStream = transport_createstream(
                    (TRANSPORT_HANDLE)pRequest, 
                    nStreamId, 
                    HTTPHeaders_FindHeaderValue(hResponseHeader, kContent_Type),
                    pRequest->pContext);
                if (NULL == pStream)
                {
                    LogError("alloc TRANSPORT_STREAM failed");
                    goto Exit;
                }
            }

            (void)OnStreamChunk(
                pStream,
                buffer + offset,
                size - offset);
        }
        else
        {
            pRequest->OnRecvCallback((TRANSPORT_HANDLE)pRequest,
                hResponseHeader,
                buffer + offset,
                size - offset,
                0,
                pRequest->pContext);
        }

    Exit:
        HTTPHeaders_Free(hResponseHeader);

	}
}

// WSIO sets the wsio instance to an error state before calling this.  It is
// safe to call wsio_open.
//
// SRS_WSIO_01_121: When on_underlying_ws_error is called while the IO is OPEN
// the wsio instance shall be set to ERROR and an error shall be indicated via
// the on_io_error callback passed to wsio_open.
static void WSIO_OnError(void* context)
{
	TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)context;
	if (context)
	{
        metrics_transport_dropped();
        OnTransportError(pRequest, transport_error_remoteclosed);
    }
}

static void WSIO_ON_SEND_COMPLETE(void* context, IO_SEND_RESULT send_result)
{
	(void)send_result;
	TRANSPORT_PACKET *pMsg = (TRANSPORT_PACKET*)context;
	// the first byte is the message type
	if (pMsg->msgtype != METRIC_MESSAGE_TYPE_INVALID)
	{
		metrics_transport_state_end(pMsg->msgtype);
	}
	free(pMsg);
}

static void WSIOQueue(
	TRANSPORT_REQUEST* request,
	TRANSPORT_PACKET*  packet)
{
    if (request->fWS  &&  request->requestId[0] == '\0')
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
    (void)queue_enqueue(request->hQueue, packet);
}

static void WSOnClose(void* context)
{
	TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)context;
    pRequest->fOpen = false;
	pRequest->state = TRANSPORT_STATE_CLOSED;
}

static void PrepareTelemetryPayload(
	void *pContext,
	const uint8_t* pEventBuffer,
	size_t eventBufferSize,
	TRANSPORT_PACKET **pPacket,
	const char *requestId)
{
	TRANSPORT_REQUEST *pRequest = (TRANSPORT_REQUEST*)pContext;

	// serialize headers.
	size_t headerLen;

	size_t payloadSize = sizeof(kTelemetryHeader) +
		sizeof(kRequestId) +
		sizeof(pRequest->requestId) +
		sizeof(kTimeStampHeaderName) +
		TIME_STRING_MAX_SIZE +
		eventBufferSize;

	TRANSPORT_PACKET *pMsg = (TRANSPORT_PACKET*)malloc(sizeof(TRANSPORT_PACKET) + payloadSize);
	pMsg->msgtype = METRIC_MESSAGE_TYPE_TELEMETRY;
	pMsg->wstype = WSIO_MSG_TYPE_TEXT;

	char timeString[TIME_STRING_MAX_SIZE];
	int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
	if (timeStringLen < 0)
	{
		return;
	}

	// serialize the entire telemetry message.
	headerLen = sprintf_s((char*)pMsg->buffer,
		payloadSize,
		kTelemetryHeader,
		kTimeStampHeaderName,
		timeString,
		kRequestId,
		IS_STRING_NULL_OR_EMPTY(requestId) ? pRequest->requestId : requestId);

	pMsg->length = headerLen;
	// body
	memcpy(pMsg->buffer + pMsg->length, pEventBuffer, eventBufferSize);
	pMsg->length += eventBufferSize;
	*pPacket = pMsg;
}

static void WSOnTelemetryData(
	const uint8_t* pBuffer,
	size_t byteToWrite,
	void *pContext, 
	const char *requestId)
{
	TRANSPORT_REQUEST *pRequest = (TRANSPORT_REQUEST*)pContext;
	TRANSPORT_PACKET *pMsg = NULL;
	PrepareTelemetryPayload(pContext, pBuffer, byteToWrite, &pMsg, requestId);

	WSIOQueue(pRequest, pMsg);
}

static void transport_create_connection_id(TRANSPORT_HANDLE hTransport)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    if (hTransport && pRequest->fWS)
    {
		UniqueId_Generate(pRequest->connectionId, sizeof(pRequest->connectionId));
        GuidDToNFormat(pRequest->connectionId);
    }
}

void transport_create_request_id(TRANSPORT_HANDLE hTransport)
{
    TRANSPORT_STREAM*  pStream;
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    if (hTransport && pRequest->fWS)
    {
        UniqueId_Generate(pRequest->requestId, sizeof(pRequest->requestId));
        GuidDToNFormat(pRequest->requestId);
        for (int i = 0; pRequest->requestId[i]; i += 2)
        {
            pRequest->bRequestId[i / 2] = (uint8_t)
                ((uint8_t)hexchar_to_int(pRequest->requestId[i]) << 4) |
                ((uint8_t)hexchar_to_int(pRequest->requestId[i + 1]));
        }
        LogInfo("SessionId: '%s'", pRequest->requestId);
        metrics_transport_requestid(pRequest->requestId);

        // There is at most one active request at a time -- associate telemetry
        // events with this request ID.
        telemetry_set_context_string("request_id", pRequest->requestId);

        for (pStream = pRequest->ws.pStreamHead; pStream != NULL; pStream = pStream->pNext)
        {
            pStream->BufferSize = 0;
            if (pStream->pIoBuffer)
            {
                IOBUFFER_Reset(pStream->pIoBuffer);
            }
        }
    }
}

TRANSPORT_HANDLE
transport_request_create(
    const char*        pszHost,
    void*              pContext
)
{
    TRANSPORT_REQUEST *pRequest;
    int err = -1;
    const char* src;
    char* dst;
    size_t l;
    int    iVal;

    if (!pszHost)
    {
        return NULL;
    }

    pRequest = (TRANSPORT_REQUEST*)malloc(sizeof(TRANSPORT_REQUEST));
    if (NULL == pRequest)
    {
        return NULL;
    }

    memset(pRequest, 0, sizeof(TRANSPORT_REQUEST));
    pRequest->pContext = pContext;
    pRequest->token_store = NULL;
    // disable by default, usually only websocket request will need it
    pRequest->hQueue = list_create();

    if (strstr(pszHost, kWSS) == pszHost)
    {
        pRequest->fWS = true;
        pRequest->ws.config.port = 443;
        pRequest->ws.config.use_ssl = 2;
    }
    else if(strstr(pszHost, kWS) == pszHost)
    {
        pRequest->fWS = true;
        pRequest->ws.config.port = 80;
        pRequest->ws.config.use_ssl = 0;
    }
    else
    {
        pRequest->fWS = false;
    }

    if (pRequest->fWS)
    {
        pRequest->ws.metricLock = Lock_Init();
        telemetry_setcallbacks(WSOnTelemetryData, pRequest);
        transport_create_request_id(pRequest);

        l = strlen(pszHost);
        pRequest->pszUrl = (char*)malloc(l + 2); // 2=2 x NULL
        if (pRequest->pszUrl != NULL)
        {
            if (pRequest->ws.config.use_ssl)
            {
                pszHost += (sizeof(kWSS) - 1);
            }
            else
            {
                pszHost += (sizeof(kWS) - 1);
            }

            pRequest->ws.config.host = dst = pRequest->pszUrl;

            // split up host + path
            src = strchr(pszHost, '/');
            if (NULL != src)
            {
                err = 0;
                memcpy(dst, pszHost, src - pszHost);
                dst += (src - pszHost);
                *dst = 0; dst++;
                pRequest->ws.config.relative_path = dst;
                strcpy_s(dst,
                    l + 2 - (dst - pRequest->pszUrl),
                    src);
                pRequest->ws.hWS = wsio_create(&pRequest->ws.config);

                // override the system default when there is no TCP activity.
                // this prevents long system timeouts in the range of minutes
                // to detect that the network went down.
                iVal = ANSWER_TIMEOUT_MS;
                wsio_setoption(pRequest->ws.hWS, "idletimeout", &iVal);
            }
        }
    }
    else
    {
        pRequest->http.tls_io_config.hostname = pszHost;
        pRequest->http.tls_io_config.port = 443;
        pRequest->http.hRequest = http_client_create(
            NULL,
            OnConnectedCallback,
            OnErrorCallback,
            ChunkedMessageRecv,
            pRequest);
        if (NULL != pRequest->http.hRequest)
        {
            err = 0;
        }
    }

    if (!err)
    {
        pRequest->hHeaders = HTTPHeaders_Alloc();
        if (NULL != pRequest->hHeaders)
        {
            return pRequest;
        }
    }

    free(pRequest);

    return NULL;
}

void
transport_request_destroy(
    TRANSPORT_HANDLE hTransport
    )
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    TRANSPORT_STREAM*  pStream;
    TRANSPORT_STREAM*  pStreamNext = NULL;

    if (!pRequest)
    {
        return;
    }

    if (pRequest->hDnsCache)
    {
        dns_cache_remove_context_matches(pRequest->hDnsCache, pRequest);
    }

    if (pRequest->fWS)
    {
        telemetry_setcallbacks(NULL, NULL);
        if (pRequest->ws.hWS)
        {
            if (pRequest->fOpen)
            {
                (void)wsio_close(pRequest->ws.hWS, WSOnClose, pRequest);
                while (pRequest->fOpen)
                {
                    wsio_dowork(pRequest->ws.hWS);
                }
            }

            wsio_destroy(pRequest->ws.hWS);
        }

        for (pStream = pRequest->ws.pStreamHead; NULL != pStream; pStream = pStreamNext)
        {
            pStreamNext = pStream->pNext;

            if (pStream->pIoBuffer)
            {
                IOBUFFER_DELETE(pStream->pIoBuffer);
            }

            if (pStream->hContentType)
            {
                STRING_delete(pStream->hContentType);
            }

            free(pStream);
        }

        metrics_transport_closed();

        if (pRequest->ws.hMetricData)
        {
            BUFFER_delete(pRequest->ws.hMetricData);
            pRequest->ws.hMetricData = NULL;
        }

        if (pRequest->ws.metricLock)
        {
            Lock_Deinit(pRequest->ws.metricLock);
        }
    }
    else
    {
        if (pRequest->http.hRequest)
        {
            if (pRequest->fOpen)
            {
                http_client_close(pRequest->http.hRequest);
            }

            http_client_destroy(pRequest->http.hRequest);
        }
    }

    invalidate_request_id(pRequest);

    if (pRequest->pszUrl)
    {
        free(pRequest->pszUrl);
    }

    if (pRequest->hHeaders)
    {
        HTTPHeaders_Free(pRequest->hHeaders);
    }

    if (pRequest->hResponseContent)
    {
        BUFFER_delete(pRequest->hResponseContent);
        pRequest->hResponseContent = NULL;
    }

    if (pRequest->hQueue)
    {
        list_destroy(pRequest->hQueue);
        pRequest->hQueue = NULL;
    }

    free(pRequest);
}

SPEECH_RESULT
transport_request_execute(
    TRANSPORT_HANDLE   hTransport,
    const char*        pszPath,
    uint8_t*           pBuffer,
    size_t             length
)
{
    TRANSPORT_REQUEST *pRequest = (TRANSPORT_REQUEST*)hTransport;
    TRANSPORT_PACKET  *pPacket = NULL;
    if (!pRequest)
    {
        return -1;
    }

    if (0 != length)
    {
        pPacket = (TRANSPORT_PACKET*)malloc(sizeof(TRANSPORT_PACKET) + length);
        if (NULL == pPacket)
        {
            return -1;
        }

        pPacket->length = length;
        memcpy(pPacket->buffer, pBuffer, length);
    }

    if (transport_request_prepare(pRequest))
    {
        if (NULL != pPacket) free(pPacket);
        return -1;
    }

    pRequest->pszPath = pszPath;

    if (NULL != pPacket)
    {
        WSIOQueue(pRequest, pPacket);
    }

    return 0;
}

static void AddMUID(TRANSPORT_REQUEST* pRequest)
{
#define GUID_BYTE_SIZE  16
    const uint8_t* pUI8;
    BUFFER_HANDLE hBuffer;
    char  CookieBuf[5 + (GUID_BYTE_SIZE * 2) + 1]; // 5='MUID='.  Guid value is a 'N' formatted

    pUI8 = (uint8_t*)get_cdp_device_thumbprint();
    if (pUI8 && *pUI8)
    {
        hBuffer = Base64_Decoder((char*)pUI8);
        if (hBuffer)
        {
            if (BUFFER_length(hBuffer) >= GUID_BYTE_SIZE)
            {
                pUI8 = BUFFER_u_char(hBuffer);
                sprintf_s(
                    CookieBuf, sizeof(CookieBuf),
                    "MUID=%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",
                    *(uint32_t*)(pUI8 + 0), 
                    *(uint16_t*)(pUI8 + 4), *(uint16_t*)(pUI8 + 6),
                    pUI8[8],  pUI8[9],  pUI8[10], pUI8[11], 
                    pUI8[12], pUI8[13], pUI8[14], pUI8[15]);

                transport_request_addrequestheader(pRequest, "Cookie", CookieBuf);
            }
            BUFFER_delete(hBuffer);
        }
    }
}

static int transport_open(TRANSPORT_REQUEST* pRequest)
{
    if (!pRequest->fOpen)
    {
        if (!pRequest->fWS)
        {
            if (http_client_open(pRequest->http.hRequest, pRequest->http.tls_io_config.hostname))
            {
                LogInfo("http_client_open failed");
                return -1;
            }

            // HTTP only opens once the request has been started.
            pRequest->fOpen = true;
        }
        else
        {
            if (NULL == pRequest->ws.hWS)
            {
                return -1;
            }
            else
            {
                transport_create_connection_id(pRequest);
                //Hack: hardcode subscription key. TODO: use access token. 
                transport_request_addrequestheader(pRequest, "Ocp-Apim-Subscription-Key", " ");
                transport_request_addrequestheader(pRequest, "X-ConnectionId", pRequest->connectionId);
                AddMUID(pRequest);
                metrics_transport_start(pRequest->connectionId);
                const int result = wsio_open(pRequest->ws.hWS,
                    WSIO_OnOpened,
                    pRequest,
                    WSIO_OnBytesRecv,
                    pRequest,
                    WSIO_OnError,
                    pRequest);
                if (result)
                {
                    LogError("wsio_open failed with result %d", result);
                    return -1;
                }
            }
        }

        pRequest->state = TRANSPORT_STATE_CONNECTED;
    }
	
    return 0;
}

static void WSOnCloseForReset(void* context)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)context;
    pRequest->fOpen = false;

    if (pRequest->state == TRANSPORT_STATE_RESETTING)
    {
        // Re-open the connection.
        pRequest->state = TRANSPORT_STATE_NETWORK_CHECK;
    }
}

static int ReplaceHeaderNameValuePair(
    HTTP_HEADERS_HANDLE hHeaders,
    const char *name,
    const char *value,
    int* valueChanged)
{
    const char* pszHeaderValue;
    *valueChanged = 0;

    pszHeaderValue = HTTPHeaders_FindHeaderValue(hHeaders, name);
    if (NULL == pszHeaderValue || strcmp(pszHeaderValue, value))
    {
        *valueChanged = 1;
        return HTTPHeaders_ReplaceHeaderNameValuePair(hHeaders, name, value);
    }

    return 0;
}

// Return zero if the tokens in the headers are already up-to-date, and
// non-zero otherwise.
static int add_auth_headers(
    TokenStore token_store,
    HTTP_HEADERS_HANDLE hHeaders,
    void* pContext)
{
	(void)pContext;

    int tokenChanged = 0;
    STRING_HANDLE access_token = STRING_new();
    int ret = 0; //token_store_get_access_token(token_store, kCortanaScope, access_token);

    if (ret == 0 && STRING_length(access_token) > 0)
    {
        ReplaceHeaderNameValuePair(
            hHeaders,
            kRPSDelegationHeaderName,
            STRING_c_str(access_token),
            &tokenChanged);

        
    }
    else
    {
        LogError("cached bing token is not valid");
    }
    
    STRING_delete(access_token);

    (void)token_store;
    return tokenChanged;
}

int transport_request_prepare(
    TRANSPORT_HANDLE hTransport
    )
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    int err = 0;
    if (NULL == pRequest || 
        NULL == pRequest->http.hRequest)
    {
        return -1;
    }

    if (pRequest->hResponseContent)
    {
        BUFFER_delete(pRequest->hResponseContent);
        pRequest->hResponseContent = NULL;
    }
    if (pRequest->token_store != NULL)
    {
        int tokenChanged = add_auth_headers(
            pRequest->token_store,
            pRequest->hHeaders, 
            pRequest->pContext);

        if ((cortana_gettickcount() - pRequest->nConnectionTime) >= WS_CONNECTION_TIME_MS)
        {
            tokenChanged = 1;
            LogInfo("forcing connection closed");
        }

        if (tokenChanged > 0 && pRequest->fWS &&
            (pRequest->state == TRANSPORT_STATE_CONNECTED))
        {
            // The token is snapped at the time that we are connected.  Reset
            // our connection if the token changes to avoid using a stale token
            // with our requests.
            //
            // We only do this for websocket connections because HTTP
            // connections are short-lived.

            // Set the transport state before calling wsio_close because
            // wsio_close may call the complete callback before returning.
            pRequest->state = TRANSPORT_STATE_RESETTING;
            LogInfo("token changed, resetting connection");
            metrics_transport_reset();

            const int closeResult =
                wsio_close(pRequest->ws.hWS, WSOnCloseForReset, pRequest);
            if (closeResult)
            {
                // We don't know how to recover from this state.  Crash the
                // process so that we start over.
                abort();
            }
        }
    }

    pRequest->fCompleted = false;
    if (pRequest->state == TRANSPORT_STATE_CLOSED)
    {
        pRequest->state = TRANSPORT_STATE_NETWORK_CHECK;
    }

    return err;
}

int transport_request_addrequestheader(
    TRANSPORT_HANDLE hTransport,
    const char *pszName,
    const char *pszValue
)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    if (NULL == pRequest ||
        NULL == pszName  ||
        NULL == pszValue)
    {
        return -1;
    }

    return HTTPHeaders_ReplaceHeaderNameValuePair(pRequest->hHeaders, pszName, pszValue);
}

int transport_stream_prepare(
    TRANSPORT_HANDLE hTransport,
    const char* pszPath
)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    int                ret;

    if (NULL == pRequest)
    {
        return -1;
    }

    pRequest->streamId++;

    if (pRequest->fWS)
    {
        if (NULL != pszPath)
        {
            pRequest->pszPath = pszPath + 1; // 1=remove "/"
            pRequest->ws.pszPathLen = strlen(pRequest->pszPath);
        }

        ret = wsio_setoption(pRequest->ws.hWS, "connectionheaders", pRequest->hHeaders);
        if (ret)
        {
            LogInfo("ERROR settings connectionheaders\n");
        }
    }

    ret = transport_request_prepare(pRequest);
    if (ret)
    {
        return ret;
    }

    if (pRequest->fWS)
    {
        // Only send agent events when we're sending a kAgentMessage.
        STRING_HANDLE turnContext =
            skill_serialize_context(pRequest->pContext, !pszPath);
        if (!turnContext)
        {
            LogError("TurnContext_Generate failed");
            return -1;
        }

        LogInfo("turnContext: %s", STRING_c_str(turnContext));

        // send a turn context
        size_t payloadSize = sizeof(kContextHeader) + 
            sizeof(kContextMessage) +
            sizeof(kRequestId) + 
            sizeof(pRequest->requestId) + 
			sizeof(kTimeStampHeaderName) +
			TIME_STRING_MAX_SIZE +
            STRING_length(turnContext);
        TRANSPORT_PACKET *pMsg = (TRANSPORT_PACKET*)malloc(sizeof(TRANSPORT_PACKET) + payloadSize);

        pMsg->msgtype = METRIC_MESSAGE_TYPE_DEVICECONTEXT;
        pMsg->wstype  = WSIO_MSG_TYPE_TEXT;

        char timeString[TIME_STRING_MAX_SIZE];
        int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
        if (timeStringLen < 0)
        {
            return -1;
        }

        // add headers
        pMsg->length = sprintf_s((char *)pMsg->buffer,
            payloadSize,
            kContextHeader,
            kTimeStampHeaderName,
            timeString,
            pszPath ? kContextMessage : kAgentMessage,
            kRequestId,
            pRequest->requestId,
            STRING_c_str(turnContext));

        WSIOQueue(pRequest, pMsg);
        STRING_delete(turnContext);
    }
    else
    {
        ret = http_client_start_chunk_request(
            pRequest->http.hRequest,
            HTTP_CLIENT_REQUEST_POST,
            pszPath,
            pRequest->hHeaders);
    }

    return ret;
}

int transport_stream_write(
    TRANSPORT_HANDLE hTransport,
    const uint8_t*   pBuffer,
    size_t           bufferSize
)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    if (NULL == pRequest)
    {
        return -1;
    }

    if (pRequest->fWS)
    {
        uint8_t msgtype = METRIC_MESSAGE_TYPE_INVALID;
        if (bufferSize == 0)
        {
            msgtype = METRIC_MESSAGE_TYPE_AUDIO_LAST;
            if (!pRequest->ws.chunksent)
            {
                return 0;
            }

            pRequest->ws.chunksent = false;
        }
        else if (!pRequest->ws.chunksent)
        {
            if (bufferSize < 6)
            {
                LogError("Bad payload");
                return -1;
            }

            if (memcmp(pBuffer, "RIFF", 4) && memcmp(pBuffer, "#!SILK", 6))
            {
                return 0;
            }

            pRequest->ws.chunksent = true;
            msgtype = METRIC_MESSAGE_TYPE_AUDIO_START;
        }

        size_t headerLen;
        size_t payloadSize = sizeof(kRequestFormat) + 
            (size_t)pRequest->ws.pszPathLen + 
            sizeof(kStreamId) + 
            30 + 
            sizeof(kRequestId) + 
            sizeof(pRequest->requestId) + 
			sizeof(kTimeStampHeaderName) +
			TIME_STRING_MAX_SIZE +
            bufferSize + 2; // 2 = header length
        TRANSPORT_PACKET *pMsg = (TRANSPORT_PACKET*)malloc(sizeof(TRANSPORT_PACKET) + WS_MESSAGE_HEADER_SIZE + payloadSize);
        pMsg->msgtype = msgtype;
        pMsg->wstype = WSIO_MSG_TYPE_BINARY;

        char timeString[TIME_STRING_MAX_SIZE];
        int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
        if (timeStringLen < 0)
        {
            return -1;
        }

        // add headers
        headerLen = sprintf_s((char*)pMsg->buffer + WS_MESSAGE_HEADER_SIZE,
            payloadSize,
            kRequestFormat,
            kTimeStampHeaderName,
            timeString,
            pRequest->pszPath,
            kStreamId,
            pRequest->streamId,
            kRequestId,
            pRequest->requestId);

        // two byte length
        pMsg->buffer[0] = (uint8_t)((headerLen >> 8) & 0xff);
        pMsg->buffer[1] = (uint8_t)((headerLen >> 0) & 0xff);
        pMsg->length = WS_MESSAGE_HEADER_SIZE + headerLen;

        // body
        memcpy(pMsg->buffer + pMsg->length, pBuffer, bufferSize);
        pMsg->length += bufferSize;

        WSIOQueue(pRequest, pMsg);
        return 0;
    }
    else
    {
        return http_client_send_chunk_request(pRequest->http.hRequest, pBuffer, bufferSize);
    }
}

int transport_stream_flush(
    TRANSPORT_HANDLE hTransport
)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    if (NULL == pRequest)
    {
        return -1;
    }

    if (pRequest->fWS)
    {
        if (!pRequest->fOpen)
        {
            return -1;
        }

        return transport_stream_write(pRequest, NULL, 0);
    }
    else
    {
        return http_client_end_chunk_request(pRequest->http.hRequest);
    }
}

int transport_dowork(TRANSPORT_HANDLE hTransport)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    TRANSPORT_PACKET*  packet;

    if (NULL == hTransport)
    {
        return -1;
    }

    switch (pRequest->state)
    {
    case TRANSPORT_STATE_CLOSED:
        while (NULL != (packet = (TRANSPORT_PACKET*)queue_dequeue(pRequest->hQueue)))
        {
            free(packet);
        }
        break;

    case TRANSPORT_STATE_RESETTING:
        // Do nothing -- we're waiting for WSOnCloseForReset to be invoked.
        break;

    case TRANSPORT_STATE_NETWORK_CHECKING:
        dns_cache_dowork(pRequest->hDnsCache, pRequest);
        return 1;

    case TRANSPORT_STATE_SENT:
        // do nothing
        break;

    case TRANSPORT_STATE_NETWORK_CHECK:
        if (NULL == pRequest->hDnsCache)
        {
            // skip dns checks
            pRequest->state = TRANSPORT_STATE_OPENING;
        }
        else
        {
            pRequest->state = TRANSPORT_STATE_NETWORK_CHECKING;
            const char* host = pRequest->fWS ? pRequest->ws.config.host : pRequest->http.tls_io_config.hostname;
            LogInfo("Start network check %s", host);
            metrics_transport_state_start(METRIC_TRANSPORT_STATE_DNS);
            if (dns_cache_getaddr(pRequest->hDnsCache, host, DnsComplete, pRequest))
            {
                LogError("dns_cache_getaddr failed");
                pRequest->state = TRANSPORT_STATE_OPENING;
            }
        }
        return 1;

    case TRANSPORT_STATE_OPENING:
        if (transport_open(pRequest))
        {
            return 0;
        }
        return 1;

    case TRANSPORT_STATE_CONNECTED:
        if (!pRequest->fWS)
        {
            // HTTP
            packet = (TRANSPORT_PACKET*)queue_peek(pRequest->hQueue);
            pRequest->state = TRANSPORT_STATE_SENT;

            if (http_client_execute_request(
                pRequest->http.hRequest,
                packet ? HTTP_CLIENT_REQUEST_POST : HTTP_CLIENT_REQUEST_GET,
                pRequest->pszPath,
                pRequest->hHeaders,
                packet ? packet->buffer : NULL,
                packet ? packet->length : 0))
            {
                LogError("http_client_execute_request");
                pRequest->state = TRANSPORT_STATE_CLOSED;
            }
        }
        else
        {
            while (pRequest->fOpen &&  NULL != (packet = (TRANSPORT_PACKET*)queue_dequeue(pRequest->hQueue)))
            {
                if (packet->msgtype != METRIC_MESSAGE_TYPE_INVALID)
                {
                    metrics_transport_state_start(packet->msgtype);
                }

                // re-stamp X-timestamp header value
                char timeString[TIME_STRING_MAX_SIZE] = "";
                int timeStringLength = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
                int offset = sizeof(kTimeStampHeaderName);
                if (packet->wstype == WSIO_MSG_TYPE_BINARY)
                {
                    // Include the first 2 bytes for header length
                    offset += 2;
                }
                memcpy((char*)packet->buffer + offset, timeString, timeStringLength);

                const int result = wsio_send(
                    pRequest->ws.hWS,
                    packet->buffer,
                    packet->length,
                    packet->wstype,
                    WSIO_ON_SEND_COMPLETE,
                    packet);
                if (result)
                {
                    LogError("Failed in wsio_send with result %d", result);
                }
            }
        }
        break;
    }

    if (pRequest->fWS)
    {
        wsio_dowork(pRequest->ws.hWS);
    }
    else if (pRequest->state == TRANSPORT_STATE_SENT)
    {
        http_client_dowork(pRequest->http.hRequest);
    }
    return pRequest->fCompleted ? 0 : 1;
}

int transport_setcallbacks(
    TRANSPORT_HANDLE             hTransport,
    PTRANSPORT_ERROR             pfnErrorCB,
    PTRANSPORT_RESPONSE_CALLBACK pfnRecvCB)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    if (NULL == pRequest)
    {
        return -1;
    }

    pRequest->OnTransportError = pfnErrorCB;
    pRequest->OnRecvCallback = pfnRecvCB;
    return 0;
}

int transport_set_tokenstore(
    TRANSPORT_HANDLE hTransport,
    TokenStore token_store)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    if (NULL == pRequest || NULL == token_store)
    {
        return -1;
    }

    pRequest->token_store = token_store;

    return 0;
}

static TRANSPORT_STREAM* transport_getstream(
    TRANSPORT_HANDLE hTransport,
    int              nStreamId)
{
    TRANSPORT_STREAM* pStream;
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;

    if (NULL == pRequest || !pRequest->fWS)
    {
        return NULL;
    }

    for (pStream = pRequest->ws.pStreamHead; pStream != NULL; pStream = pStream->pNext)
    {
        if (pStream->id == nStreamId)
        {
            return pStream;
        }
    }

    return NULL;
}

static TRANSPORT_STREAM* transport_createstream(
    TRANSPORT_HANDLE hTransport,
    int              nStreamId,
    const char*      pszContentType,
    void*            pContext)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    TRANSPORT_STREAM* pStream;

    if (NULL == pRequest || !pRequest->fWS || !pszContentType)
    {
        return NULL;
    }

    for (pStream = pRequest->ws.pStreamHead; pStream != NULL; pStream = pStream->pNext)
    {
        if (pStream->BufferSize == 0)
        {
            break;
        }
    }

    // stream wasn't found, prepare a new one.
    if (NULL == pStream)
    {
        // there wasn't a free stream, alloc a new one
        pStream = (TRANSPORT_STREAM*)malloc(sizeof(TRANSPORT_STREAM));
        if (NULL == pStream)
        {
            LogError("alloc TRANSPORT_STREAM failed");
            return NULL;
        }

        memset(pStream, 0, sizeof(TRANSPORT_STREAM));

        // attach the stream to the list
        pStream->pNext = pRequest->ws.pStreamHead;
        pRequest->ws.pStreamHead = pStream;
    }

    pStream->id = nStreamId;
    pStream->pContext = pContext;

    // copy the content type
    if (NULL == pStream->hContentType)
    {
        pStream->hContentType = STRING_construct(pszContentType);
    }
    else if (NULL == pStream->hContentType)
    {
        STRING_copy(pStream->hContentType, pszContentType);
    }

    return pStream;
}

void transport_set_dns_cache(
    TRANSPORT_HANDLE hTransport,
    DNS_CACHE_HANDLE hDnsCache)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    pRequest->hDnsCache = hDnsCache;
}

const char* transport_get_request_id(
    TRANSPORT_HANDLE hTransport)
{
    TRANSPORT_REQUEST* pRequest = (TRANSPORT_REQUEST*)hTransport;
    return pRequest->requestId;
}

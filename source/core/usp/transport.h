//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// transport.h: defines functions provided by the transport layer.
//

#pragma once

#include <stdbool.h>

#include "tokenstore.h"
#include "dnscache.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KEYWORD_PATH "Path"

typedef struct _TransportRequest* TransportHandle;

typedef struct _TELEMETRY_CONTEXT* TELEMETRY_HANDLE;

typedef struct HTTP_HEADERS_HANDLE_DATA_TAG* HTTP_HEADERS_HANDLE;

/**
 * Creates a new transport request.
 * @param host The host name.
 * @param context The application defined context that will be passed back during callback.
 * @param telemetry Telemetry handle to record various transport events.
 * @param connectionHeaders A handle to headers that will be used to establish a connection.
 * @param connectionId An identifier of a connection, used for diagnostics of errors on the server side.
 * @return A new transport handle.
 */
TransportHandle TransportRequestCreate(const char* host, void* context, TELEMETRY_HANDLE telemetry, HTTP_HEADERS_HANDLE connectionHeaders, const char* connectionId);

/**
 * Destroys a transport request.
 * @param transportHandle The transport handle representing the request to be destroyed.
 */
void TransportRequestDestroy(TransportHandle transportHandle);

/**
 * Executes a transport request.
 * @param transportHandle The request to execute.
 * @param path The path to request.
 * @param buffer The buffer to send to the transport service.
 * @param length The length of pBuffer.
 * @return A return code or zero if successful.
 */
int TransportRequestExecute(TransportHandle transportHandle, const char* path, uint8_t* buffer, size_t length);

/**
 * Prepares the start of a new transport request.
 * @param transportHandle The request to prepare.
 * @return A return code or zero if successful.
 */
int TransportRequestPrepare(TransportHandle transportHandle);

/**
 * Prepares the start of a new transport stream.
 * @param transportHandle The request to prepare.
 * @param path The path to use for stream I/O.
 * @return A return code or zero if successful.
 */
int TransportStreamPrepare(TransportHandle transportHandle, const char* path);

/**
 * Write a text message to the websocket.
 * @param transportHandle The request to prepare.
 * @param path The path to use for message
 * @param buffer The buffer to write to the websocket.
 * @param bufferSize The byte size of the buffer.
 * @return A return code or zero if successful.
 */
int TransportMessageWrite(TransportHandle transportHandle, const char* path, const uint8_t* buffer, size_t bufferSize);


/**
 * Writes to the transport stream.
 * @param transportHandle The request to prepare.
 * @param buffer The buffer to write to the stream.
 * @param bufferSize The byte size of pBuffer.
 * @return A return code or zero if successful.
 */
int TransportStreamWrite(TransportHandle transportHandle, const uint8_t* buffer, size_t bufferSize);

/**
 * Flushes any outstanding I/O on the transport stream.
 * @param transportHandle The request to prepare.
 * @return A return code or zero if successful.
 */
int TransportStreamFlush(TransportHandle transportHandle);

/**
 * Processes any outstanding operations that need attention.
 * @param transportHandle The request to process.
 */
void TransportDoWork(TransportHandle transportHandle);

typedef enum _TransportError
{
    TRANSPORT_ERROR_UNKNOWN,
    TRANSPORT_ERROR_REMOTE_CLOSED,
    TRANSPORT_ERROR_CONNECTION_FAILURE,
    TRANSPORT_ERROR_WEBSOCKET_UPGRADE,
    TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME,
    TRANSPORT_ERROR_WEBSOCKET_ERROR,
    TRANSPORT_ERROR_DNS_FAILURE
} TransportError;

typedef enum _HttpStatusCode 
{
    HTTP_BADREQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_TOO_MANY_REQUESTS = 429,
} HttpStatusCode;

/** Transport error with additional information.
 *
 * For internal documentation, here is a rough overview on what errorCode contains:
 *
 * For TRANSPORT_ERROR_REMOTE_CLOSED, it should be a WebSocket close code if
 * there was one or -1 if none was received. See CLOSE_* in uws_client.h or
 * WebSocket RFC.
 *
 * For TRANSPORT_ERROR_CONNECTION_FAILURE, it is a WS_OPEN_RESULT. (The
 * errorString should contain an internal numerical code as well. There are
 * a variety of sources for this one, including native socket operation errors.)
 *
 * For TRANSPORT_ERROR_WEBSOCKET_UPGRADE, it is an HTTP status code != 101,
 * received as a upgrade response.
 *
 * For TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME and an HTTP connection it's -1.
 *
 * For TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME and an WebSocket connection it's 1
 * or a line number where the error originated (e.g., from uws_client.c).
 *
 * For TRANSPORT_ERROR_WEBSOCKET_ERROR, it is a WS_ERROR.
 *
 * For TRANSPORT_ERROR_DNS_FAILURE, which cannot trigger on Windows, is is a
 * getaddrinfo() return value.
 */
typedef struct _TransportErrorInfo
{
    TransportError reason;
    int errorCode;
    const char *errorString;
} TransportErrorInfo;

/**
 * The TransportErrorCallback type represents an application-defined
 * status callback function used for signaling when the transport has failed.
 * @param transportHandle The transport handle.
 * @param errorInfo Pointer to struct containing transport error information (or NULL).
 * @param context A pointer to the application-defined callback context.
 */
typedef void(*TransportErrorCallback)(TransportHandle transportHandle, TransportErrorInfo* errorInfo, void* context);

/**
 * The TransportReponseCallback type represents an application-defined
 * status callback function used for signaling when data has been received.
 * @param transportHandle The transport handle.
 * @param responseHeader A response header handle.
 * @param buffer A pointer to the received content.
 * @param bufferLen The length of pBuffer.
 * @param errorCode The transport error code.
 * @param context A pointer to the application-defined callback context.
 */
typedef void(*TransportResponseCallback)(TransportHandle transportHandle, HTTP_HEADERS_HANDLE  responseHeader, const unsigned char* buffer, size_t bufferLen, unsigned int errorCode, void* context); 

/**
 * Registers for events from the transport.
 * @param transportHandle The request to prepare.
 * @param errorCallback The error callback.
 * @param recvCallback The response callback.
 * @return A return code or zero if successful.
 */
int TransportSetCallbacks(TransportHandle transportHandle, TransportErrorCallback errorCallback, TransportResponseCallback recvCallback);

/**
 * Enables authorization header on transport.
 * @param transportHandle The request to set.
 * @param token_store The token store to pull tokens from.
 * @return A return code or zero if successful.
 */
int TransportSetTokenStore(TransportHandle transportHandle, TokenStore token_store);

/**
* Creates a new request id on transport
* @param transportHandle The transport handle.
*/
void TransportCreateRequestId(TransportHandle transportHandle);

/**
* Gets the current request id on transport
* @param transportHandle The transport handle.
*/
const char* TransportGetRequestId(TransportHandle transportHandle);

/**
* Sets the DNS cache on transport
* @param transportHandle The transport handle.
* @param dnsCache The DNS cache handle.
*/
void TransportSetDnsCache(TransportHandle transportHandle, DnsCacheHandle dnsCache);

/**
 * Sends the provided buffer content as a telemetry event (using 'telemetry' message path).
 * @param transportHandle Transport handle.
 * @param buffer The buffer to write.
 * @param bytesToWrite Size of the buffer in bytes.
 * @param requestId Request id to tag the telemetry message with.
 * @return A return code or zero if successful.
 */
void TransportWriteTelemetry(TransportHandle transportHandle, const uint8_t* buffer, size_t bytesToWrite, const char *requestId);

#ifdef __cplusplus
}
#endif


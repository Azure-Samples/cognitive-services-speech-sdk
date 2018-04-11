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
 * @return A new transport handle.
 */
TransportHandle TransportRequestCreate(const char* host, void* context, TELEMETRY_HANDLE telemetry, HTTP_HEADERS_HANDLE connectionHeaders);

/**
 * Destroys a transport request.
 * @param transportHandle The tranport handle representing the request to be destroyed.
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
 * @param tranportHandle The request to process.
 */
void TransportDoWork(TransportHandle transportHandle);

typedef enum _TransportError
{
    TRANSPORT_ERROR_NONE = 0,
    TRANSPORT_ERROR_REMOTECLOSED,
    TRANSPORT_ERROR_CONNECTION_FAILURE,
    TRANSPORT_ERROR_DNS_FAILURE,
    TRANSPORT_ERROR_HTTP_UNAUTHORIZED = 401,
    TRANSPORT_ERROR_HTTP_FORBIDDEN = 403,
} TransportError;

/**
 * The TransportErrorCallback type represents an application-defined
 * status callback function used for signaling when the transport has failed.
 * @param transportHandle The transport handle.
 * @param reason The reeason for the error.
 * @param context A pointer to the application-defined callback context.
 */
typedef void(*TransportErrorCallback)(TransportHandle transportHandle, TransportError reason, void* context);

/**
 * The PTRANSPORT_RECV_CALLBACK type represents an application-defined
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
* sets the DNS cache on transport
* @param transportHandle The transport handle.
*/
void TransportSetDnsCache(TransportHandle transportHandle, DnsCacheHandle dnsCache);

/**
 * Sends the provided buffer content as a telemetry event (using 'telemetry' message path).
 * @param transportHandle Transport handle.
 * @param buffer The buffer to write.
 * @param bufferSize Size of the buffer in bytes.
 * @param requestId Request id to tag the telemetry message with.
 * @return A return code or zero if successful.
 */
void TransportWriteTelemetry(TransportHandle transportHandle, const uint8_t* buffer, size_t bytesToWrite, const char *requestId);

#ifdef __cplusplus
}
#endif


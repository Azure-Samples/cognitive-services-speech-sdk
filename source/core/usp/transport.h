//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// transport.h: defines functions provided by the transport layer.
//

#pragma once

#include <stdbool.h>

#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/uhttp.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_c_shared_utility/list.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/lock.h"

#include "tokenstore.h"
#include "dnscache.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KEYWORD_PATH "Path"

typedef void* TransportHandle;

/**
 * Creates a new transport request.
 * @param host The host name.
 * @param context The application defined context that will be passed back during callback.
 * @return A new transport handle.
 */
TransportHandle TransportRequestCreate(const char* host, void* context);

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
 * Adds a request header to be used in the transport request.
 * @param transportHandle The request to prepare.
 * @param name The name of the header.
 * @param value The value of the header.
 * @return A return code or zero if successful.
 */
int TransportRequestAddRequestHeader(TransportHandle transportHandle, const char *name, const char *´value);

/**
 * Prepares the start of a new transport stream.
 * @param transportHandle The request to prepare.
 * @param path The path to use for stream I/O.
 * @return A return code or zero if successful.
 */
int TransportStreamPrepare(TransportHandle transportHandle, const char* path);

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
 * @return Returns zero when the request has completed, or non-zero if there is still pending I/O.
 */
int TransportDoWork(TransportHandle transportHandle);

typedef enum _TransportError
{
    TRANSPORT_ERROR_NONE = 0,
    TRANSPORT_ERROR_AUTHENTICATION,
    TRANSPORT_ERROR_REMOTECLOSED,
    TRANSPORT_ERROR_CONNECTION_FAILURE,
    TRANSPORT_ERROR_DNS_FAILURE
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

#ifdef __cplusplus
}
#endif


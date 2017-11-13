#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "azure_c_shared_utility/httpheaders.h"
#include "SpeechAPI.h"
#include "token_store.h"
#include "dns_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void*           TRANSPORT_HANDLE;

/**
 * Creates a new transport request.
 * @param pszHost The host name.
 * @param pContext The application defined context to return.
 * @return A new transport handle.
 */
TRANSPORT_HANDLE transport_request_create(
    const char*        pszHost,
    void*              pContext
    );

/**
 * Destroys a transport request.
 * @param hTransport The request to destroy.
 */
void transport_request_destroy(
    TRANSPORT_HANDLE hTransport
    );

/**
 * Executes a transport request.
 * @param hTransport The request to execute.
 * @param pszPath The path to request.
 * @param pBuffer The buffer to send to the transport service.
 * @param length The length of pBuffer.
 * @return A return code or zero if successful.
 */
SPEECH_RESULT transport_request_execute(
    TRANSPORT_HANDLE   hTransport,
    const char*        pszPath,
    uint8_t*           pBuffer,
    size_t             length
    );

/**
 * Prepares the start of a new transport request.
 * @param hTransport The request to prepare.
 * @return A return code or zero if successful.
 */
int transport_request_prepare(
    TRANSPORT_HANDLE hTransport
);

/**
 * Adds a request header to be used in each transport request.
 * @param hTransport The request to prepare.
 * @param pszName The name of the header.
 * @param pszValue The value of the header.
 * @return A return code or zero if successful.
 */
int transport_request_addrequestheader(
    TRANSPORT_HANDLE hTransport,
    const char *pszName,
    const char *pszValue
);

/**
 * Prepares the start of a new transport stream.
 * @param hTransport The request to prepare.
 * @param pszPath The path to use for stream I/O.
 * @return A return code or zero if successful.
 */
int transport_stream_prepare(
    TRANSPORT_HANDLE hTransport,
    const char* pszPath
);

/**
 * Writes to the transport stream.
 * @param hTransport The request to prepare.
 * @param pBuffer The buffer to write to the stream.
 * @param bufferSize The byte size of pBuffer.
 * @return A return code or zero if successful.
 */
int transport_stream_write(
    TRANSPORT_HANDLE hTransport,
    const uint8_t*   pBuffer,
    size_t           bufferSize
);

/**
 * Flushes any outstanding I/O on the transport stream.
 * @param hTransport The request to prepare.
 * @return A return code or zero if successful.
 */
int transport_stream_flush(
    TRANSPORT_HANDLE hTransport
);

/**
 * Processes any outstanding operations that need attention.
 * @param handle The request handle.
 * @return Returns zero when the request has completed, or non-zero if there is still pending I/O.
 */
int transport_dowork(
    TRANSPORT_HANDLE hTransport
);

enum transport_error
{
    transport_error_none = 0,
    transport_error_authentication,
    transport_error_remoteclosed,
    transport_error_connection_failure,
    transport_error_dns_failure,
};

/**
 * The PTRANSPORT_ERROR type represents an application-defined
 * status callback function used for signaling when the transport has failed.
 * @param hTransport The transport handle.
 * @param reason The reeason for the error.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*PTRANSPORT_ERROR)(
    TRANSPORT_HANDLE        hTransport,
    enum transport_error    reason,
    void*                   pContext);

/**
 * The PTRANSPORT_RECV_CALLBACK type represents an application-defined
 * status callback function used for signaling when data has been received.
 * @param hTransport The transport handle.
 * @param hResponseHeader A response header handle.
 * @param pBuffer A pointer to the received content.
 * @param bufferLen The length of pBuffer.
 * @param errorCode The transport error code.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*PTRANSPORT_RESPONSE_CALLBACK)(
    TRANSPORT_HANDLE     hTransport,
    HTTP_HEADERS_HANDLE  hResponseHeader,
    const unsigned char* pBuffer,
    size_t               bufferLen,
    unsigned int         errorCode,
    void*                pContext); 

/**
 * Registers for events from the transport.
 * @param hTransport The request to prepare.
 * @param pfnErrorCB The error callback.
 * @param pfnRecvCB The response callback.
 * @return A return code or zero if successful.
 */
int transport_setcallbacks(
    TRANSPORT_HANDLE             hTransport,
    PTRANSPORT_ERROR             pfnErrorCB,
    PTRANSPORT_RESPONSE_CALLBACK pfnRecvCB
);

/**
 * Enable authorization header on transport
 * @param token_store The token store to pull tokens from
 * @return A return code or zero if successful.
 */
int transport_set_tokenstore(
    TRANSPORT_HANDLE hTransport,
    TokenStore token_store
);

#ifdef __cplusplus
}
#endif

#endif
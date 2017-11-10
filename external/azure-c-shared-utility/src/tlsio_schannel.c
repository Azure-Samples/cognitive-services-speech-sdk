// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define SECURITY_WIN32

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/tlsio_schannel.h"
#include "azure_c_shared_utility/socketio.h"
#include "windows.h"
#include "sspi.h"
#include "schannel.h"
#include "azure_c_shared_utility/xlogging.h"

typedef enum TLSIO_STATE_TAG
{
    TLSIO_STATE_NOT_OPEN,
    TLSIO_STATE_OPENING_UNDERLYING_IO,
    TLSIO_STATE_HANDSHAKE_CLIENT_HELLO_SENT,
    TLSIO_STATE_HANDSHAKE_SERVER_HELLO_RECEIVED,
    TLSIO_STATE_OPEN,
    TLSIO_STATE_CLOSING,
    TLSIO_STATE_ERROR
} TLSIO_STATE;

typedef struct TLS_IO_INSTANCE_TAG
{
    XIO_HANDLE socket_io;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    void* on_io_open_complete_context;
    void* on_io_close_complete_context;
    void* on_bytes_received_context;
    void* on_io_error_context;
    CtxtHandle security_context;
    TLSIO_STATE tlsio_state;
    SEC_CHAR* host_name;
    CredHandle credential_handle;
    bool credential_handle_allocated;
    unsigned char* received_bytes;
    size_t received_byte_count;
    size_t buffer_size;
    size_t needed_bytes;
} TLS_IO_INSTANCE;

static const IO_INTERFACE_DESCRIPTION tlsio_schannel_interface_description =
{
    tlsio_schannel_create,
    tlsio_schannel_destroy,
    tlsio_schannel_open,
    tlsio_schannel_close,
    tlsio_schannel_send,
    tlsio_schannel_dowork,
    tlsio_schannel_setoption
};

static void indicate_error(TLS_IO_INSTANCE* tls_io_instance)
{
    if (tls_io_instance->on_io_error != NULL)
    {
        tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
    }
}

static int resize_receive_buffer(TLS_IO_INSTANCE* tls_io_instance, size_t needed_buffer_size)
{
    int result;

    if (needed_buffer_size > tls_io_instance->buffer_size)
    {
        unsigned char* new_buffer = realloc(tls_io_instance->received_bytes, needed_buffer_size);
        if (new_buffer == NULL)
        {
            result = __LINE__;
        }
        else
        {
            tls_io_instance->received_bytes = new_buffer;
            tls_io_instance->buffer_size = needed_buffer_size;
            result = 0;
        }
    }
    else
    {
        result = 0;
    }

    return result;
}

static void on_underlying_io_close_complete(void* context)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;
    if (tls_io_instance->tlsio_state == TLSIO_STATE_CLOSING)
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
        if (tls_io_instance->on_io_close_complete != NULL)
        {
            tls_io_instance->on_io_close_complete(tls_io_instance->on_io_close_complete_context);
        }

        /* Free security context resources corresponding to creation with open */
        DeleteSecurityContext(&tls_io_instance->security_context);

        if (tls_io_instance->credential_handle_allocated)
        {
            (void)FreeCredentialHandle(&tls_io_instance->credential_handle);
            tls_io_instance->credential_handle_allocated = false;
        }
    }
}

static void on_underlying_io_open_complete(void* context, IO_OPEN_RESULT io_open_result)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;
    (void)io_open_result;

    if (tls_io_instance->tlsio_state != TLSIO_STATE_OPENING_UNDERLYING_IO)
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        indicate_error(tls_io_instance);
    }
    else
    {
        SecBuffer init_security_buffers[2];
        ULONG context_attributes;
        SECURITY_STATUS status;
        SCHANNEL_CRED auth_data;

        auth_data.dwVersion = SCHANNEL_CRED_VERSION;
        auth_data.cCreds = 0;
        auth_data.paCred = NULL;
        auth_data.hRootStore = NULL;
        auth_data.cSupportedAlgs = 0;
        auth_data.palgSupportedAlgs = NULL;
        auth_data.grbitEnabledProtocols = 0;
        auth_data.dwMinimumCipherStrength = 0;
        auth_data.dwMaximumCipherStrength = 0;
        auth_data.dwSessionLifespan = 0;
        auth_data.dwFlags = SCH_USE_STRONG_CRYPTO | SCH_CRED_NO_DEFAULT_CREDS;
        auth_data.dwCredFormat = 0;

        status = AcquireCredentialsHandle(NULL, UNISP_NAME, SECPKG_CRED_OUTBOUND, NULL,
            &auth_data, NULL, NULL, &tls_io_instance->credential_handle, NULL);
        if (status != SEC_E_OK)
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
            indicate_error(tls_io_instance);
        }
        else
        {
            tls_io_instance->credential_handle_allocated = true;

            init_security_buffers[0].cbBuffer = 0;
            init_security_buffers[0].BufferType = SECBUFFER_TOKEN;
            init_security_buffers[0].pvBuffer = NULL;
            init_security_buffers[1].cbBuffer = 0;
            init_security_buffers[1].BufferType = SECBUFFER_EMPTY;
            init_security_buffers[1].pvBuffer = 0;

            SecBufferDesc security_buffers_desc;
            security_buffers_desc.cBuffers = 2;
            security_buffers_desc.pBuffers = init_security_buffers;
            security_buffers_desc.ulVersion = SECBUFFER_VERSION;

            status = InitializeSecurityContextA(&tls_io_instance->credential_handle,
                NULL, tls_io_instance->host_name, ISC_REQ_EXTENDED_ERROR | ISC_REQ_STREAM | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_USE_SUPPLIED_CREDS, 0, 0, NULL, 0,
                &tls_io_instance->security_context, &security_buffers_desc,
                &context_attributes, NULL);

            if ((status == SEC_I_COMPLETE_NEEDED) || (status == SEC_I_CONTINUE_NEEDED) || (status == SEC_I_COMPLETE_AND_CONTINUE))
            {
                if (xio_send(tls_io_instance->socket_io, init_security_buffers[0].pvBuffer, init_security_buffers[0].cbBuffer, NULL, NULL) != 0)
                {
                    tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                    indicate_error(tls_io_instance);
                }
                else
                {
                    /* set the needed bytes to 1, to get on the next byte how many we actually need */
                    tls_io_instance->needed_bytes = 1;
                    if (resize_receive_buffer(tls_io_instance, tls_io_instance->needed_bytes + tls_io_instance->received_byte_count) != 0)
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        indicate_error(tls_io_instance);
                    }
                    else
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_HANDSHAKE_CLIENT_HELLO_SENT;
                    }
                }
            }
        }
    }
}

static int set_receive_buffer(TLS_IO_INSTANCE* tls_io_instance, size_t buffer_size)
{
    int result;

    unsigned char* new_buffer = realloc(tls_io_instance->received_bytes, buffer_size);
    if (new_buffer == NULL)
    {
        result = __LINE__;
    }
    else
    {
        tls_io_instance->received_bytes = new_buffer;
        tls_io_instance->buffer_size = buffer_size;
        result = 0;
    }

    return result;
}

static void on_underlying_io_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;
    size_t consumed_bytes;

    if (resize_receive_buffer(tls_io_instance, tls_io_instance->received_byte_count + size) == 0)
    {
        memcpy(tls_io_instance->received_bytes + tls_io_instance->received_byte_count, buffer, size);
        tls_io_instance->received_byte_count += size;

        if (size > tls_io_instance->needed_bytes)
        {
            tls_io_instance->needed_bytes = 0;
        }
        else
        {
            tls_io_instance->needed_bytes -= size;
        }

        /* Drain what we received */
        while (tls_io_instance->needed_bytes == 0)
        {
            if (tls_io_instance->tlsio_state == TLSIO_STATE_HANDSHAKE_CLIENT_HELLO_SENT)
            {
                SecBuffer input_buffers[2];
                SecBuffer output_buffers[2];
                ULONG context_attributes;

                /* we need to try and perform the second (next) step of the init */
                input_buffers[0].cbBuffer = (int)tls_io_instance->received_byte_count;
                input_buffers[0].BufferType = SECBUFFER_TOKEN;
                input_buffers[0].pvBuffer = (void*)tls_io_instance->received_bytes;
                input_buffers[1].cbBuffer = 0;
                input_buffers[1].BufferType = SECBUFFER_EMPTY;
                input_buffers[1].pvBuffer = 0;

                SecBufferDesc input_buffers_desc;
                input_buffers_desc.cBuffers = 2;
                input_buffers_desc.pBuffers = input_buffers;
                input_buffers_desc.ulVersion = SECBUFFER_VERSION;

                output_buffers[0].cbBuffer = 0;
                output_buffers[0].BufferType = SECBUFFER_TOKEN;
                output_buffers[0].pvBuffer = NULL;
                output_buffers[1].cbBuffer = 0;
                output_buffers[1].BufferType = SECBUFFER_EMPTY;
                output_buffers[1].pvBuffer = 0;

                SecBufferDesc output_buffers_desc;
                output_buffers_desc.cBuffers = 2;
                output_buffers_desc.pBuffers = output_buffers;
                output_buffers_desc.ulVersion = SECBUFFER_VERSION;

                unsigned long flags = ISC_REQ_EXTENDED_ERROR | ISC_REQ_STREAM | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_USE_SUPPLIED_CREDS;
                SECURITY_STATUS status = InitializeSecurityContextA(&tls_io_instance->credential_handle,
                    &tls_io_instance->security_context, tls_io_instance->host_name, flags, 0, 0, &input_buffers_desc, 0,
                    &tls_io_instance->security_context, &output_buffers_desc,
                    &context_attributes, NULL);

                switch (status)
                {
                case SEC_E_INCOMPLETE_MESSAGE:
                    if (input_buffers[1].BufferType != SECBUFFER_MISSING)
                    {
                        //If SECBUFFER_MISSING not sent, try to read byte by byte. 
                        tls_io_instance->needed_bytes = 1;
                    }
                    else
                    {
                        tls_io_instance->needed_bytes = input_buffers[1].cbBuffer;
                    }

                    if (resize_receive_buffer(tls_io_instance, tls_io_instance->received_byte_count + tls_io_instance->needed_bytes) != 0)
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        if (tls_io_instance->on_io_open_complete != NULL)
                        {
                            tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_ERROR);
                        }
                    }

                    break;
                case SEC_E_OK:
                    consumed_bytes = tls_io_instance->received_byte_count;
                    /* Any extra bytes left over or did we fully consume the receive buffer? */
                    if (input_buffers[1].BufferType == SECBUFFER_EXTRA)
                    {
                        consumed_bytes -= input_buffers[1].cbBuffer;
                        (void)memmove(tls_io_instance->received_bytes, tls_io_instance->received_bytes + consumed_bytes, tls_io_instance->received_byte_count - consumed_bytes);
                    }
                    tls_io_instance->received_byte_count -= consumed_bytes;

                    /* if nothing more to consume, set the needed bytes to 1, to get on the next byte how many we actually need */
                    tls_io_instance->needed_bytes = tls_io_instance->received_byte_count == 0 ? 1 : 0;

                    if (set_receive_buffer(tls_io_instance, tls_io_instance->needed_bytes + tls_io_instance->received_byte_count) != 0)
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        if (tls_io_instance->on_io_open_complete != NULL)
                        {
                            tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_ERROR);
                        }
                    }
                    else
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
                        if (tls_io_instance->on_io_open_complete != NULL)
                        {
                            tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_OK);
                        }
                    }
                    break;
                case SEC_I_COMPLETE_NEEDED:
                case SEC_I_CONTINUE_NEEDED:
                case SEC_I_COMPLETE_AND_CONTINUE:
                    if ((output_buffers[0].cbBuffer > 0) && xio_send(tls_io_instance->socket_io, output_buffers[0].pvBuffer, output_buffers[0].cbBuffer, NULL, NULL) != 0)
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        if (tls_io_instance->on_io_open_complete != NULL)
                        {
                            tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_ERROR);
                        }
                    }
                    else
                    {
                        consumed_bytes = tls_io_instance->received_byte_count;
                        /* Any extra bytes left over or did we fully consume the receive buffer? */
                        if (input_buffers[1].BufferType == SECBUFFER_EXTRA)
                        {
                            consumed_bytes -= input_buffers[1].cbBuffer;
                            (void)memmove(tls_io_instance->received_bytes, tls_io_instance->received_bytes + consumed_bytes, tls_io_instance->received_byte_count - consumed_bytes);
                        }
                        tls_io_instance->received_byte_count -= consumed_bytes;

                        /* if nothing more to consume, set the needed bytes to 1, to get on the next byte how many we actually need */
                        tls_io_instance->needed_bytes = tls_io_instance->received_byte_count == 0 ? 1 : 0;

                        if (set_receive_buffer(tls_io_instance, tls_io_instance->needed_bytes + tls_io_instance->received_byte_count) != 0)
                        {
                            tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                            if (tls_io_instance->on_io_open_complete != NULL)
                            {
                                tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_ERROR);
                            }
                        }
                        else
                        {
                            tls_io_instance->tlsio_state = TLSIO_STATE_HANDSHAKE_CLIENT_HELLO_SENT;
                        }
                    }
                    break;
                case SEC_E_UNTRUSTED_ROOT:
                    tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                    if (tls_io_instance->on_io_open_complete != NULL)
                    {
                        tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_ERROR);
                    }
                    break;
                default:
                    {
                        LPVOID srcText = NULL;
                        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                            status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)srcText, 0, NULL) > 0)
                        {
                            LogError("[%#x] %s", status, (LPTSTR)srcText);
                            LocalFree(srcText);
                        }
                        else
                        {
                            LogError("[%#x]", status);
                        }

                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        indicate_error(tls_io_instance);
                    }
                    break;
                }
            }
            else if (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN)
            {
                SecBuffer security_buffers[4];
                SecBufferDesc security_buffers_desc;

                security_buffers[0].BufferType = SECBUFFER_DATA;
                security_buffers[0].pvBuffer = tls_io_instance->received_bytes;
                security_buffers[0].cbBuffer = (unsigned long)tls_io_instance->received_byte_count;
                security_buffers[1].BufferType = SECBUFFER_EMPTY;
                security_buffers[2].BufferType = SECBUFFER_EMPTY;
                security_buffers[3].BufferType = SECBUFFER_EMPTY;

                security_buffers_desc.cBuffers = sizeof(security_buffers) / sizeof(security_buffers[0]);
                security_buffers_desc.pBuffers = security_buffers;
                security_buffers_desc.ulVersion = SECBUFFER_VERSION;

                SECURITY_STATUS status = DecryptMessage(&tls_io_instance->security_context, &security_buffers_desc, 0, NULL);
                switch (status)
                {
                case SEC_E_INCOMPLETE_MESSAGE:
                    if (security_buffers[1].BufferType != SECBUFFER_MISSING)
                    {
                        //If SECBUFFER_MISSING not sent, try to read byte by byte.
                        tls_io_instance->needed_bytes = 1;
                    }
                    else
                    {
                        tls_io_instance->needed_bytes = security_buffers[1].cbBuffer;
                    }

                    if (resize_receive_buffer(tls_io_instance, tls_io_instance->received_byte_count + tls_io_instance->needed_bytes) != 0)
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        indicate_error(tls_io_instance);
                    }
                    break;
                case SEC_E_OK:
                    if (security_buffers[1].BufferType != SECBUFFER_DATA)
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        indicate_error(tls_io_instance);
                    }
                    else
                    {
                        /* notify of the received data */
                        if (tls_io_instance->on_bytes_received != NULL)
                        {
                            tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, security_buffers[1].pvBuffer, security_buffers[1].cbBuffer);
                        }

                        consumed_bytes = tls_io_instance->received_byte_count;

                        for (size_t i = 0; i < sizeof(security_buffers) / sizeof(security_buffers[0]); i++)
                        {
                            /* Any extra bytes left over or did we fully consume the receive buffer? */
                            if (security_buffers[i].BufferType == SECBUFFER_EXTRA)
                            {
                                consumed_bytes -= security_buffers[i].cbBuffer;
                                (void)memmove(tls_io_instance->received_bytes, tls_io_instance->received_bytes + consumed_bytes, tls_io_instance->received_byte_count - consumed_bytes);
                                break;
                            }
                        }
                        tls_io_instance->received_byte_count -= consumed_bytes;

                        /* if nothing more to consume, set the needed bytes to 1, to get on the next byte how many we actually need */
                        tls_io_instance->needed_bytes = tls_io_instance->received_byte_count == 0 ? 1 : 0;

                        if (set_receive_buffer(tls_io_instance, tls_io_instance->needed_bytes + tls_io_instance->received_byte_count) != 0)
                        {
                            tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                            indicate_error(tls_io_instance);
                        }
                    }
                    break;
                default:
                    {
                        LPVOID srcText = NULL;
                        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                            status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)srcText, 0, NULL) > 0)
                        {
                            LogError("[%#x] %s", status, (LPTSTR)srcText);
                            LocalFree(srcText);
                        }
                        else
                        {
                            LogError("[%#x]", status);
                        }

                        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                        indicate_error(tls_io_instance);
                    }
                    break;
                }
            }
            else
            {
                /* Received data in error or other state */
                break;
            }
        }
    
    }
}

static void on_underlying_io_error(void* context)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    switch (tls_io_instance->tlsio_state)
    {
    default:
    case TLSIO_STATE_NOT_OPEN:
    case TLSIO_STATE_ERROR:
        break;

    case TLSIO_STATE_OPENING_UNDERLYING_IO:
    case TLSIO_STATE_HANDSHAKE_CLIENT_HELLO_SENT:
    case TLSIO_STATE_HANDSHAKE_SERVER_HELLO_RECEIVED:
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        if (tls_io_instance->on_io_open_complete != NULL)
        {
            tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_ERROR);
        }
        break;

    case TLSIO_STATE_CLOSING:
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        if (tls_io_instance->on_io_close_complete != NULL)
        {
            tls_io_instance->on_io_close_complete(tls_io_instance->on_io_close_complete_context);
        }
        break;

    case TLSIO_STATE_OPEN:
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        indicate_error(tls_io_instance);
        break;
    }
}

CONCRETE_IO_HANDLE tlsio_schannel_create(void* io_create_parameters)
{
    TLSIO_CONFIG* tls_io_config = io_create_parameters;
    TLS_IO_INSTANCE* result;

    if (tls_io_config == NULL)
    {
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(TLS_IO_INSTANCE));
        if (result != NULL)
        {
            SOCKETIO_CONFIG socketio_config;
			size_t host_name_len;
            socketio_config.hostname = tls_io_config->hostname;
            socketio_config.port = tls_io_config->port;
            socketio_config.accepted_socket = NULL;

            result->on_bytes_received = NULL;
            result->on_io_open_complete = NULL;
            result->on_io_close_complete = NULL;
            result->on_io_error = NULL;
            result->on_io_open_complete_context = NULL;
            result->on_io_close_complete_context = NULL;
            result->on_bytes_received_context = NULL;
            result->on_io_error_context = NULL;
            result->credential_handle_allocated = false;

			host_name_len = sizeof(SEC_CHAR) * (1 + strlen(tls_io_config->hostname));
            result->host_name = (SEC_CHAR*)malloc(host_name_len);
            if (result->host_name == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                (void)strcpy_s(result->host_name, host_name_len, tls_io_config->hostname);

                const IO_INTERFACE_DESCRIPTION* socket_io_interface = socketio_get_interface_description();
                if (socket_io_interface == NULL)
                {
                    free(result->host_name);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->socket_io = xio_create(socket_io_interface, &socketio_config);
                    if (result->socket_io == NULL)
                    {
                        free(result->host_name);
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        result->received_bytes = NULL;
                        result->received_byte_count = 0;
                        result->buffer_size = 0;
                        result->tlsio_state = TLSIO_STATE_NOT_OPEN;
                    }
                }
            }
        }
    }

    return result;
}

void tlsio_schannel_destroy(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io != NULL)
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        if (tls_io_instance->credential_handle_allocated)
        {
            (void)FreeCredentialHandle(&tls_io_instance->credential_handle);
            tls_io_instance->credential_handle_allocated = false;
        }

        if (tls_io_instance->received_bytes != NULL)
        {
            free(tls_io_instance->received_bytes);
        }

        xio_destroy(tls_io_instance->socket_io);
        free(tls_io_instance->host_name);
        free(tls_io);
    }
}

int tlsio_schannel_open(CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    if (tls_io == NULL)
    {
        result = __LINE__;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN)
        {
            result = __LINE__;
        }
        else
        {
            tls_io_instance->on_io_open_complete = on_io_open_complete;
            tls_io_instance->on_io_open_complete_context = on_io_open_complete_context;

            tls_io_instance->on_bytes_received = on_bytes_received;
            tls_io_instance->on_bytes_received_context = on_bytes_received_context;

            tls_io_instance->on_io_error = on_io_error;
            tls_io_instance->on_io_error_context = on_io_error_context;

            tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_UNDERLYING_IO;

            if (xio_open(tls_io_instance->socket_io, on_underlying_io_open_complete, tls_io_instance, on_underlying_io_bytes_received, tls_io_instance, on_underlying_io_error, tls_io_instance) != 0)
            {
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

int tlsio_schannel_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result = 0;

    if (tls_io == NULL)
    {
        result = __LINE__;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if ((tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN) ||
            (tls_io_instance->tlsio_state == TLSIO_STATE_CLOSING))
        {
            result = __LINE__;
        }
        else
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_CLOSING;
            tls_io_instance->on_io_close_complete = on_io_close_complete;
            tls_io_instance->on_io_close_complete_context = callback_context;
            if (xio_close(tls_io_instance->socket_io, on_underlying_io_close_complete, tls_io_instance) != 0)
            {
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

static int send_chunk(CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    if ((tls_io == NULL) ||
        (buffer == NULL) ||
        (size == 0))
    {
        /* Invalid arguments */
        result = __LINE__;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN)
        {
            result = __LINE__;
        }
        else
        {
            SecPkgContext_StreamSizes  sizes;
            SECURITY_STATUS status = QueryContextAttributes(&tls_io_instance->security_context, SECPKG_ATTR_STREAM_SIZES, &sizes);
            if (status != SEC_E_OK)
            {
                result = __LINE__;
            }
            else
            {
                SecBuffer security_buffers[4];
                SecBufferDesc security_buffers_desc;
                size_t needed_buffer = sizes.cbHeader + size + sizes.cbTrailer;
                unsigned char* out_buffer = (unsigned char*)malloc(needed_buffer);
                if (out_buffer == NULL)
                {
                    result = __LINE__;
                }
                else
                {
                    memcpy(out_buffer + sizes.cbHeader, buffer, size);

                    security_buffers[0].BufferType = SECBUFFER_STREAM_HEADER;
                    security_buffers[0].cbBuffer = sizes.cbHeader;
                    security_buffers[0].pvBuffer = out_buffer;
                    security_buffers[1].BufferType = SECBUFFER_DATA;
                    security_buffers[1].cbBuffer = (int)size;
                    security_buffers[1].pvBuffer = out_buffer + sizes.cbHeader;
                    security_buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
                    security_buffers[2].cbBuffer = sizes.cbTrailer;
                    security_buffers[2].pvBuffer = out_buffer + sizes.cbHeader + size;
                    security_buffers[3].cbBuffer = 0;
                    security_buffers[3].BufferType = SECBUFFER_EMPTY;
                    security_buffers[3].pvBuffer = 0;

                    security_buffers_desc.cBuffers = sizeof(security_buffers) / sizeof(security_buffers[0]);
                    security_buffers_desc.pBuffers = security_buffers;
                    security_buffers_desc.ulVersion = SECBUFFER_VERSION;

                    status = EncryptMessage(&tls_io_instance->security_context, 0, &security_buffers_desc, 0);
                    if (FAILED(status))
                    {
                        result = __LINE__;
                    }
                    else
                    {
                        if (xio_send(tls_io_instance->socket_io, out_buffer, security_buffers[0].cbBuffer + security_buffers[1].cbBuffer + security_buffers[2].cbBuffer, on_send_complete, callback_context) != 0)
                        {
                            result = __LINE__;
                        }
                        else
                        {
                            result = 0;
                        }
                    }

                    free(out_buffer);
                }
            }
        }
    }

    return result;
}

int tlsio_schannel_send(CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    while (size > 0)
    {
        size_t to_send = 16 * 1024;
        if (to_send > size)
        {
            to_send = size;
        }

        if (send_chunk(tls_io, buffer, to_send, (to_send == size) ? on_send_complete : NULL, callback_context) != 0)
        {
            break;
        }

        size -= to_send;
        buffer = ((const unsigned char*)buffer) + to_send;
    }

    if (size > 0)
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
    }

    return result;
}

void tlsio_schannel_dowork(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io != NULL)
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        xio_dowork(tls_io_instance->socket_io);
    }
}

int tlsio_schannel_setoption(CONCRETE_IO_HANDLE tls_io, const char* optionName, const void* value)
{
    int result;

    if (tls_io == NULL || optionName == NULL)
    {
        result = __LINE__;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (tls_io_instance->socket_io == NULL)
        {
            result = __LINE__;
        }
        else
        {
            result = xio_setoption(tls_io_instance->socket_io, optionName, value);
        }
    }

    return result;
}

const IO_INTERFACE_DESCRIPTION* tlsio_schannel_get_interface_description(void)
{
    return &tlsio_schannel_interface_description;
}

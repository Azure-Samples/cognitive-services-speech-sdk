// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stddef.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mstcpip.h>
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/list.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"

typedef enum IO_STATE_TAG
{
    IO_STATE_CLOSED,
    IO_STATE_OPENING,
    IO_STATE_OPEN,
    IO_STATE_CLOSING
} IO_STATE;

typedef struct PENDING_SOCKET_IO_TAG
{
    unsigned char* bytes;
    size_t size;
    ON_SEND_COMPLETE on_send_complete;
    void* callback_context;
    LIST_HANDLE pending_io_list;
} PENDING_SOCKET_IO;

typedef struct SOCKET_IO_INSTANCE_TAG
{
    SOCKET socket;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    void* on_bytes_received_context;
    void* on_io_error_context;
    char* hostname;
    int port;
    IO_STATE io_state;
    LIST_HANDLE pending_io_list;
    struct tcp_keepalive keep_alive;
} SOCKET_IO_INSTANCE;

static const IO_INTERFACE_DESCRIPTION socket_io_interface_description =
{
    socketio_create,
    socketio_destroy,
    socketio_open,
    socketio_close,
    socketio_send,
    socketio_dowork,
    socketio_setoption
};

static void indicate_error(SOCKET_IO_INSTANCE* socket_io_instance)
{
    if (socket_io_instance->on_io_error != NULL)
    {
        socket_io_instance->on_io_error(socket_io_instance->on_io_error_context);
    }
}

static int add_pending_io(SOCKET_IO_INSTANCE* socket_io_instance, const unsigned char* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;
    PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)malloc(sizeof(PENDING_SOCKET_IO));
    if (pending_socket_io == NULL)
    {
        result = __LINE__;
    }
    else
    {
        pending_socket_io->bytes = (unsigned char*)malloc(size);
        if (pending_socket_io->bytes == NULL)
        {
            LogError("Allocation Failure: Unable to allocate pending list.");
            free(pending_socket_io);
            result = __LINE__;
        }
        else
        {
            pending_socket_io->size = size;
            pending_socket_io->on_send_complete = on_send_complete;
            pending_socket_io->callback_context = callback_context;
            pending_socket_io->pending_io_list = socket_io_instance->pending_io_list;
            (void)memcpy(pending_socket_io->bytes, buffer, size);

            if (list_add(socket_io_instance->pending_io_list, pending_socket_io) == NULL)
            {
                LogError("Failure: Unable to add socket to pending list.");
                free(pending_socket_io->bytes);
                free(pending_socket_io);
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

CONCRETE_IO_HANDLE socketio_create(void* io_create_parameters)
{
    SOCKETIO_CONFIG* socket_io_config = io_create_parameters;
    SOCKET_IO_INSTANCE* result;

    if (socket_io_config == NULL)
    {
        LogError("Invalid argument: socket_io_config is NULL");
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(SOCKET_IO_INSTANCE));
        if (result != NULL)
        {
            result->pending_io_list = list_create();
            if (result->pending_io_list == NULL)
            {
                LogError("Failure: list_create unable to create pending list.");
                free(result);
                result = NULL;
            }
            else
            {
                if (socket_io_config->hostname != NULL)
                {
					size_t hostnamelen = strlen(socket_io_config->hostname) + 1;
                    result->hostname = (char*)malloc(hostnamelen);
                    if (result->hostname != NULL)
                    {
                        (void)strcpy_s(result->hostname, hostnamelen, socket_io_config->hostname);
                    }

                    result->socket = INVALID_SOCKET;
                }
                else
                {
                    result->hostname = NULL;
                    result->socket = *((SOCKET*)socket_io_config->accepted_socket);
                }

                if ((result->hostname == NULL) && (result->socket == INVALID_SOCKET))
                {
                    LogError("Failure: hostname == NULL and socket is invalid.");
                    list_destroy(result->pending_io_list);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->port = socket_io_config->port;
                    result->on_bytes_received = NULL;
                    result->on_io_error = NULL;
                    result->on_bytes_received_context = NULL;
                    result->on_io_error_context = NULL;
                    result->io_state = IO_STATE_CLOSED;
                    result->keep_alive = (struct tcp_keepalive) { 0, 0, 0 };
                }
            }
        }
        else
        {
            LogError("Allocation Failure: SOCKET_IO_INSTANCE");
        }
    }

    return (XIO_HANDLE)result;
}

void socketio_destroy(CONCRETE_IO_HANDLE socket_io)
{
    if (socket_io != NULL)
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        /* we cannot do much if the close fails, so just ignore the result */
        (void)closesocket(socket_io_instance->socket);

        /* clear allpending IOs */
        LIST_ITEM_HANDLE first_pending_io;
        while ((first_pending_io = list_get_head_item(socket_io_instance->pending_io_list)) != NULL)
        {
            PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)list_item_get_value(first_pending_io);
            if (pending_socket_io != NULL)
            {
                free(pending_socket_io->bytes);
                free(pending_socket_io);
            }

            list_remove(socket_io_instance->pending_io_list, first_pending_io);
        }

        list_destroy(socket_io_instance->pending_io_list);
        if (socket_io_instance->hostname != NULL)
        {
            free(socket_io_instance->hostname);
        }

        free(socket_io);
    }
}

int socketio_open(CONCRETE_IO_HANDLE socket_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
    if (socket_io == NULL)
    {
        LogError("Invalid argument: SOCKET_IO_INSTANCE is NULL");
        result = __LINE__;
    }
    else
    {
        if (socket_io_instance->io_state != IO_STATE_CLOSED)
        {
            LogError("Failure: socket state is not closed.");
            result = __LINE__;
        }
        else if (socket_io_instance->socket != INVALID_SOCKET)
        {
            // Opening an accepted socket
            socket_io_instance->on_bytes_received_context = on_bytes_received_context;
            socket_io_instance->on_bytes_received = on_bytes_received;
            socket_io_instance->on_io_error = on_io_error;
            socket_io_instance->on_io_error_context = on_io_error_context;

            socket_io_instance->io_state = IO_STATE_OPEN;

            if (on_io_open_complete != NULL)
            {
                on_io_open_complete(on_io_open_complete_context, IO_OPEN_OK);
            }
            result = 0;
        }
        else
        {
            socket_io_instance->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (socket_io_instance->socket == INVALID_SOCKET)
            {
                LogError("Failure: socket create failure %d.", WSAGetLastError());
                result = __LINE__;
            }
            else
            {
                char portString[16];
                ADDRINFO addrHint = { 0 };
                ADDRINFO* addrInfo = NULL;

                addrHint.ai_family = AF_INET;
                addrHint.ai_socktype = SOCK_STREAM;
                addrHint.ai_protocol = 0;
                sprintf_s(portString, sizeof(portString), "%u", socket_io_instance->port);
                if (getaddrinfo(socket_io_instance->hostname, portString, &addrHint, &addrInfo) != 0)
                {
                    LogError("Failure: getaddrinfo failure %d.", WSAGetLastError());
                    (void)closesocket(socket_io_instance->socket);
                    socket_io_instance->socket = INVALID_SOCKET;
                    result = __LINE__;
                }
                else
                {
                    u_long iMode = 1;

                    if (connect(socket_io_instance->socket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen) != 0)
                    {
                        LogError("Failure: connect failure %d.", WSAGetLastError());
                        (void)closesocket(socket_io_instance->socket);
                        socket_io_instance->socket = INVALID_SOCKET;
                        result = __LINE__;
                    }
                    else if (ioctlsocket(socket_io_instance->socket, FIONBIO, &iMode) != 0)
                    {
                        LogError("Failure: ioctlsocket failure %d.", WSAGetLastError());
                        (void)closesocket(socket_io_instance->socket);
                        socket_io_instance->socket = INVALID_SOCKET;
                        result = __LINE__;
                    }
                    else
                    {
                        socket_io_instance->on_bytes_received = on_bytes_received;
                        socket_io_instance->on_bytes_received_context = on_bytes_received_context;

                        socket_io_instance->on_io_error = on_io_error;
                        socket_io_instance->on_io_error_context = on_io_error_context;

                        socket_io_instance->io_state = IO_STATE_OPEN;

                        if (on_io_open_complete != NULL)
                        {
                            on_io_open_complete(on_io_open_complete_context, IO_OPEN_OK);
                        }

                        result = 0;
                    }

                    freeaddrinfo(addrInfo);
                }
            }
        }
    }

    return result;
}

int socketio_close(CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    if (socket_io == NULL)
    {
        LogError("Invalid argument: socket_io is NULL");
        result = __LINE__;
    }
    else
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;

        if ((socket_io_instance->io_state != IO_STATE_CLOSING) &&
            (socket_io_instance->io_state != IO_STATE_CLOSED))
        {
            (void)closesocket(socket_io_instance->socket);
            socket_io_instance->socket = INVALID_SOCKET;
            socket_io_instance->io_state = IO_STATE_CLOSED;
        }
        if (on_io_close_complete != NULL)
        {
            on_io_close_complete(callback_context);
        }
        result = 0;
    }

    return result;
}

int socketio_send(CONCRETE_IO_HANDLE socket_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    if ((socket_io == NULL) ||
        (buffer == NULL) ||
        (size == 0))
    {
        /* Invalid arguments */
        LogError("Invalid argument: send given invalid parameter");
        result = __LINE__;
    }
    else
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        if (socket_io_instance->io_state != IO_STATE_OPEN)
        {
            LogError("Failure: socket state is not opened.");
            result = __LINE__;
        }
        else
        {
            LIST_ITEM_HANDLE first_pending_io = list_get_head_item(socket_io_instance->pending_io_list);
            if (first_pending_io != NULL)
            {
                if (add_pending_io(socket_io_instance, buffer, size, on_send_complete, callback_context) != 0)
                {
                    LogError("Failure: add_pending_io failed.");
                    result = __LINE__;
                }
                else
                {
                    result = 0;
                }
            }
            else
            {
                int send_result = send(socket_io_instance->socket, buffer, (int)size, 0);
                if (send_result != (int)size)
                {
                    int last_error = WSAGetLastError();
                    if (last_error != WSAEWOULDBLOCK)
                    {
                        indicate_error(socket_io_instance);
                        LogError("Failure: sending socket failed %d.", last_error);
                        result = __LINE__;
                    }
                    else
                    {
                        /* queue data */
                        if (add_pending_io(socket_io_instance, buffer, size, on_send_complete, callback_context) != 0)
                        {
                            LogError("Failure: add_pending_io failed.");
                            result = __LINE__;
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
                else
                {
                    if (on_send_complete != NULL)
                    {
                        on_send_complete(callback_context, IO_SEND_OK);
                    }

                    result = 0;
                }
            }
        }
    }

    return result;
}

void socketio_dowork(CONCRETE_IO_HANDLE socket_io)
{
    if (socket_io != NULL)
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        if (socket_io_instance->io_state == IO_STATE_OPEN)
        {
            int received = 1;

            LIST_ITEM_HANDLE first_pending_io = list_get_head_item(socket_io_instance->pending_io_list);
            while (first_pending_io != NULL)
            {
                PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)list_item_get_value(first_pending_io);
                if (pending_socket_io == NULL)
                {
                    LogError("Failure: retrieving socket from list");
                    indicate_error(socket_io_instance);
                    break;
                }

                int send_result = send(socket_io_instance->socket, (char*)pending_socket_io->bytes, (int)pending_socket_io->size, 0);
                if (send_result != (int)pending_socket_io->size)
                {
                    int last_error = WSAGetLastError();
                    if (last_error != WSAEWOULDBLOCK)
                    {
                        free(pending_socket_io->bytes);
                        free(pending_socket_io);
                        (void)list_remove(socket_io_instance->pending_io_list, first_pending_io);
                    }
                    else
                    {
                        /* try again */
                    }
                }
                else
                {
                    if (pending_socket_io->on_send_complete != NULL)
                    {
                        pending_socket_io->on_send_complete(pending_socket_io->callback_context, IO_SEND_OK);
                    }

                    free(pending_socket_io->bytes);
                    free(pending_socket_io);
                    if (list_remove(socket_io_instance->pending_io_list, first_pending_io) != 0)
                    {
                        LogError("Failure: removing socket from list");
                        indicate_error(socket_io_instance);
                    }
                }

                first_pending_io = list_get_head_item(socket_io_instance->pending_io_list);
            }

            while (received > 0)
            {
                unsigned char* recv_bytes = malloc(RECEIVE_BYTES_VALUE);
                if (recv_bytes == NULL)
                {
                    LogError("Socketio_Failure: NULL allocating input buffer.");
                    indicate_error(socket_io_instance);
                }
                else
                {
                    received = recv(socket_io_instance->socket, (char*)recv_bytes, RECEIVE_BYTES_VALUE, 0);
                    if (received > 0)
                    {
                        if (socket_io_instance->on_bytes_received != NULL)
                        {
                            /* explictly ignoring here the result of the callback */
                            (void)socket_io_instance->on_bytes_received(socket_io_instance->on_bytes_received_context, recv_bytes, received);
                        }
                    }
                    else
                    {
                        int last_error = WSAGetLastError();
                        if (last_error != WSAEWOULDBLOCK)
                        {
                            LogError("Socketio_Failure: Recieving data from endpoint: %d.", last_error);
                            indicate_error(socket_io_instance);
                        }
                    }
                    free(recv_bytes);
                }
            }
        }
    }
}

static int set_keepalive(SOCKET_IO_INSTANCE* socket_io, struct tcp_keepalive* keepAlive)
{
    int result;
    DWORD bytesReturned;

    int err = WSAIoctl(socket_io->socket, SIO_KEEPALIVE_VALS, keepAlive,
        sizeof(struct tcp_keepalive), NULL, 0, &bytesReturned, NULL, NULL);
    if (err != 0)
    {
        LogError("Failure: setting keep-alive on the socket: %d.\r\n", err == SOCKET_ERROR ? WSAGetLastError() : err);
        result = __LINE__;
    }
    else
    {
        socket_io->keep_alive = *keepAlive;
        result = 0;
    }

    return result;
}

int socketio_setoption(CONCRETE_IO_HANDLE socket_io, const char* optionName, const void* value)
{
    int result;

    if (socket_io == NULL ||
        optionName == NULL ||
        value == NULL)
    {
        result = __LINE__;
    }
    else
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;

        if (strcmp(optionName, "tcp_keepalive") == 0)
        {
            struct tcp_keepalive keepAlive = socket_io_instance->keep_alive;
            keepAlive.onoff = *(int *)value;

            result = set_keepalive(socket_io_instance, &keepAlive);
        }
        else if (strcmp(optionName, "tcp_keepalive_time") == 0)
        {
            unsigned long kaTime = *(int *)value * 1000; // convert to ms
            struct tcp_keepalive keepAlive = socket_io_instance->keep_alive;
            keepAlive.keepalivetime = kaTime;

            result = set_keepalive(socket_io_instance, &keepAlive);
        }
        else if (strcmp(optionName, "tcp_keepalive_interval") == 0)
        {
            unsigned long kaInterval = *(int *)value * 1000; // convert to ms
            struct tcp_keepalive keepAlive = socket_io_instance->keep_alive;
            keepAlive.keepaliveinterval = kaInterval;

            result = set_keepalive(socket_io_instance, &keepAlive);
        }
        else
        {
            result = __LINE__;
        }
    }

    return result;
}

const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description(void)
{
    return &socket_io_interface_description;
}

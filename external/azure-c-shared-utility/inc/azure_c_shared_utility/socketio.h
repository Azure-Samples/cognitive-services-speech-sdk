// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SOCKETIO_H
#define SOCKETIO_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif /* __cplusplus */

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/umock_c_prod.h"

typedef struct SOCKETIO_CONFIG_TAG
{
    const char* hostname;
    int port;
    void* accepted_socket;
} SOCKETIO_CONFIG;

#define RECEIVE_BYTES_VALUE     64

MOCKABLE_FUNCTION(, CONCRETE_IO_HANDLE, socketio_create, void*, io_create_parameters);
MOCKABLE_FUNCTION(, void, socketio_destroy, CONCRETE_IO_HANDLE, socket_io);
MOCKABLE_FUNCTION(, int, socketio_open, CONCRETE_IO_HANDLE, socket_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, void*, on_io_open_complete_context, ON_BYTES_RECEIVED, on_bytes_received, void*, on_bytes_received_context, ON_IO_ERROR, on_io_error, void*, on_io_error_context);
MOCKABLE_FUNCTION(, int, socketio_close, CONCRETE_IO_HANDLE, socket_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
MOCKABLE_FUNCTION(, int, socketio_send, CONCRETE_IO_HANDLE, socket_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
MOCKABLE_FUNCTION(, void, socketio_dowork, CONCRETE_IO_HANDLE, socket_io);
MOCKABLE_FUNCTION(, int, socketio_setoption, CONCRETE_IO_HANDLE, socket_io, const char*, optionName, const void*, value);

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, socketio_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SOCKETIO_H */

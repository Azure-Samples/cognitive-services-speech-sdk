// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef WSIO_H
#define WSIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/xio.h"

typedef struct WSIO_CONFIG_TAG
{
	const char* host;
	int port;
	const char* protocol_name;
	const char* relative_path;
	const char* trusted_ca;
	int use_ssl;
} WSIO_CONFIG;

typedef enum _WSIO_MSG_TYPE
{
    WSIO_MSG_TYPE_TEXT,
    WSIO_MSG_TYPE_BINARY,
} WSIO_MSG_TYPE;

typedef void(*ON_WSIO_BYTES_RECEIVED)(void* context, const unsigned char* buffer, size_t size, WSIO_MSG_TYPE message_type);

extern CONCRETE_IO_HANDLE wsio_create(void* io_create_parameters);
extern void wsio_destroy(CONCRETE_IO_HANDLE ws_io);
extern int wsio_open(CONCRETE_IO_HANDLE ws_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_WSIO_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context);
extern int wsio_close(CONCRETE_IO_HANDLE ws_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context);
extern int wsio_send(CONCRETE_IO_HANDLE ws_io, const void* buffer, size_t size, WSIO_MSG_TYPE message_type, ON_SEND_COMPLETE on_send_complete, void* callback_context);
extern void wsio_dowork(CONCRETE_IO_HANDLE ws_io);
extern int wsio_setoption(CONCRETE_IO_HANDLE socket_io, const char* optionName, const void* value);
extern int wsio_gethttpstatus(CONCRETE_IO_HANDLE socket_io);

extern const IO_INTERFACE_DESCRIPTION* wsio_get_interface_description(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WSIO_H */

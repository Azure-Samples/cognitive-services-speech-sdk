//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI connection_from_recognizer(SPXRECOHANDLE recognizerHandle, SPXCONNECTIONHANDLE* connectionHandle);

SPXAPI_(bool) connection_handle_is_valid(SPXCONNECTIONHANDLE handle);
SPXAPI connection_handle_release(SPXCONNECTIONHANDLE handle);

SPXAPI connection_open(SPXCONNECTIONHANDLE handle, bool forContinuousRecognition);
SPXAPI connection_close(SPXCONNECTIONHANDLE handle);
SPXAPI connection_set_message_property(SPXCONNECTIONHANDLE handle, const char* path, const char* name, const char* value);
SPXAPI connection_send_message(SPXCONNECTIONHANDLE handle, const char* path, const char* payload);

typedef void(*CONNECTION_CALLBACK_FUNC)(SPXEVENTHANDLE event, void* context);
SPXAPI connection_connected_set_callback(SPXCONNECTIONHANDLE connection, CONNECTION_CALLBACK_FUNC callback, void* context);
SPXAPI connection_disconnected_set_callback(SPXCONNECTIONHANDLE connection, CONNECTION_CALLBACK_FUNC Callback, void* context);


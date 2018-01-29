//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usperror.h: defines error code in usplib.
//

#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#define USP_SUCCESS ((UspResult)0)

#define USP_ERRCODE(x) ((UspResult)(0x800f6000 | (x & 0x0fff)))

// Initialization errors.

/**
* USP_INITIALIZATION_FAILURE An error during USP initialization.
*/
#define USP_INITIALIZATION_FAILURE USP_ERRCODE(0x001)

/**
* USP_WRONG_STATE indicates the current operation cannot be performed in the current USP state.
*/
#define USP_WRONG_STATE USP_ERRCODE(0x002)

// Parameter errors.

/**
* USP_INVALID_HANDLE indicates that the USP handle is invalid.
*/
#define USP_INVALID_HANDLE USP_ERRCODE(0x010)

/**
* USP_CALLBACKS_NOT_SET indicates that the callbacks have not been set.
*/
#define USP_CALLBACKS_NOT_SET USP_ERRCODE(0x011)

/**
 * USP_INVALID_ARGUMENT indicates an invalid argument passed to a function.
*/
#define USP_INVALID_ARGUMENT USP_ERRCODE(0x012)

/**
* USP_BUFFER_TOO_SMALL indicates the buffer passed to the function is too small.
*/
#define USP_BUFFER_TOO_SMALL USP_ERRCODE(0x013)

// Authentication errors

/**
* USP_AUTH_TYPE_NOT_SUPPORTED The authentication type is not supported.
*/
#define USP_AUTH_TYPE_NOT_SUPPORTED USP_ERRCODE(0x020)

/**
* USP_AUTH_ERROR indicates an authentication error.
*/
#define USP_AUTH_ERROR USP_ERRCODE(0x021)

// Communication errors

/**
* USP_TRANSPORT_ERROR_GENERIC indicates an error during network communication.
*/
#define USP_TRANSPORT_ERROR_GENERIC USP_ERRCODE(0x030)

/**
* USP_CONNECTION_FAILURE indicates connection failures.
*/
#define USP_CONNECTION_FAILURE USP_ERRCODE(0x031)

/**
* USP_CONNECTION_TIMEOUT_ERROR indicates timeout of a network connection.
*/
#define USP_CONNECTION_TIMEOUT_ERROR USP_ERRCODE(0x032)

/**
* USP_CONNECTION_REMOTE_CLOSED indicates the service closed the connection.
*/
#define USP_CONNECTION_REMOTE_CLOSED USP_ERRCODE(0x033)

/**
* USP_WRITE_ERROR indicates an error when calling UspWrite().
*/
#define USP_WRITE_ERROR USP_ERRCODE(0x034)

// Response errors

/**
* USP_UNKNOWN_PATH_IN_RESPONSE indicates that the message path is unknown.
*/
#define USP_UNKNOWN_PATH_IN_RESPONSE USP_ERRCODE(0x040)

/**
* USP_INVALID_RESPONSE indicates that a message does not conform to the protocol specification.
*/
#define USP_INVALID_RESPONSE USP_ERRCODE(0x041)

// Service errors

/**
* USP_UNKNOWN_SERVICE_ENDPOINT indicates that the service endpoint is invalid.
*/
#define USP_UNKNOWN_SERVICE_ENDPOINT USP_ERRCODE(0x050)

/**
* USP_UNKNOWN_RECOGNITION_MODE indicates that the recognition mode is invalid.
*/
#define USP_UNKNOWN_RECOGNITION_MODE USP_ERRCODE(0x051)

/**
* USP_NOT_SUPPORTED_OPTION indicates that the configuration option is not supported by the service being used.
*/
#define USP_NOT_SUPPORTED_OPTION USP_ERRCODE(0x052)

// Runtime errors

/**
* USP_OUT_OF_MEMORY indicates the method failed to allocate the necessary memory.
*/
#define USP_OUT_OF_MEMORY USP_ERRCODE(0xffd)

/**
* USP_RUNTIME_ERROR indicates unexpected error during runtime.
*/
#define USP_RUNTIME_ERROR USP_ERRCODE(0xffe)

/**
* USP_NOT_IMPLEMENTED indicates that the required functionality is not implemented yet.
*/
#define USP_NOT_IMPLEMENTED USP_ERRCODE(0xfff)

#ifdef __cplusplus
}
#endif
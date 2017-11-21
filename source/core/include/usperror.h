//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usperror.h: defines error code in usplib.
//

#pragma once

#define USP_SUCCESS ((UspResult)0)

#define USP_ERRCODE(x) (0x800f6000 | (x & 0x0fff))

/**
 * USP_NOT_IMPLEMENTED USP_ERRCODE indicates the required functionality is not implemented yet.
*/
#define USP_NOT_IMPLEMENTED USP_ERRCODE(0xfff)

/**
 * USP_UNINTIALIZED inidcates the USP handle is not initialized yet. 
*/
#define USP_UNINTIALIZED USP_ERRCODE(0x001)

/**
* USP_ALREADY_INTIALIZED indicates the USP handle has already been initialized.
*/
#define USP_ALREADY_INTIALIZED USP_ERRCODE(0x002)

/**
* USP_INITIALIZATION_FAILURE indicates an error during USP initialization.
*/
#define USP_INITIALIZATION_FAILURE USP_ERRCODE(0x003)

/**
* USP_INVALID_HANDLE indicates the USP handle is invalid.
*/
#define USP_INVALID_HANDLE USP_ERRCODE(0x004)

/**
 * USP_INVALID_PARAMETER indicates a parameter for a call is invalid.
*/
#define USP_INVALID_PARAMETER USP_ERRCODE(0x005)

/**
 * USP_INVALID_DATA indicates data is invalid.
*/
#define USP_INVALID_DATA USP_ERRCODE(0x006)

/**
* USP_WRITE_ERROR indicates an error when calling UspWrite().
*/
#define USP_WRITE_ERROR USP_ERRCODE(0x010)

/**
* USP_TRANSPORT_ERROR_GENERIC indicates an error during network communication.
*/
#define USP_TRANSPORT_ERROR_GENERIC USP_ERRCODE(0x020)

/**
* USP_AUTH_ERROR indicates an authentication error.
*/
#define USP_AUTH_ERROR USP_ERRCODE(0x021)

/**
* USP_NO_CONNECTION indicates no network connection to the service.
*/
#define USP_NO_CONNECTION_ERROR USP_ERRCODE(0x022)

/**
* USP_CONNECTION_TIMEOUT_ERROR indicates timeout of a network connection.
*/
#define USP_CONNECTION_TIMEOUT_ERROR USP_ERRCODE(0x023)


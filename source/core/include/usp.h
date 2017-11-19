//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp.h: the header for usplib
//

#pragma once

#include <stdint.h>

#include "uspmessages.h"


#define USP_VERSION 1.0

/**
 * The UspHandle represents an opaque handle used by usplib.
 */
typedef void * UspHandle;

/**
  * The UspResult represents the result of a function call of usplib.
*/
typedef int UspResult;

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



/**
 * The UspOnSpeechStartDetectedCallback represents an application-defined callback function
 * used for signaling a speech.startDetected message.
 * @param handle The UspHandle.
 * @param context A pointer to the application-defined callback context.
 * @param message A pointer to the speech.startDetected message.
*/
typedef void(*UspOnSpeechStartDetectedCallback)(UspHandle handle, void* context, UspMsgSpeechStartDetected *message);

/**
* The UspOnSpeechEndDetectedCallback represents an application-defined callback function
* used for signaling a speech.endDetected message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.endDetected message.
*/
typedef void(*UspOnSpeechEndDetectedCallback)(UspHandle handle, void* context, UspMsgSpeechEndDetected *message);

/**
* The UspOnSpeechHypothesisCallback represents an application-defined callback function
* used for signaling a speech.hypothesis message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.hypothesis message.
*/
typedef void(*UspOnSpeechHypothesisCallback)(UspHandle handle, void* context, UspMsgSpeechHypothesis *message);

/**
* The UspOnSpeechPhraseCallback represents an application-defined callback function
* used for signaling a speech.phrase message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.phrase message.
*/
typedef void(*UspOnSpeechPhraseCallback)(UspHandle handle, void* context, UspMsgSpeechPhrase *message);

/**
* The UspOnTurnStartCallback represents an application-defined callback function
* used for signaling a turn.start message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.start message.
*/
typedef void(*UspOnTurnStartCallback)(UspHandle handle, void* context, UspMsgTurnStart *message);

/**
* The UspOnTurnEndDetectedCallback represents an application-defined callback function
* used for signaling a turn.end message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.end message.
*/
typedef void(*UspOnTurnEndCallback)(UspHandle handle, void* context, UspMsgTurnEnd *message);

/**
* The UspOnErrorCallback represents an application-defined callback function
* used for signaling the an error.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param error an error code.
*/
typedef void(*UspOnErrorCallback)(UspHandle handle, void* context, UspResult error);

/**
* The UspCallbacks type represents an application-defined
* structure used to register all usp events.
*/
typedef struct _UspCallbacks
{
    uint16_t size;
    uint16_t version;
    UspOnSpeechStartDetectedCallback onSpeechStartDetected;
    UspOnSpeechEndDetectedCallback onSpeechEndDetected;
    UspOnSpeechHypothesisCallback onSpeechHypothesis;
    UspOnSpeechPhraseCallback onSpeechPhrase;
    UspOnTurnStartCallback onTurnStart;
    UspOnTurnEndCallback onTurnEnd;
    UspOnErrorCallback OnError;
} UspCallbacks;


UspResult UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext);

UspResult UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);

UspResult UspShutdown(UspHandle handle);

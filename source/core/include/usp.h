//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp.h: the header file includes types, methods and error code exposed by usplib
//

#pragma once

#include <stdint.h>

#include "uspmessages.h"

/**
 * The UspHandle represents an opaque handle used by usplib.
 */
typedef void * UspHandle;

/**
 * The UspError represents error codes.
 */
typedef unsigned int UspError;

#define USP_SUCCESS ((UspError)0)

#define USP_ERRCODE(x) (0x800f6000 | (x & 0x0fff))

#define USP_NOT_IMPLEMENTED USP_ERRCODE(0xfff)

/**
 * The UspOnSpeechStartDetected represents an application-defined callback function
 * used for signaling a speech.startDetected message.
 * @param handle The UspHandle.
 * @param context A pointer to the application-defined callback context.
 * @param message A pointer to the speech.startDetected message.
*/
typedef void(*UspOnSpeechStartDetected)(UspHandle handle, void* context, UspMsgSpeechStartDetected *message);

/**
* The UspOnSpeechEndDetected represents an application-defined callback function
* used for signaling a speech.endDetected message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.endDetected message.
*/
typedef void(*UspOnSpeechEndDetected)(UspHandle handle, void* context, UspMsgSpeechEndDetected *message);

/**
* The UspOnSpeechHypothesis represents an application-defined callback function
* used for signaling a speech.hypothesis message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.hypothesis message.
*/
typedef void(*UspOnSpeechHypothesis)(UspHandle handle, void* context, UspMsgSpeechHypothesis *message);

/**
* The UspOnSpeechPhrase represents an application-defined callback function
* used for signaling a speech.phrase message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.phrase message.
*/
typedef void(*UspOnSpeechPhrase)(UspHandle handle, void* context, UspMsgSpeechPhrase *message);

/**
* The UspOnTurnStart represents an application-defined callback function
* used for signaling a turn.start message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.start message.
*/
typedef void(*UspOnTurnStart)(UspHandle handle, void* context, UspMsgTurnStart *message);

/**
* The UspOnTurnEndDetected represents an application-defined callback function
* used for signaling a turn.end message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.end message.
*/
typedef void(*UspOnTurnEnd)(UspHandle handle, void* context, UspMsgTurnEnd *message);

/**
* The UspOnError represents an application-defined callback function
* used for signaling the an error.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param error an error code.
*/
typedef void(*UspOnError)(UspHandle handle, void* context, UspError error);


/**
* The UspCallbacks type represents an application-defined
* structure used to register all USP events.
*/
typedef struct _UspCallbacks
{
    uint16_t size;
    uint16_t version;
    UspOnSpeechStartDetected onSpeechStartDetected;
    UspOnSpeechEndDetected onSpeechEndDetected;
    UspOnSpeechHypothesis onSpeechHypothesis;
    UspOnSpeechPhrase onSpeechPhrase;
    UspOnTurnStart onTurnStart;
    UspOnTurnEnd onTurnEnd;
    UspOnError OnError;
} UspCallbacks;


int UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext);

int UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);

int UspShutdown(UspHandle handle);

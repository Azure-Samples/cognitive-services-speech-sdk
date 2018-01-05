//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp.h: the header file includes types, methods and error code exposed by usplib
//

#pragma once

#include <stdint.h>

#include "uspmessages.h"
#include "usperror.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USP_VERSION 1.0

/**
 * The UspHandle represents an opaque handle used by usplib.
 */
typedef void * UspHandle;

/**
 * The UspResult represents the result of a function call of usplib.
*/
typedef int UspResult;

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
typedef void(*UspOnError)(UspHandle handle, void* context, UspResult error);


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


/**
* Opens and initializes a UspHandle.
* @param handle The pointer to UspHandle. If the function returns USP_SUCCESS, the value pointered by handle is the initialzed UspHandle that is ready for use.
* @param callbacks The struct contains callbacks that will be called when vairous USP events.
* @param bacllbackContext A pointer to the caller provided data, which will be passed as paraemter of the callback.
*/
UspResult UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext);

/**
* Sends data to the handle.
* @param handle The UspHandle to which the data is sent.
* @param buffer The buffer contains data to be sent.
* @param byteToWrite The amount of data in bytes to be sent.
*/
UspResult UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);

/**
* Closes the handle.
* @param handle The UspHandle to be closed.
*/
UspResult UspShutdown(UspHandle handle);

/**
* Runs the event loop.
* @param handle The UspHandle.
*/
void UspRun(UspHandle handle);

#ifdef __cplusplus
}
#endif
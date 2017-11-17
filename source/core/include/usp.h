//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp.h: the header for usplib
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
* The PUspOnSpeechHypothesisCallback represents an application-defined callback function
* used for signaling a speech.hypothesis message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.hypothesis message.
*/
typedef void(*UspOnSpeechHypothesisCallback)(UspHandle handle, void* context, UspMsgSpeechHypothesis *message);

/**
* The PUspOnSpeechPhraseCallback represents an application-defined callback function
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
typedef void(*UspOnErrorCallback)(UspHandle handle, void* context, UspError error);


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


int UspInitialize(UspHandle* pHandle, UspCallbacks *pCallbacks, void* pContext);

int UspWrite(UspHandle handle, const uint8_t* pBuffer, size_t byteToWrite);
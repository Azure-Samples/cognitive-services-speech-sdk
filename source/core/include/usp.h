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
 * The USP_ERROR represents error codes.
 */
typedef unsigned int USP_ERROR;

#define USP_SUCCESS ((USP_ERROR)0)

#define _USP_ERRCODE(x) (0x800f6000 | (x & 0x0fff))

#define USP_NOT_IMPLEMENTED _USP_ERRCODE(0xfff)

/**
 * The PUspOnSpeechStartDetectedCallback represents an application-defined callback function
 * used for signaling a speech.startDetected message.
 * @param handle The UspHandle.
 * @param context A pointer to the application-defined callback context.
 * @param message A pointer to the speech.startDetected message.
*/
typedef void(*PUspOnSpeechStartDetectedCallback)(UspHandle handle, void* context, UspMsgSpeechStartDetected *message);

/**
* The PUspOnSpeechEndDetectedCallback represents an application-defined callback function
* used for signaling a speech.endDetected message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.endDetected message.
*/
typedef void(*PUspOnSpeechEndDetectedCallback)(UspHandle handle, void* context, UspMsgSpeechEndDetected *message);

/**
* The PUspOnSpeechHypothesisCallback represents an application-defined callback function
* used for signaling a speech.hypothesis message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.hypothesis message.
*/
typedef void(*PUspOnSpeechHypothesisCallback)(UspHandle handle, void* context, UspMsgSpeechHypothesis *message);

/**
* The PUspOnSpeechPhraseCallback represents an application-defined callback function
* used for signaling a speech.phrase message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the speech.phrase message.
*/
typedef void(*PUspOnSpeechPhraseCallback)(UspHandle handle, void* context, UspMsgSpeechPhrase *message);

/**
* The PUspOnTurnStartCallback represents an application-defined callback function
* used for signaling a turn.start message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.start message.
*/
typedef void(*PUspOnTurnStartCallback)(UspHandle handle, void* context, UspMsgTurnStart *message);

/**
* The PUspOnTurnEndDetectedCallback represents an application-defined callback function
* used for signaling a turn.end message.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param message A pointer to the turn.end message.
*/
typedef void(*PUspOnTurnEndCallback)(UspHandle handle, void* context, UspMsgTurnEnd *message);

/**
* The PUspOnErrorCallback represents an application-defined callback function
* used for signaling the an error.
* @param handle The UspHandle.
* @param context A pointer to the application-defined callback context.
* @param error an error code.
*/
typedef void(*PUspOnErrorCallback)(UspHandle handle, void* context, USP_ERROR error);


/**
* The UspCallbacks type represents an application-defined
* structure used to register all various usp events.
*/
typedef struct _UspCallbacks
{
    uint16_t size;
    uint16_t version;
    PUspOnSpeechStartDetectedCallback onSpeechStartDetected;
    PUspOnSpeechEndDetectedCallback onSpeechEndDetected;
    PUspOnSpeechHypothesisCallback onSpeechHypothesis;
    PUspOnSpeechPhraseCallback onSpeechPhrase;
    PUspOnTurnStartCallback onTurnStart;
    PUspOnTurnEndCallback onTurnEnd;
    PUspOnErrorCallback OnError;
} UspCallbacks;


int UspInitialize(UspHandle* pHandle, UspCallbacks *pCallbacks, void* pContext);

int UspWrite(UspHandle handle, const uint8_t* pBuffer, size_t byteToWrite);
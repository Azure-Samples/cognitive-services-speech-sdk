//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspmessages.h: definition of USP messages that are exposed to users.
//
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "iobuffer.h"
#include <stdint.h>
#include <wchar.h>

typedef uint64_t UspOffsetType;
typedef uint64_t UspDurationType;

/**
 * Represents recognition status in speech phrase.
 */
typedef enum _UspRecognitionStatus
{
    USP_RECOGNITON_SUCCESS,
    USP_RECOGNITION_NO_MATCH,
    USP_RECOGNITION_INITIAL_SILENCE_TIMEOUT,
    USP_RECOGNITION_BABBLE_TIMEOUT,
    USP_RECOGNITION_ERROR,
    USP_RECOGNITION_END_OF_DICTATION
} UspRecognitionStatus;

/**
 * Represents speech.startDectected message
 */
typedef struct _UspMsgSpeechStartDetected
{
    UspOffsetType offset;
} UspMsgSpeechStartDetected;

/**
 * Represents speech.hypothesis message
 */
typedef struct _UspMsgSpeechHypothesis
{
    wchar_t* text;
    UspOffsetType offset;
    UspDurationType duration;
} UspMsgSpeechHypothesis;

/**
 * Represents speech.phrase message
 */
typedef struct _UspMsgSpeechPhrase
{
    UspRecognitionStatus recognitionStatus;
    wchar_t* displayText;
    UspOffsetType offset;
    UspDurationType duration;
} UspMsgSpeechPhrase;

/**
* Represents speech.fragment message
*/
typedef struct _UspMsgSpeechFragment
{
    wchar_t* text;
    UspOffsetType offset;
    UspDurationType duration;
} UspMsgSpeechFragment;

/**
 * Represents speech.endDetected message
 */
typedef struct _UspMsgSpeechEndDetected
{
    UspOffsetType offset;
} UspMsgSpeechEndDetected;

/**
 * Represents that the start of an audio stream has been received.
 */
typedef struct _UspMsgAudioStreamStart
{
    // Whenever a chunk in the audio stream is received, it is appended to this ioBuffer.
    // Reading in a data chunk of size 0, or checking the ioBuffer->hasCompleted flag indicates
    // that the buffer has finished receiving new data.
    IOBUFFER* ioBuffer;
} UspMsgAudioStreamStart;

/**
 * Represents turn.start message
 */
typedef struct _UspMsgTurnStart
{
    wchar_t* contextServiceTag;
} UspMsgTurnStart;

/**
 * Represents turn.end message
 * Note: Body is empty.
 */
typedef struct _UspMsgTurnEnd UspMsgTurnEnd;

#ifdef __cplusplus
}
#endif
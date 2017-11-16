//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp.h: the header for usplib
//
#pragma once

#include <stdint.h>

typedef uint32_t UspOffsetType;
typedef uint32_t UspDurationType;

/**
 * Represents recognition status in speech phrase.
 */
typedef enum _UspRecognitionStatus
{
    USP_RECOGNITON_SUCCESS,
    USP_RECOGNITION_NO_MATCH,
    USP_RECOGNITION_INITIAL_SILENCE_TIMEOUT,
    USP_RECOGNITION_BABBLE_TIMEOUT,
    USP_RECOGNITION_ERROR
} UspRecognitionStatus;

/**
 * Represents speech.startDectected message
 */
typedef struct _UspMsgSpeechStartDetected
{
    UspOffsetType Offset;
} UspMsgSpeechStartDetected;

/**
 * Represents speech.hypothesis message
 */
typedef struct _UspMsgSpeechHypothesis
{
    wchar_t* Text;
    UspOffsetType Offset;
    UspDurationType Duration;
} UspMsgSpeechHypothesis;

/**
 * Represents speech.phrase message
 */
typedef struct _UspMsgSpeechPhrase
{
    UspRecognitionStatus RecognitionStatus;
    wchar_t* DisplayText;
    UspOffsetType Offset;
    UspDurationType Duration;
} UspMsgSpeechPhrase;

/**
 * Represents speech.endDetected message
 */
typedef struct _UspMsgSpeechEndDetected
{
    UspOffsetType Offset;
} UspMsgSpeechEndDetected;

/**
 * Represents turn.start message
 */
typedef struct _UspMsgTurnStart
{
    wchar_t* ContextServiceTag;
} UspMsgTurnStart;

/**
 * Represents turn.end message
 * Note: Body is empty.
 */
typedef struct _UspMsgTurnEnd UspMsgTurnEnd;


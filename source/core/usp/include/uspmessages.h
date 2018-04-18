//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspmessages.h: definition of USP messages that are exposed to users.
//
#pragma once

#include "iobuffer.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

namespace USP {

typedef uint64_t OffsetType;
typedef uint64_t DurationType;

/**
 * Represents recognition status in speech phrase.
 */
enum class RecognitionStatus : int
{
    Success, NoMatch, InitialSilenceTimeout, BabbleTimeout, Error, EndOfDictation, Unknown
};

/**
* Represents translation status in translation phrase.
*/
enum class TranslationStatus : int
{
    Success, Error, Unknown
};


/**
 * Represents speech.startDectected message
 */
struct SpeechStartDetectedMsg
{
    OffsetType offset { 0 };
};

/**
 * Represents speech.hypothesis message
 */
struct SpeechHypothesisMsg
{
    std::wstring text;
    OffsetType offset { 0 };
    DurationType duration { 0 };
};

/**
 * Represents speech.phrase message
 */
struct SpeechPhraseMsg
{
    RecognitionStatus recognitionStatus { RecognitionStatus::Error };
    std::wstring displayText;
    OffsetType offset { 0 };
    DurationType duration { 0 };
};

/**
* Represents speech.fragment message
*/
struct SpeechFragmentMsg
{
    std::wstring text;
    OffsetType offset { 0 };
    DurationType duration { 0 };
};

/**
 * Represents speech.endDetected message
 */
struct SpeechEndDetectedMsg
{
    OffsetType offset { 0 };
};

/**
 * Represents that the start of an audio stream has been received.
 */
struct AudioStreamStartMsg
{
    // Whenever a chunk in the audio stream is received, it is appended to this ioBuffer.
    // Reading in a data chunk of size 0, or checking the ioBuffer->hasCompleted flag indicates
    // that the buffer has finished receiving new data.
    IOBUFFER* ioBuffer { nullptr };
};

/**
 * Represents turn.start message
 */
struct TurnStartMsg
{
    std::string contextServiceTag;
};

/**
 * Represents turn.end message
 * Note: Body is empty.
 */
struct TurnEndMsg {};

/**
* Represents translation results.
*/
struct TranslationResult
{
    TranslationStatus translationStatus;
    // An array of value pair <targetLanguage, translationText>.
    std::map<std::wstring, std::wstring> translations;
};
/**
* Represents translation.hypothesis message
*/
struct TranslationHypothesisMsg
{
    OffsetType offset { 0 };
    DurationType duration { 0 };
    std::wstring text;
    TranslationResult translation;
};

/**
* Represents translation.phrase message
*/
struct TranslationPhraseMsg
{
    RecognitionStatus recognitionStatus;
    OffsetType offset {0};
    DurationType duration {0};
    std::wstring text;
    struct TranslationResult translation;
};

/**
* Represents translation.synthesis message
*/
struct TranslationSynthesisMsg
{
    std::shared_ptr<const uint8_t[]> audioBuffer;
    size_t audioLength { 0 };
};

}


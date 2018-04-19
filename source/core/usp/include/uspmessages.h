//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspmessages.h: definition of USP messages that are exposed to users.
//
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include <map>

#include "iobuffer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
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

struct JsonMsg {

    JsonMsg() = default;
    JsonMsg(const JsonMsg&) = default;

    std::wstring json;

protected:
    JsonMsg(std::wstring&& content) :
        json(std::move(content))
    {}
};

/**
 * Represents speech.startDectected message
 */
struct SpeechStartDetectedMsg : public JsonMsg
{
    SpeechStartDetectedMsg(std::wstring&& content, OffsetType offset) :
        JsonMsg(std::move(content)),
        offset(offset)
    {}

    OffsetType offset { 0 };
};

/**
* Represents speech.endDetected message
*/
struct SpeechEndDetectedMsg : public JsonMsg
{
    SpeechEndDetectedMsg(std::wstring&& content, OffsetType offset) :
        JsonMsg(std::move(content)),
        offset(offset)
    {}

    OffsetType offset{ 0 };
};

/**
* Represents turn.start message
*/
struct TurnStartMsg : public JsonMsg
{
    TurnStartMsg(std::wstring&& content, const std::string& tag) :
        JsonMsg(std::move(content)),
        contextServiceTag(tag)
    {
    }

    std::string contextServiceTag;
};

/**
* Represents turn.end message
* Note: Body is empty.
*/
struct TurnEndMsg : JsonMsg
{
    TurnEndMsg() :
        JsonMsg(std::wstring())
    {
    }
};

struct SpeechMsg : public JsonMsg
{
    SpeechMsg() = default;
    SpeechMsg(const SpeechMsg&) = default;
    SpeechMsg(std::wstring&& content, OffsetType offset, DurationType duration) :
        JsonMsg(std::move(content)),
        offset(offset),
        duration(duration)
    {}

    OffsetType offset{ 0 };
    DurationType duration{ 0 };
};

/**
 * Represents speech.hypothesis message
 */
struct SpeechHypothesisMsg : public SpeechMsg
{
    SpeechHypothesisMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring&& text) :
        SpeechMsg(std::move(content), offset, duration),
        text(std::move(text))
    {}

    std::wstring text;
};

/**
* Represents speech.fragment message
*/
struct SpeechFragmentMsg : public SpeechMsg
{
    SpeechFragmentMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring&& text) :
        SpeechMsg(std::move(content), offset, duration),
        text(std::move(text))
    {}

    std::wstring text;
};

/**
 * Represents speech.phrase message
 */
struct SpeechPhraseMsg : public SpeechMsg
{
    SpeechPhraseMsg() = default;
    SpeechPhraseMsg(const SpeechPhraseMsg&) = default;

    SpeechPhraseMsg(std::wstring&& content, OffsetType offset, DurationType duration, RecognitionStatus status, std::wstring&& text) :
        SpeechMsg(std::move(content), offset, duration),
        recognitionStatus(status),
        displayText(std::move(text))
    {}

    RecognitionStatus recognitionStatus { RecognitionStatus::Unknown };
    std::wstring displayText;
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
* Represents translation results.
*/
struct TranslationResult
{
    TranslationStatus translationStatus { TranslationStatus::Unknown };
    // An array of value pair <targetLanguage, translationText>.
    std::map<std::wstring, std::wstring> translations;
};
/**
* Represents translation.hypothesis message
*/
struct TranslationHypothesisMsg : public SpeechHypothesisMsg
{
    TranslationHypothesisMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring&& text, TranslationResult&& translation) :
        SpeechHypothesisMsg(std::move(content), offset, duration, std::move(text)),
        translation(translation)
    {}

    TranslationResult translation;
};

/**
* Represents translation.phrase message
*/
struct TranslationPhraseMsg : public TranslationHypothesisMsg
{
    TranslationPhraseMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring&& text, TranslationResult&& translation, RecognitionStatus status) :
        TranslationHypothesisMsg(std::move(content), offset, duration, std::move(text), std::move(translation)),
        recognitionStatus(status)
    {}

    RecognitionStatus recognitionStatus;
};

/**
* Represents translation.synthesis message
*/
struct TranslationSynthesisMsg
{
    const uint8_t* audioBuffer;
    size_t audioLength { 0 };
};

}
}
}
}
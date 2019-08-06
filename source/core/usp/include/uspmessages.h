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
#include <map>

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
    Success,
    NoMatch,
    InitialSilenceTimeout,
    InitialBabbleTimeout,
    Error,
    EndOfDictation,
    TooManyRequests,
    BadRequest,
    Forbidden,
    ServiceUnavailable,
    InvalidMessage,
};

/**
 * Represents keyword status in speech keyword.
 */
enum class KeywordVerificationStatus : int
{
    Accepted,
    Rejected,
    InvalidMessage
};

/**
* Represents translation status in translation phrase.
*/
enum class TranslationStatus : int
{
    Success, Error, InvalidMessage
};

/**
* Represents USP error code.
 */
enum class ErrorCode : int
{
    AuthenticationError = 1,
    BadRequest,
    TooManyRequests,
    Forbidden,
    ConnectionError,
    ServiceUnavailable,
    ServiceError,
    RuntimeError
};

struct JsonMsg
{

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
    TurnStartMsg(std::wstring&& content, const std::string& tag, const std::string& request) :
        JsonMsg(std::move(content)),
        contextServiceTag(tag),
        requestId{ request }
    {
    }

    std::string contextServiceTag;
    std::string requestId;
};

/**
* Represents turn.end message
* Note: Body is empty.
*/
struct TurnEndMsg : JsonMsg
{
    TurnEndMsg(const std::string& request) :
        JsonMsg(std::wstring()),
        requestId{ request }
    {}

    std::string requestId;
};

struct SpeechMsg : public JsonMsg
{
    SpeechMsg() = default;
    SpeechMsg(const SpeechMsg&) = default;
    SpeechMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring&& speaker = L"") :
        JsonMsg(std::move(content)),
        offset(offset),
        duration(duration),
        speaker(std::move(speaker))
    {}

    OffsetType offset{ 0 };
    DurationType duration{ 0 };
    std::wstring speaker{ L"" };
};

/**
 * Represents speech.hypothesis message
 */
struct SpeechHypothesisMsg : public SpeechMsg
{
    SpeechHypothesisMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring&& text, std::wstring&& speaker = L"") :
        SpeechMsg(std::move(content), offset, duration, std::move(speaker)),
        text(std::move(text))
    {}

    SpeechHypothesisMsg() {}
    std::wstring text;
};

/**
* Represents speech.fragment message
*/
struct SpeechFragmentMsg : public SpeechMsg
{
    SpeechFragmentMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring&& text, std::wstring&& speaker = L"") :
        SpeechMsg(std::move(content), offset, duration, std::move(speaker)),
        text(std::move(text))
    {}

    std::wstring text;
};

/**
* Represents speech.keyword message
*/
struct SpeechKeywordDetectedMsg : public SpeechMsg
{
    SpeechKeywordDetectedMsg(std::wstring&& content, OffsetType offset, DurationType duration, KeywordVerificationStatus status, std::wstring&& text) :
        SpeechMsg(std::move(content), offset, duration),
        status(status),
        text(std::move(text))
    {}

    KeywordVerificationStatus status;
    std::wstring text;
};

/**
 * Represents speech.phrase message
 */
struct SpeechPhraseMsg : public SpeechMsg
{
    SpeechPhraseMsg() = default;
    SpeechPhraseMsg(const SpeechPhraseMsg&) = default;

    SpeechPhraseMsg(std::wstring&& content, OffsetType offset, DurationType duration, RecognitionStatus status, std::wstring&& text, std::wstring&& speaker = L"") :
        SpeechMsg(std::move(content), offset, duration, std::move(speaker)),
        recognitionStatus(status),
        displayText(std::move(text))
    {}

    RecognitionStatus recognitionStatus { RecognitionStatus::Error };
    std::wstring displayText;
};

/**
* Represents translation results.
*/
struct TranslationResult
{
    TranslationStatus translationStatus { TranslationStatus::Error };
    // A string indicates failure reasons in case that the translationStatus is an error.
    std::wstring failureReason;
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
    TranslationHypothesisMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring text, const TranslationResult& translation) :
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
    TranslationPhraseMsg(std::wstring&& content, OffsetType offset, DurationType duration, std::wstring text, const TranslationResult& translation, RecognitionStatus status) :
        TranslationHypothesisMsg(std::move(content), offset, duration, std::move(text), std::move(translation)),
        recognitionStatus(status)
    {}


    RecognitionStatus recognitionStatus;
};

/**
* Represents an audio output chunk message
*/
struct AudioOutputChunkMsg
{
    int streamId { -1 };
    std::string requestId;
    const uint8_t* audioBuffer { nullptr };
    size_t audioLength { 0 };
};

/**
* Represents a text boundary for alignment with audio output
*/
struct TextBoundary
{
    OffsetType audioOffset { 0 };
    std::wstring text;
};

/**
* Represents an audio output metadata
*/
struct AudioOutputMetadata
{
    std::string type;
    TextBoundary textBoundary;
};

/**
* Represents an audio output metadata message
*/
struct AudioOutputMetadataMsg
{
    std::string requestId;
    size_t size;
    std::list<AudioOutputMetadata> metadatas;
};

/**
* Represents a message corresponding to a user defined path.
*/
struct UserMsg
{
    const std::string path;
    const std::string contentType;
    const std::string requestId;
    const uint8_t* buffer;
    size_t size;
};

}}}}

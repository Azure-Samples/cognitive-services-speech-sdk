//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rnntmessages.h: definition of RNN-T messages that are exposed to users.
//
#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace RNNT {

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
    EndOfDictation
};

struct Msg
{
    Msg() = default;
    Msg(const Msg&) = default;

    OffsetType offset{ 0 };

protected:
    Msg(OffsetType offset) :
        offset(offset)
    {}
};

/**
 * Represents speech start detected message
 */
struct SpeechStartDetectedMsg : public Msg
{
    SpeechStartDetectedMsg(OffsetType offset) :
        Msg(offset)
    {}
};

/**
 * Represents speech end detected message
 */
struct SpeechEndDetectedMsg : public Msg
{
    SpeechEndDetectedMsg(OffsetType offset) :
        Msg(offset)
    {}
};

/**
 * Represents turn start message
 */
struct TurnStartMsg
{
    TurnStartMsg(const std::string& tag) :
        contextTag(tag)
    {}

    std::string contextTag;
};

struct SpeechMsg : public Msg
{
    SpeechMsg() = default;
    SpeechMsg(const SpeechMsg&) = default;
    SpeechMsg(OffsetType offset, DurationType duration, std::wstring&& text = L"") :
        Msg(offset),
        duration(duration),
        text(std::move(text))
    {}

    DurationType duration{ 0 };
    std::wstring text;
};

/**
 * Represents speech hypothesis message
 */
struct SpeechHypothesisMsg : public SpeechMsg
{
    SpeechHypothesisMsg(OffsetType offset, DurationType duration, std::wstring&& text) :
        SpeechMsg(offset, duration, std::move(text))
    {}
};

/**
 * Represents speech phrase message
 */
struct SpeechPhraseMsg : public SpeechMsg
{
    SpeechPhraseMsg() = default;
    SpeechPhraseMsg(const SpeechPhraseMsg&) = default;

    SpeechPhraseMsg(OffsetType offset, DurationType duration, RecognitionStatus status, std::wstring&& text) :
        SpeechMsg(offset, duration, std::move(text)),
        recognitionStatus(status)
    {}

    RecognitionStatus recognitionStatus{ RecognitionStatus::Error };
};

}}}}

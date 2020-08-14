//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <memory>
#include <string>

#include <speechapi_cxx_enums.h>

#include <interfaces/audio.h>
#include <interfaces/base.h>
#include <interfaces/errors.h>
#include <interfaces/types.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxRecognitionResult : public ISpxInterfaceBaseFor<ISpxRecognitionResult>
{
public:
    virtual std::wstring GetResultId() = 0;
    virtual std::wstring GetText() = 0;

    virtual ResultReason GetReason() = 0;
    virtual CancellationReason GetCancellationReason() = 0;
    virtual std::shared_ptr<ISpxErrorInformation> GetError() = 0;
    virtual NoMatchReason GetNoMatchReason() = 0;

    virtual uint64_t GetOffset() const = 0;
    virtual void SetOffset(uint64_t) = 0;
    virtual uint64_t GetDuration() const = 0;

    virtual void SetLatency(uint64_t) = 0;

    virtual std::shared_ptr<ISpxAudioDataStream> GetAudioDataStream() = 0;
};

using RecognitionResultPtr = std::shared_ptr<ISpxRecognitionResult>;

class ISpxRecognitionResultInit : public ISpxInterfaceBaseFor<ISpxRecognitionResultInit>
{
public:
    virtual void InitIntermediateResult(const wchar_t* text, uint64_t offset, uint64_t duration) = 0;
    virtual void InitFinalResult(ResultReason reason, NoMatchReason noMatchReason, const wchar_t* text, uint64_t offset, uint64_t duration) = 0;
    virtual void InitErrorResult(const std::shared_ptr<ISpxErrorInformation>& error) = 0;
    virtual void InitEndOfStreamResult() = 0;
};

enum class TranslationStatusCode { Success, Error };

class ISpxTranslationRecognitionResult : public ISpxInterfaceBaseFor<ISpxTranslationRecognitionResult>
{
public:
    virtual const std::map<std::wstring, std::wstring>& GetTranslationText() = 0;
};

class ISpxTranslationRecognitionResultInit : public ISpxInterfaceBaseFor<ISpxTranslationRecognitionResultInit>
{
public:
    virtual void InitTranslationRecognitionResult(TranslationStatusCode status, const std::map<std::wstring, std::wstring>& translations, const std::wstring& failureReason) = 0;
};

class ISpxConversationTranscriptionResult : public ISpxInterfaceBaseFor<ISpxConversationTranscriptionResult>
{
public:
    virtual std::wstring GetUserId() = 0;
    virtual std::wstring GetUtteranceId() = 0;
};

class ISpxConversationTranscriptionResultInit : public ISpxInterfaceBaseFor< ISpxConversationTranscriptionResultInit>
{
public:
    virtual void InitConversationResult(const wchar_t* userId, const wchar_t* utteranceId = nullptr) = 0;
};


} } } }

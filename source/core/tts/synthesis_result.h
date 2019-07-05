//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// synthesis_result.h: Implementation declarations for CSpxSynthesisResult C++ class
//

#pragma once
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxSynthesisResult :
    public ISpxSynthesisResult,
    public ISpxSynthesisResultInit,
    public ISpxPropertyBagImpl
{
public:

    CSpxSynthesisResult();
    ~CSpxSynthesisResult();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxSynthesisResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxSynthesisResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxSynthesisResult ---
    std::wstring GetResultId() override;
    std::wstring GetRequestId() override;
    std::shared_ptr<ISpxSynthesizerEvents> GetEvents() override;
    std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> GetFutureResult() override;
    ResultReason GetReason() override;
    CancellationReason GetCancellationReason() override;
    CancellationErrorCode GetCancellationErrorCode() override;
    uint32_t GetAudioLength() override;
    std::shared_ptr<std::vector<uint8_t>> GetAudioData() override;
    std::shared_ptr<ISpxAudioDataStream> GetAudioDataStream() override;
    uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) override;
    bool HasHeader() override;

    // --- ISpxSynthesisResultInit ---
    void InitSynthesisResult(const std::wstring& requestId, ResultReason reason,
        CancellationReason cancellation, CancellationErrorCode errorCode,
        uint8_t* audio_buffer, size_t audio_length, SPXWAVEFORMATEX* format, bool hasHeader) override;
    void SetEvents(const std::shared_ptr<ISpxSynthesizerEvents>& events) override;
    void SetFutureResult(std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> futureResult) override;

    // --- ISpxNamedProperties (overrides)
    void SetStringValue(const char* name, const char* value) override;


private:

    CSpxSynthesisResult(const CSpxSynthesisResult&) = delete;
    CSpxSynthesisResult(const CSpxSynthesisResult&&) = delete;

    CSpxSynthesisResult& operator=(const CSpxSynthesisResult&) = delete;

    std::wstring m_resultId;
    std::wstring m_requestId;
    std::shared_ptr<ISpxSynthesizerEvents> m_events;
    std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> m_futureResult;
    ResultReason m_reason;
    CancellationReason m_cancellationReason;
    CancellationErrorCode m_cancellationErrorCode;
    std::vector<uint8_t> m_audiodata;
    SpxWAVEFORMATEX_Type m_audioformat;
    bool m_hasHeader = true;

    std::shared_ptr<ISpxAudioDataStream> m_audioStream;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

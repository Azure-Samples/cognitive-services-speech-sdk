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
    std::string GetResultId() override;
    std::string GetRequestId() override;
    std::shared_ptr<ISpxSynthesizerEvents> GetEvents() override;
    std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> GetFutureResult() override;
    ResultReason GetReason() override;
    CancellationReason GetCancellationReason() override;
    const std::shared_ptr<ISpxErrorInformation>& GetError() override;
    uint32_t GetAudioLength() override;
    std::shared_ptr<std::vector<uint8_t>> GetAudioData() override;
    std::shared_ptr<std::vector<uint8_t>> GetRawAudioData() override;
    std::shared_ptr<ISpxAudioDataStream> GetAudioDataStream() override;
    uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) override;
    bool HasHeader() override;

    // --- ISpxSynthesisResultInit ---
    void InitSynthesisResult(const std::string& requestId, ResultReason reason,
        CancellationReason cancellation, const std::shared_ptr<ISpxErrorInformation>& error,
        uint8_t* audio_buffer, size_t audio_length, SPXWAVEFORMATEX* format, bool hasHeader) override;
    void SetEvents(const std::shared_ptr<ISpxSynthesizerEvents>& events) override;
    void SetFutureResult(std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> futureResult) override;
    void Reset() override;

    // --- ISpxNamedProperties (overrides)
    void SetStringValue(const char* name, const char* value) override;


private:

    DISABLE_COPY_AND_MOVE(CSpxSynthesisResult);

    std::string m_requestId;
    std::weak_ptr<ISpxSynthesizerEvents> m_events;
    std::shared_ptr<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>> m_futureResult;
    ResultReason m_reason;
    CancellationReason m_cancellationReason;
    std::shared_ptr<ISpxErrorInformation> m_error;
    std::vector<uint8_t> m_audiodata;
    SpxWAVEFORMATEX_Type m_audioformat;
    bool m_hasHeader = true;
    uint32_t m_headerLength = 0;

    std::shared_ptr<ISpxAudioDataStream> m_audioStream;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
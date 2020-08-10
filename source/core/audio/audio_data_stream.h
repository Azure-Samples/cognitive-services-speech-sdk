//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_data_stream.h: Implementation definitions for CSpxAudioDataStream C++ class
//

#pragma once
#include "stdafx.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include "pull_audio_output_stream.h"
#include <queue>
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxAudioDataStream :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxAudioDataStream,
    public ISpxPropertyBagImpl,
    public CSpxPullAudioOutputStream
{
public:

    CSpxAudioDataStream() = default;
    ~CSpxAudioDataStream();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioDataStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutput)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputReader)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioDataStream ---
    void InitFromSynthesisResult(std::shared_ptr<ISpxSynthesisResult> result) override;
    void InitFromFormat(const SPXWAVEFORMATEX& format, bool hasHeader) final;
    StreamStatus GetStatus() noexcept final;
    CancellationReason GetCancellationReason() override;
    CancellationErrorCode GetCancellationErrorCode() override;
    bool CanReadData(uint32_t requestedSize) override;
    bool CanReadData(uint32_t requestedSize, uint32_t pos) override;
    uint32_t Read(uint8_t* buffer, uint32_t bufferSize) override;
    uint32_t Read(uint8_t* buffer, uint32_t bufferSize, uint32_t pos) override;
    void SaveToWaveFile(const wchar_t* fileName) override;
    uint32_t GetPosition() override;
    void SetPosition(uint32_t pos) override;
    inline void DetachInput() final {}

    // --- CSpxPullAudioOutputStream ---
    uint32_t Write(uint8_t* buffer, uint32_t size) override;
    void Close() override;

private:

    using SynthesisCallbackFunction_Type = std::function<void(std::shared_ptr<ISpxSynthesisEventArgs>)>;

    DISABLE_COPY_AND_MOVE(CSpxAudioDataStream);

    uint32_t FillBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t pos);
    void DisconnectSynthEvents();

    std::wstring m_requestId;

    SynthesisCallbackFunction_Type m_pfnSynthesizing;
    SynthesisCallbackFunction_Type m_pfnSynthesisStopped; // This is for synthesis is either completed or canceled

    std::weak_ptr<ISpxSynthesizerEvents> m_synthEvents;
    std::mutex m_eventMutex;

    std::list<std::pair<SpxSharedAudioBuffer_Type, uint32_t>> m_audioList;

    uint32_t m_position = 0;

    ResultReason m_beginningReason = ResultReason::SynthesizingAudioStarted;
    ResultReason m_latestReason = ResultReason::SynthesizingAudioStarted;
    CancellationReason m_cancellationReason = CancellationReason::EndOfStream;
    CancellationErrorCode m_cancellationErrorCode = CancellationErrorCode::NoError;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
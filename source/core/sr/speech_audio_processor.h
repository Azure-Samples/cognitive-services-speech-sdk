//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_audio_processor.h: Implementation declarations for CSpxSpeechAudioProcessor C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "audio_chunk.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSpeechAudioProcessor :
    public ISpxObjectWithSiteInitImpl<ISpxSpeechAudioProcessorAdapterSite>,
    public ISpxSpeechAudioProcessorAdapter
{
public:

    CSpxSpeechAudioProcessor();
    virtual ~CSpxSpeechAudioProcessor();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
        SPX_INTERFACE_MAP_ENTRY(ISpxSpeechAudioProcessorAdapter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObject

    void Init() override;
    void Term() override;

    // --- ISpxAudioProcessor

    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;

    // -- ISpxSpeechAudioProcessorAdapter

    void SetSpeechDetectionThreshold(uint32_t threshold) override;
    void SetSpeechDetectionSilenceCount(uint32_t count) override;

private:

    CSpxSpeechAudioProcessor(const CSpxSpeechAudioProcessor&) = delete;
    CSpxSpeechAudioProcessor(const CSpxSpeechAudioProcessor&&) = delete;
    CSpxSpeechAudioProcessor& operator=(const CSpxSpeechAudioProcessor&) = delete;
    CSpxSpeechAudioProcessor& operator=(const CSpxSpeechAudioProcessor&&) = delete;

    bool HasFormat() { return m_format.get() != nullptr; }

    void InitFormat(const SPXWAVEFORMATEX* pformat);
    void TermFormat();
    void End();

private:
    void NotifySiteSpeechStart();
    void NotifySiteSpeechEnd();

    SpxWAVEFORMATEX_Type m_format;

    bool m_bSpeechStarted;

    uint32_t m_cbSilenceChunkCount;
    uint32_t m_cbSilenceChunkCountMax;
    uint32_t m_energyThreshold;
    uint64_t m_cbAudioProcessed;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_reco_engine_adapter.h: Implementation declarations for CSpxMockRecoEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxMockRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxRecoEngineAdapter
{
public:

    CSpxMockRecoEngineAdapter();
    virtual ~CSpxMockRecoEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObject
    void Init() override;
    void Term() override;

    // --- ISpxRecoEngineAdapter
    void SetAdapterMode(bool singleShot) override;

    // --- ISpxAudioProcessor
    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;


private:

    CSpxMockRecoEngineAdapter(const CSpxMockRecoEngineAdapter&) = delete;
    CSpxMockRecoEngineAdapter(const CSpxMockRecoEngineAdapter&&) = delete;
    CSpxMockRecoEngineAdapter& operator=(const CSpxMockRecoEngineAdapter&) = delete;
    CSpxMockRecoEngineAdapter& operator=(const CSpxMockRecoEngineAdapter&&) = delete;

    bool HasFormat() { return m_format.get() != nullptr; }

    void InitFormat(WAVEFORMATEX* pformat);
    void TermFormat();
    void End();

    void FireIntermediateResult();
    void FireFinalResult();
    void FireSpeechStartDetected();
    void FireSpeechEndDetected();
    
    void EnsureFireFinalResult();


private:

    bool m_singleShot = false;
    SpxWAVEFORMATEX_Type m_format;

    const std::wstring m_firstMockWord = L"mock";
    const std::wstring m_eachIntermediateAddsMockWord = L" mock";
    
    std::wstring m_mockResultText;

    const uint64_t m_numMsBeforeVeryFirstIntermediate = 500;
    const uint64_t m_numMsBetweenFirstIntermediateAndFinal = 800;
    const uint64_t m_numMsBetweenFinalAndNextIntermediate = 2000;
    const uint64_t m_numMsBetweenIntermediates = 250;

    uint64_t m_cbAudioProcessed;
    uint64_t m_cbFireNextIntermediate;
    uint64_t m_cbFireNextFinalResult;

    uint64_t m_cbFiredLastIntermediate;
    uint64_t m_cbFiredLastFinal;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

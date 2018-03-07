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


namespace CARBON_IMPL_NAMESPACE() {


class CSpxMockRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxRecoEngineAdapter
{
public:

    CSpxMockRecoEngineAdapter();
    virtual ~CSpxMockRecoEngineAdapter();

    // --- ISpxObject

    void Init() override;
    void Term() override;

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

    void FireIntermediateResult();
    void FireFinalResult();
    
    void EnsureFireFinalResult();


private:

    SpxWAVEFORMATEX_Type m_format;

    std::wstring m_mockResultText;

    uint64_t m_cbAudioProcessed;
    uint64_t m_cbLastIntermediateFired;
    uint64_t m_cbLastFinalResultFired;
};


} // CARBON_IMPL_NAMESPACE

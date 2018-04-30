//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_kws_engine_adapter.h: Implementation declarations for CSpxMockKwsEngineAdapter C++ class
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


class CSpxMockKwsEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxKwsEngineAdapterSite>,
    public ISpxKwsEngineAdapter
{
public:

    CSpxMockKwsEngineAdapter();
    virtual ~CSpxMockKwsEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxKwsEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObject

    void Init() override;
    void Term() override;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;


private:

    CSpxMockKwsEngineAdapter(const CSpxMockKwsEngineAdapter&) = delete;
    CSpxMockKwsEngineAdapter(const CSpxMockKwsEngineAdapter&&) = delete;
    CSpxMockKwsEngineAdapter& operator=(const CSpxMockKwsEngineAdapter&) = delete;
    CSpxMockKwsEngineAdapter& operator=(const CSpxMockKwsEngineAdapter&&) = delete;

    bool HasFormat() { return m_format.get() != nullptr; }

    void InitFormat(WAVEFORMATEX* pformat);
    void TermFormat();
    void End();

    void FireKeywordDetected(AudioData_Type data, uint32_t size);


private:

    const uint64_t m_numMsBeforeVeryFirstKeyword = 200;
    const uint64_t m_numMsBetweenKeywords = 5000;

    SpxWAVEFORMATEX_Type m_format;
    uint64_t m_cbAudioProcessed;

    uint64_t m_cbFireNextKeyword;
    uint64_t m_cbLastKeywordFired;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

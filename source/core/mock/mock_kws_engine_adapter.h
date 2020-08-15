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
#include <object_with_site_init_impl.h>

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

    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;


private:

    CSpxMockKwsEngineAdapter(const CSpxMockKwsEngineAdapter&) = delete;
    CSpxMockKwsEngineAdapter(const CSpxMockKwsEngineAdapter&&) = delete;
    CSpxMockKwsEngineAdapter& operator=(const CSpxMockKwsEngineAdapter&) = delete;
    CSpxMockKwsEngineAdapter& operator=(const CSpxMockKwsEngineAdapter&&) = delete;

    bool HasFormat() { return m_format.get() != nullptr; }

    void InitFormat(const SPXWAVEFORMATEX* pformat);
    void TermFormat();
    void End();

    void FireKeywordDetected();


private:

    const uint64_t ticksPerSecond = 1000 * 1000 * 10; // 1000 == to_msec, 1000 == to_usec, 10 == to_100nsec
    const uint64_t m_numMsKeywordEvery = 5000;
    const uint64_t m_numMsKeywordOffset = 4750;
    const uint64_t m_numMsKeywordDuration = 750;
    const uint64_t m_numMsKeywordUPL = 100;

    SpxWAVEFORMATEX_Type m_format;
    std::list<DataChunkPtr> m_audio;
    uint64_t m_cbAudioProcessed;

    uint64_t m_cbFireKeyword;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

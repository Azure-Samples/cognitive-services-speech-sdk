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
#include <object_with_site_init_impl.h>

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
    std::future<bool> SendNetworkMessage(std::string&&, std::string&&) override { std::future<bool> f; return f;}
    std::future<bool> SendNetworkMessage(std::string&&, std::vector<uint8_t>&&) override { std::future<bool> f; return f; }

    // --- ISpxAudioProcessor
    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;

private:
    using SitePtr = std::shared_ptr<ISpxRecoEngineAdapterSite>;

    CSpxMockRecoEngineAdapter(const CSpxMockRecoEngineAdapter&) = delete;
    CSpxMockRecoEngineAdapter(const CSpxMockRecoEngineAdapter&&) = delete;
    CSpxMockRecoEngineAdapter& operator=(const CSpxMockRecoEngineAdapter&) = delete;
    CSpxMockRecoEngineAdapter& operator=(const CSpxMockRecoEngineAdapter&&) = delete;

    bool HasFormat() { return m_format.get() != nullptr; }

    void InitFormat(const SPXWAVEFORMATEX* pformat);
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

    const uint64_t ticksPerSecond = 1000 * 1000 * 10; // 1000 == to_msec, 1000 == to_usec, 10 == to_100nsec
    const uint64_t m_numMsFinalEvery = 3300;
    const uint64_t m_numMsFinalOffset = 3050;
    const uint64_t m_numMsFinalDuration = 1250;
    const uint64_t m_numMsFinalUPL = 250;
    // result.offset = Every - Offset; result.duration = duration;
    const uint64_t m_numMsBetweenIntermediates = 250;
    const uint64_t m_numIntermediates = 3;

    uint64_t m_cbAudioProcessed;

    uint64_t m_cbFireFinal;
    uint64_t m_cbResultStartsAt;
    uint64_t m_cbFireIntermediate;
    uint32_t m_numIntermediatesFired;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
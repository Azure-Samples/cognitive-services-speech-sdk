//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// kws_engine_adapter.h: Implementation declarations for CSpxSdkKwsEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"

#include <kws_engine.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSdkKwsEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxKwsEngineAdapterSite>,
    public ISpxKwsEngineAdapter
{
public:
    CSpxSdkKwsEngineAdapter();
    virtual ~CSpxSdkKwsEngineAdapter();

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
    void FireDoneProcessingAudioEvent();
    void FireKeywordDetectedEvent(const DataChunkPtr& audioChunk);

    CSpxSdkKwsEngineAdapter(const CSpxSdkKwsEngineAdapter&) = delete;
    CSpxSdkKwsEngineAdapter(const CSpxSdkKwsEngineAdapter&&) = delete;
    CSpxSdkKwsEngineAdapter& operator=(const CSpxSdkKwsEngineAdapter&) = delete;
    CSpxSdkKwsEngineAdapter& operator=(const CSpxSdkKwsEngineAdapter&&) = delete;

    class Impl; // forward declaration
    friend Impl;
    Impl* p_impl;

#ifdef _DEBUG
    FILE* m_audioDumpFile = nullptr;
    std::string m_audioDumpDir;
    uint32_t m_audioDumpInstCount = 0;
    uint32_t m_audioDumpMaxCount = 5;
    uint32_t m_audioDumpFileLengthMs = 20 * 1000;
    uint32_t m_audioDumpBytesWritten = 0;
    uint32_t m_audioDumpBytesMax = 0;
    void InitAudioDumpFile();
    void CycleAudioDumpFile();
#endif
};

}}}}

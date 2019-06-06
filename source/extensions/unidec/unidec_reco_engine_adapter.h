//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// unidec_reco_engine_adapter.h: Implementation declarations for CSpxUnidecRecoEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "string_utils.h"
#include "UnidecRuntime.h"
#include "AudioEndMetadata.h"
#include "unidec_config.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class RingBuffer;

class CSpxUnidecRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxRecoEngineAdapter
{
public:

    CSpxUnidecRecoEngineAdapter();
    virtual ~CSpxUnidecRecoEngineAdapter();

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
    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;


private:

    CSpxUnidecRecoEngineAdapter(const CSpxUnidecRecoEngineAdapter&) = delete;
    CSpxUnidecRecoEngineAdapter(const CSpxUnidecRecoEngineAdapter&&) = delete;

    CSpxUnidecRecoEngineAdapter& operator=(const CSpxUnidecRecoEngineAdapter&) = delete;

    bool IsInit() { return m_config.get() != nullptr; };
    bool HasFormat() { return m_format.get() != nullptr; }

    void InitUnidecConfig();
    std::string GetBaseModelPath();

    void InitFormat(const SPXWAVEFORMATEX* pformat);
    void TermFormat();

    void InitEngine();
    void InitSearchGraphs();

    void StartEngine();
    void StopEngine();
    void FlushBuffers();

    static bool NextStreamCallback(AudioStreamDescriptor** pAudioStream, IUnidecSearchGraphCombo*& pCombo, bool* continuousReco, void* callbackContext);
    static void IntermediateCallback(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult, void* callbackContext);
    static void SentenceCallback(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest, void* callbackContext);
    static void EndCallback(const WCHAR* wavId, const AudioEndMetadata& metadata, void* callbackContext);

    static size_t AudioStreamReadCallback(AudioStreamDescriptor* pAudioStream, void *pBuffer, size_t maxSize);
    static bool AudioStreamIsEndCallback(AudioStreamDescriptor* pAudioStream);
    static bool AudioStreamIsErrorCallback(AudioStreamDescriptor* pAudioStream);


    bool NextStream(AudioStreamDescriptor** pAudioStream, IUnidecSearchGraphCombo*& pCombo,  bool* continuousReco);
    bool InitStream(AudioStreamDescriptor** pAudioStream, IUnidecSearchGraphCombo*& pCombo);
    void Intermediate(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult);
    void Sentence(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest);
    void End(const WCHAR* wavId, const AudioEndMetadata& metadata);

    size_t AudioStreamRead(AudioStreamDescriptor* pAudioStream, void *pBuffer, size_t maxSize);
    bool AudioStreamIsEnd(AudioStreamDescriptor* pAudioStream);
    bool AudioStreamIsError(AudioStreamDescriptor* pAudioStream);

    void AudioBufferWrite(std::shared_ptr<uint8_t> buffer, size_t size);
    size_t AudioBufferRead(uint8_t* buffer, size_t maxSize);

    std::wstring TrimWords(std::vector<const WCHAR*>& words, const WCHAR* trim1, const WCHAR* trim2);
    void SetServiceJsonResultProperties(const std::shared_ptr<ISpxRecognitionResult>& result, uint64_t offset, uint64_t duration, const std::string& text);

private:

    std::shared_ptr<RingBuffer> m_ringBuffer;
    std::vector<unsigned char> m_audioBuffer;
    
    bool m_endOfStream = false;
    bool m_turnStarted = false;

    SpxWAVEFORMATEX_Type m_format;
    std::unique_ptr<CSpxUnidecConfig> m_config;
    std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>> m_graphs;
    std::unique_ptr<IUnidecEngine, std::function<void(IUnidecEngine*)>> m_unidecEngine;

    AudioStreamDescriptor m_audioStream;
    wchar_string m_streamId;
    std::atomic<bool> m_stopImmediately;
    bool m_sentenceEndDetected;
    bool m_singleShot = false;
    std::mutex m_mutex;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

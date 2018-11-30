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
#include "UnidecRuntime.h"
#include "unidec_config.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


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
    void ProcessAudio(AudioData_Type data, uint32_t size) override;


private:

    CSpxUnidecRecoEngineAdapter(const CSpxUnidecRecoEngineAdapter&) = delete;
    CSpxUnidecRecoEngineAdapter(const CSpxUnidecRecoEngineAdapter&&) = delete;

    CSpxUnidecRecoEngineAdapter& operator=(const CSpxUnidecRecoEngineAdapter&) = delete;

    bool IsInit() { return m_config.get() != nullptr; };
    bool HasFormat() { return m_format.get() != nullptr; }

    void InitConfig();
    std::wstring GetBaseModelPath();

    void InitFormat(const SPXWAVEFORMATEX* pformat);
    void TermFormat();

    void EnsureEngine();
    void InitEngine();
    void InitSearchGraphs();

    void StartEngine();
    void StopEngine();

    static bool __stdcall NextStreamCallback(AudioStreamDescriptor** pAudioStream,  bool* enableSegmentation, void* callbackContext);
    static void __stdcall IntermediateCallback(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult, void* callbackContext);
    static void __stdcall SentenceCallback(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest, void* callbackContext);
    static void __stdcall EndCallback(const wchar_t* wavId, UnidecEndReason reason, void* callbackContext);

    static size_t __stdcall AudioStreamReadCallback(AudioStreamDescriptor* pAudioStream, void *pBuffer, size_t maxSize);
    static bool __stdcall AudioStreamIsEndCallback(AudioStreamDescriptor* pAudioStream);
    static bool __stdcall AudioStreamIsErrorCallback(AudioStreamDescriptor* pAudioStream);

    bool NextStream(AudioStreamDescriptor** pAudioStream,  bool* enableSegmentation);
    bool InitStream(AudioStreamDescriptor** pAudioStream, bool* enableSegmentation);
    void Intermediate(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult);
    void Sentence(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest);
    void End(const wchar_t* wavId, UnidecEndReason reason);

    size_t AudioStreamRead(AudioStreamDescriptor* pAudioStream, void *pBuffer, size_t maxSize);
    bool AudioStreamIsEnd(AudioStreamDescriptor* pAudioStream);
    bool AudioStreamIsError(AudioStreamDescriptor* pAudioStream);

    void AudioBufferWrite(std::shared_ptr<uint8_t> buffer, size_t size);
    size_t AudioBufferRead(uint8_t* buffer, size_t maxSize);

    void EnsureAudioBuffer();

    std::wstring TrimWords(std::vector<const wchar_t*>& words, const wchar_t* trim1, const wchar_t* trim2);


private:

    bool m_singleShot = false;

    SpxWAVEFORMATEX_Type m_format;
    std::unique_ptr<CSpxUnidecConfig> m_config;
    std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>> m_graphs;
    std::unique_ptr<IUnidecEngine, std::function<void(IUnidecEngine*)>> m_unidecEngine;

    AudioStreamDescriptor m_audioStream;
    std::wstring m_streamId;

    bool m_sentenceComplete;

    std::shared_ptr<uint8_t> m_buffer;
    size_t m_bytesLeftInBuffer;
    uint8_t* m_ptrIntoBuffer;

    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<std::pair<std::shared_ptr<uint8_t>, size_t>> m_additionalBuffers;
    bool m_stopImmediately;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

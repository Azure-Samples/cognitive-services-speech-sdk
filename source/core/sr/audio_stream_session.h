//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.h: Implementation declarations for CSpxAudioStreamSession C++ class
//

#pragma once
#include "spxcore_common.h"
#include "service_helpers.h"
#include "session.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxAudioStreamSession : public CSpxSession, 
    public ISpxObjectWithSiteInitImpl<ISpxSite>,
    public ISpxServiceProvider,
    public ISpxRecoEngineAdapterSite,
    public ISpxRecognizerSite,
    public ISpxRecoResultFactory,
    public ISpxEventArgsFactory,
    public ISpxAudioStreamSessionInit, 
    public ISpxAudioProcessor
{
public:

    CSpxAudioStreamSession();
    virtual ~CSpxAudioStreamSession();

    // --- ISpxAudioStreamSessionInit
    
    void InitFromFile(const wchar_t* pszFileName) override;
    void InitFromMicrophone() override;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY(ISpxRecoResultFactory)
    SPX_SERVICE_MAP_ENTRY(ISpxEventArgsFactory)
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()


protected:

    void StartRecognizing() override;
    void StopRecognizing() override;

    std::shared_ptr<ISpxRecognitionResult> WaitForRecognition() override;


private:

    // --- ISpxRecoEngineAdapterSite
    void SpeechStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void SpeechEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;

    void SoundStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void SoundEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;

    void IntermediateResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;

    void DoneProcessingAudio(ISpxRecoEngineAdapter* adapter) override;

    void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload) override;

    void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) override;

    // --- ISpxRecognizerSite
    std::shared_ptr<ISpxSession> GetDefaultSession() override;

    // --- ISpxRecoResultFactory
    std::shared_ptr<ISpxRecognitionResult> CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text) override;
    std::shared_ptr<ISpxRecognitionResult> CreateFinalResult(const wchar_t* resultId, const wchar_t* text) override;
    std::shared_ptr<ISpxRecognitionResult> CreateNoMatchResult() override;

    // -- ISpxEventArgsFactory
    std::shared_ptr<ISpxSessionEventArgs> CreateSessionEventArgs(const std::wstring& sessionId) override;
    std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;

    
private:

    using Base_Type = CSpxSession;

    CSpxAudioStreamSession(const CSpxAudioStreamSession&) = delete;
    CSpxAudioStreamSession(const CSpxAudioStreamSession&&) = delete;

    CSpxAudioStreamSession& operator=(const CSpxAudioStreamSession&) = delete;

    void InitRecoEngineAdapter();

    enum SessionState { Idle, StartingPump, ProcessingAudio, StoppingPump, WaitingForAdapterDone };
    bool IsState(SessionState state);
    bool ChangeState(SessionState from, SessionState to);

    SessionState m_state;
    std::mutex m_stateMutex;

    std::shared_ptr<ISpxAudioPump> m_audioPump;
    std::shared_ptr<ISpxRecoEngineAdapter> m_adapter;
};


} // CARBON_IMPL_NAMESPACE

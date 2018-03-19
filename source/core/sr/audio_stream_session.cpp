//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.cpp: Implementation definitions for CSpxAudioStreamSession C++ class
//

#include "stdafx.h"
#include "audio_stream_session.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <thread>
#include "create_object_helpers.h"
#include "site_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxAudioStreamSession::CSpxAudioStreamSession() :
    m_state(SessionState::Idle)
{
}

CSpxAudioStreamSession::~CSpxAudioStreamSession()
{
    auto ptr = std::dynamic_pointer_cast<ISpxObjectInit>(m_adapter);
    if (ptr.get() != nullptr)
    {
        ptr->Term();
        ptr = nullptr;
    }

    m_adapter = nullptr;    
}

void CSpxAudioStreamSession::InitFromFile(const wchar_t* pszFileName)
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);

    // Create the wav file pump
    auto audioFilePump = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFilePump", this);
    m_audioPump = std::dynamic_pointer_cast<ISpxAudioPump>(audioFilePump);

    // Open the WAV file
    audioFilePump->Open(pszFileName);

    // Defer calling InitRecoEngineAdapter() until later ... (see ::EnsureInitRecoEngineAdapter())
}

void CSpxAudioStreamSession::InitFromMicrophone()
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);

    // Create the microphone pump
    m_audioPump = SpxCreateObjectWithSite<ISpxAudioPump>("CSpxInteractiveMicrophone", this);

    // Defer calling InitRecoEngineAdapter() until later ... (see ::EnsureInitRecoEngineAdapter())
}

void CSpxAudioStreamSession::SetFormat(WAVEFORMATEX* pformat)
{
    EnsureInitRecoEngineAdapter();

    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : "non-PCM",
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);

    if (pformat != nullptr && ChangeState(SessionState::StartingPump, SessionState::ProcessingAudio))
    {
        // The pump started successfully, we have a live running session now!
        FireSessionStartedEvent();
        m_adapter->SetFormat(pformat);
    }
    else if (pformat == nullptr && ChangeState(SessionState::StoppingPump, SessionState::WaitingForAdapterDone))
    {
        // Our stop pump request has been satisfied... Let's wait for the adapter to finish...
        m_adapter->ProcessAudio(nullptr, 0);
        SPX_DBG_TRACE_VERBOSE("%s: ProcessingAudio - size=%d", __FUNCTION__, 0);
        m_adapter->SetFormat(pformat);
    }
    else if (pformat == nullptr && ChangeState(SessionState::ProcessingAudio, SessionState::WaitingForAdapterDone))
    {
        // The pump stopped itself... That's possible when WAV files reach EOS. Let's wait for the adapter to finish...
        m_adapter->SetFormat(pformat);
    }
    else
    {
        // Valid transitions are:
        //
        //   StartingPump --> ProcessingAudio (when pformat != nullptr, signifying beginning of stream)
        //   StoppingPump --> WaitingForAdapterDone (when pformat == nullptr, signifying we will get no more data)
        //   ProcessingAudio --> WaitingForAdapterDone (this can happen when the pump runs dry of audio data itself)
        //
        //   NOTE: All other state transitions are invalid inside ISpxAudioProcessor::SetFormat.
        
        SPX_THROW_HR(SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION);
    }
}

void CSpxAudioStreamSession::ProcessAudio(AudioData_Type data, uint32_t size)
{
    SPX_DBG_ASSERT(m_adapter != nullptr);

    if (IsState(SessionState::ProcessingAudio))
    {
        // SPX_DBG_TRACE_VERBOSE("%s - size=%d", __FUNCTION__, size);
        m_adapter->ProcessAudio(data, size);
    }
    else if (IsState(SessionState::StoppingPump))
    {
        // don't process this data if we're actively stopping...
        // SPX_DBG_TRACE_VERBOSE("%s - size=%d -- Ignoring (actively stopping pump)", __FUNCTION__, size);
    }
    else
    {
        // Valid states to encounter are:
        //
        // - ProcessingAudio: We're allowed to process audio while in this state
        // - StoppingPump: We're allowed to be called to process audio, but we'll ignore the data passed in while we're attempting to stop the pump
        //
        // NOTE: All other states are invalid inside ISpxAudioProcessor::ProcessAudio.

        SPX_THROW_HR(SPXERR_PROCESS_AUDIO_INVALID_STATE);
    }
}

void CSpxAudioStreamSession::StartRecognizing()
{
    if (ChangeState(SessionState::Idle, SessionState::StartingPump))
    {
        auto ptr = (ISpxAudioProcessor*)this;
        auto pISpxAudioProcessor = ptr->shared_from_this();
        m_audioPump->StartPump(pISpxAudioProcessor);
    }
    else
    {
        // The only valid state transition is Idle --> StartingPump. 
        // All other state transitions are invalid when attempting to start recognizing...

        SPX_THROW_HR(SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION);
    }
}

void CSpxAudioStreamSession::StopRecognizing()
{
    if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
    {
        m_audioPump->StopPump();
    }
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::WaitForRecognition()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    SPX_DBG_TRACE_VERBOSE("Waiting for Recognition...");
    m_recoAsyncWaiting = true;
    m_cv.wait_for(lock, std::chrono::seconds(m_recoAsyncTimeout), [&] { return !m_recoAsyncWaiting; });
    SPX_DBG_TRACE_VERBOSE("Waiting for Recognition... Done!");

    // TODO: Why do we need this condition?
    // Shouldn't the pump be stopped whenever the recognition is done (wether or not we have a result)?
    if (!m_recoAsyncResult)
    {
        if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
        {
            lock.unlock();
            m_audioPump->StopPump();
            lock.lock();
        }

        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterDone...");
        m_cv.wait_for(lock, std::chrono::seconds(m_waitForDoneTimeout), [this] {
            return m_state != SessionState::StoppingPump && m_state != SessionState::WaitingForAdapterDone;
        });
        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterDone... Done!!");
    }

    return std::move(m_recoAsyncResult);
}

void CSpxAudioStreamSession::SpeechStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::SpeechEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::SoundStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::SoundEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

std::shared_ptr<ISpxSessionEventArgs> CSpxAudioStreamSession::CreateSessionEventArgs(const std::wstring& sessionId)
{
    auto sessionEvent = SpxCreateObjectWithSite<ISpxSessionEventArgs>("CSpxSessionEventArgs", this);

    auto argsInit = std::dynamic_pointer_cast<ISpxSessionEventArgsInit>(sessionEvent);
    argsInit->Init(sessionId);

    return sessionEvent;
}

std::shared_ptr<ISpxRecognitionEventArgs> CSpxAudioStreamSession::CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    auto site = SpxSiteFromThis(this);
    auto recoEvent = SpxCreateObjectWithSite<ISpxRecognitionEventArgs>("CSpxRecognitionEventArgs", site);

    auto argsInit = std::dynamic_pointer_cast<ISpxRecognitionEventArgsInit>(recoEvent);
    argsInit->Init(sessionId, result);

    return recoEvent;
}

void CSpxAudioStreamSession::IntermediateResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    UNUSED(offset);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::Idle), "ERROR! IntermediateResult was called with SessionState==Idle");
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::StartingPump), "ERROR! IntermediateResult was called with SessionState==StartingPump");

    FireResultEvent(GetSessionId(), result);
}

void CSpxAudioStreamSession::FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    UNUSED(offset);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::Idle), "ERROR! FinalResult was called with SessionState==Idle");
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::StartingPump), "ERROR! FinalResult was called with SessionState==StartingPump");

    WaitForRecognition_Complete(result);
}

void CSpxAudioStreamSession::DoneProcessingAudio(ISpxRecoEngineAdapter* adapter)
{
    UNUSED(adapter);
    if (ChangeState(SessionState::WaitingForAdapterDone, SessionState::Idle))
    {
        // The adapter request to finish processing audio has completed
        FireSessionStoppedEvent();
    }
}

void CSpxAudioStreamSession::AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload)
{
    UNUSED(adapter);
    UNUSED(offset);
    UNUSED(payload);
    // TODO: RobCh: FUTURE: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload)
{
    UNUSED(adapter);
    UNUSED(payload);
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

std::shared_ptr<ISpxSession> CSpxAudioStreamSession::GetDefaultSession()
{
    return SpxSharedPtrFromThis<ISpxSession>(this);
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = std::dynamic_pointer_cast<ISpxRecognitionResultInit>(result);
    initResult->InitIntermediateResult(resultId, text);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateFinalResult(const wchar_t* resultId, const wchar_t* text)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = std::dynamic_pointer_cast<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(resultId, text);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateNoMatchResult()
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = std::dynamic_pointer_cast<ISpxRecognitionResultInit>(result);
    initResult->InitNoMatch();

    return result;
}

void CSpxAudioStreamSession::EnsureInitRecoEngineAdapter()
{
    if (m_adapter == nullptr)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        InitRecoEngineAdapter();
    }
}

void CSpxAudioStreamSession::InitRecoEngineAdapter()
{
    // determine which type (or types) of reco engine adapters we should try creating...
    bool tryHybrid = GetBooleanValue(L"__useHybridRecoEngine", false);
    bool tryUnidec = GetBooleanValue(L"__useUnidecRecoEngine", false);
    bool tryMock = GetBooleanValue(L"__useMockRecoEngine", false);
    bool tryUsp = GetBooleanValue(L"__useUspRecoEngine", false);

    // if nobody specified which type(s) of reco engine adapters this session should use, we'll use the USP
    if (!tryHybrid && !tryUnidec && !tryMock && !tryUsp)
    {
        tryUsp = true;
    }

    // try to creat the hybrid adapter...
    if (m_adapter == nullptr && tryHybrid)
    {
        m_adapter = SpxCreateObjectWithSite<ISpxRecoEngineAdapter>("CSpxHybridRecoEngineAdapter", this);
    }

    // try to create the Unidec adapter... 
    if (m_adapter == nullptr && tryUnidec)
    {
        m_adapter = SpxCreateObjectWithSite<ISpxRecoEngineAdapter>("CSpxUnidecRecoEngineAdapter", this);
    }

    // try to create the Usp adapter... 
    if (m_adapter == nullptr && tryUsp)
    {
        m_adapter = SpxCreateObjectWithSite<ISpxRecoEngineAdapter>("CSpxUspRecoEngineAdapter", this);
    }

    // try to create the mock reco engine adapter...
    if (m_adapter == nullptr && tryMock)
    {
        m_adapter = SpxCreateObjectWithSite<ISpxRecoEngineAdapter>("CSpxMockRecoEngineAdapter", this);
    }

    // if we still don't have an adapter... that's an exception
    SPX_IFTRUE_THROW_HR(m_adapter == nullptr, SPXERR_NOT_FOUND);
}

bool CSpxAudioStreamSession::IsState(SessionState state)
{
    std::unique_lock<std::mutex> lock(m_stateMutex);

    return m_state == state;
}

bool CSpxAudioStreamSession::ChangeState(SessionState from, SessionState to)
{
    std::unique_lock<std::mutex> lock(m_stateMutex);

    if (m_state == from)
    {
        SPX_DBG_TRACE_VERBOSE("%s; from=%d, to=%d", __FUNCTION__, from, to);
        m_state = to;
        m_cv.notify_all();
        return true;
    }

    return false;
}

std::shared_ptr<ISpxNamedProperties> CSpxAudioStreamSession::GetParentProperties()
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

} // CARBON_IMPL_NAMESPACE

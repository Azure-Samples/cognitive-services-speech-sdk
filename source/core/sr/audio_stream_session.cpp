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
#include "audio_pump.h"
#include "microphone.h"
#include "recognition_result.h"
#include "usp_reco_engine_adapter.h"
#include "wav_file_reader.h"


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
    }
}

void CSpxAudioStreamSession::InitFromFile(const wchar_t* pszFileName)
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);

    // Open the file
    auto audioFile = SpxMakeShared<CSpxWavFileReader, ISpxAudioFile>();
    audioFile->Open(pszFileName);
    
    // Create the pump, and set the reader
    auto audioReader = std::dynamic_pointer_cast<ISpxAudioReader>(audioFile);
    m_audioPump = SpxMakeShared<CSpxAudioPump, ISpxAudioPump>(audioReader);

    // And finally ... init the USP reco engine adapter
    InitUspRecoEngineAdapter();
}

void CSpxAudioStreamSession::InitFromMicrophone()
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);

    // Create the microphone pump
    m_audioPump = Microphone::Create();

    // And finally ... init the USP reco engine adapter
    InitUspRecoEngineAdapter();
}

void CSpxAudioStreamSession::SetFormat(WAVEFORMATEX* pformat)
{
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
    m_fRecoAsyncWaiting = true;
    m_cv.wait_for(lock, std::chrono::seconds(m_recoAsyncTimeout), [&] { return !m_fRecoAsyncWaiting; });
    SPX_DBG_TRACE_VERBOSE("Waiting for Recognition... Done!");

    if (!m_recoAsyncResult)
    {
        if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
        {
            m_audioPump->StopPump();
        }

        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterDone...");
        m_cv.wait_for(lock, std::chrono::seconds(m_waitForDoneTimeout), [&] {
            return !this->IsState(SessionState::StoppingPump) && !this->IsState(SessionState::WaitingForAdapterDone);
        });
        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterDone... Done!!");
    }

    return std::move(m_recoAsyncResult);
}

void CSpxAudioStreamSession::SpeechStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::SpeechEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::SoundStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::SoundEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::IntermediateResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::Idle), "ERROR! IntermediateResult was called with SessionState==Idle");
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::StartingPump), "ERROR! IntermediateResult was called with SessionState==StartingPump");

    FireResultEvent(GetSessionId(), result);
}

void CSpxAudioStreamSession::FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::Idle), "ERROR! FinalResult was called with SessionState==Idle");
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::StartingPump), "ERROR! FinalResult was called with SessionState==StartingPump");

    WaitForRecognition_Complete(result);
}

void CSpxAudioStreamSession::DoneProcessingAudio(ISpxRecoEngineAdapter* adapter)
{
    if (ChangeState(SessionState::WaitingForAdapterDone, SessionState::Idle))
    {
        // The adapter request to finish processing audio has completed
        FireSessionStoppedEvent();
    }
}

void CSpxAudioStreamSession::AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload)
{
    // TODO: RobCh: FUTURE: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload)
{
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::InitUspRecoEngineAdapter()
{
    // Create the USP reco engine adapter
    m_adapter = SpxMakeShared<CSpxUspRecoEngineAdapter, ISpxRecoEngineAdapter>();

    // Convert 'this' to the adapter site interface
    auto ptr = ((ISpxRecoEngineAdapterSite*)this);
    auto site = std::dynamic_pointer_cast<ISpxSite>(ptr->shared_from_this());

    // And set the site on the adapter
    auto wsite = std::dynamic_pointer_cast<ISpxObjectWithSite>(m_adapter);
    wsite->SetSite(site);
}

bool CSpxAudioStreamSession::IsState(SessionState state)
{
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


}; // CARBON_IMPL_NAMESPACE()

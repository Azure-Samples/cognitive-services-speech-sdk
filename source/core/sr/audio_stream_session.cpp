//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.cpp: Implementation definitions for CSpxAudioStreamSession C++ class
//
//  Typical CSpxAudioStreamSession initialization call flow:
//
//      [caller thread] 1.) ctor
//      [caller thread] 2.) base::SetSite()
//      [caller thread] 3.) this::Init()
//      [caller thread] 4.) this::InitFromFile() or this::InitFromMicrophone()
//
//  Typical CSpxAudioStreamSession::RecognizeAsync() call flow ('SingleShot'):
//
//      [caller thread]        1.) base::RecognizeAsync()
//      [caller thread]        2.)   returns std::future after starting [bg task thread] (see #3)
//      [bg task thread]       3.) this::StartRecognizing(SingleShot)
//      [bg task thread]       4.)   this::ChangeState(Idle ==> StartingPump)
//      [bg task thread]       5.)   this::StartAudioPump(startStop=Speech, keyword="")
//      [bg task thread]       6.)     this::FireSessionStartedEvent()
//      [bg task thread]       7.)     audioPump::StartPump() (kicks off audio bg thread, see #9)
//      [bg task thread]       8.) base::WaitForRecognition() ... BLOCKs [bg task thread] UNTIL !m_recoAsyncWaiting (or timeout)
//      [audio bg thread]      9.) this::SetFormat()
//      [audio bg thread]     10.)   this::ChangeState(StartingPump ==> ProcessingAudio))
//      [audio bg thread]     11.)   this::InformAdapterStartingProcessingAudio()
//      [audio bg thread]     12.)     recoAdapter(as ISpxAudioProcessor)->SetFormat()
//      [audio bg thread]     13.) this::ProcessAudio(), one or more times
//      [audio bg thread]     14.)   recoAdapter(as ISpxAudioProcessor)->ProcessAudio(), one or more times
//                            <-- SOME TIME LATER -->
//      [reco adapter thread] 15.) this::FinalRecoResult
//      [reco adapter thread] 16.)   base::WaitForRecognition_Complete (sets m_recoAsyncWaiting=false, UNBLOCKING [bg task thread], see #18)
//      [reco adapter thread] 17.)     base::FireResultEvent
//      [bg task thread]      18.) base::WaitForRecognition UNBLOCKS
//      [bg task thread]      19.)   this::StopRecognizing(SingleShot)
//      [bg task thread]      20.)     this::ChangeState(ProcessingAudio ==> StoppingPump)
//      [bg task thread]      21.)     audioPump::StopPump() (which instigates [audio bg thread] shutdown, see #22)
//      [audio bg thread]     22.) this::ProcessAudio(nullptr, 0)
//      [audio bg thread]     23.) this::SetFormat(nullptr)
//      [audio bg thread]     24.)   this::InformAdapterWaitingForDone(==>StoppingPump)
//      [audio bg thread]     25.)     recoAdapter(as ISpxAudioProcessor)->ProcessAudio(nullptr, 0)
//      [audio bg thread]     26.)     recoAdapter(as ISpxAudioProcessor)->SetFormat(nullptr)
//      [audio bg thread]     27.) audioPump::ThreadProc exits
//                            <-- SHORT TIME LATER -->
//      [reco adapter thread] 28.) this::DoneProcessingAudio, then this::DoneProcessingAudio(Speech)
//      [reco adapter thread] 29.) this::ChangeState(WaitingForAdapterDone ==> Idle)
//      [reco adapter thread] 30.) this::FireSessionStoppedEvent
//


/*


+--------------------------------------------------------------
|   Just single shot
+--------------------------------------------------------------
 1  RecognizeAsync()
 2  _lambda WaitForRecognition/StopRecognizing
    -----------------------------------------------------------
 1  []/None/Idle                            ==>     []/SingleShot/StartingPump              (see this::StartRecognizing())
 1  []/SingleShot/StartingPump              ==>     []/SingleShot/ProcessingAudio           (see this::SetFormat(!nullptr))
 2  []/SingleShot/ProcessingAudio           ==>     []/SingleShot/StoppingPump              (see this::StopRecognizing)
 2  []/SingleShot/StoppingPump              ==>     []/SingleShot/WaitingForAdapterDone     (see this::SetFormat(nullptr))
 2  []/SingleShot/WaitingForAdapterDone     ==>     []/None/Idle                            (see this::DoneProcessingAudio())



+--------------------------------------------------------------
|   Just Continuous
+--------------------------------------------------------------
 3  StartContinuousRecognitionAsync()
 4  StopContinuousRecognitionAsync()
    -----------------------------------------------------------
 3  []/None/Idle                            ==>     []/Continuous/StartingPump              (see this::StartRecognizing())
 3  []/Continuous/StartingPump              ==>     []/Continuous/ProcessingAudio           (see this::SetFormat(!nullptr))
 4  []/Continuous/ProcessingAudio           ==>     []/Continuous/StoppingPump              (see this::StopRecognizing)
 4  []/Continuous/StoppingPump              ==>     []/Continuous/WaitingForAdapterDone     (see this::SetFormat(nullptr))
 4  []/Continuous/WaitingForAdapterDone     ==>     []/None/Idle                            (see this::DoneProcessingAudio())



+--------------------------------------------------------------
|   Just Keyword Spotting
+--------------------------------------------------------------
 5  StartKeywordRecognition()
 6  StopKeywordRecognition()
    -----------------------------------------------------------
 5  []/None/Idle                            ==>     [kw]/Keyword/StartingPump               (see this::StartRecognizing())
 5  [kw]/Keyword/StartingPump               ==>     [kw]/Keyword/ProcessingAudio            (see this::SetFormat(!nullptr))
 6  [kw]/Keyword/ProcessingAudio            ==>     []/Keyword/StoppingPump                 (see this::StopRecognizing)
 6  []/Keyword/StoppingPump                 ==>     []/Keyword/WaitingForAdapterDone        (see this::SetFormat(nullptr))
 6  []/Keyword/WaitingForAdapterDone        ==>     []/None/Idle                            (see this::DoneProcessingAudio())



+--------------------------------------------------------------
|   Keyword Spotting all the time, Manual SingleShot in the middle
+--------------------------------------------------------------
 5  StartKeywordRecognition()
 1  RecognizeAsync()
 2  _lambda WaitForRecognition/StopRecognizing
 6  StopKeywordRecognition()
    -----------------------------------------------------------
 5  []/None/Idle                            ==>     [kw]/Keyword/StartingPump               (see this::StartRecognizing(Keyword))
 5  [kw]/Keyword/StartingPump               ==>     [kw]/Keyword/ProcessingAudio            (see this::SetFormat(!nullptr))
 1  [kw]/Keyword/ProcessingAudio            ==>     [kw]/SingleShot/Paused                  (see this::StartRecognizing(SingleShot))
 1  [kw]/SingleShot/Paused                  ==>     [kw]/SingleShot/ProcessingAudio         (see this::DoneProcessingAudio(Keyword))
 2  [kw]/SingleShot/ProcessingAudio         ==>     [kw]/Keyword/Paused                     (see this::StopRecognizing(SingleShot))
 2  [kw]/Keyword/Paused                     ==>     [kw]/Keyword/ProcessingAudio            (see this::DoneProcessingAudio(SingleShot))
 6  [kw]/Keyword/ProcessingAudio            ==>     []/Keyword/StoppingPump                 (see this::StopRecognizing(Keyword))
 6  []/Keyword/StoppingPump                 ==>     []Keyword/WaitingForAdapterDone         (see this::SetFormat(nullptr))
 6  []/Keyword/WaitingForAdapterDone        ==>     []None/Idle                             (see this::DoneProcessingAudio(Keyword))


+--------------------------------------------------------------
|   Keyword Spotting all the time, KeywordDetected SingleShot in the middle
+--------------------------------------------------------------
 5  StartKeywordRecognition()
 7  KeywordDetected()
 8  _lambda WaitForRecognition/StopRecognizing
 6  StopKeywordRecognition()
    -----------------------------------------------------------
 5  []/None/Idle                            ==>     [kw]/Keyword/StartingPump               (see this::StartRecognizing(Keyword))
 5  [kw]/Keyword/StartingPump               ==>     [kw]/Keyword/ProcessingAudio            (see this::SetFormat(!nullptr))
 7  [kw]/Keyword/ProcessingAudio            ==>     [kw]/KwsSingleShot/Paused               (see this::KeywordDetected())
 7  [kw]/KwsSingleShot/Paused               ==>     [kw]/KwsSingleShot/ProcessingAudio      (see this::DoneProcessingAudio(Keyword))
 8  [kw]/KwsSingleShot/ProcessingAudio      ==>     [kw]/Keyword/WaitingForAdapterDone      (see this::StopRecognizing(KwsSingleShot))
 8  [kw]/Keyword/Paused                     ==>     [kw]/Keyword/ProcessingAudio            (see this::DoneProcessingAudio(KwsSingleShot))
 6  [kw]/Keyword/WaitingForAdapterDone      ==>     []/Keyword/StoppingPump                 (see this::StopRecognizing(Keyword))
 6  []/Keyword/StoppingPump                 ==>     []Keyword/WaitingForAdapterDone         (see this::SetFormat(nullptr))
 6  []/Keyword/WaitingForAdapterDone        ==>     []None/Idle                             (see this::DoneProcessingAudio(Keyword))



+--------------------------------------------------------------
|   Keyword Spotting all the time, Continuous in the middle
+--------------------------------------------------------------
 5  StartKeywordRecognition()
 3  StartContinuousRecognitionAsync()
 4  StopContinuousRecognitionAsync()
 6  StopKeywordRecognition()
    -----------------------------------------------------------
 5  []/None/Idle                            ==>     [kw]/Keyword/StartingPump               (see this::StartRecognizing(Keyword))
 5  [kw]/Keyword/StartingPump               ==>     [kw]/Keyword/ProcessingAudio            (see this::SetFormat(!nullptr))
 3  [kw]/Keyword/ProcessingAudio            ==>     [kw]/Continuous/Paused                  (see this::StartRecognizing(Continuous))
 3  [kw]/Continuous/Paused                  ==>     [kw]/Continuous/ProcessingAudio         (see this::DoneProcessingAudio(Keyword))
 4  [kw]/Continuous/ProcessingAudio         ==>     [kw]/Keyword/Paused                     (see this::StopRecognizing(Continuous)
 4  [kw]/Keyword/Paused                     ==>     [kw]/Keyword/ProcessingAudio            (see this::DoneProcessingAudio(Continuous))
 6  [kw]/Keyword/ProcessingAudio            ==>     []/Keyword/StoppingPump                 (see this::StopRecognizing(Keyword))
 6  []/Keyword/StoppingPump                 ==>     []Keyword/WaitingForAdapterDone         (see this::SetFormat(nullptr))
 6  []/Keyword/WaitingForAdapterDone        ==>     []None/Idle                             (see this::DoneProcessingAudio(Keyword))



+--------------------------------------------------------------
|   Continuous all the time, Keyword Spotting in the middle
+--------------------------------------------------------------
 3  StartContinuousRecognitionAsync()
 5  StartKeywordRecognition()
 6  StopKeywordRecognition()
 4  StopContinuousRecognitionAsync()
    -----------------------------------------------------------
 3  []/None/Idle                            ==>     []/Continuous/StartingPump              (see this::StartRecognizing(Continuous))
 3  []/Continuous/StartingPump              ==>     []/Continuous/ProcessingAudio           (see this::SetFormat(!nullptr))
 5  []/Continuous/ProcessingAudio           ==>     [kw]/Continuous/ProcessingAudio         (see this::StartRecognizing(Keyword))
 6  [kw]/Continuous/ProcessingAudio         ==>     []/Continuous/ProcessingAudio           (see this::StopRecognizing(Keyword))
 4  []/Continuous/ProcessingAudio           ==>     []/Continuous/StoppingPump              (see this::StopRecognizing(Continuous))
 4  []/Continuous/StoppingPump              ==>     []/Continuous/WaitingForAdapterDone     (see this::SetFormat(nullptr))
 4  []/Continuous/WaitingForAdapterDone     ==>     []/None/Idle                            (see this::DoneProcessingAudio())



+--------------------------------------------------------------
|   Staggered, start with Continuous
+--------------------------------------------------------------
 3  StartContinuousRecognitionAsync()
 5  StartKeywordRecognition()
 4  StopContinuousRecognitionAsync()
 6  StopKeywordRecognition()
    -----------------------------------------------------------
 3  []/None/Idle                            ==>     []/Continuous/StartingPump              (see this::StartRecognizing(Continuous))
 3  []/Continuous/StartingPump              ==>     []/Continuous/ProcessingAudio           (see this::SetFormat(!nullptr))
 5  []/Continuous/ProcessingAudio           ==>     [kw]/Continuous/ProcessingAudio         (see this::StartRecognizing(Keyword))
 4  [kw]/Continuous/ProcessingAudio         ==>     [kw]/Keyword/Paused                     (see this::StopRecognizing(Continuous))
 4  [kw]/Keyword/Paused                     ==>     [kw]/Keyword/ProcessingAudio            (see this::DoneProcessingAudio(Continuous))
 6  [kw]/Keyword/ProcessingAudio            ==>     []/Keyword/StoppingPump                 (see this::StopRecognizing(Keyword))
 6  []/Keyword/StoppingPump                 ==>     []Keyword/WaitingForAdapterDone         (see this::SetFormat(nullptr))
 6  []/Keyword/WaitingForAdapterDone        ==>     []None/Idle                             (see this::DoneProcessingAudio(Keyword))



+--------------------------------------------------------------
|   Staggered, start with Keyword Spotting
+--------------------------------------------------------------
 5  StartKeywordRecognition()
 3  StartContinuousRecognitionAsync()
 6  StopKeywordRecognition()
 4  StopContinuousRecognitionAsync()
    -----------------------------------------------------------
 5  []/None/Idle                            ==>     [kw]/Keyword/StartingPump               (see this::StartRecognizing(Keyword))
 5  [kw]/Keyword/StartingPump               ==>     [kw]/Keyword/ProcessingAudio            (see this::SetFormat(!nullptr))
 3  [kw]/Keyword/ProcessingAudio            ==>     [kw]/Continuous/Paused                  (see this::StartRecognizing(Continuous))
 3  [kw]/Continuous/Paused                  ==>     [kw]/Continuous/ProcessingAudio         (see this::DoneProcessingAudio(Keyword))
 6  [kw]/Continuous/ProcessingAudio         ==>     []/Continuous/ProcessingAudio           (see this::StopRecognizing(Keyword))
 4  []/Continuous/ProcessingAudio           ==>     []/Continuous/StoppingPump              (see this::StopRecognizing)
 4  []/Continuous/StoppingPump              ==>     []/Continuous/WaitingForAdapterDone     (see this::SetFormat(nullptr))
 4  []/Continuous/WaitingForAdapterDone     ==>     []/None/Idle                            (see this::DoneProcessingAudio(Continuous))



*/




#include "stdafx.h"
#include "audio_stream_session.h"
#include "create_object_helpers.h"
#include "site_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxAudioStreamSession::CSpxAudioStreamSession() :
    m_recoKind(RecognitionKind::Idle),
    m_sessionState(SessionState::Idle)
{
}

CSpxAudioStreamSession::~CSpxAudioStreamSession()
{
    SpxTermAndClear(m_kwsAdapter);
    SpxTermAndClear(m_recoAdapter);
    SpxTermAndClear(m_luAdapter);
}

void CSpxAudioStreamSession::Init() 
{
    // no-op.
}

void CSpxAudioStreamSession::Term()
{
    // adapters need to be terminated before the destructor is invoked,
    // otherwise, if destructor is in progress (waiting for an adapter callback to finish), 
    // GetSite() inside that adapter callbacks returns null.
    SpxTermAndClear(m_kwsAdapter);
    SpxTermAndClear(m_recoAdapter);
    SpxTermAndClear(m_luAdapter);
}

void CSpxAudioStreamSession::InitFromFile(const wchar_t* pszFileName)
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
    SPX_DBG_ASSERT(IsKind(RecognitionKind::Idle) && IsState(SessionState::Idle));
    SPX_DBG_TRACE_VERBOSE("%s: Now Idle ...", __FUNCTION__);

    // Create the wav file pump
    auto audioFilePump = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFilePump", this);
    m_audioPump = SpxQueryInterface<ISpxAudioPump>(audioFilePump);

    // Open the WAV file
    audioFilePump->Open(pszFileName);

    // Defer calling InitRecoEngineAdapter() or InitKwsEngineAdapter() until later ...
}

void CSpxAudioStreamSession::InitFromMicrophone()
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
    SPX_DBG_ASSERT(IsKind(RecognitionKind::Idle) && IsState(SessionState::Idle));
    SPX_DBG_TRACE_VERBOSE("%s: Now Idle ...", __FUNCTION__);

    // Create the microphone pump
    m_audioPump = SpxCreateObjectWithSite<ISpxAudioPump>("CSpxInteractiveMicrophone", this);

    // Defer calling InitRecoEngineAdapter() or InitKwsEngineAdapter() until later ...
}

void CSpxAudioStreamSession::InitFromStream(AudioInputStream* audioInputStream)
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
    SPX_DBG_ASSERT(IsKind(RecognitionKind::Idle) && IsState(SessionState::Idle));
    SPX_DBG_TRACE_VERBOSE("%s: Now Idle ...", __FUNCTION__);

    // Create the stream pump
    auto audioStreamPump = SpxCreateObjectWithSite<ISpxStreamPumpReaderInit>("CSpxStreamPump", this);
    m_audioPump = SpxQueryInterface<ISpxAudioPump>(audioStreamPump);

    // Attach the stream to the pump
    audioStreamPump->SetAudioStream(audioInputStream);

    // Defer calling InitRecoEngineAdapter() until later ... (see ::EnsureInitRecoEngineAdapter())
}

void CSpxAudioStreamSession::SetFormat(WAVEFORMATEX* pformat)
{
    // Since we're checking the RecoKind and SessionState multiple times, take a read lock
    ReadLock_Type readLock(m_combinedAdapterAndStateMutex);

    if (pformat != nullptr && ChangeState(SessionState::StartingPump, SessionState::ProcessingAudio))
    {
        // The pump started successfully, we have a live running session now!
        SPX_DBG_TRACE_VERBOSE("%s: Now ProcessingAudio ...", __FUNCTION__);

        SPX_DBG_ASSERT(readLock.owns_lock()); // Keep the lock to call InformAdapterStartingProcessingAudio()...
        InformAdapterStartingProcessingAudio(pformat);
    }
    else if (pformat == nullptr && ChangeState(SessionState::StoppingPump, SessionState::WaitingForAdapterDone))
    {
        // Our stop pump request has been satisfied... Let's wait for the adapter to finish...
        SPX_DBG_TRACE_VERBOSE("%s: Now WaitingForAdapterDone (from StoppingPump)...", __FUNCTION__);

        SPX_DBG_ASSERT(readLock.owns_lock()); // Keep the lock to call InformAdapterWaitingForDone()...
        InformAdapterWaitingForDone(SessionState::StoppingPump);
    }
    else if (pformat == nullptr && ChangeState(SessionState::ProcessingAudio, SessionState::WaitingForAdapterDone))
    {
        // The pump stopped itself... That's possible when WAV files reach EOS. Let's wait for the adapter to finish...
        SPX_DBG_TRACE_VERBOSE("%s: Now WaitingForAdapterDone (from ProcessingAudio) ...", __FUNCTION__);

        SPX_DBG_ASSERT(readLock.owns_lock()); // Keep the lock to call InformAdapterWaitingForDone()...
        InformAdapterWaitingForDone(SessionState::ProcessingAudio);
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
    // NOTE: Don't hold the m_combinedAdapterAndStateMutex here...
    ReadLock_Type readStateLock(m_stateMutex);
    auto sessionState = m_sessionState;
    readStateLock.unlock();

    if (sessionState == SessionState::ProcessingAudio)
    {
        // Go ahead and process the audio data
        SPX_DBG_TRACE_VERBOSE_IF(0, "%s - size=%d", __FUNCTION__, size);
        m_audioProcessor->ProcessAudio(data, size);
    }
    else if (sessionState == SessionState::Paused)
    {
        // Don't process this data, if we're paused... 
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s - size=%d -- Ignoring (state == Paused)", __FUNCTION__, size);
        // TODO: RobCh: Actually do something with this paused audio data ... 
    }
    else if (sessionState == SessionState::StoppingPump)
    {
        // Don't process this data if we're actively stopping...
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s - size=%d -- Ignoring (state == StoppingPump)", __FUNCTION__, size);
    }
    else
    {
        // Valid states to encounter are:
        //
        // - ProcessingAudio: We're allowed to process audio while in this state
        // - StoppingPump: We're allowed to be called to process audio, but we'll ignore the data passed in while we're attempting to stop the pump
        // - Paused: We're allowed to be called to process audio, but we'll ignore that data (when we un-pause, we'll pick back up at the appropriate location)
        //
        // NOTE: All other states are invalid inside ISpxAudioProcessor::ProcessAudio.

        SPX_TRACE_WARNING("%s: Unexpected SessionState: recoKind %d; sessionState %d", __FUNCTION__, m_recoKind, sessionState);
    }
}

void CSpxAudioStreamSession::StartRecognizing(RecognitionKind startKind, std::wstring keyword)
{
    // Since we're checking the RecoKind and SessionState multiple times, take a read lock
    ReadLock_Type readLock(m_combinedAdapterAndStateMutex);

    if (ChangeState(RecognitionKind::Idle, SessionState::Idle, startKind, SessionState::StartingPump))
    {
        // We're starting from idle!! Let's get the Audio Pump running
        SPX_DBG_TRACE_VERBOSE("%s: Now StartingPump ...", __FUNCTION__);

        readLock.unlock(); // StartAudioPump() will take a write lock, so we need to clear our read lock
        StartAudioPump(startKind, keyword);

        // Calling StartAudioPump(), ultimately initiates a call to this::SetFormat(waveformat) which is
        // where we'll complete the state transition to startKind/ProcessingAudio
    }
    else if (ChangeState(RecognitionKind::Keyword, SessionState::ProcessingAudio, startKind, SessionState::Paused))
    {
        // We're moving from Keyword/ProcessingAudio to startKind/Paused... 
        SPX_DBG_TRACE_VERBOSE("%s: Now Paused ...", __FUNCTION__);

        readLock.unlock(); // HotSwapAdaptersWhilePaused() will take a write lock, so we need to clear our read lock
        HotSwapAdaptersWhilePaused(startKind, keyword);

        // Calling HotSwapAdaptersWhilePaused() will instigate a call to this::DoneProcessingAudio(Keyword) which
        // is where we'll complete changing state to startKind/ProcessingAudio
    }
    else if (startKind == RecognitionKind::Keyword && !IsKind(RecognitionKind::Keyword) && IsState(SessionState::ProcessingAudio))
    {
        // We're already doing something other than keyword spotting, but, someone wants to change/update the keyword ...
        // So ... let's update it
        SPX_DBG_TRACE_VERBOSE("%s: Changing keyword to '%S' ... nothing else...", __FUNCTION__, keyword.c_str());
        m_keyword = keyword;
    }
    else
    {
        // All other state transitions are invalid when attempting to start recognizing...
        SPX_TRACE_ERROR("%s: Unexpected/Invalid State Transition: recoKind %d; sessionState %d", __FUNCTION__, m_recoKind, m_sessionState);
        SPX_THROW_HR(SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION);
    }
}

void CSpxAudioStreamSession::StopRecognizing(RecognitionKind stopKind)
{
    // Since we're checking the RecoKind and SessionState multiple times, take a read lock
    ReadLock_Type readLock(m_combinedAdapterAndStateMutex);
    SPX_DBG_ASSERT(stopKind != RecognitionKind::Idle);

    if (!m_keyword.empty() && stopKind != RecognitionKind::Keyword && IsState(SessionState::ProcessingAudio) &&
        ChangeState(stopKind, SessionState::ProcessingAudio, RecognitionKind::Keyword, SessionState::Paused))
    {
        // We've got a keyword, we're not trying to stop keyword spotting, and we're currently procesing audio...
        // So ... We should hot swap over to the keyword spotter (which will stop the current adapter doing whatever its doing)
        SPX_DBG_TRACE_VERBOSE("%s: Now Keyword/Paused ...", __FUNCTION__);

        readLock.unlock(); // HotSwapAdaptersWhilePaused() will take a write lock, so we need to clear our read lock
        HotSwapAdaptersWhilePaused(RecognitionKind::Keyword, m_keyword);
        if (stopKind == RecognitionKind::KwsSingleShot || stopKind == RecognitionKind::SingleShot)
        {
            FireSessionStoppedEvent();
        }

        // Calling HotSwapAdaptersWhilePaused() will instigate a call to this::DoneProcessingAudio(Speech), 
        // where we'll complete changing state to Keyword/ProcessingAudio
    }
    else if (stopKind == RecognitionKind::Keyword && IsKind(RecognitionKind::Keyword) &&
             ChangeState(stopKind, SessionState::ProcessingAudio, stopKind, SessionState::StoppingPump))
    {
        // We're actually keyword spotting right now, and we've been asked to stop doing that...
        // So ... Let's clear our keyword (since we're not spotting, we don't need it anymore)
        SPX_DBG_TRACE_VERBOSE("%s: Changing keyword to '' ... nothing else...", __FUNCTION__);
        m_keyword.clear();

        // And we'll stop the pump
        SPX_DBG_TRACE_VERBOSE("%s: Now StoppingPump ...", __FUNCTION__);
        m_audioPump->StopPump();

        // Calling StopPump() will instigate a call to this::DoneProcessingAudio(Keyword), where
        // we'll complete the changing state to Idle/Idle
    }
    else if (stopKind == RecognitionKind::Keyword && !IsKind(RecognitionKind::Keyword))
    {
        // We've been asked to stop keyword spotting, but we're not keyword spotting right now ...
        // So ... Let's just clear the keyword
        SPX_DBG_TRACE_VERBOSE("%s: Changing keyword to '' ... nothing else...", __FUNCTION__);
        m_keyword.clear();
    }
    else if (stopKind == RecognitionKind::KwsSingleShot && IsKind(RecognitionKind::KwsSingleShot) &&
             ChangeState(stopKind, SessionState::ProcessingAudio, stopKind, SessionState::WaitingForAdapterDone))
    {
        // TODO: RobCh: Is this ever called?!! How does it not get eaten by the first if statement above?!!

        // We're going to wait for done ... (which will flip us back to wherever we need to go...)
        SPX_DBG_TRACE_VERBOSE("%s: Now KwsSingleShot/WaitingForAdapterDone ...", __FUNCTION__);
    }
    else if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
    {
        // We've been asked to stop whatever it is we're doing, while we're actively processing audio ...
        // So ... Let's stop the pump
        SPX_DBG_TRACE_VERBOSE("%s: Now StoppingPump ...", __FUNCTION__);
        m_audioPump->StopPump();

        // Calling StopPump() will instigate a call to this::DoneProcessingAudio(), where
        // we'll complete the changing state to Idle/Idle
    }
    else if (IsState(SessionState::WaitingForAdapterDone))
    {
        // If we're already in the WaitingForAdapterDone state... That's fine ... We'll eventually transit to Idle once DoneProcessingAudio() is called...
        SPX_DBG_TRACE_VERBOSE("%s: Now (still) WaitingForDone ...", __FUNCTION__);
    }
    else if (IsKind(RecognitionKind::Idle) && IsState(SessionState::Idle))
    {
        // If we're already in the idle state... Awesome!!!
        SPX_DBG_TRACE_VERBOSE("%s: Now Idle/Idle ...", __FUNCTION__);
    }
    else
    {
        SPX_TRACE_WARNING("%s: Unexpected State: recoKind %d; sessionState %d", __FUNCTION__, m_recoKind, m_sessionState);
    }
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::WaitForRecognition()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    SPX_DBG_TRACE_VERBOSE("Waiting for Recognition...");
    m_cv.wait_for(lock, std::chrono::seconds(m_recoAsyncTimeout), [&] { return !m_recoAsyncWaiting; });
    SPX_DBG_TRACE_VERBOSE("Waiting for Recognition... Done!");

    if (!m_recoAsyncResult) // Deal with the timeout condition...
    {
        SPX_DBG_TRACE_VERBOSE("%s: Timed out waiting for recognition... ", __FUNCTION__);

        // We'll need to stop the pump ourselves...
        if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
        {
            SPX_DBG_TRACE_VERBOSE("%s: Now StoppingPump ...", __FUNCTION__);

            lock.unlock();
            m_audioPump->StopPump();
            lock.lock();
        }

        /*
        lock.unlock();
        EnsureFireResultEvent();
        lock.lock();
        */

        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterDone...");
        m_cv.wait_for(lock, std::chrono::seconds(m_waitForDoneTimeout), [&] {
            return !this->IsState(SessionState::StoppingPump) && !this->IsState(SessionState::WaitingForAdapterDone);
        });
        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterDone... Done!!");
    }

    return std::move(m_recoAsyncResult);
}

void CSpxAudioStreamSession::KeywordDetected(ISpxKwsEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);

    SPX_DBG_TRACE_VERBOSE("Keyword detected!! Starting KwsSingleShot recognition...");
    if (ChangeState(RecognitionKind::Keyword, SessionState::ProcessingAudio, RecognitionKind::KwsSingleShot, SessionState::Paused))
    {
        // We've been told a keyword was recognized... Start KwsSingleShot recognition!!
        SPX_DBG_TRACE_VERBOSE("%s: Now KwsSingleShot/Paused ...", __FUNCTION__);
        HotSwapToKwsSingleShotWhilePaused();

        // Calling StartRecognizingKwsSingleShot() will instigate a call to HotSwapAdaptersWhilePaused(), which will
        // instigate a call to DoneProcessingAudio(Keyword) where we'll complete changing state to KwsSingleShot/ProcessingAudio
    }
}

void CSpxAudioStreamSession::DoneProcessingAudio(ISpxKwsEngineAdapter* adapter)
{
    UNUSED(adapter);
    DoneProcessingAudio(AdapterDoneProcessingAudio::Keyword);
}

std::list<std::string> CSpxAudioStreamSession::GetListenForList()
{
    return m_luAdapter != nullptr
        ? GetListenForListFromLuEngineAdapter()
        : std::list<std::string>();
}

void CSpxAudioStreamSession::GetIntentInfo(std::string& provider, std::string& id, std::string& key)
{
    if (m_luAdapter != nullptr)
    {
        GetIntentInfoFromLuEngineAdapter(provider, id, key);
    }
}

void CSpxAudioStreamSession::SpeechStartDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);
    
    FireSpeechStartDetectedEvent();
}

void CSpxAudioStreamSession::SpeechEndDetected(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);

    FireSpeechEndDetectedEvent();
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

    auto argsInit = SpxQueryInterface<ISpxSessionEventArgsInit>(sessionEvent);
    argsInit->Init(sessionId);

    return sessionEvent;
}

std::shared_ptr<ISpxRecognitionEventArgs> CSpxAudioStreamSession::CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    auto site = SpxSiteFromThis(this);
    auto recoEvent = SpxCreateObjectWithSite<ISpxRecognitionEventArgs>("CSpxRecognitionEventArgs", site);

    auto argsInit = SpxQueryInterface<ISpxRecognitionEventArgsInit>(recoEvent);
    argsInit->Init(sessionId, result);

    return recoEvent;
}

void CSpxAudioStreamSession::IntermediateRecoResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    UNUSED(offset);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::Idle), "ERROR! IntermediateRecoResult was called with SessionState==Idle");
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::StartingPump), "ERROR! IntermediateRecoResult was called with SessionState==StartingPump");

    FireResultEvent(GetSessionId(), result);
}

void CSpxAudioStreamSession::TranslationSynthesisResult(ISpxRecoEngineAdapter* adapter, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::Idle), "ERROR! TranslationSynthesisResult was called with SessionState==Idle");
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::StartingPump), "ERROR! TranslationSynthesisResult was called with SessionState==StartingPump");

    FireResultEvent(GetSessionId(), result);
}

void CSpxAudioStreamSession::FinalRecoResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    UNUSED(offset);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::Idle), "ERROR! FinalRecoResult was called with SessionState==Idle");
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::StartingPump), "ERROR! FinalRecoResult was called with SessionState==StartingPump");

    // Only try and process the result with the LU Adapter if we have one (we won't have one, if nobody every added an IntentTrigger)
    if (m_luAdapter != nullptr)
    {
        m_luAdapter->ProcessResult(result);
    }

    // Todo: For translation, this means that RecognizeAsync() only returns text result, but no audio result. Audio result
    // has to be received via callbacks.
    // Waiting for Rob's change for direct LUIS integration, which introduces a state machine in usp_reco_engine.
    WaitForRecognition_Complete(result);
}

void CSpxAudioStreamSession::DoneProcessingAudio(ISpxRecoEngineAdapter* adapter)
{
    UNUSED(adapter);
    DoneProcessingAudio(AdapterDoneProcessingAudio::Speech);
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
    if (m_recoAsyncWaiting)
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
        auto error = factory->CreateErrorResult(PAL::ToWString(payload).c_str());
        WaitForRecognition_Complete(error);
    }
}

std::shared_ptr<ISpxSession> CSpxAudioStreamSession::GetDefaultSession()
{
    return SpxSharedPtrFromThis<ISpxSession>(this);
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitIntermediateResult(resultId, text, type);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateFinalResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(resultId, text, type);

    return result;
}


std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateNoMatchResult(enum ResultType type)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitNoMatch(type);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateErrorResult(const wchar_t* text)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    SpxQueryInterface<ISpxRecognitionResultInit>(result)->InitError(text);

    return result;
}

std::shared_ptr<ISpxRecoEngineAdapter> CSpxAudioStreamSession::EnsureInitRecoEngineAdapter()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_recoAdapter == nullptr)
    {
        InitRecoEngineAdapter();
    }

    return m_recoAdapter;
}

void CSpxAudioStreamSession::InitRecoEngineAdapter()
{
    // determine which type (or types) of reco engine adapters we should try creating...
    bool tryUnidec = GetBooleanValue(L"CARBON-INTERNAL-UseRecoEngine-Unidec", false);
    bool tryMock = GetBooleanValue(L"CARBON-INTERNAL-UseRecoEngine-Mock", false);
    bool tryUsp = GetBooleanValue(L"CARBON-INTERNAL-UseRecoEngine-Usp", false);

    // if nobody specified which type(s) of reco engine adapters this session should use, we'll use the USP
    if (!tryUnidec && !tryMock && !tryUsp)
    {
        tryUsp = true;
    }

    // try to create the Unidec adapter... 
    if (m_recoAdapter == nullptr && tryUnidec)
    {
        m_recoAdapter = SpxCreateObjectWithSite<ISpxRecoEngineAdapter>("CSpxUnidecRecoEngineAdapter", this);
    }

    // try to create the Usp adapter... 
    if (m_recoAdapter == nullptr && tryUsp)
    {
        m_recoAdapter = SpxCreateObjectWithSite<ISpxRecoEngineAdapter>("CSpxUspRecoEngineAdapter", this);
    }

    // try to create the mock reco engine adapter...
    if (m_recoAdapter == nullptr && tryMock)
    {
        m_recoAdapter = SpxCreateObjectWithSite<ISpxRecoEngineAdapter>("CSpxMockRecoEngineAdapter", this);
    }

    // if we still don't have an adapter... that's an exception
    SPX_IFTRUE_THROW_HR(m_recoAdapter == nullptr, SPXERR_NOT_FOUND);
}

std::shared_ptr<ISpxKwsEngineAdapter> CSpxAudioStreamSession::EnsureInitKwsEngineAdapter(const std::wstring& keyword)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_kwsAdapter == nullptr || keyword != m_keyword)
    {
        SpxTermAndClear(m_kwsAdapter);
        InitKwsEngineAdapter(keyword);
    }

    return m_kwsAdapter;
}

void CSpxAudioStreamSession::InitKwsEngineAdapter(const std::wstring& keyword)
{
    m_keyword = keyword;

    // determine which type (or types) of reco engine adapters we should try creating...
    bool tryCortana = GetBooleanValue(L"CARBON-INTERNAL-UseKwsEngine-Cortana", false);
    bool tryMock = GetBooleanValue(L"CARBON-INTERNAL-UseKwsEngine-Mock", false);
    bool trySdk = GetBooleanValue(L"CARBON-INTERNAL-UseKwsEngine-Sdk", false);

    // if nobody specified which type(s) of reco engine adapters this session should use, we'll use the SDK KWS engine
    if (!tryCortana && !tryMock && !trySdk)
    {
        trySdk = true;
        tryMock = true;
    }

    // try to create the Cortana adapter... 
    if (m_kwsAdapter == nullptr && tryCortana)
    {
        m_kwsAdapter = SpxCreateObjectWithSite<ISpxKwsEngineAdapter>("CSpxCortanaKwsEngineAdapter", this);
    }

    // try to create the SDK adapter... 
    if (m_kwsAdapter == nullptr && trySdk)
    {
        m_kwsAdapter = SpxCreateObjectWithSite<ISpxKwsEngineAdapter>("CSpxSdkKwsEngineAdapter", this);
    }

    // try to create the mock adapter...
    if (m_kwsAdapter == nullptr && tryMock)
    {
        m_kwsAdapter = SpxCreateObjectWithSite<ISpxKwsEngineAdapter>("CSpxMockKwsEngineAdapter", this);
    }

    // if we still don't have an adapter... that's an exception
    SPX_IFTRUE_THROW_HR(m_kwsAdapter == nullptr, SPXERR_NOT_FOUND);
}

void CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused()
{
    // We need to do all this work on a background thread, because we can't guarantee it's safe
    // to spend any significant amount of time blocking this the KWS or Audio threads...
    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<void()> task([=](){

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused kicked-off THREAD started ***", "*** CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        // Keep track of the fact that we have a thread pending waiting for the single
        // shot recognition to complete (so it can call StopRecognizing(KwsSingleShot)) and
        // then wait for done... 
        m_recoAsyncWaiting = true;

        SPX_DBG_ASSERT(IsKind(RecognitionKind::KwsSingleShot) && IsState(SessionState::Paused));
        this->HotSwapAdaptersWhilePaused(RecognitionKind::KwsSingleShot);

        // Calling HotSwapAdaptersWhilePaused() will instigate a call to this::DoneProcessingAudio(), 
        // where we'll complete changing state to KwsSingleShot/ProcessingAudio

        this->WaitForKwsSingleShotRecognition();
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();
}

void CSpxAudioStreamSession::WaitForKwsSingleShotRecognition()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    SPX_DBG_TRACE_VERBOSE("Waiting for KwsSingleShot Recognition...");
    m_cv.wait_for(lock, std::chrono::seconds(m_recoAsyncTimeout), [&] { return !m_recoAsyncWaiting; });
    SPX_DBG_TRACE_VERBOSE("Waiting for KwsSingleShot Recognition... Done!");

    lock.unlock();
    this->StopRecognizing(RecognitionKind::KwsSingleShot);
    lock.lock();

    if (!m_recoAsyncResult) // Deal with the timeout condition...
    {
        SPX_DBG_TRACE_VERBOSE("KwsSingleShot Waiting for AdapterDone...");
        m_cv.wait_for(lock, std::chrono::seconds(m_waitForDoneTimeout), [&] { return !this->IsState(SessionState::WaitingForAdapterDone); });
        SPX_DBG_TRACE_VERBOSE("KwsSingleShot Waiting for AdapterDone... Done!!");
    }
}

void CSpxAudioStreamSession::StartAudioPump(RecognitionKind startKind, const std::wstring& keyword)
{
    SPX_DBG_ASSERT(IsState(SessionState::StartingPump));

    // Tell everyone we're starting...
    if (startKind != RecognitionKind::Keyword)
    {
        FireSessionStartedEvent();
    }

    // Lock the Audio Processor mutex, since we'll be picking which one we're using...
    WriteLock_Type writeLock(m_combinedAdapterAndStateMutex);

    // Depending on the startKind, we'll either use the Kws Engine Adapter or the Reco Engine Adapter
    m_audioProcessor = startKind == RecognitionKind::Keyword
        ? SpxQueryInterface<ISpxAudioProcessor>(EnsureInitKwsEngineAdapter(keyword))
        : SpxQueryInterface<ISpxAudioProcessor>(EnsureInitRecoEngineAdapter());

    writeLock.unlock(); // DON'T HOLD THE LOCK any longer than here. 
    // The StartPump() call (see below) will instigate a call to this::SetFormat, which will try to acquire the reader lock from a different thread...

    // Start pumping audio data from the pump, to the Audio Stream Session
    auto ptr = (ISpxAudioProcessor*)this;
    auto pISpxAudioProcessor = ptr->shared_from_this();
    m_audioPump->StartPump(pISpxAudioProcessor);

    // The call to StartPump (immediately above) will initiate calls from the pump to this::SetFormat() and then this::ProcessAudio()... 
}

void CSpxAudioStreamSession::HotSwapAdaptersWhilePaused(RecognitionKind startKind, const std::wstring& keyword)
{
    SPX_DBG_ASSERT(IsKind(startKind) && IsState(SessionState::Paused));
    SPX_DBG_ASSERT(m_audioProcessor != nullptr);

    // Lock the Audio Processor mutex, since we're switching which one we'll be using...
    WriteLock_Type writeLock(m_combinedAdapterAndStateMutex);
    auto oldAudioProcessor = m_audioProcessor;

    // Depending on the startKind, we'll either switch to the Kws Engine Adapter or the Reco Engine Adapter
    m_audioProcessor = startKind == RecognitionKind::Keyword
        ? SpxQueryInterface<ISpxAudioProcessor>(EnsureInitKwsEngineAdapter(keyword))
        : SpxQueryInterface<ISpxAudioProcessor>(EnsureInitRecoEngineAdapter());

    // Tell the old Audio Processor that we've sent the last bit of audio data we're going to send
    SPX_DBG_TRACE_VERBOSE_IF(1, "%s: ProcessingAudio - size=%d", __FUNCTION__, 0);
    oldAudioProcessor->ProcessAudio(nullptr, 0);

    // Then tell it we're finally done, by sending a nullptr WAVEFORMAT
    oldAudioProcessor->SetFormat(nullptr);

    // Get the format in which the pump is currently sending audio data to the Session
    auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    m_audioPump->GetFormat(waveformat.get(), cbFormat);

    // Inform the Audio Processor that we're starting...
    SPX_DBG_ASSERT(writeLock.owns_lock());
    InformAdapterStartingProcessingAudio(waveformat.get());

    // The call to ProcessAudio(nullptr, 0) and SetFormat(nullptr) will instigate a call from that Adapter to this::DoneProcessingAudio(adapter) shortly...
}

void CSpxAudioStreamSession::InformAdapterStartingProcessingAudio(WAVEFORMATEX* format)
{
    // NOTE: Callers will have already taken a reader or writer lock... Do NOT take a lock here ourselves!
    // ReadLock_Type readerLock(m_combinedAdapterAndStateMutex);

    SPX_DBG_ASSERT(format != nullptr);
    m_audioProcessor->SetFormat(format);
}

void CSpxAudioStreamSession::InformAdapterWaitingForDone(SessionState comingFromState)
{
    // NOTE: Callers will have already taken a reader or writer lock... Do NOT take a lock here ourselves!
    // ReadLock_Type readerLock(m_combinedAdapterAndStateMutex);

    // If we transitioned --> WaitingForAdapterDone state because this Session was trying to stop the pump...
    // We need to tell the audio processor that we've sent the last bit of audio we're planning on sending
    SPX_DBG_ASSERT(comingFromState == SessionState::ProcessingAudio || comingFromState == SessionState::StoppingPump);
    if (comingFromState == SessionState::StoppingPump)
    {
        SPX_DBG_TRACE_VERBOSE("%s: ProcessingAudio - size=%d", __FUNCTION__, 0);
        m_audioProcessor->ProcessAudio(nullptr, 0);
    }

    // Then we can finally tell it we're done, by sending a nullptr WAVEFORMAT
    SPX_DBG_TRACE_VERBOSE("%s: SetFormat(nullptr)", __FUNCTION__);
    m_audioProcessor->SetFormat(nullptr);
}

void CSpxAudioStreamSession::DoneProcessingAudio(AdapterDoneProcessingAudio doneAdapter)
{
    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<void()> task([=]() mutable {

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::DoneProcessingAudio kicked-off THREAD started ***", "*** CSpxAudioStreamSession::DoneProcessingAudio kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        // Since we're checking the RecoKind and SessionState multiple times, take a read lock
        ReadLock_Type readLock(m_combinedAdapterAndStateMutex);

        if (ChangeState(RecognitionKind::KwsSingleShot, SessionState::WaitingForAdapterDone, RecognitionKind::Keyword, SessionState::ProcessingAudio))
        {
            SPX_DBG_TRACE_VERBOSE("KwsSingleShot Waiting for done ... Done!! Switching back to Keyword/Processing");
            SPX_DBG_TRACE_VERBOSE("%s: Now Keyword/ProcessingAudio ...", __FUNCTION__);

            FireSessionStoppedEvent();
        }
        else if (ChangeState(SessionState::Paused, SessionState::ProcessingAudio))
        {
            SPX_DBG_TRACE_VERBOSE("Previous Adapter is done processing audio ... resuming Processing with the new adapter...");
            SPX_DBG_TRACE_VERBOSE("%s: Now ProcessingAudio ...", __FUNCTION__);

            if (doneAdapter == AdapterDoneProcessingAudio::Keyword && IsKind(RecognitionKind::KwsSingleShot))
            {
                FireSessionStartedEvent();
            }

            // TODO: RobCh: How should we get the audio from the kws and give it to other adapter? 
            // TODO: RobCh: Similarly, how should we  rewind the audio when the USP reco adatper finishes, and we want to restart kws?
        }
        else if (ChangeState(SessionState::WaitingForAdapterDone, RecognitionKind::Idle, SessionState::Idle))
        {
            if (doneAdapter == AdapterDoneProcessingAudio::Speech)
            {
                // The Reco Engine adapter request to finish processing audio has completed, that signifies that the "session" has stopped
                FireSessionStoppedEvent();

                // Restart the keyword spotter if necessary...
                if (!m_keyword.empty() && ChangeState(SessionState::Idle, RecognitionKind::Keyword, SessionState::StartingPump))
                {
                    // Go ahead re-start the pump
                    SPX_DBG_TRACE_VERBOSE("%s: Now StartingPump ...", __FUNCTION__);

                    readLock.unlock(); // StartAudioPump() will take a write lock, so we need to clear our read lock
                    StartAudioPump(RecognitionKind::Keyword, m_keyword);
                }
            }
        }
        else
        {
            SPX_DBG_TRACE_WARNING("%s: Is this OK? doneAdapter=%d; sessionState=%d **************************", __FUNCTION__, doneAdapter, m_sessionState);
        }
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();
}

bool CSpxAudioStreamSession::IsKind(RecognitionKind kind)
{
    ReadLock_Type readStateLock(m_stateMutex);
    return m_recoKind == kind;
}

bool CSpxAudioStreamSession::IsState(SessionState state)
{
    ReadLock_Type readStateLock(m_stateMutex);
    return m_sessionState == state;
}

bool CSpxAudioStreamSession::ChangeState(SessionState sessionStateFrom, SessionState sessionStateTo)
{
    auto recoKind = m_recoKind;
    return ChangeState(recoKind, sessionStateFrom, recoKind, sessionStateTo);
}

bool CSpxAudioStreamSession::ChangeState(SessionState sessionStateFrom, RecognitionKind recoKindTo, SessionState sessionStateTo)
{
    auto recoKindFrom = m_recoKind;
    return ChangeState(recoKindFrom, sessionStateFrom, recoKindTo, sessionStateTo);
}

bool CSpxAudioStreamSession::ChangeState(RecognitionKind recoKindFrom, SessionState sessionStateFrom, RecognitionKind recoKindTo, SessionState sessionStateTo)
{
    // Check to see if the Audio StreamState and RecognitionKind match...
    ReadLock_Type readLock(m_stateMutex);
    if (m_sessionState == sessionStateFrom && m_recoKind == recoKindFrom)
    {
        // If they do, upgrade our lock to a writer lock, check again, and if they still match, ChangeStates as requested
        readLock.unlock();
        WriteLock_Type writeLock(m_stateMutex);
        if (m_sessionState == sessionStateFrom && m_recoKind == recoKindFrom)
        {
            SPX_DBG_TRACE_VERBOSE("%s; recoKind/sessionState: %d/%d => %d/%d", __FUNCTION__, recoKindFrom, sessionStateFrom, recoKindTo, sessionStateTo);
            m_sessionState = sessionStateTo;
            m_recoKind = recoKindTo;
            m_cv.notify_all();
            return true;
        }
    }

    return false;
}

void CSpxAudioStreamSession::EnsureInitLuEngineAdapter()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_luAdapter == nullptr)
    {
        InitLuEngineAdapter();
    }
}

void CSpxAudioStreamSession::InitLuEngineAdapter()
{
    SPX_IFTRUE_THROW_HR(m_luAdapter != nullptr, SPXERR_ALREADY_INITIALIZED);

    // determine which type (or types) of reco engine adapters we should try creating...
    bool tryLuisDirect = GetBooleanValue(L"CARBON-INTERNAL-UseLuEngine-LuisDirect", false);
    bool tryMock = GetBooleanValue(L"CARBON-INTERNAL-UseLuEngine-Mock", false);

    // if nobody specified which type(s) of LU engine adapters this session should use, we'll use LuisDirect
    if (!tryLuisDirect && !tryMock)
    {
        tryLuisDirect = true;
    }

    // try to create the Luis Direct adapter
    if (m_luAdapter == nullptr && tryLuisDirect)
    {
        m_luAdapter = SpxCreateObjectWithSite<ISpxLuEngineAdapter>("CSpxLuisDirectEngineAdapter", this);
    }

    // try to create the mock adapter...
    if (m_luAdapter == nullptr && tryMock)
    {
        m_luAdapter = SpxCreateObjectWithSite<ISpxLuEngineAdapter>("CSpxMockLuEngineAdapter", this);
    }

    // if we still don't have an adapter... that's an exception
    SPX_IFTRUE_THROW_HR(m_luAdapter == nullptr, SPXERR_NOT_FOUND);
}

std::list<std::string> CSpxAudioStreamSession::GetListenForListFromLuEngineAdapter()
{
    SPX_DBG_ASSERT(m_luAdapter != nullptr);
    auto triggerService = SpxQueryInterface<ISpxIntentTriggerService>(m_luAdapter);
    return triggerService->GetListenForList();
}

void CSpxAudioStreamSession::GetIntentInfoFromLuEngineAdapter(std::string& provider, std::string& id, std::string& key)
{
    SPX_DBG_ASSERT(GetLuEngineAdapter() != nullptr);
    auto triggerService = SpxQueryInterface<ISpxIntentTriggerService>(m_luAdapter);
    return triggerService->GetIntentInfo(provider, id, key);
}

std::shared_ptr<ISpxLuEngineAdapter> CSpxAudioStreamSession::GetLuEngineAdapter()
{
    EnsureInitLuEngineAdapter();
    return m_luAdapter;
}

std::shared_ptr<ISpxNamedProperties> CSpxAudioStreamSession::GetParentProperties()
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.cpp: Implementation definitions for CSpxAudioStreamSession C++ class
//

#include "stdafx.h"
#include <future>
#include <list>
#include <memory>
#include "audio_stream_session.h"
#include "spxcore_common.h"
#include "asyncop.h"
#include "create_object_helpers.h"
#include "guid_utils.h"
#include "string_utils.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "site_helpers.h"
#include "service_helpers.h"
#include "property_id_2_name_map.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

constexpr std::chrono::seconds CSpxAudioStreamSession::MaxBufferedBeforeOverflow;
constexpr std::chrono::milliseconds CSpxAudioStreamSession::MaxBufferedBeforeSimulateRealtime;
constexpr std::chrono::seconds CSpxAudioStreamSession::ShutdownTimeout;

CSpxAudioStreamSession::CSpxAudioStreamSession() :
    m_sessionId(PAL::CreateGuidWithoutDashes()),
    m_recoKind(RecognitionKind::Idle),
    m_sessionState(SessionState::Idle),
    m_expectAdapterStartedTurn(false),
    m_expectAdapterStoppedTurn(false),
    m_adapterAudioMuted(false),
    m_turnEndStopKind(RecognitionKind::Idle),
    m_recoAsyncWaiting(false)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxAudioStreamSession::~CSpxAudioStreamSession()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_ASSERT(m_kwsAdapter == nullptr);
    SPX_DBG_ASSERT(m_recoAdapter == nullptr);
    SPX_DBG_ASSERT(m_luAdapter == nullptr);
}

void CSpxAudioStreamSession::Init()
{
    // no-op.
}

void CSpxAudioStreamSession::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    // To "protect" against shutdown issues...
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Make sure no more async tasks are still needing to be completed...
    m_taskHelper.RunAsyncWaitAndClear();

    // Let's check to see if we're still processing audio ...
    WriteLock_Type writeLock(m_combinedAdapterAndStateMutex);
    if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
    {
        writeLock.unlock(); // can't hold the lock while we're shutting down...

        // We're terminating, and we were still processing audio ... So ... Let's shut down the pump
        SPX_DBG_TRACE_VERBOSE("%s: Now StoppingPump ...", __FUNCTION__);
        m_audioPump->StopPump();

        // Let's wait until we're actually stopped...
        WaitForIdle();
    }

    // Don't need the recognizers any more...
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_recognizers.clear();
    }

    // Destroy the pump and all the adapters... (we can't hold any shared_ptr's after this function returns...)
    SpxTermAndClear(m_audioPump);
    SpxTermAndClear(m_kwsAdapter);
    SpxTermAndClear(m_recoAdapter);
    SpxTermAndClear(m_luAdapter);

    if (!writeLock.owns_lock())
        writeLock.lock();

    m_audioProcessor = nullptr;
    m_audioBuffer = nullptr;
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

    // Limit the maximal speed to 2 times of real-time streaming
    audioFilePump->SetRealTimePercentage(SimulateRealtimePercentage);

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

    // Limit the maximal speed to 2 times of real-time streaming
    audioStreamPump->SetRealTimePercentage(50);

    // Defer calling InitRecoEngineAdapter() until later ... (see ::EnsureInitRecoEngineAdapter())
}

void CSpxAudioStreamSession::SetFormat(WAVEFORMATEX* pformat)
{
    // Since we're checking the RecoKind and SessionState multiple times, take a read lock
    ReadLock_Type readLock(m_combinedAdapterAndStateMutex);

    if (pformat != nullptr && ChangeState(SessionState::WaitForPumpSetFormatStart, SessionState::ProcessingAudio))
    {
        // The pump started successfully, we have a live running session now!
        SPX_DBG_TRACE_VERBOSE("%s: Now ProcessingAudio ...", __FUNCTION__);

        SPX_DBG_ASSERT(readLock.owns_lock()); // Keep the lock to call InformAdapterSetFormatStarting()...
        InformAdapterSetFormatStarting(pformat);
    }
    else if (pformat == nullptr && ChangeState(SessionState::StoppingPump, SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // Our stop pump request has been satisfied... Let's wait for the adapter to finish...
        SPX_DBG_TRACE_VERBOSE("%s: Now WaitForAdapterCompletedSetFormatStop (from StoppingPump)...", __FUNCTION__);
        SPX_DBG_ASSERT(readLock.owns_lock()); // Keep the lock to call InformAdapterSetFormatStopping()...
        InformAdapterSetFormatStopping(SessionState::StoppingPump);
    }
    else if (pformat == nullptr && ChangeState(SessionState::ProcessingAudio, SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // The pump stopped itself... That's possible when WAV files reach EOS. Let's wait for the adapter to finish...
        SPX_DBG_TRACE_VERBOSE("%s: Now WaitForAdapterCompletedSetFormatStop (from ProcessingAudio) ...", __FUNCTION__);
        SPX_DBG_ASSERT(readLock.owns_lock()); // Keep the lock to call InformAdapterSetFormatStopping()...
        InformAdapterSetFormatStopping(SessionState::ProcessingAudio);
    }
    else
    {
        SPX_THROW_HR(SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION);
    }
}

void CSpxAudioStreamSession::ProcessAudio(AudioData_Type data, uint32_t size)
{
    static_assert(MaxBufferedBeforeSimulateRealtime < MaxBufferedBeforeOverflow,
        "Buffer size triggering real time data consumption cannot be bigger than overflow limit");

    bool shouldSlowdown = false;
    bool overflowHappened = false;
    {
        WriteLock_Type writeLock(m_combinedAdapterAndStateMutex);
        if (!m_audioBuffer)
        {
            SPX_DBG_TRACE_VERBOSE("%s: Session has been shutdown while processing was in flight, buffer has already been destroyed", __FUNCTION__);
            return;
        }

        auto buffered = milliseconds(m_audioBuffer->StashedSizeInBytes() * 1000 / m_format->nAvgBytesPerSec);
        shouldSlowdown = buffered > MaxBufferedBeforeSimulateRealtime;
        overflowHappened = buffered > MaxBufferedBeforeOverflow;
        if (overflowHappened)
        {
            // drop everything from the buffer
            SPX_DBG_TRACE_VERBOSE("%s: Overflow happened, dropping the buffer and resetting the adapter, stashed size %d bytes.", __FUNCTION__, m_audioBuffer->StashedSizeInBytes());
            m_audioBuffer->Drop();
        }
    }

    if (overflowHappened)
    {
        // In case of overflow we send the error and reset the adapter
        Error(m_recoAdapter.get(), std::make_shared<SpxRecoEngineAdapterError>(false, "Service timeout. Resetting the buffer"));
        StartResetEngineAdapter();
        return;
    }

    if (shouldSlowdown)
    {
        // Sleep to slow down pulling.
        auto sleepPeriod = std::chrono::milliseconds(size * 1000 / m_format->nAvgBytesPerSec * SimulateRealtimePercentage / 100);
        SPX_DBG_TRACE_VERBOSE("%s - Stashing ... sleeping for %d ms", __FUNCTION__, sleepPeriod.count());
        std::this_thread::sleep_for(sleepPeriod);
    }

    {
        // Add the chunk to the buffer.
        WriteLock_Type writeLock(m_combinedAdapterAndStateMutex);
        if (!m_audioBuffer || !m_audioProcessor)
        {
            SPX_DBG_TRACE_VERBOSE("%s: Session has been shutdown while processing was in flight, buffer/processor has already been destroyed", __FUNCTION__);
            return;
        }

        m_audioBuffer->Add(data, size);
    }

    while (ProcessNextAudio())
    {
    }
}

bool CSpxAudioStreamSession::ProcessNextAudio()
{
    ReadLock_Type readStateLock(m_stateMutex);
    auto sessionState = m_sessionState;
    readStateLock.unlock();

    if (sessionState == SessionState::ProcessingAudio && !m_adapterAudioMuted)
    {
        AudioBufferPtr buffer;
        std::shared_ptr<ISpxAudioProcessor> processor;
        bool isKwsProcessor = false;

        {
            ReadLock_Type readLock(m_combinedAdapterAndStateMutex);
            buffer = m_audioBuffer;
            processor = m_audioProcessor;
            isKwsProcessor = m_isKwsProcessor;
        }

        if(!buffer || !processor)
        {
            SPX_DBG_TRACE_VERBOSE("%s: Session has been shutdown while processing was in flight, buffer/processor has already been destroyed", __FUNCTION__);
            return false;
        }

        auto item = buffer->GetNext();
        if (item)
        {
            if (isKwsProcessor)
            {
                // In KWS we always discard what we give out, because currently
                // there is no ACKING logic from KWS adapter, this will change with unidec in place.
                buffer->DiscardBytes(item->size);
            }

            processor->ProcessAudio(item->data, (uint32_t)item->size);
            return true;
        }
    }
    else if (sessionState == SessionState::HotSwapPaused || m_adapterAudioMuted)
    {
        // Don't process this data, if we're paused, it has been buffered...
        SPX_DBG_TRACE_VERBOSE("%s: Saving for later ... sessionState %d; adapterRequestedIdle %s", __FUNCTION__, sessionState, m_adapterAudioMuted ? "true" : "false");
    }
    else if (sessionState == SessionState::StoppingPump)
    {
        // Don't process this data if we're actively stopping, it is buffered and
        // we will process it if the source is resilient...
    }
    else
    {
        SPX_TRACE_WARNING("%s: Unexpected SessionState: recoKind %d; sessionState %d", __FUNCTION__, m_recoKind, sessionState);
    }
    return false;
}

const std::wstring& CSpxAudioStreamSession::GetSessionId() const
{
    return m_sessionId;
}

void CSpxAudioStreamSession::AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_recognizers.push_back(recognizer);
}

void CSpxAudioStreamSession::RemoveRecognizer(ISpxRecognizer* recognizer)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_recognizers.remove_if([&](std::weak_ptr<ISpxRecognizer>& item) {
        std::shared_ptr<ISpxRecognizer> sharedPtr = item.lock();
        return sharedPtr.get() == recognizer;
    });
}

void CSpxAudioStreamSession::WaitForIdle()
{
    // TODO: When KWS tests are in place,
    // split m_cv into two variables, one depending on state, another - on actual result.
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_DBG_TRACE_VERBOSE("%s: Waiting for Idle", __FUNCTION__);
    if (!m_cv.wait_for(lock, ShutdownTimeout, [&] { return IsState(SessionState::Idle); }))
    {
        SPX_DBG_TRACE_WARNING("%s: Timeout happened during wait for Idle", __FUNCTION__);
    }
    SPX_DBG_TRACE_VERBOSE("%s: Waiting for Idle finished", __FUNCTION__);
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxAudioStreamSession::RecognizeAsync()
{
    SPX_DBG_TRACE_FUNCTION();

    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<std::shared_ptr<ISpxRecognitionResult>()> task([=](){

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::RecognizeAsync kicked-off THREAD started ***", "*** CSpxAudioStreamSession::RecognizeAsync kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        // Keep track of the fact that we have a thread pending waiting to hear
        // what the final recognition result is, and then stop recognizing...
        m_recoAsyncWaiting = true;
        this->StartRecognizing(RecognitionKind::SingleShot);

        // Wait for the recognition result, and then stop recognizing
        auto result = this->WaitForRecognition();
        this->StopRecognizing(RecognitionKind::SingleShot);

        // Wait for the session to become idle, otherwise next RecognizeAsync can come in an unexpected state.
        WaitForIdle();

        // Return our result back to the future/caller
        return result;
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();

    return CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>(
        std::forward<std::future<std::shared_ptr<ISpxRecognitionResult>>>(taskFuture),
        AOS_Started);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StartContinuousRecognitionAsync()
{
    return StartRecognitionAsync(RecognitionKind::Continuous);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StopContinuousRecognitionAsync()
{
    return StopRecognitionAsync(RecognitionKind::Continuous);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model)
{
    return StartRecognitionAsync(RecognitionKind::Keyword, model);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StopKeywordRecognitionAsync()
{
    return StopRecognitionAsync(RecognitionKind::Keyword);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StartRecognitionAsync(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_TRACE_FUNCTION();

    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<void()> task([=](){

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::StartRecognitionAsync kicked-off THREAD started ***", "*** CSpxAudioStreamSession::StartRecognitionAsync kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        this->StartRecognizing(startKind, model);
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(taskFuture),
        AOS_Started);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StopRecognitionAsync(RecognitionKind stopKind)
{
    SPX_DBG_TRACE_FUNCTION();

    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<void()> task([=](){

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::StopRecognitionAsync kicked-off THREAD started ***", "*** CSpxAudioStreamSession::StopRecognitionAsync kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        this->StopRecognizing(stopKind);

        // Wait for the session to become idle, otherwise next RecognizeAsync can come in an unexpected state.
        WaitForIdle();
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(taskFuture),
        AOS_Started);
}

void CSpxAudioStreamSession::StartRecognizing(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model)
{
    // Since we're checking the RecoKind and SessionState multiple times, take a read lock
    ReadLock_Type readLock(m_combinedAdapterAndStateMutex);
    if (ChangeState(RecognitionKind::Idle, SessionState::Idle, startKind, SessionState::WaitForPumpSetFormatStart))
    {
        // We're starting from idle!! Let's get the Audio Pump running
        SPX_DBG_TRACE_VERBOSE("%s: Now WaitForPumpSetFormatStart ...", __FUNCTION__);

        // Make sure we drop left overs of the previous recognition
        // if the source does not need reliability guarantees.
        if (m_audioBuffer && !m_isReliableDelivery)
        {
            m_audioBuffer->Drop();
        }

        readLock.unlock(); // StartAudioPump() will take a write lock, so we need to clear our read lock
        StartAudioPump(startKind, model);
    }
    else if (ChangeState(RecognitionKind::Keyword, SessionState::ProcessingAudio, startKind, SessionState::HotSwapPaused))
    {
        // We're moving from Keyword/ProcessingAudio to startKind/Paused...
        SPX_DBG_TRACE_VERBOSE("%s: Now Paused ...", __FUNCTION__);

        readLock.unlock(); // HotSwap* will take a write lock, so we need to clear our read lock
        HotSwapAdaptersWhilePaused(startKind, model);
    }
    else if (m_resetRecoAdapter != nullptr && startKind == RecognitionKind::Continuous &&
             ChangeState(startKind, SessionState::ProcessingAudio, startKind, SessionState::HotSwapPaused))
    {
        SPX_DBG_TRACE_WARNING("%s: Resetting adapter via HotSwap (ProcessingAudio -> HotSwapPaused) ... attempting to stay in continuous mode!!! ...", __FUNCTION__);

        readLock.unlock(); // HotSwap* will take a write lock, so we need to clear our read lock
        HotSwapAdaptersWhilePaused(startKind, model);

        SPX_DBG_TRACE_WARNING("%s: Simulating GetSite()->AdapterCompletedSetFormatStop() ...", __FUNCTION__);
        AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio::Speech);
    }
    else if (startKind == RecognitionKind::Keyword && !IsKind(RecognitionKind::Keyword) && IsState(SessionState::ProcessingAudio))
    {
        // We're already doing something other than keyword spotting, but, someone wants to change/update the keyword ...
        // So ... let's update it
        SPX_DBG_TRACE_VERBOSE("%s: Changing keyword ... nothing else...", __FUNCTION__);
        m_kwsModel = model;
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

    if (m_kwsModel != nullptr && stopKind != RecognitionKind::Keyword && IsState(SessionState::ProcessingAudio) &&
        ChangeState(stopKind, SessionState::ProcessingAudio, RecognitionKind::Keyword, SessionState::HotSwapPaused))
    {
        // We've got a keyword, we're not trying to stop keyword spotting, and we're currently procesing audio...
        // So ... We should hot swap over to the keyword spotter (which will stop the current adapter doing whatever its doing)
        SPX_DBG_TRACE_VERBOSE("%s: Now Keyword/Paused ...", __FUNCTION__);

        readLock.unlock(); // HotSwapAdaptersWhilePaused() will take a write lock, so we need to clear our read lock
        HotSwapAdaptersWhilePaused(RecognitionKind::Keyword, m_kwsModel);
        if (stopKind == RecognitionKind::KwsSingleShot || stopKind == RecognitionKind::SingleShot)
        {
            FireSessionStoppedEvent();
        }
    }
    else if (stopKind == RecognitionKind::Keyword && IsKind(RecognitionKind::Keyword) &&
             ChangeState(stopKind, SessionState::ProcessingAudio, stopKind, SessionState::StoppingPump))
    {
        // We're actually keyword spotting right now, and we've been asked to stop doing that...
        // So ... Let's clear our keyword (since we're not spotting, we don't need it anymore)
        SPX_DBG_TRACE_VERBOSE("%s: Changing keyword to '' ... nothing else...", __FUNCTION__);
        m_kwsModel.reset();
        readLock.unlock();

        // And we'll stop the pump
        SPX_DBG_TRACE_VERBOSE("%s: Now StoppingPump ...", __FUNCTION__);
        m_audioPump->StopPump();
    }
    else if (stopKind == RecognitionKind::Keyword && !IsKind(RecognitionKind::Keyword))
    {
        // We've been asked to stop keyword spotting, but we're not keyword spotting right now ...
        // So ... Let's just clear the keyword
        SPX_DBG_TRACE_VERBOSE("%s: Changing keyword to '' ... nothing else...", __FUNCTION__);
        m_kwsModel.reset();
    }
    else if (stopKind == RecognitionKind::KwsSingleShot && !IsKind(RecognitionKind::KwsSingleShot))
    {
        // We've been asked to stop KwsSingleShot, but we've already stopped that, and have switched back to Keyword, or Idle
        SPX_DBG_TRACE_VERBOSE("%s: Already stopped KwsSingleShot ... recoKind %d; sessionState %d", __FUNCTION__, m_recoKind, m_sessionState);
    }
    else if (stopKind == RecognitionKind::KwsSingleShot && IsKind(RecognitionKind::KwsSingleShot) &&
             ChangeState(stopKind, SessionState::ProcessingAudio, stopKind, SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // TODO: RobCh: Is this ever called?!! How does it not get eaten by the first if statement above?!!

        // We're going to wait for done ... (which will flip us back to wherever we need to go...)
        SPX_DBG_TRACE_VERBOSE("%s: Now KwsSingleShot/WaitForAdapterCompletedSetFormatStop ...", __FUNCTION__);
    }
    else if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
    {
        readLock.unlock();

        // We've been asked to stop whatever it is we're doing, while we're actively processing audio ...
        // So ... Let's stop the pump
        SPX_DBG_TRACE_VERBOSE("%s: Now StoppingPump ...", __FUNCTION__);
        m_audioPump->StopPump();
    }
    else if (IsState(SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // If we're already in the WaitForAdapterCompletedSetFormatStop state... That's fine ... We'll eventually transit to Idle once AdapterCompletedSetFormatStop() is called...
        SPX_DBG_TRACE_VERBOSE("%s: Now (still) WaitForAdapterCompletedSetFormatStop ...", __FUNCTION__);
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
    m_cv.wait_for(lock, m_recoAsyncTimeoutDuration, [&] { return !m_recoAsyncWaiting; });
    SPX_DBG_TRACE_VERBOSE("Waiting for Recognition... Done!");

    if (!m_recoAsyncResult) // Deal with the timeout condition...
    {
        SPX_DBG_TRACE_VERBOSE("%s: Timed out waiting for recognition result... ", __FUNCTION__);

        // We'll need to stop the pump ourselves...
        if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
        {
            SPX_DBG_TRACE_VERBOSE("%s: Now StoppingPump ...", __FUNCTION__);

            lock.unlock();
            m_audioPump->StopPump();
            lock.lock();
        }

        lock.unlock();
        EnsureFireResultEvent();
        lock.lock();

        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterCompletedSetFormatStop...");
        m_cv.wait_for(lock, m_waitForAdapterCompletedSetFormatStopTimeout, [&] {
            return !this->IsState(SessionState::StoppingPump) && !this->IsState(SessionState::WaitForAdapterCompletedSetFormatStop);
        });
        SPX_DBG_TRACE_VERBOSE("Waiting for AdapterCompletedSetFormatStop... Done!!");
    }

    return std::move(m_recoAsyncResult);
}

void CSpxAudioStreamSession::WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result)
{
    FireResultEvent(GetSessionId(), result);

    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_recoAsyncWaiting)
    {
        m_recoAsyncWaiting = false;
        m_recoAsyncResult = result;

        m_cv.notify_all();
    }
}

void CSpxAudioStreamSession::FireSessionStartedEvent()
{
    SPX_DBG_TRACE_FUNCTION();

    FireEvent(EventType::SessionStart);
}

void CSpxAudioStreamSession::FireSessionStoppedEvent()
{
    SPX_DBG_TRACE_FUNCTION();
    EnsureFireResultEvent();

    FireEvent(EventType::SessionStop);
}

void CSpxAudioStreamSession::FireSpeechStartDetectedEvent(uint64_t offset)
{
    SPX_DBG_TRACE_FUNCTION();

    FireEvent(EventType::SpeechStart, nullptr, nullptr, offset);
}

void CSpxAudioStreamSession::FireSpeechEndDetectedEvent(uint64_t offset)
{
    SPX_DBG_TRACE_FUNCTION();

    FireEvent(EventType::SpeechEnd, nullptr, nullptr, offset);
}

void CSpxAudioStreamSession::EnsureFireResultEvent()
{
     // Since we're not holding a lock throughout this "ensure" method, a conflict is still possible.
     // That said, the conflict is benign, in the worst case we just created a throw away no-match result.
     if (m_recoAsyncWaiting)
     {
         auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
         auto timeoutCanceledResult = factory->CreateErrorResult(L"Timeout: no recognition result received.", ResultType::Speech);
         WaitForRecognition_Complete(timeoutCanceledResult);
     }
}

void CSpxAudioStreamSession::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_FUNCTION();

    FireEvent(EventType::RecoResultEvent, result, const_cast<wchar_t*>(sessionId.c_str()));
}

void CSpxAudioStreamSession::FireEvent(EventType sessionType, std::shared_ptr<ISpxRecognitionResult> result, wchar_t* sessionId, uint64_t offset)
{
    // Make a copy of the recognizers (under lock), to use to send events;
    // otherwise the underlying list could be modified while we're sending events...

    std::unique_lock<std::mutex> lock(m_mutex);
    decltype(m_recognizers) weakRecognizers(m_recognizers.begin(), m_recognizers.end());
    lock.unlock();

    auto sessionId_local = (sessionId != nullptr) ? sessionId : m_sessionId;

    for (auto weakRecognizer : weakRecognizers)
    {
        auto recognizer = weakRecognizer.lock();
        auto ptr = SpxQueryInterface<ISpxRecognizerEvents>(recognizer);
        if (recognizer)
        {
            switch (sessionType)
            {
                case EventType::SessionStart:
                    ptr->FireSessionStarted(sessionId_local);
                break;

                case EventType::SessionStop:
                    ptr->FireSessionStopped(sessionId_local);
                break;

                case EventType::SpeechStart:
                    ptr->FireSpeechStartDetected(sessionId_local, offset);
                break;

                case EventType::SpeechEnd:
                    ptr->FireSpeechEndDetected(sessionId_local, offset);
                break;

                case EventType::RecoResultEvent:
                    ptr->FireResultEvent(sessionId_local, result);
                break;
            }
        }
    }
}

void CSpxAudioStreamSession::KeywordDetected(ISpxKwsEngineAdapter* adapter, uint64_t offset, uint32_t size, AudioData_Type audioData)
{
    UNUSED(adapter);
    UNUSED(offset);

    SPX_DBG_TRACE_VERBOSE("Keyword detected!! Starting KwsSingleShot recognition... offset=%d; size=%d", offset, size);

    if (ChangeState(RecognitionKind::Keyword, SessionState::ProcessingAudio, RecognitionKind::KwsSingleShot, SessionState::HotSwapPaused))
    {
        // We've been told a keyword was recognized... Stash the audio, and start KwsSingleShot recognition!!
        SPX_DBG_TRACE_VERBOSE("%s: Now KwsSingleShot/Paused ...", __FUNCTION__);

        // Currently not under lock, because this is changed only in HotSwapPaused state (by a single thread at a time).
        m_spottedKeyword = std::make_shared<DataChunk>(audioData, size);
        HotSwapToKwsSingleShotWhilePaused();
    }
}

void CSpxAudioStreamSession::GetScenarioCount(uint16_t* countSpeech, uint16_t* countIntent, uint16_t* countTranslation)
{
    std::shared_ptr<ISpxRecognizer> recognizer;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        SPX_DBG_ASSERT(m_recognizers.size() == 1); // we only support 1 recognizer today...
        recognizer = m_recognizers.front().lock();
    }

    auto intentRecognizer = SpxQueryInterface<ISpxIntentRecognizer>(recognizer);
    auto translationRecognizer = SpxQueryInterface<ISpxTranslationRecognizer>(recognizer);

    *countIntent = (intentRecognizer != nullptr) ? 1 : 0;
    *countTranslation = (translationRecognizer != nullptr) ? 1 : 0;
    *countSpeech = 1 - *countIntent - *countTranslation;

    SPX_DBG_TRACE_VERBOSE("%s: countSpeech=%d; countIntent=%d; countTranslation=%d", __FUNCTION__, *countSpeech, *countIntent, *countTranslation);
}

std::list<std::string> CSpxAudioStreamSession::GetListenForList()
{
    return m_luAdapter != nullptr
        ? GetListenForListFromLuEngineAdapter()
        : std::list<std::string>();
}

void CSpxAudioStreamSession::GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region)
{
    if (m_luAdapter != nullptr)
    {
        GetIntentInfoFromLuEngineAdapter(provider, id, key, region);
    }
}

void CSpxAudioStreamSession::AdapterStartingTurn(ISpxRecoEngineAdapter* /* adapter */)
{
    SPX_DBG_ASSERT(!m_expectAdapterStartedTurn);
    m_expectAdapterStartedTurn = true;
}

void CSpxAudioStreamSession::AdapterStartedTurn(ISpxRecoEngineAdapter* /* adapter */, const std::string& /* id */)
{
    SPX_DBG_ASSERT(!m_expectAdapterStoppedTurn);
    m_expectAdapterStoppedTurn = true;
    m_expectAdapterStartedTurn = false;
}

void CSpxAudioStreamSession::AdapterStoppedTurn(ISpxRecoEngineAdapter* /* adapter */)
{
    SPX_DBG_ASSERT(m_expectAdapterStoppedTurn);
    m_expectAdapterStoppedTurn = false;

    // Since we're checking the SessionState, take a read-lock
    ReadLock_Type readLock(m_combinedAdapterAndStateMutex);
    if (m_audioBuffer)
    {
        m_audioBuffer->NewTurn();
    }

    if (IsState(SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // We are waiting for the adapter to confirm it got the SetFormat(nullptr), but we haven't sent the request yet...
        SPX_DBG_TRACE_VERBOSE("%s: Still WaitForAdapterCompletedSetFormatStop, calling ->SetFormat(nullptr) ...", __FUNCTION__);

        SPX_DBG_ASSERT(readLock.owns_lock()); // Keep the lock to call InformAdapterSetFormatStopping()...
        InformAdapterSetFormatStopping(SessionState::WaitForAdapterCompletedSetFormatStop);
    }
    else if (m_adapterAudioMuted && IsState(SessionState::ProcessingAudio) &&
             IsKind(m_turnEndStopKind) &&  m_turnEndStopKind != RecognitionKind::Idle)
    {
        // The adapter previously requested to stop processing audio ... We can now safely stop recognizing...
        SPX_DBG_TRACE_VERBOSE("%s: ->StopRecognizing(%d) ...", __FUNCTION__, m_turnEndStopKind);

        readLock.unlock(); // Don't hold the lock when calling StopRecognizing()...
        auto stopKind = m_turnEndStopKind;
        m_turnEndStopKind = RecognitionKind::Idle;
        StopRecognizing(stopKind);
    }
}

void CSpxAudioStreamSession::AdapterDetectedSpeechStart(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);

    auto buffer = m_audioBuffer;
    offset = buffer ? buffer->ToAbsolute(offset) : offset;

    FireSpeechStartDetectedEvent(offset);
}

void CSpxAudioStreamSession::AdapterDetectedSpeechEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);

    auto buffer = m_audioBuffer;
    offset = buffer ? buffer->ToAbsolute(offset) : offset;

    FireSpeechEndDetectedEvent(offset);
}

void CSpxAudioStreamSession::AdapterDetectedSoundStart(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    UNUSED(offset);
    // TODO: RobCh: Next: Implement
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxAudioStreamSession::AdapterDetectedSoundEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset)
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

std::shared_ptr<ISpxRecognitionEventArgs> CSpxAudioStreamSession::CreateRecognitionEventArgs(const std::wstring& sessionId, uint64_t offset)
{
    auto site = SpxSiteFromThis(this);
    auto recoEvent = SpxCreateObjectWithSite<ISpxRecognitionEventArgs>("CSpxRecognitionEventArgs", site);

    auto argsInit = SpxQueryInterface<ISpxRecognitionEventArgsInit>(recoEvent);
    argsInit->Init(sessionId, offset);

    return recoEvent;
}

std::shared_ptr<ISpxRecognitionEventArgs> CSpxAudioStreamSession::CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    auto site = SpxSiteFromThis(this);
    auto recoEvent = SpxCreateObjectWithSite<ISpxRecognitionEventArgs>("CSpxRecognitionEventArgs", site);

    auto argsInit = SpxQueryInterface<ISpxRecognitionEventArgsInit>(recoEvent);
    argsInit->Init(sessionId, result);

    return recoEvent;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, ResultType type, uint64_t offset, uint64_t duration)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitIntermediateResult(resultId, text, type, offset, duration);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateFinalResult(ResultType type, const wchar_t* resultId, Reason reason, const wchar_t* text, uint64_t offset, uint64_t duration)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(resultId, reason, text, type, offset, duration);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateErrorResult(const wchar_t* text, ResultType type)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    SpxQueryInterface<ISpxRecognitionResultInit>(result)->InitError(text, type);

    return result;
}

void CSpxAudioStreamSession::FireAdapterResult_Intermediate(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    UNUSED(offset);
    SPX_DBG_ASSERT(!IsState(SessionState::WaitForPumpSetFormatStart));

    auto buffer = m_audioBuffer;
    offset = buffer ? buffer->ToAbsolute(offset) : offset;
    result->SetOffset(offset);

    FireResultEvent(GetSessionId(), result);
}

void CSpxAudioStreamSession::FireAdapterResult_FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::WaitForPumpSetFormatStart), "ERROR! FireAdapterResult_FinalResult was called with SessionState==WaitForPumpSetFormatStart");

    auto buffer = m_audioBuffer;
    m_shouldRetry = true;
    if (buffer)
    {
        result->SetOffset(buffer->ToAbsolute(offset));
        buffer->DiscardTill(offset + result->GetDuration());
    }

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

void CSpxAudioStreamSession::FireAdapterResult_TranslationSynthesis(ISpxRecoEngineAdapter* adapter, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::WaitForPumpSetFormatStart));

    FireResultEvent(GetSessionId(), result);
}

void CSpxAudioStreamSession::AdapterRequestingAudioMute(ISpxRecoEngineAdapter* /* adapter */, bool muteAudio)
{
    // Since we're checking the RecoKind and SessionState multiple times, take a read lock
    ReadLock_Type readLock(m_combinedAdapterAndStateMutex);

    if (muteAudio && m_expectAdapterStoppedTurn &&
        (IsKind(RecognitionKind::SingleShot) ||
         IsKind(RecognitionKind::KwsSingleShot)) &&
        (IsState(SessionState::ProcessingAudio) ||
         IsState(SessionState::WaitForAdapterCompletedSetFormatStop)))
    {
        // The adapter is letting us know that it wants us to stop sending it audio data for this single shot utterance
        SPX_DBG_TRACE_VERBOSE("%s: Muting audio (SingleShot or KwsSingleShot) ... recoKind/sessionState: %d/%d", __FUNCTION__, m_recoKind, m_sessionState);
        m_turnEndStopKind = m_recoKind;
        m_adapterAudioMuted = true;
    }
    else if (muteAudio && m_expectAdapterStoppedTurn &&
             IsKind(RecognitionKind::Continuous) &&
             (IsState(SessionState::ProcessingAudio) ||
              IsState(SessionState::WaitForAdapterCompletedSetFormatStop)))
    {
        // The adapter is letting us know that it wants us to stop sending it audio data for this until it requests to restart the audio flow
        SPX_DBG_TRACE_VERBOSE("%s: Muting audio (Continuous) ... recoKind/sessionState: %d/%d", __FUNCTION__, m_recoKind, m_sessionState);
        m_adapterAudioMuted = true;
    }
    else if (!muteAudio && m_adapterAudioMuted)
    {
        // The adapter is letting us know that its OK to once again send audio data to it
        SPX_DBG_TRACE_VERBOSE("%s: UN-muting audio ... recoKind/sessionState: %d/%d", __FUNCTION__, m_recoKind, m_sessionState);
        m_adapterAudioMuted = false;
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: Is this OK? recoKind/sessionState: %d/%d", __FUNCTION__, m_recoKind, m_sessionState);
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

    // If it is a transport error and the connection was successfull before, we retry in continuous mode.
    // Otherwise report the error to the user, so that he can recreate a recognizer.
    if (IsKind(RecognitionKind::Continuous) && payload->IsTransportError() && m_shouldRetry)
    {
        // Currently no back-off, retrying only once.
        m_shouldRetry = false;
        StartResetEngineAdapter();
    }
    else
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
        auto error = factory->CreateErrorResult(PAL::ToWString(payload->Info()).c_str(), ResultType::Speech);
        WaitForRecognition_Complete(error);
    }
}

std::shared_ptr<ISpxSession> CSpxAudioStreamSession::GetDefaultSession()
{
    return SpxSharedPtrFromThis<ISpxSession>(this);
}

std::string CSpxAudioStreamSession::GetStringValue(const char* name, const char* defaultValue) const
{
    if (PAL::stricmp(name, "KWSModelPath") == 0 && m_kwsModel != nullptr)
    {
        return PAL::ToString(m_kwsModel->GetFileName());
    }
    else if (PAL::stricmp(name, GetPropertyName(SpeechPropertyId::Speech_SessionId)) == 0)
    {
        return PAL::ToString(m_sessionId);
    }

    return ISpxPropertyBagImpl::GetStringValue(name, defaultValue);
}

std::shared_ptr<ISpxRecoEngineAdapter> CSpxAudioStreamSession::EnsureInitRecoEngineAdapter()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_recoAdapter == nullptr ||  m_resetRecoAdapter == m_recoAdapter)
    {
        EnsureResetEngineEngineAdapterComplete();
        InitRecoEngineAdapter();
        EnsureIntentRegionSet();
    }
    return m_recoAdapter;
}

void CSpxAudioStreamSession::InitRecoEngineAdapter()
{
    // determine which type (or types) of reco engine adapters we should try creating...
    bool tryUnidec = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Unidec", PAL::BoolToString(false).c_str()));
    bool tryMock = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Mock", PAL::BoolToString(false).c_str()));
    bool tryUsp = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Usp", PAL::BoolToString(false).c_str()));
    
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

void CSpxAudioStreamSession::StartResetEngineAdapter()
{
    m_resetRecoAdapter = m_recoAdapter;
    if (IsKind(RecognitionKind::Continuous))
    {
        // If we were continuous, let's try and restart recognition, same kind...
        StartRecognitionAsync(m_recoKind);
    }
    else if (!IsKind(RecognitionKind::Idle))
    {
        // Otherwise... Let's stop the recognition
        StopRecognitionAsync(m_recoKind);
    }
}

void CSpxAudioStreamSession::EnsureResetEngineEngineAdapterComplete()
{
    if (m_resetRecoAdapter != nullptr && m_resetRecoAdapter == m_recoAdapter)
    {
        // Let's term and clear our reco adapter...
        SPX_DBG_TRACE_VERBOSE("%s: resetting reco adapter (0x%8x)...", __FUNCTION__, m_resetRecoAdapter.get());
        SpxTermAndClear(m_resetRecoAdapter);

        m_expectAdapterStartedTurn = false;
        m_expectAdapterStoppedTurn = false;
        m_adapterAudioMuted = false;

        m_recoAdapter = nullptr; // Already termed (see above)
        m_resetRecoAdapter = nullptr;
    }
}

std::shared_ptr<ISpxKwsEngineAdapter> CSpxAudioStreamSession::EnsureInitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_kwsAdapter == nullptr || model != m_kwsModel)
    {
        SpxTermAndClear(m_kwsAdapter);
        InitKwsEngineAdapter(model);
    }

    return m_kwsAdapter;
}

void CSpxAudioStreamSession::EnsureIntentRegionSet()
{
    // Let's default the "intentRegion" to the speech region
    auto intentRegion = this->GetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Region), "");

    // Now ... let's check to see if we have a different region specified for intent...
    SPX_DBG_ASSERT(m_recognizers.size() == 1); // we only support 1 recognizer today...
    auto recognizer = m_recognizers.front();
    auto intentRecognizer = SpxQueryInterface<ISpxIntentRecognizer>(recognizer.lock());
    if (intentRecognizer != nullptr && m_luAdapter != nullptr)
    {
        // we have an intent recognizer... and an lu adapter (meaning someone called ->AddIntent())...
       std::string provider, id, key, region;
       GetIntentInfoFromLuEngineAdapter(provider, id, key, region);

       // Let's use that region if it's not empty (could be empty if user specified only the lu appid)
       if (!region.empty())
       {
           intentRegion = region;
       }
    }
    // Finally ... Let's actually store the region
    SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_IntentRegion), SpeechRegionFromIntentRegion(intentRegion).c_str());
}

std::string CSpxAudioStreamSession::SpeechRegionFromIntentRegion(const std::string& intentRegion)
{
    static std::pair<std::string, std::string> intentToSpeechRegion[] = {

        std::make_pair("West US",          "uswest"),
        std::make_pair("US West",          "uswest"),
        std::make_pair("westus",           "uswest"),

        std::make_pair("West US 2",        "uswest2"),
        std::make_pair("US West 2",        "uswest2"),
        std::make_pair("westus2",          "uswest2"),

        std::make_pair("South Central US", "ussouthcentral"),
        std::make_pair("US South Central", "ussouthcentral"),
        std::make_pair("southcentralus",   "ussouthcentral"),

        std::make_pair("West Central US",  "uswestcentral"),
        std::make_pair("US West Central",  "uswestcentral"),
        std::make_pair("westcentralus",    "uswestcentral"),

        std::make_pair("East US",          "useast"),
        std::make_pair("US East",          "useast"),
        std::make_pair("eastus",           "useast"),

        std::make_pair("East US 2",        "useast2"),
        std::make_pair("US East 2",        "useast2"),
        std::make_pair("eastus2",          "useast2"),

        std::make_pair("West Europe",      "europewest"),
        std::make_pair("Europe West",      "europewest"),
        std::make_pair("westeurope",       "europewest"),

        std::make_pair("North Europe",     "europenorth"),
        std::make_pair("Europe North",     "europenorth"),
        std::make_pair("northeurope",      "europenorth"),

        std::make_pair("Brazil South",     "brazilsouth"),
        std::make_pair("South Brazil",     "brazilsouth"),
        std::make_pair("southbrazil",      "brazilsouth"),

        std::make_pair("Australia East",   "australiaeast"),
        std::make_pair("East Australia",   "australiaeast"),
        std::make_pair("eastaustralia",    "australiaeast"),

        std::make_pair("Southeast Asia",   "asiasoutheast"),
        std::make_pair("Asia Southeast",   "asiasoutheast"),
        std::make_pair("southeastasia",    "asiasoutheast"),

        std::make_pair("East Asia",        "asiaeast"),
        std::make_pair("Asia East",        "asiaeast"),
        std::make_pair("eastasia",         "asiaeast"),
    };

    for (auto item : intentToSpeechRegion)
    {
        if (PAL::stricmp(item.first.c_str(), intentRegion.c_str()) == 0)
        {
            return item.second;
        }
    }

    return intentRegion;
}

void CSpxAudioStreamSession::InitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model)
{
    m_kwsModel = model;

    bool tryMock = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseKwsEngine-Mock", "false").c_str());
    bool tryDdk = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseKwsEngine-Ddk", "false").c_str());
    bool trySdk = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseKwsEngine-Sdk", "false").c_str());

    // if nobody specified which type(s) of reco engine adapters this session should use, we'll use the SDK KWS engine
    if (!tryMock && !trySdk && !tryDdk)
    {
        trySdk = true;
        tryDdk = true;
        tryMock = true;
    }

    // try to create the DDK adapter...
    if (m_kwsAdapter == nullptr && tryDdk)
    {
        m_kwsAdapter = SpxCreateObjectWithSite<ISpxKwsEngineAdapter>("CSpxSpeechDdkKwsEngineAdapter", this);
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
    std::packaged_task<void()> task([=]() mutable {

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused kicked-off THREAD started ***", "*** CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        // Keep track of the fact that we have a thread pending waiting for the single
        // shot recognition to complete (so it can call StopRecognizing(KwsSingleShot)) and
        // then wait for done...
        m_recoAsyncWaiting = true;

        SPX_DBG_ASSERT(IsKind(RecognitionKind::KwsSingleShot) && IsState(SessionState::HotSwapPaused));
        this->HotSwapAdaptersWhilePaused(RecognitionKind::KwsSingleShot);

        this->WaitForKwsSingleShotRecognition();
    });

    m_taskHelper.RunAsync(std::move(task));
}

void CSpxAudioStreamSession::WaitForKwsSingleShotRecognition()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    SPX_DBG_TRACE_VERBOSE("Waiting for KwsSingleShot Recognition...");
    m_cv.wait_for(lock, m_recoAsyncTimeoutDuration, [&] { return !m_recoAsyncWaiting; });
    SPX_DBG_TRACE_VERBOSE("Waiting for KwsSingleShot Recognition... Done!");

    lock.unlock();
    this->StopRecognizing(RecognitionKind::KwsSingleShot);
    lock.lock();

    if (!m_recoAsyncResult) // Deal with the timeout condition...
    {
        SPX_DBG_TRACE_VERBOSE("KwsSingleShot Waiting for AdapterDone...");
        m_cv.wait_for(lock, m_waitForAdapterCompletedSetFormatStopTimeout, [&] { return !this->IsState(SessionState::WaitForAdapterCompletedSetFormatStop); });
        SPX_DBG_TRACE_VERBOSE("KwsSingleShot Waiting for AdapterDone... Done!!");
    }
}

void CSpxAudioStreamSession::StartAudioPump(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_ASSERT(IsState(SessionState::WaitForPumpSetFormatStart));

    // Tell everyone we're starting...
    if (startKind != RecognitionKind::Keyword)
    {
        FireSessionStartedEvent();
    }

    // Lock the Audio Processor mutex, since we'll be picking which one we're using...
    WriteLock_Type writeLock(m_combinedAdapterAndStateMutex);

    auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    m_audioPump->GetFormat(waveformat.get(), cbFormat);

    if (!m_audioBuffer)
    {
        m_audioBuffer = std::make_shared<PcmAudioBuffer>(*waveformat);
    }
    m_audioBuffer->NewTurn();

    // Depending on the startKind, we'll either switch to the Kws Engine Adapter or the Reco Engine Adapter
    m_audioProcessor = startKind == RecognitionKind::Keyword
        ? SpxQueryInterface<ISpxAudioProcessor>(EnsureInitKwsEngineAdapter(model))
        : SpxQueryInterface<ISpxAudioProcessor>(EnsureInitRecoEngineAdapter());
    m_isKwsProcessor = (startKind == RecognitionKind::Keyword);

    writeLock.unlock(); // DON'T HOLD THE LOCK any longer than here.
    // The StartPump() call (see below) will instigate a call to this::SetFormat, which will try to acquire the reader lock from a different thread...

    // Start pumping audio data from the pump, to the Audio Stream Session
    auto ptr = (ISpxAudioProcessor*)this;
    auto pISpxAudioProcessor = ptr->shared_from_this();
    m_audioPump->StartPump(pISpxAudioProcessor);

    // The call to StartPump (immediately above) will initiate calls from the pump to this::SetFormat() and then this::ProcessAudio()...
}

void CSpxAudioStreamSession::HotSwapAdaptersWhilePaused(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_ASSERT(IsKind(startKind) && IsState(SessionState::HotSwapPaused));
    SPX_DBG_ASSERT(m_audioProcessor != nullptr);

    // Lock the Audio Processor mutex, since we're switching which one we'll be using...
    WriteLock_Type writeLock(m_combinedAdapterAndStateMutex);
    auto oldAudioProcessor = m_audioProcessor;

    // Get the format in which the pump is currently sending audio data to the Session
    auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    m_audioPump->GetFormat(waveformat.get(), cbFormat);

    // Depending on the startKind, we'll either switch to the Kws Engine Adapter or the Reco Engine Adapter
    if (startKind == RecognitionKind::Keyword)
    {
        m_audioProcessor = SpxQueryInterface<ISpxAudioProcessor>(EnsureInitKwsEngineAdapter(model));
        m_isKwsProcessor = true;
    }
    else
    {
        m_audioProcessor = SpxQueryInterface<ISpxAudioProcessor>(EnsureInitRecoEngineAdapter());
        m_isKwsProcessor = false;
        if (m_spottedKeyword)
        {
            // Switching from KWS to single shot, creating buffer from scratch with added keyword.
            auto currentBuffer = m_audioBuffer;
            m_audioBuffer = std::make_shared<PcmAudioBuffer>(*waveformat);
            SPX_DBG_TRACE_VERBOSE_IF(1, "%s: Size of spotted keyword %d", __FUNCTION__, m_spottedKeyword->size);
            m_audioBuffer->Add(m_spottedKeyword->data, m_spottedKeyword->size);
            currentBuffer->CopyNonAcknowledgedDataTo(m_audioBuffer);
            m_spottedKeyword = nullptr;
        }
    }

    // Tell the old Audio Processor that we've sent the last bit of audio data we're going to send
    SPX_DBG_TRACE_VERBOSE_IF(1, "%s: ProcessingAudio - size=%d", __FUNCTION__, 0);
    oldAudioProcessor->ProcessAudio(nullptr, 0);

    // Then tell it we're finally done, by sending a nullptr WAVEFORMAT
    oldAudioProcessor->SetFormat(nullptr);
    m_adapterAudioMuted = false;

    if (m_audioBuffer)
        m_audioBuffer->NewTurn();

    // Inform the Audio Processor that we're starting...
    SPX_DBG_ASSERT(writeLock.owns_lock());
    InformAdapterSetFormatStarting(waveformat.get());

    // The call to ProcessAudio(nullptr, 0) and SetFormat(nullptr) will instigate a call from that Adapter to this::AdapterCompletedSetFormatStop(adapter) shortly...
}

void CSpxAudioStreamSession::InformAdapterSetFormatStarting(WAVEFORMATEX* format)
{
    // NOTE: Callers will have already taken a reader or writer lock... Do NOT take a lock here ourselves!
    // ReadLock_Type readerLock(m_combinedAdapterAndStateMutex);

    auto sizeOfFormat = sizeof(WAVEFORMATEX) + format->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), format, sizeOfFormat);

    if (m_recoAdapter != nullptr)
    {
        m_recoAdapter->SetAdapterMode(!IsKind(RecognitionKind::Continuous));
    }

    SPX_DBG_ASSERT(format != nullptr);
    m_audioProcessor->SetFormat(format);
}

void CSpxAudioStreamSession::InformAdapterSetFormatStopping(SessionState comingFromState)
{
    // NOTE: Callers will have already taken a reader or writer lock... Do NOT take a lock here ourselves!
    // ReadLock_Type readerLock(m_combinedAdapterAndStateMutex);

    // If we transitioned --> WaitForAdapterCompletedSetFormatStop state because this Session was trying to stop the pump...
    // We need to tell the audio processor that we've sent the last bit of audio we're planning on sending
    SPX_DBG_ASSERT(comingFromState == SessionState::ProcessingAudio ||
                   comingFromState == SessionState::StoppingPump ||
                   comingFromState == SessionState::WaitForAdapterCompletedSetFormatStop);
    if (comingFromState == SessionState::StoppingPump)
    {
        SPX_DBG_TRACE_VERBOSE("%s: ProcessingAudio - size=%d", __FUNCTION__, 0);
        if (m_audioProcessor)
        {
            m_audioProcessor->ProcessAudio(nullptr, 0);
        }
    }

    // Then we can finally tell it we're done, by sending a nullptr WAVEFORMAT
    if (!m_expectAdapterStartedTurn && !m_expectAdapterStoppedTurn)
    {
        SPX_DBG_TRACE_VERBOSE("%s: SetFormat(nullptr)", __FUNCTION__);
        if (m_audioProcessor)
        {
            m_audioProcessor->SetFormat(nullptr);
        }
        m_adapterAudioMuted = false;
    }
}

void CSpxAudioStreamSession::AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio doneAdapter)
{
    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<void()> task([=]() mutable {

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::AdapterCompletedSetFormatStop kicked-off THREAD started ***", "*** CSpxAudioStreamSession::AdapterCompletedSetFormatStop kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        // Since we're checking the RecoKind and SessionState multiple times, take a read lock
        ReadLock_Type readLock(m_combinedAdapterAndStateMutex);

        if (ChangeState(RecognitionKind::KwsSingleShot, SessionState::WaitForAdapterCompletedSetFormatStop, RecognitionKind::Keyword, SessionState::ProcessingAudio))
        {
            SPX_DBG_TRACE_VERBOSE("KwsSingleShot Waiting for done ... Done!! Switching back to Keyword/Processing");
            SPX_DBG_TRACE_VERBOSE("%s: Now Keyword/ProcessingAudio ...", __FUNCTION__);

            // holding readLock will cause deadlock if this calling thread holds the last reference to the recognizer.
            readLock.unlock();
            FireSessionStoppedEvent();
        }
        else if (ChangeState(SessionState::HotSwapPaused, SessionState::ProcessingAudio))
        {
            SPX_DBG_TRACE_VERBOSE("Previous Adapter is done processing audio ... resuming Processing with the new adapter...");
            SPX_DBG_TRACE_VERBOSE("%s: Now ProcessingAudio ...", __FUNCTION__);

            if (doneAdapter == AdapterDoneProcessingAudio::Keyword && IsKind(RecognitionKind::KwsSingleShot))
            {
                // holding readLock will cause deadlock if this calling thread holds the last reference to the recognizer.
                readLock.unlock();
                FireSessionStartedEvent();
            }
        }
        else if (ChangeState(SessionState::WaitForAdapterCompletedSetFormatStop, RecognitionKind::Idle, SessionState::Idle))
        {
            if (doneAdapter == AdapterDoneProcessingAudio::Speech)
            {
                // holding readLock will cause deadlock if this calling thread holds the last reference to the recognizer.
                readLock.unlock();
                // The Reco Engine adapter request to finish processing audio has completed, that signifies that the "session" has stopped
                FireSessionStoppedEvent();
                readLock.lock();

                // Restart the keyword spotter if necessary...
                if (m_kwsModel != nullptr && ChangeState(SessionState::Idle, RecognitionKind::Keyword, SessionState::WaitForPumpSetFormatStart))
                {
                    // Go ahead re-start the pump
                    SPX_DBG_TRACE_VERBOSE("%s: Now WaitForPumpSetFormatStart ...", __FUNCTION__);

                    readLock.unlock(); // StartAudioPump() will take a write lock, so we need to clear our read lock
                    StartAudioPump(RecognitionKind::Keyword, m_kwsModel);
                }
            }
        }
        else
        {
            SPX_DBG_TRACE_WARNING("%s: Is this OK? doneAdapter=%d; sessionState=%d **************************", __FUNCTION__, doneAdapter, m_sessionState);
        }
    });

    m_taskHelper.RunAsync(std::move(task));
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
    bool tryLuisDirect = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseLuEngine-LuisDirect", "false").c_str());
    bool tryMock = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseLuEngine-Mock", "false").c_str());
    
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

void CSpxAudioStreamSession::GetIntentInfoFromLuEngineAdapter(std::string& provider, std::string& id, std::string& key, std::string& region)
{
    SPX_DBG_ASSERT(m_luAdapter != nullptr);
    auto triggerService = SpxQueryInterface<ISpxIntentTriggerService>(m_luAdapter);
    return triggerService->GetIntentInfo(provider, id, key, region);
}

std::shared_ptr<ISpxLuEngineAdapter> CSpxAudioStreamSession::GetLuEngineAdapter()
{
    EnsureInitLuEngineAdapter();
    return m_luAdapter;
}

std::shared_ptr<ISpxNamedProperties> CSpxAudioStreamSession::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

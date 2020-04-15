//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.cpp: Implementation definitions for CSpxAudioStreamSession C++ class
//

#define NOMINMAX
#include "stdafx.h"
#include <future>
#include <list>
#include <memory>
#include <inttypes.h>
#include <limits.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TraceLoggingProvider.h>

#define MICROSOFT_KEYWORD_MEASURES 0x0000400000000000 // Bit 46
#define MICROSOFT_KEYWORD_TELEMETRY 0x0000200000000000 // Bit 45
#define TraceLoggingOptionMicrosoftTelemetry() TraceLoggingOptionGroup(0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba)

// forward-declare
TRACELOGGING_DECLARE_PROVIDER(tracingEventProvider);
#endif

#ifdef _WIN32
TRACELOGGING_DEFINE_PROVIDER(tracingEventProvider,
    "Microsoft.CognitiveServices.Speech.SDK",
    (0xbbdb52c5, 0xeb5c, 0x4616, 0x83, 0x1d, 0x90, 0x12, 0x13, 0xd3, 0x79, 0x1c),
    TraceLoggingOptionMicrosoftTelemetry());
#endif

#include "audio_stream_session.h"
#include "speech_audio_processor.h"
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
#include "try_catch_helpers.h"
#include "time_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using json = nlohmann::json;

seconds CSpxAudioStreamSession::StopRecognitionTimeout = 10s;

atomic<int64_t> CSpxAudioStreamSession::Operation::OperationId;
const minutes CSpxAudioStreamSession::Operation::Timeout = 1min;

CSpxAudioStreamSession::CSpxAudioStreamSession() :
    m_sessionId(PAL::CreateGuidWithoutDashes()),
    m_recoKind(RecognitionKind::Idle),
    m_sessionState(SessionState::Idle),
    m_sawEndOfStream(false),
    m_fireEndOfStreamAtSessionStop(false),
    m_expectAdapterStartedTurn(false),
    m_expectAdapterStoppedTurn(false),
    m_expectFirstHypothesis(true),
    m_adapterAudioMuted(false),
    m_audioPumpStoppedBeforeHotSwap(false),
    m_turnEndStopKind(RecognitionKind::Idle),
    m_kwsProcessorMode{ KWSProcessorMode::None },
    m_isReliableDelivery{ false },
    m_currentTurnGlobalOffset{ 0 },
    m_bytesTransited(0)
{
    SPX_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::CSpxAudioStreamSession", (void*)this);
}

CSpxAudioStreamSession::~CSpxAudioStreamSession()
{
    SPX_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::~CSpxAudioStreamSession", (void*)this);
    SPX_DBG_ASSERT(m_kwsAdapter == nullptr);
    SPX_DBG_ASSERT(m_recoAdapter == nullptr);
    SPX_DBG_ASSERT(m_luAdapter == nullptr);
}

void CSpxAudioStreamSession::Init()
{
    SPX_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::Init:... ", (void*)this);

    // NOTE: Due to current ownership model, and our late-into-the-cycle changes for SpeechConfig objects
    // the CSpxAudioStreamSession is sited to the CSpxApiFactory. This ApiFactory is not held by the
    // dev user at or above the CAPI. Thus ... we must hold it alive in order for the properties to be
    // obtainable via the standard ISpxNamedProperties mechanisms... It will be released on ::Term()
    m_siteKeepAlive = GetSite();

    // Currently we do not perform lazy initialization in order not to introduce latency in the
    // future calls (i.e. RecognizeAsync).
    m_threadService = make_shared<CSpxThreadService>();
    m_threadService->Init();
}

void CSpxAudioStreamSession::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::Term:... ", (void*)this);

    // No other threads after this point can access internal structures,
    // so it is safe to clean them up.
    // Let's check to see if we're still processing audio ...
    // We can be here in some extreme cases where the StartPump did not complete yet so we are waiting for that case.

    // The pump start was called in both of these states so we force the issue and stop it regardless if the transition to processing was complete
    if (ChangeState(SessionState::WaitForPumpSetFormatStart, SessionState::StoppingPump)
        || ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
    {
        // We're terminating, and we were still processing audio ... So ... Let's shut down the pump
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::Term: Now StoppingPump ...", (void*)this);
        if (m_audioPump)
        {
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::Term: StopPump[%p]", (void*)this, (void*)m_audioPump.get());
            m_audioPump->StopPump();
        }

        if (m_codecAdapter)
        {
            m_codecAdapter->Close();
        }
    }
    else
    {
        SPX_DBG_TRACE_WARNING("[%p]CSpxAudioStreamSession::Term: **NOT CALLED** StopPump[%p] - state: %d", (void*)this, (void*)m_audioPump.get(), (int)m_sessionState);
    }

    // Stopping all threads.
    m_threadService->Term();

    // Make sure there is nobody waiting on a single shot.
    CancelPendingSingleShot(RecognitionKind::SingleShot);
    CancelPendingSingleShot(RecognitionKind::KeywordOnce);

    {
        unique_lock<mutex> lock(m_recognizersLock);
        m_recognizers.clear();
    }

    SpxTermAndClear(m_audioPump);
    SpxTermAndClear(m_kwsAdapter);
    SpxTermAndClear(m_recoAdapter);
    SpxTermAndClear(m_luAdapter);
    SpxTermAndClear(m_siteKeepAlive);
    SpxTermAndClear(m_codecAdapter);
    m_audioProcessor = nullptr;
    m_speechProcessor = nullptr;
    m_audioBuffer = nullptr;
}

void CSpxAudioStreamSession::CancelPendingSingleShot(RecognitionKind kind)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::CancelPendingSingleShot", (void*)this);

    auto& currentSingleShot = kind == RecognitionKind::KeywordOnce ?
        m_singleShotKeywordInFlight : m_singleShotInFlight;

    // Make sure there is nobody waiting on a single shot.
    if (currentSingleShot &&
        currentSingleShot->m_future.wait_for(0s) == future_status::timeout)
    {
        auto result = CreateFinalResult(nullptr, ResultReason::Canceled,
            NO_MATCH_REASON_NONE, CancellationReason::Error,
            CancellationErrorCode::RuntimeError, L"Shutdown while waiting on result.",
            0, 0);

        currentSingleShot->m_promise.set_value(result);
        currentSingleShot->m_spottedKeywordResult = nullptr;
        currentSingleShot = nullptr;
    }
}

packaged_task<void()> CSpxAudioStreamSession::CreateTask(function<void()> func, bool catchAll)
{
    // Creates a packaged task that propagates all exceptions
    // to the user thread and then user callback.
    auto keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    if (catchAll)
    {
        // Catches all exceptions and sends them to the user thread.
        return packaged_task<void()>([this, keepAlive, func]() {
            string error;
            SPXAPI_TRY()
            {
                func();
                return;
            }
            SPXAPI_CATCH_ONLY()
            CheckError(error);
        });
    }
    else
    {
        return packaged_task<void()>([this, keepAlive, func]() {
            // Make Android compiler happy.
            UNUSED(this);
            UNUSED(keepAlive);
            func();
        });
    }
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

    SetStringValue(GetPropertyName(PropertyId::AudioConfig_AudioSource), g_audioSourceFile);
    SetAudioConfigurationInProperties();

    m_isReliableDelivery = true;
}

void CSpxAudioStreamSession::InitFromMicrophone()
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
    SPX_DBG_ASSERT(IsKind(RecognitionKind::Idle) && IsState(SessionState::Idle));
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::InitFromMicrophone: Now Idle", (void*)this);

    // Create the microphone pump
    auto site = SpxSiteFromThis(this);
    m_audioPump = SpxCreateObjectWithSite<ISpxAudioPump>("CSpxInteractiveMicrophone", site);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::InitFromMicrophone: Pump from microphone:[%p]", (void*)this, (void*)m_audioPump.get());

    SetStringValue(GetPropertyName(PropertyId::AudioConfig_AudioSource), g_audioSourceMicrophone);
    SetAudioConfigurationInProperties();

    // Write microphone device name and session id
    WriteTracingEvent();
}

void CSpxAudioStreamSession::InitFromStream(std::shared_ptr<ISpxAudioStream> stream)
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
    SPX_DBG_ASSERT(IsKind(RecognitionKind::Idle) && IsState(SessionState::Idle));
    SPX_DBG_TRACE_VERBOSE("%s: Now Idle ...", __FUNCTION__);
    std::shared_ptr<ISpxAudioPumpInit> audioPump;
    std::shared_ptr<ISpxAudioStreamReader> reader;

    // Create the stream pump
    auto cbFormat = stream->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    stream->GetFormat(waveformat.get(), cbFormat);

    if (waveformat->wFormatTag != WAVE_FORMAT_PCM)
    {
        m_codecAdapter = SpxCreateObjectWithSite<ISpxAudioStreamReader>("CSpxCodecAdapter", GetSite());
        SPX_IFTRUE_THROW_HR(m_codecAdapter == nullptr, SPXERR_GSTREAMER_NOT_FOUND_ERROR);

        reader = SpxQueryInterface<ISpxAudioStreamReader>(stream);

        auto initCallbacks = SpxQueryInterface<ISpxAudioStreamReaderInitCallbacks>(m_codecAdapter);
        initCallbacks->SetCallbacks(
            [=](uint8_t* buffer, uint32_t size) { return reader->Read(buffer, size); },
            [=]() { { reader->Close(); } });

        initCallbacks->SetPropertyCallback2(
            [=](PropertyId propertyId) {
            return reader->GetProperty(propertyId);
        });

        auto adapterAsSetFormat = SpxQueryInterface<ISpxAudioStreamInitFormat>(m_codecAdapter);

        string numChannelsString = GetStringValue("OutputPCMChannelCount", "1");
        string numBitsPerSampleString = GetStringValue("OutputPCMNumBitsPerSample", "16");
        string sampleRateString = GetStringValue("OutputPCMSamplerate", "16000");

        try
        {
            waveformat->nChannels = (uint16_t)std::stoi(numChannelsString);
            waveformat->wBitsPerSample = (uint16_t)std::stoi(numBitsPerSampleString);
            waveformat->nSamplesPerSec = (uint32_t)std::stoi(sampleRateString);
        }
        catch (const std::exception& e)
        {
            SPX_DBG_TRACE_VERBOSE("Error Parsing %s", e.what());
            SPX_DBG_TRACE_VERBOSE("Setting default output format samplerate = 16000, bitspersample = 16 and numchannels = 1");
            waveformat->nChannels = 1;
            waveformat->wBitsPerSample = 16;
            waveformat->nSamplesPerSec = 16000;
        }

        adapterAsSetFormat->SetFormat(waveformat.get());
    }

    audioPump = SpxCreateObjectWithSite<ISpxAudioPumpInit>("CSpxAudioPump", this);

    m_audioPump = SpxQueryInterface<ISpxAudioPump>(audioPump);

    // Attach the stream to the pump
    if (m_codecAdapter == nullptr)
    {
        reader = SpxQueryInterface<ISpxAudioStreamReader>(stream);
    }
    else
    {
        reader = SpxQueryInterface<ISpxAudioStreamReader>(m_codecAdapter);
    }

    audioPump->SetReader(reader);

    SetStringValue(GetPropertyName(PropertyId::AudioConfig_AudioSource), g_audioSourceStream);
    SetAudioConfigurationInProperties();

    m_isReliableDelivery = true;
}

void CSpxAudioStreamSession::SetAudioConfigurationInProperties()
{
    auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    m_audioPump->GetFormat(waveformat.get(), cbFormat);

    // Make sure channel count is consistent if it has been set.
    auto channelCountInProperty = GetStringValue(GetPropertyName(PropertyId::AudioConfig_NumberOfChannelsForCapture), "");
    if (channelCountInProperty.empty())
    {
        SetStringValue(GetPropertyName(PropertyId::AudioConfig_NumberOfChannelsForCapture), to_string(waveformat->nChannels).c_str());
    }
    else
    {
        SPX_THROW_HR_IF(SPXERR_RUNTIME_ERROR, std::stoi(channelCountInProperty) != waveformat->nChannels);
    }

    SetStringValue(GetPropertyName(PropertyId::AudioConfig_SampleRateForCapture), to_string(waveformat->nSamplesPerSec).c_str());
    SetStringValue(GetPropertyName(PropertyId::AudioConfig_BitsPerSampleForCapture), to_string(waveformat->wBitsPerSample).c_str());

    // copy input audio device name retrieved from system
    SetStringValue("SPEECH-MicrophoneNiceName", m_audioPump->GetPropertyValue("SPEECH-MicrophoneNiceName").c_str());
}

void CSpxAudioStreamSession::WriteTracingEvent()
{
#ifdef _WIN32
    auto sessionId = PAL::ToString(m_sessionId);
    auto micName = GetStringValue("SPEECH-MicrophoneNiceName", "");

    TraceLoggingWrite(tracingEventProvider, "RecognizerCreationEvent",
        TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
        TraceLoggingString(sessionId.c_str(), "SessionId"),
        TraceLoggingString(micName.c_str(), "Microphone"));
#endif
}

void CSpxAudioStreamSession::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    auto format = pformat ? make_shared<SPXWAVEFORMATEX>(*pformat) : nullptr;
    auto task = CreateTask([this, format]() {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::SetFormat: format %s nullptr", (void*)this, format == nullptr ? "==" : "!=");

        if (format != nullptr && ChangeState(SessionState::WaitForPumpSetFormatStart, SessionState::ProcessingAudio))
        {
            // The pump started successfully, we have a live running session now!
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::SetFormat: Now ProcessingAudio ...", (void*)this);
            InformAdapterSetFormatStarting(format.get());
        }
        else if (format == nullptr && ChangeState(SessionState::StoppingPump, SessionState::WaitForAdapterCompletedSetFormatStop))
        {
            // Our stop pump request has been satisfied... Let's wait for the adapter to finish...
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::SetFormat: Now WaitForAdapterCompletedSetFormatStop (from StoppingPump)...", (void*)this);
            InformAdapterSetFormatStopping(SessionState::StoppingPump);
        }
        else if (format == nullptr && ChangeState(SessionState::ProcessingAudio, SessionState::ProcessingAudioLeftovers))
        {
            // The pump stopped itself... That's possible when WAV files reach EOS. Let's wait for the adapter to finish...
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::SetFormat: Have seen the end of the stream on the client, processing audio leftovers ...", (void*)this);

            // Currently there are no sufficient tests for KWS, so we preserve the old logic,
            // For single shot and continuous we are processing the last pieces of data without indicating the end of the stream.
            if (!m_audioBuffer->NonAcknowledgedSizeInBytes() || (m_recoKind != RecognitionKind::SingleShot && m_recoKind != RecognitionKind::Continuous))
            {
                SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::SetFormat: Now WaitForAdapterCompletedSetFormatStop (from ProcessingAudio) ...", (void*)this);

                ChangeState(SessionState::ProcessingAudioLeftovers, SessionState::WaitForAdapterCompletedSetFormatStop);
                InformAdapterSetFormatStopping(SessionState::ProcessingAudio);
                EncounteredEndOfStream();
            }
        }
        else if (format == nullptr && IsKind(RecognitionKind::KwsSingleShot) && IsState(SessionState::HotSwapPaused))
        {
            // The pump stopped itself, probably due to the end of stream.
            // Wait until the hot swap is done before taking action.
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::SetFormat: AudioPump thread has stopped!", (void*)this);
            m_audioPumpStoppedBeforeHotSwap = true;
        }
        else
        {
            SPX_THROW_HR(SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION);
        }
    });

    m_threadService->ExecuteAsync(move(task));
}

void CSpxAudioStreamSession::ProcessAudio(const DataChunkPtr& audioChunk)
{
    SPX_TRACE_VERBOSE("Received audio chunk: time: %s, size:%d.", PAL::GetTimeInString(audioChunk->receivedTime).c_str(), audioChunk->size);

    SlowDownThreadIfNecessary(audioChunk->size);
    auto task = CreateTask([=]() {
        bool overflowHappened = false;
        {
            if (!m_audioBuffer)
            {
                SPX_DBG_TRACE_VERBOSE("%s: Session has been shutdown while processing was in flight, buffer has already been destroyed", __FUNCTION__);
                return;
            }

            auto buffered = milliseconds(m_audioBuffer->StashedSizeInBytes() * 1000 / m_format->nAvgBytesPerSec);
            overflowHappened = buffered > m_maxBufferedBeforeOverflow;
            if (overflowHappened)
            {
                // drop everything from the buffer
                SPX_DBG_TRACE_VERBOSE("%s: Overflow happened, dropping the buffer and resetting the adapter, stashed size %" PRIu64 " bytes.", __FUNCTION__, m_audioBuffer->StashedSizeInBytes());
                m_audioBuffer->Drop();
            }
        }

        if (overflowHappened)
        {
            // In case of overflow we send the error and reset the adapter
            Error(m_recoAdapter.get(), std::make_shared<SpxRecoEngineAdapterError>(false, CancellationReason::Error, CancellationErrorCode::ServiceTimeout, "Due to service inactivity the client buffer size exceeded. Resetting the buffer."));
            StartResetEngineAdapter();
            return;
        }

        {
            // Add the chunk to the buffer.
            if (!m_audioBuffer || !m_audioProcessor)
            {
                SPX_DBG_TRACE_VERBOSE("%s: Session has been shutdown while processing was in flight, buffer/processor has already been destroyed", __FUNCTION__);
                return;
            }

            m_audioBuffer->Add(audioChunk);
        }

        while (ProcessNextAudio())
        {
        }
    });

    // Synchronously execute on the background thread.
    m_threadService->ExecuteAsync(move(task));
}

void CSpxAudioStreamSession::SlowDownThreadIfNecessary(uint32_t dataSize)
{
    // Slowdown the thread anytime we have sent more than 5 seconds of audio.

    unsigned long slowdownRate = UINT_MAX;
    m_bytesTransited += dataSize;

    auto buffer = m_audioBuffer;

    if (m_bytesTransited >= m_maxFastLaneSizeBytes)
    {
        slowdownRate = m_simulateRealtimePercentage;
    }

    // The amount of time we should ensure current audio packet takes to process.
    auto currentPacketAudioDelay = std::chrono::milliseconds((long)((dataSize * 1000 / m_avgBytesPerSecond) * (1 / (slowdownRate / 100.0))));

    // Calculate how long we need to delay the current thread.
    // This is the delta between the current time, and the previously calculated next audio time if > 0.
    milliseconds delayInterval;

    if (!m_useDurationBasedThrottle)
    {
        delayInterval = std::max(duration_cast<milliseconds>(m_nextAudioProcessTime - steady_clock::now()),
            milliseconds(0));
    }
    else
    {
        delayInterval = currentPacketAudioDelay;
    }

    if (delayInterval.count() > 0)
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::SlowDownThreadIfNecessary: Stashing ... sleeping for %llu ms", (void*)this, delayInterval.count());
        std::this_thread::sleep_for(delayInterval);
    }

    if (!m_useDurationBasedThrottle)
    {
        // The next time we will process an audio packet.
        m_nextAudioProcessTime = steady_clock::now() + currentPacketAudioDelay;
    }
}

bool CSpxAudioStreamSession::ProcessNextAudio()
{
    if ((m_sessionState == SessionState::ProcessingAudio || m_sessionState == SessionState::ProcessingAudioLeftovers)
        && !m_adapterAudioMuted)
    {
        AudioBufferPtr buffer;
        std::shared_ptr<ISpxAudioProcessor> processor;
        std::shared_ptr<ISpxAudioProcessor> speechProcessor;
        KWSProcessorMode kwsProcessorMode = KWSProcessorMode::None;

        {
            buffer = m_audioBuffer;
            processor = m_audioProcessor;
            speechProcessor = m_speechProcessor;
            kwsProcessorMode = m_kwsProcessorMode;
        }

        if (!buffer || !processor)
        {
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::ProcessNextAudio: Session has been shutdown while processing was in flight, buffer/processor has already been destroyed", (void*)this);
            return false;
        }

        auto item = buffer->GetNext();
        if (item)
        {
            if (kwsProcessorMode != KWSProcessorMode::None)
            {
                // In KWS we always discard what we give out, because currently
                // there is no ACKING logic from KWS adapter, this will change with unidec in place.
                buffer->DiscardBytes(item->size);
            }

            processor->ProcessAudio(item);

            if (speechProcessor)
            {
                speechProcessor->ProcessAudio(item);
            }

            if (item->size > 0)
            {
                m_fireEndOfStreamAtSessionStop = false;
            }

            return true;
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::ProcessNextAudio: no audio buffer in the m_audioBuffer.", (void*)this);
        }
    }
    else if (m_sessionState == SessionState::HotSwapPaused || m_adapterAudioMuted)
    {
        // Don't process this data, if we're paused, it has been buffered...
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::ProcessNextAudio Saving for later ... sessionState %d; adapterRequestedIdle %s", (void*)this, m_sessionState, m_adapterAudioMuted ? "true" : "false");
    }
    else if (m_sessionState == SessionState::StoppingPump)
    {
        // Don't process this data if we're actively stopping, it is buffered and
        // we will process it if the source is resilient...
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::ProcessNextAudio: Stopping pump, not processing data", (void*)this);
    }
    else
    {
        SPX_TRACE_ERROR("[%p]CSpxAudioStreamSession::ProcessNextAudio: Unexpected SessionState: recoKind %d; sessionState %d", (void*)this, m_recoKind, m_sessionState);
    }
    return false;
}

const std::wstring& CSpxAudioStreamSession::GetSessionId() const
{
    return m_sessionId;
}

void CSpxAudioStreamSession::AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer)
{
    SPX_DBG_TRACE_FUNCTION();

    unique_lock<mutex> lock(m_recognizersLock);
    m_recognizers.push_back(recognizer);
}

void CSpxAudioStreamSession::SetConversation(std::shared_ptr<ISpxConversation> conversation)
{
    SPX_DBG_TRACE_FUNCTION();

    unique_lock<mutex> lock(m_conversationLock);
    m_conversation = conversation;
}

void CSpxAudioStreamSession::RemoveRecognizer(ISpxRecognizer* recognizer)
{
    SPX_DBG_TRACE_FUNCTION();

    unique_lock<mutex> lock(m_recognizersLock);
    m_recognizers.remove_if([&](weak_ptr<ISpxRecognizer>& item)
    {
        return item.lock().get() == recognizer;
    });
}

void CSpxAudioStreamSession::OpenConnection(bool forContinuousRecognition)
{
    auto task = CreateTask([=]() {
        SPX_IFTRUE_THROW_HR(!IsState(SessionState::Idle) && !IsKind(RecognitionKind::Keyword), SPXERR_CHANGE_CONNECTION_STATUS_NOT_ALLOWED);
        EnsureInitRecoEngineAdapter();
        m_recoAdapter->OpenConnection(forContinuousRecognition ? false : true);
    }, false);

    shared_future<void> taskFuture(task.get_future());
    promise<bool> executed;
    shared_future<bool> executedFuture(executed.get_future());
    m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, move(executed));
    if (executedFuture.get())
    {
        taskFuture.get();
    }
}

void CSpxAudioStreamSession::CloseConnection()
{
    auto task = CreateTask([=]() {
        SPX_IFTRUE_THROW_HR(!IsState(SessionState::Idle) && !IsKind(RecognitionKind::Keyword), SPXERR_CHANGE_CONNECTION_STATUS_NOT_ALLOWED);
        if (m_recoAdapter != nullptr)
        {
            m_recoAdapter->CloseConnection();
        }
    }, false);

    shared_future<void> taskFuture(task.get_future());
    promise<bool> executed;
    shared_future<bool> executedFuture(executed.get_future());
    m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, move(executed));
    if (executedFuture.get())
    {
        taskFuture.get();
    }
}

// Thread safe method used on the boundary of the API, gets called by incoming threads.
void CSpxAudioStreamSession::WaitForIdle(std::chrono::milliseconds timeout)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::WaitForIdle: Timeout happened during waiting for Idle", (void*)this);
    unique_lock<mutex> lock(m_stateMutex);
    auto success = m_cv.wait_for(lock, timeout, [&]
    {
        return IsState(SessionState::Idle) || (IsKind(RecognitionKind::Keyword) && (IsState(SessionState::ProcessingAudio)));
    });
    if (!success)
    {
        SPX_TRACE_WARNING("[%p]CSpxAudioStreamSession::WaitForIdle: Timeout happened during waiting for Idle", (void*)this);
    }
}

template<class  ISpx_Recognizer_Type>
bool CSpxAudioStreamSession::IsRecognizerType() const noexcept
{
    /* We need a better way of doing this. */
    std::shared_ptr<ISpxRecognizer> recognizer;
    {
        std::unique_lock<std::mutex> lock{ m_recognizersLock };
        SPX_DBG_ASSERT(m_recognizers.size() == 1); // we only support 1 recognizer today...
        recognizer = m_recognizers.front().lock();
    }
    auto ispxrecognizer = SpxQueryInterface<ISpx_Recognizer_Type>(recognizer);
    return ispxrecognizer != nullptr;
}

CSpxAsyncOp<std::string> CSpxAudioStreamSession::SendActivityAsync(std::string activity)
{
    SPX_DBG_TRACE_FUNCTION();

    auto keep_alive = SpxSharedPtrFromThis<ISpxSession>(this);

    /* Need to change thread service to support generic tasks */
    std::shared_future<std::string> taskFuture = std::async(std::launch::async, [this, activity{ std::move(activity) }, keep_alive]()
    {
        auto interaction_id = CSpxInteractionIdProvider::NextInteractionId(InteractionIdPurpose::Activity);

        auto task = CreateTask([&]() {
            auto message_payload = nlohmann::json::parse(activity);
            nlohmann::json message = {
                { "version", "0.5"},
                { "context", {
                    {"interactionId", interaction_id}
                }},
                { "messagePayload", message_payload }
            };
            EnsureInitRecoEngineAdapter();
            m_recoAdapter->SendAgentMessage(message.dump());
        });
        std::promise<bool> executed;
        std::shared_future<bool> executedFuture{ executed.get_future() };
        m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, std::move(executed));
        if (!executedFuture.get())
        {
            // The task has not been even executed, throwing.
            SPX_THROW_HR(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
        }

        return interaction_id;
    });
    return CSpxAsyncOp<std::string>{ taskFuture, AOS_Started};
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxAudioStreamSession::RecognizeAsync()
{
    SPX_DBG_TRACE_FUNCTION();

    // Wait till single shot result has been received and report to the user.
    // Because we are blocking here, we do this on std::async.
    auto keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    shared_future<shared_ptr<ISpxRecognitionResult>> waitForResult(async(launch::async, [this, keepAlive]() {

        auto singleShotInFlight = make_shared<Operation>(RecognitionKind::SingleShot);
        auto task = CreateTask([=]() { RecognizeOnceAsync(singleShotInFlight); });

        // Wait for the task execution.
        promise<bool> executed;
        shared_future<bool> executedFuture(executed.get_future());
        m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, move(executed));
        if (!executedFuture.get())
        {
            // The task has not been even executed, throwing.
            // This can happen i.e. if the user disposing the recognizer while
            // async is still in flight.
            SPX_THROW_HR(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
        }

        auto result = singleShotInFlight->m_future.get();
        // Make sure we are in Idle state due to end turn.
        WaitForIdle(StopRecognitionTimeout);
        return result;
    }));

    return CSpxAsyncOp<shared_ptr<ISpxRecognitionResult>>(waitForResult, AOS_Started);
}

void CSpxAudioStreamSession::RecognizeOnceAsync(const shared_ptr<Operation>& singleShot, std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::RecognizeAsync kicked-off THREAD started ***", "*** CSpxAudioStreamSession::RecognizeAsync kicked-off THREAD stopped ***");

    auto& currentSingleShot = singleShot->m_kind == RecognitionKind::KeywordOnce ?
        m_singleShotKeywordInFlight : m_singleShotInFlight;

    // Keep track of the fact that we have a thread pending waiting to hear
    // what the final recognition result is, and then start/stop recognizing...
    if (currentSingleShot != nullptr)
    {
        // There is another single shot in flight. Report an error.
        singleShot->m_promise.set_exception(
            make_exception_ptr(ExceptionWithCallStack(SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION)));
        return;
    }

    // Start recognizing.
    currentSingleShot = singleShot;
    StartRecognizing(singleShot->m_kind, model);

    if (singleShot->m_kind == RecognitionKind::SingleShot)
    {
        // Schedule timeout for the operation.
        auto cancelTimer = CreateTask([this, singleShot]() {
            auto status = singleShot->m_future.wait_for(0ms);
            if (status != future_status::ready &&
                m_singleShotInFlight &&
                m_singleShotInFlight->m_operationId == singleShot->m_operationId)
            {
                // No result, timeout it.
                EnsureFireResultEvent();
            }
        });

        m_threadService->ExecuteAsync(move(cancelTimer), Operation::Timeout);
    }
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StartContinuousRecognitionAsync()
{
    return StartRecognitionAsync(RecognitionKind::Continuous);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StopContinuousRecognitionAsync()
{
    return StopRecognitionAsync(RecognitionKind::Continuous);
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxAudioStreamSession::RecognizeAsync(std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_TRACE_FUNCTION();

    // Wait till single shot result has been received and report to the user.
    // Because we are blocking here, we do this on std::async.
    auto keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::shared_future<std::shared_ptr<ISpxRecognitionResult>> waitForResult(std::async(std::launch::async, [this, model, keepAlive]()
    {
        auto singleShotInFlight = std::make_shared<Operation>(RecognitionKind::KeywordOnce);
        auto task = CreateTask([=]()
        {
            RecognizeOnceAsync(singleShotInFlight, model);
        });

        // Wait for the task execution.
        std::promise<bool> executed;
        std::shared_future<bool> executedFuture(executed.get_future());
        m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, move(executed));
        if (!executedFuture.get())
        {
            // The task has not been even executed, throwing.
            // This can happen i.e. if the user disposing the recognizer while
            // async is still in flight.
            SPX_THROW_HR(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
        }

        auto result = singleShotInFlight->m_future.get();
        // Make sure we are in Idle state due to end turn.
        WaitForIdle(StopRecognitionTimeout);
        return result;
    }));

    return CSpxAsyncOp<shared_ptr<ISpxRecognitionResult>>(waitForResult, AOS_Started);
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
    auto keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    shared_future<void> started(async(launch::async, [=]() {
        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::StartRecognitionAsync kicked-off THREAD started ***", "*** CSpxAudioStreamSession::StartRecognitionAsync kicked-off THREAD stopped ***");

        auto task = CreateTask([=]() { StartRecognizing(startKind, model); }, false);

        shared_future<void> taskFuture(task.get_future());
        promise<bool> executed;
        shared_future<bool> executedFuture(executed.get_future());
        m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, move(executed));
        if (executedFuture.get())
        {
            taskFuture.get();
        }
    }));

    return CSpxAsyncOp<void>(started, AOS_Started);
}

CSpxAsyncOp<void> CSpxAudioStreamSession::StopRecognitionAsync(RecognitionKind stopKind)
{
    SPX_DBG_TRACE_FUNCTION();

    // Wait till the session switches to Idle state.
    // Because we are blocking here, we do this on std::async.
    auto keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    shared_future<void> waitForIdle(async(launch::async,
    [this, keepAlive, stopKind]()
    {
        auto task = CreateTask([=]() {
            SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::StopRecognitionAsync kicked-off THREAD started ***", "*** CSpxAudioStreamSession::StopRecognitionAsync kicked-off THREAD stopped ***");
            StopRecognizing(stopKind);
        });

        shared_future<void> taskFuture(task.get_future());
        promise<bool> executed;
        shared_future<bool> stoppedStarted(executed.get_future());
        m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, move(executed));
        if (stoppedStarted.get())
        {
            taskFuture.get();
        }

        WaitForIdle(StopRecognitionTimeout);
    }));

    return CSpxAsyncOp<void>(waitForIdle, AOS_Started);
}

void CSpxAudioStreamSession::StartRecognizing(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartRecognizing", (void*)this);

    if (IsKind(startKind) && IsKeywordKind() && m_kwsModel && (model->GetFileName() == m_kwsModel->GetFileName()))
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartRecognizing: Already recognizing keyword, ignoring call...", (void*)this);
        return;
    }

    if (startKind == RecognitionKind::Keyword)
    {
        Ensure16kHzSampleRate();
    }

    if (ChangeState(RecognitionKind::Idle, SessionState::Idle, startKind, SessionState::WaitForPumpSetFormatStart))
    {
        // We're starting from idle!! Let's get the Audio Pump running
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartRecognizing:  Now WaitForPumpSetFormatStart ...", (void*)this);

        // Make sure we drop left overs of the previous recognition
        // if the source does not need reliability guarantees.
        if (m_audioBuffer && !m_isReliableDelivery)
        {
            m_audioBuffer->Drop();
        }
        StartAudioPump(startKind, model);
    }
    else if (
        ChangeState(RecognitionKind::Keyword, SessionState::ProcessingAudio, startKind, SessionState::HotSwapPaused) ||
        ChangeState(RecognitionKind::KeywordOnce, SessionState::ProcessingAudio, startKind, SessionState::HotSwapPaused)
        )
    {
        // We're moving from Keyword/ProcessingAudio to startKind/Paused...
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartRecognizing: Now Paused ...", (void*)this);

        HotSwapAdaptersWhilePaused(startKind, model);
    }
    else if (m_resetRecoAdapter != nullptr && startKind == RecognitionKind::Continuous &&
            ChangeState(RecognitionKind::Continuous, SessionState::ProcessingAudio, RecognitionKind::Continuous, SessionState::HotSwapPaused))
    {
        SPX_DBG_TRACE_WARNING("[%p]CSpxAudioStreamSession::StartRecognizing: Resetting adapter via HotSwap (ProcessingAudio -> HotSwapPaused) ... attempting to stay in continuous mode!!! ...", (void*)this);

        HotSwapAdaptersWhilePaused(startKind, model);

        SPX_DBG_TRACE_WARNING("[%p]CSpxAudioStreamSession::StartRecognizing: Simulating GetSite()->AdapterCompletedSetFormatStop() ...", (void*)this);
        AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio::Speech);
    }
    else if (m_resetRecoAdapter != nullptr && startKind == RecognitionKind::Continuous &&
        ChangeState(RecognitionKind::Continuous, SessionState::ProcessingAudioLeftovers, RecognitionKind::Continuous, SessionState::HotSwapPaused))
    {
        SPX_DBG_TRACE_WARNING("[%p]CSpxAudioStreamSession::StartRecognizing: Resetting adapter via HotSwap (ProcessingAudioLeftovers -> HotSwapPaused) ... attempting to stay in continuous mode!!! ...", (void*)this);

        HotSwapAdaptersWhilePaused(startKind, model);

        // Change back and replay the leftovers.
        SPX_DBG_TRACE_WARNING("[%p]CSpxAudioStreamSession::StartRecognizing: Resending audio leftovers ...", (void*)this);
        ChangeState(SessionState::HotSwapPaused, SessionState::ProcessingAudioLeftovers);
        while (ProcessNextAudio())
        {}
    }
    else if (IsKindKeyword(startKind) && !IsKeywordKind() && IsState(SessionState::ProcessingAudio))
    {
        // We're already doing something other than keyword spotting, but, someone wants to change/update the keyword ...
        // So ... let's update it
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartRecognizing: Changing keyword ... nothing else...", (void*)this);
        m_kwsModel = model;
    }
    else
    {
        // All other state transitions are invalid when attempting to start recognizing...
        SPX_TRACE_ERROR("[%p]CSpxAudioStreamSession::StartRecognizing: Unexpected/Invalid State Transition: recoKind %d; sessionState %d", (void*)this, m_recoKind, m_sessionState);
        SPX_THROW_HR(SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION);
    }
}

void CSpxAudioStreamSession::StopRecognizing(RecognitionKind stopKind)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing ...", (void*)this);

    SPX_DBG_ASSERT(stopKind != RecognitionKind::Idle);

    if (m_kwsModel != nullptr && stopKind != RecognitionKind::Keyword && IsState(SessionState::ProcessingAudio) &&
        ChangeState(stopKind, SessionState::ProcessingAudio, RecognitionKind::Keyword, SessionState::HotSwapPaused))
    {
        // We've got a keyword, we're not trying to stop keyword spotting, and we're currently procesing audio...
        // So ... We should hot swap over to the keyword spotter (which will stop the current adapter doing whatever its doing)
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Now Keyword/Paused ...", (void*)this);

        HotSwapAdaptersWhilePaused(RecognitionKind::Keyword, m_kwsModel);
        if (stopKind == RecognitionKind::KwsSingleShot || stopKind == RecognitionKind::SingleShot)
        {
            FireSessionStoppedEvent();
        }
    }
    else if (IsKindKeyword(stopKind) && IsKeywordKind() &&
            (
                ChangeState(stopKind, SessionState::ProcessingAudio, RecognitionKind::Keyword, SessionState::StoppingPump) ||
                /* This covers the scenario where we stop a KeywordOnce right after we receive the keyword detected */
                ChangeState(stopKind, SessionState::HotSwapPaused, stopKind, SessionState::StoppingPump)
            ))
    {
        // We're actually keyword spotting right now, and we've been asked to stop doing that...
        // So ... Let's clear our keyword (since we're not spotting, we don't need it anymore)
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Changing keyword to '' ... nothing else...", (void*)this);
        m_kwsModel.reset();

        // And we'll stop the pump
        auto audioPump = m_audioPump;
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Now StoppingPump[%p] ...", (void*)this, (void*)audioPump.get());
        if (audioPump)
        {
            audioPump->StopPump();
        }
        if (m_codecAdapter)
        {
            m_codecAdapter->Close();
        }

    }
    else if (stopKind == RecognitionKind::Keyword && !IsKind(RecognitionKind::Keyword))
    {
        // We've been asked to stop keyword spotting, but we're not keyword spotting right now ...
        // So ... Let's just clear the keyword
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Changing keyword to '' ... nothing else...", (void*)this);
        m_kwsModel.reset();
    }
    else if (stopKind == RecognitionKind::KwsSingleShot && !IsKind(RecognitionKind::KwsSingleShot))
    {
        // We've been asked to stop KwsSingleShot, but we've already stopped that, and have switched back to Keyword, or Idle
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Already stopped KwsSingleShot ... recoKind %d; sessionState %d", (void*)this, m_recoKind, m_sessionState);
    }
    else if (stopKind == RecognitionKind::KwsSingleShot && IsKind(RecognitionKind::KwsSingleShot) &&
             ChangeState(stopKind, SessionState::ProcessingAudio, stopKind, SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // TODO: RobCh: Is this ever called?!! How does it not get eaten by the first if statement above?!!

        // We're going to wait for done ... (which will flip us back to wherever we need to go...)
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Now KwsSingleShot/WaitForAdapterCompletedSetFormatStop ...", (void*)this);
    }
    else if (ChangeState(SessionState::ProcessingAudio, SessionState::StoppingPump))
    {
        // We've been asked to stop whatever it is we're doing, while we're actively processing audio ...
        // So ... Let's stop the pump
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: We've been asked to stop whatever it is we're doing, while we're actively processing audio ...", (void*)this);

        auto audioPump = m_audioPump;
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Now StoppingPump[%p] ...", (void*)this, (void*)audioPump.get());
        if (audioPump)
        {
            audioPump->StopPump();
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Pump has already been released", (void*)this);
        }
        if (m_codecAdapter)
        {
            m_codecAdapter->Close();
        }

    }
    else if (IsState(SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // If we're already in the WaitForAdapterCompletedSetFormatStop state... That's fine ... We'll eventually transit to Idle once AdapterCompletedSetFormatStop() is called...
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Now (still) WaitForAdapterCompletedSetFormatStop ...", (void*)this);
    }
    else if (IsState(SessionState::ProcessingAudioLeftovers))
    {
        // If we're already in the ProcessingAudioLeftovers state... That's fine ... We'll eventually transit to Idle once AdapterCompletedSetFormatStop() is called
        // and we processed all data.
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Now (still) ProcessingAudioLeftovers ...", (void*)this);
    }
    else if (IsKind(RecognitionKind::Idle) && IsState(SessionState::Idle))
    {
        // If we're already in the idle state... Awesome!!!
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Now Idle/Idle already...", (void*)this);
    }
    else
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StopRecognizing: Unexpected State: recoKind %d; sessionState %d", (void*)this, m_recoKind, m_sessionState);
    }
}

void CSpxAudioStreamSession::WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::WaitForRecognition_Complete: ...", (void*)this);

    FireResultEvent(GetSessionId(), result);

    /* If we have a kw single shot active, we need to resolve it */
    if (m_singleShotKeywordInFlight)
    {
        m_singleShotKeywordInFlight->m_promise.set_value(result);
        m_singleShotKeywordInFlight = nullptr;
    }

    if (m_singleShotInFlight)
    {
        // Make sure we fullfil the promise if StopRecognizing throws.
        auto operation = m_singleShotInFlight;
        auto finish = shared_ptr<void>(nullptr, [&](void*) { operation->m_promise.set_value(result); });
        m_singleShotInFlight = nullptr;
        StopRecognizing(operation->m_kind);
    }
}

void CSpxAudioStreamSession::FireSessionStartedEvent()
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireSessionStartedEvent: ...", (void*)this);
    std::wstring sessionIdOverride;
    /* For dialog service connector, we replace session id with the interaction id that's going to be send in the context message. */
    if (IsRecognizerType<ISpxDialogServiceConnector>())
    {
        sessionIdOverride = PAL::ToWString(CSpxInteractionIdProvider::NextInteractionId(InteractionIdPurpose::Speech));
    }

    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireSessionStartedEvent: Firing SessionStarted event: SessionId: %ls", (void*)this, m_sessionId.c_str());

    FireEvent(EventType::SessionStart, nullptr, sessionIdOverride.empty() ? nullptr : sessionIdOverride.c_str());
    m_fireEndOfStreamAtSessionStop = true;
}

void CSpxAudioStreamSession::FireSessionStoppedEvent()
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireSessionStoppedEvent: Firing SessionStopped event: SessionId: %ls", (void*)this, m_sessionId.c_str());

    /* For dialog service connector, we replace session id with the interaction id that was sent in the context message. */
    std::wstring sessionIdOverride;
    if (IsRecognizerType<ISpxDialogServiceConnector>())
    {
        sessionIdOverride = PAL::ToWString(CSpxInteractionIdProvider::GetInteractionId(InteractionIdPurpose::Speech));
    }

    EnsureFireResultEvent();

    FireEvent(EventType::SessionStop, nullptr, sessionIdOverride.empty() ? nullptr : sessionIdOverride.c_str());
}

void CSpxAudioStreamSession::FireConnectedEvent()
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireConnectedEvent", (void*)this);

    FireEvent(EventType::Connected);
}

void CSpxAudioStreamSession::FireDisconnectedEvent()
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireDisconnectedEvent", (void*)this);

    FireEvent(EventType::Disconnected);
}

void CSpxAudioStreamSession::FireConnectionMessageReceived(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool isBufferBinary)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireConnectionMessageReceived; path=%s", (void*)this, path.c_str());

    auto bufferKeepAlive = SpxAllocSharedUint8Buffer(bufferSize);
    memcpy(bufferKeepAlive.get(), buffer, bufferSize);

    auto task = CreateTask([=]() {
        ForEachRecognizer([=](auto recognizer) {
            auto ptr = SpxQueryInterface<ISpxRecognizerEvents>(recognizer);
            ptr->FireConnectionMessageReceived(headers, path, bufferKeepAlive.get(), bufferSize, isBufferBinary);
        });
    });
    m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::User);
}

void CSpxAudioStreamSession::FireSpeechStartDetectedEvent(uint64_t offset)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireSpeechStartDetectedEvent", (void*)this);

    FireEvent(EventType::SpeechStart, nullptr, nullptr, offset);
}

void CSpxAudioStreamSession::FireSpeechEndDetectedEvent(uint64_t offset)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireSpeechEndDetectedEvent", (void*)this);

    FireEvent(EventType::SpeechEnd, nullptr, nullptr, offset);
}

void CSpxAudioStreamSession::EnsureFireResultEvent()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::EnsureFireResultEvent", (void*)this);

    if (m_singleShotInFlight || (m_fireEndOfStreamAtSessionStop && m_sawEndOfStream))
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::EnsureFireResultEvent: Getting ready to fire ResultReason::Canceled result (sawEos=%d, fireEos=%d)", (void*)this, m_sawEndOfStream, m_fireEndOfStreamAtSessionStop);

        auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
        auto result = (m_fireEndOfStreamAtSessionStop && m_sawEndOfStream)
        ? factory->CreateFinalResult(nullptr, ResultReason::Canceled, NO_MATCH_REASON_NONE, CancellationReason::EndOfStream, CancellationErrorCode::NoError, nullptr, 0, 0)
        : factory->CreateFinalResult(nullptr, ResultReason::Canceled, NO_MATCH_REASON_NONE, CancellationReason::Error, CancellationErrorCode::ServiceTimeout, L"Timeout: no recognition result received.", 0, 0);

        WaitForRecognition_Complete(result);
        m_fireEndOfStreamAtSessionStop = false;
    }
}

void CSpxAudioStreamSession::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireResultEvent", (void*)this);

    auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);

    string errorDetails = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails));
    if (!errorDetails.empty())
    {
        errorDetails += " SessionId: " + PAL::ToString(m_sessionId);
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), errorDetails.c_str());
    }

    if (result->GetReason() == ResultReason::Canceled)
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::FireResultEvent: Firing RecoResultEvent(Canceled): SessionId: %ls", (void*)this, m_sessionId.c_str());
    }

    FireEvent(EventType::RecoResultEvent, result, const_cast<wchar_t*>(sessionId.c_str()));
}

void CSpxAudioStreamSession::DispatchEvent(const list<weak_ptr<ISpxRecognizer>>& weakRecognizers,
    const wstring& sessionId, EventType sessionType, uint64_t offset,
    std::shared_ptr<ISpxRecognitionResult> result,
    std::string activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    for (auto weakRecognizer : weakRecognizers)
    {
        string error;
        SPXAPI_TRY()
        {
            auto recognizer = weakRecognizer.lock();
            if (!recognizer)
            {
                continue;
            }

            auto ptr = SpxQueryInterface<ISpxRecognizerEvents>(recognizer);
            if (!ptr)
            {
                continue;
            }

            switch (sessionType)
            {
            case EventType::SessionStart:
                ptr->FireSessionStarted(sessionId);
                break;

            case EventType::SessionStop:
                ptr->FireSessionStopped(sessionId);
                break;

            case EventType::SpeechStart:
                ptr->FireSpeechStartDetected(sessionId, offset);
                break;

            case EventType::SpeechEnd:
                ptr->FireSpeechEndDetected(sessionId, offset);
                break;

            case EventType::RecoResultEvent:
                ptr->FireResultEvent(sessionId, result);
                break;

            case EventType::Connected:
                ptr->FireConnected(sessionId);
                break;

            case EventType::Disconnected:
                ptr->FireDisconnected(sessionId);
                break;

            case EventType::ActivityReceivedEvent:
            {
                auto c_events = SpxQueryInterface<ISpxDialogServiceConnectorEvents>(ptr);
                if (c_events != nullptr)
                {
                    c_events->FireActivityReceived(sessionId, std::move(activity), audio);
                }
                break;
            }
            default:
                SPX_TRACE_ERROR("EventDelivery unknown event type %d", (int)sessionType);
            }
        }
        SPXAPI_CATCH_ONLY()
        // We do not communicate anything to the user here
        // in order to avoid a live lock :
        //   failed event delivery causes other failed deliveries.
    }
}

void CSpxAudioStreamSession::FireEvent(EventType eventType, shared_ptr<ISpxRecognitionResult> result, const wchar_t* eventSessionId, uint64_t offset, std::string activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    // Make a copy of the recognizers (under lock), to use to send events;
    // otherwise the underlying list could be modified while we're sending events...

    list<weak_ptr<ISpxRecognizer>> weakRecognizers;
    {
        unique_lock<mutex> lock(m_recognizersLock);
        weakRecognizers.assign(m_recognizers.begin(), m_recognizers.end());
    }

    std::wstring sessionId = (eventSessionId != nullptr) ? eventSessionId : m_sessionId;

    // Schedule event dispatch on the user facing thread.
    // DispatchEvent is exception safe in order not to cause livelock of failed messages.
    // i.e. if some events cannot be delivered to the user, we do not try to deliver events about failed events...
    auto task = CreateTask([this, weakRecognizers, sessionId, eventType, offset, result, activity{ std::move(activity) }, audio]()
    {
        SPX_DBG_TRACE_SCOPE("DispatchEvent task started...", "DispatchEvent task complete!");
        DispatchEvent(weakRecognizers, sessionId, eventType, offset, result, std::move(activity), audio);
    }, false);
    m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::User);
}

void CSpxAudioStreamSession::KeywordDetected(ISpxKwsEngineAdapter* adapter, uint64_t offset, uint64_t duration, double confidence, const std::string& keyword, const DataChunkPtr& audioChunk)
{
    UNUSED(adapter);

    SPX_DBG_TRACE_VERBOSE("[%p] CSpxAudioStreamSession::KeywordDetected: Keyword detected!! Starting KwsSingleShot recognition... offset=%" PRIu64 "; size=%d", (void*)this, offset, audioChunk->size);
    SPX_DBG_ASSERT_WITH_MESSAGE(m_threadService->IsOnServiceThread(), "called on wrong thread, must be thread service managed thread.");

    // Report that we have a keyword candidate
    auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));

    auto keywordVerificationEnabled = PAL::ToBool(GetStringValue(KeywordConfig_EnableKeywordVerification, PAL::BoolToString(false)));
    auto keywordOnly = PAL::ToBool(GetStringValue(g_keyword_KeywordOnly, PAL::BoolToString(false)));
    (void)keywordOnly;

    // Keyword verification is part of USP protocol, thus, we have to store the spotted keyword to enable keyword verification.
    // If keyword verification is enabled, fire the keyword result as intermediate result. Otherwise, fire it as intermediate and final result
    // so it's a consistent experience regardless of whether keyword verification is enabled or not.
    auto spottedKeywordResult = factory->CreateKeywordResult(confidence, offset, duration, PAL::ToWString(keyword).c_str(), ResultReason::RecognizingKeyword);
    FireResultEvent(GetSessionId(), spottedKeywordResult);

    if (!keywordVerificationEnabled) {
        spottedKeywordResult = factory->CreateKeywordResult(confidence, offset, duration, PAL::ToWString(keyword).c_str(), ResultReason::RecognizedKeyword);
        FireResultEvent(GetSessionId(), spottedKeywordResult);

        if (m_singleShotKeywordInFlight)
        {
            m_singleShotKeywordInFlight->m_promise.set_value(spottedKeywordResult);
            m_singleShotKeywordInFlight = nullptr;
        }

        // Set spottedKeywordResult to null since KWV is off and there is no need to verify KWS spotted keyword
        spottedKeywordResult = nullptr;
    }

    if (ChangeState(RecognitionKind::Keyword, SessionState::ProcessingAudio, RecognitionKind::KwsSingleShot, SessionState::HotSwapPaused) ||
        ChangeState(RecognitionKind::KeywordOnce, SessionState::ProcessingAudio, RecognitionKind::KwsSingleShot, SessionState::HotSwapPaused))
    {
        if (!keywordOnly)
        {
            // We've been told a keyword was recognized... Stash the audio, and start KwsSingleShot recognition!!
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::KeywordDetected: Now KwsSingleShot/Paused ...", (void*)this);

            m_spottedKeyword = audioChunk;
            HotSwapToKwsSingleShotWhilePaused(spottedKeywordResult);
        }
        else
        {
            /* We get the kind based on the KWS mode we are in. */
            auto kind = OriginKindFromKWSMode(m_kwsProcessorMode);
            if (kind == RecognitionKind::Keyword)
            {
                /* For "continuous" keyword recognition, we just get the adapter back to processing audio. */
                SPX_DBG_ASSERT(ChangeState(RecognitionKind::KwsSingleShot, SessionState::HotSwapPaused, RecognitionKind::Keyword, SessionState::HotSwapPaused));
                HotSwapAdaptersWhilePaused(RecognitionKind::Keyword, m_kwsModel);
            }
            else
            {
                SPX_DBG_ASSERT(ChangeState(RecognitionKind::KwsSingleShot, SessionState::HotSwapPaused, RecognitionKind::KeywordOnce, SessionState::HotSwapPaused));
                StopRecognizing(RecognitionKind::KeywordOnce);
            }
        }
    }
}

void CSpxAudioStreamSession::GetScenarioCount(uint16_t* countSpeech, uint16_t* countIntent, uint16_t* countTranslation, uint16_t* countDialog, uint16_t* countTranscriber)
{
    unique_lock<mutex> lock(m_recognizersLock);
    if (m_recognizers.empty())
    {
        // we only support 1 recognizer today... but can be deleted if user is killing it right now.
        *countSpeech = *countIntent = *countTranslation = *countDialog =  *countTranscriber = 0;
        return;
    }

    SPX_DBG_ASSERT(m_recognizers.size() == 1);
    auto recognizer = m_recognizers.front().lock();
    auto intentRecognizer = SpxQueryInterface<ISpxIntentRecognizer>(recognizer);
    auto translationRecognizer = SpxQueryInterface<ISpxTranslationRecognizer>(recognizer);
    auto dialogServiceConnector = SpxQueryInterface<ISpxDialogServiceConnector>(recognizer);
    auto transcriber = SpxQueryInterface<ISpxConversationTranscriber>(recognizer);

    *countTranscriber = (transcriber != nullptr) ? 1 : 0;
    *countDialog = (dialogServiceConnector != nullptr) ? 1 : 0;
    *countIntent = (intentRecognizer != nullptr) ? 1 : 0;
    *countTranslation = (translationRecognizer != nullptr) ? 1 : 0;
    *countSpeech = 1 - *countIntent - *countTranslation - *countDialog - *countTranscriber;

    SPX_DBG_TRACE_VERBOSE("%s: countSpeech=%d; countIntent=%d; countTranslation=%d; countDialog=%d, countTranscriber=%d", __FUNCTION__, *countSpeech, *countIntent, *countTranslation, *countDialog, *countTranscriber);
}

std::list<std::string> CSpxAudioStreamSession::GetListenForList()
{
    unique_lock<mutex> lock(m_recognizersLock);
    SPX_DBG_ASSERT(m_recognizers.size() == 1);
    auto recognizer = m_recognizers.front().lock();
    lock.unlock();
    if (!recognizer)
    {
        SPX_TRACE_ERROR("%s: going to throw recognizer destroyed runtime_error", __FUNCTION__);
        ThrowRuntimeError("GetListenForList: Recognizer is already destroyed, cannot continue.");
    }
    // Get the listen for list from the recognizer(s)
    auto grammarlist = SpxQueryInterface<ISpxGrammar>(recognizer);
    auto listenForList = grammarlist->GetListenForList();

    // If we also have an lu adapter...
    if (m_luAdapter != nullptr)
    {
        // Get the listen for list for the lu adapter
        auto intentListenForList = GetListenForListFromLuEngineAdapter();

        // Combine the two listen for lists into a single list
        auto insertAt = listenForList.end();
        auto insertBegin = intentListenForList.begin();
        auto insertEnd = intentListenForList.end();
        listenForList.insert(insertAt, insertBegin, insertEnd);
    }

    return listenForList;
}

void CSpxAudioStreamSession::GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region)
{
    if (m_luAdapter != nullptr)
    {
        GetIntentInfoFromLuEngineAdapter(provider, id, key, region);
    }
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::GetSpottedKeywordResult()
{
    return m_singleShotInFlight != nullptr ? m_singleShotInFlight->m_spottedKeywordResult : nullptr;
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

    uint64_t bufferedBytes = 0;
    uint64_t previousTurnGlobalOffset = m_currentTurnGlobalOffset;
    if (m_audioBuffer)
    {
        m_audioBuffer->NewTurn();
        m_currentTurnGlobalOffset = m_audioBuffer->GetAbsoluteOffset();
        bufferedBytes = m_audioBuffer->StashedSizeInBytes();
    }
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterStoppedTurn: m_currentTurnGlobalOffset=%" PRIu64 ", previousTurnGlobalOffset=%" PRIu64 " bufferedBytes=%" PRIu64, (void*)this, m_currentTurnGlobalOffset, previousTurnGlobalOffset, bufferedBytes);

    auto bIsConversationTranscriber = IsRecognizerType<ISpxConversationTranscriber>();

    if (IsState(SessionState::ProcessingAudioLeftovers))
    {
        // In single shot we only have a single result, so simply stop so that the following RecognizeOnce can take place.
        if (m_recoKind != RecognitionKind::Continuous)
        {
            ChangeState(SessionState::ProcessingAudioLeftovers, SessionState::WaitForAdapterCompletedSetFormatStop);
            if (!bufferedBytes) // No more data, report to the user.
            {
                EncounteredEndOfStream();
            }
        }
        else // In continuous mode we have to make sure we resend all leftovers.
        {
            // Currently the last portion of data (silence) can be non acknowledged by the Bing service,
            // in order to avoid the live lock, we stop if there was no progress during the last
            // turn.
            if (!bufferedBytes || m_currentTurnGlobalOffset == previousTurnGlobalOffset || bIsConversationTranscriber)
            {
                SPX_TRACE_WARNING_IF(m_currentTurnGlobalOffset == previousTurnGlobalOffset, "[%p]CSpxAudioStreamSession::AdapterStoppedTurn: Dropping %d bytes due to no progress in the last turn", (void*)this, (int)bufferedBytes);
                ChangeState(SessionState::ProcessingAudioLeftovers, SessionState::WaitForAdapterCompletedSetFormatStop);
                EncounteredEndOfStream();
            }
            else
            {
                while (ProcessNextAudio())
                {
                }

                // Ensure that SessionStopped event is sent.
                bool useRecoEngineUnidec = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Unidec", PAL::BoolToString(false)));
                if (useRecoEngineUnidec)
                {
                    m_expectAdapterStoppedTurn = false;
                    ChangeState(SessionState::ProcessingAudioLeftovers, SessionState::WaitForAdapterCompletedSetFormatStop);
                }
            }
        }
    }

    if (IsState(SessionState::WaitForAdapterCompletedSetFormatStop))
    {
        // We are waiting for the adapter to confirm it got the SetFormat(nullptr), but we haven't sent the request yet...
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterStoppedTurn: Still WaitForAdapterCompletedSetFormatStop, calling ->SetFormat(nullptr) ...", (void*)this);
        InformAdapterSetFormatStopping(SessionState::WaitForAdapterCompletedSetFormatStop);
    }
    else if (m_adapterAudioMuted && IsState(SessionState::ProcessingAudio) &&
             IsKind(m_turnEndStopKind) &&  m_turnEndStopKind != RecognitionKind::Idle)
    {
        // The adapter previously requested to stop processing audio ... We can now safely stop recognizing...
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterStoppedTurn: ->StopRecognizing(%d) ...", (void*)this, m_turnEndStopKind);
        auto stopKind = m_turnEndStopKind;
        m_turnEndStopKind = RecognitionKind::Idle;
        StopRecognizing(stopKind);
    }
}

void CSpxAudioStreamSession::AdapterEndOfDictation(ISpxRecoEngineAdapter*, uint64_t offset, uint64_t duration)
{
    if (IsKind(RecognitionKind::Continuous))
    {
        auto buffer = m_audioBuffer;
        if (buffer)
        {
            buffer->DiscardTill(offset + duration);
        }
    }
}

void CSpxAudioStreamSession::AdapterDetectedSpeechStart(ISpxRecoEngineAdapter* adapter, uint64_t offset)
{
    UNUSED(adapter);
    if (!m_speechProcessor)
    {
        auto buffer = m_audioBuffer;
        offset = buffer ? buffer->ToAbsolute(offset) : offset;

        FireSpeechStartDetectedEvent(offset);
    }
}

void CSpxAudioStreamSession::AdapterDetectedSpeechEnd(ISpxRecoEngineAdapter* adapter, uint64_t originalOffset)
{
    UNUSED(adapter);
    if (!m_speechProcessor)
    {
        auto buffer = m_audioBuffer;
        auto offset = buffer ? buffer->ToAbsolute(originalOffset) : originalOffset;
        FireSpeechEndDetectedEvent(offset);
    }
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

std::shared_ptr<ISpxActivityEventArgs> CSpxAudioStreamSession::CreateActivityEventArgs(std::string activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    auto activityAudioEvent = SpxCreateObjectWithSite<ISpxActivityEventArgs>("CSpxActivityEventArgs", this);

    auto argsInit = SpxQueryInterface<ISpxActivityEventArgsInit>(activityAudioEvent);
    argsInit->Init(activity, audio);

    return activityAudioEvent;
}

std::shared_ptr<ISpxConnectionEventArgs> CSpxAudioStreamSession::CreateConnectionEventArgs(const std::wstring& sessionId)
{
    auto connectionEvent = SpxCreateObjectWithSite<ISpxConnectionEventArgs>("CSpxConnectionEventArgs", this);

    auto argsInit = SpxQueryInterface<ISpxConnectionEventArgsInit>(connectionEvent);
    argsInit->Init(sessionId);

    return connectionEvent;
}

std::shared_ptr<ISpxConnectionMessageEventArgs> CSpxAudioStreamSession::CreateConnectionMessageEventArgs(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool isBufferBinary)
{
    auto message = SpxCreateObjectWithSite<ISpxConnectionMessage>("CSpxConnectionMessage", this);
    auto messageInit = SpxQueryInterface<ISpxConnectionMessageInit>(message);
    messageInit->Init(headers, path, buffer, bufferSize, isBufferBinary);

    auto connectionMessageEvent = SpxCreateObjectWithSite<ISpxConnectionMessageEventArgs>("CSpxConnectionMessageEventArgs", this);
    auto argsInit = SpxQueryInterface<ISpxConnectionMessageEventArgsInit>(connectionMessageEvent);
    argsInit->Init(message);

    return connectionMessageEvent;
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

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, uint64_t offset, uint64_t duration)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitIntermediateResult(resultId, text, offset, duration);

    m_mostRecentIntermediateRecoResult = result;
    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxKeywordRecognitionResultInit>(result);
    initResult->InitKeywordResult(confidence, offset, duration, keyword, reason);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateFinalResult(const wchar_t* resultId, ResultReason reason, NoMatchReason noMatchReason, CancellationReason cancellation, CancellationErrorCode errorCode, const wchar_t* text, uint64_t offset, uint64_t duration, const wchar_t* userId)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(resultId, reason, noMatchReason, cancellation, errorCode, text, offset, duration);

    auto initConversationResult = SpxQueryInterface<ISpxConversationTranscriptionResultInit>(result);
    if (initConversationResult)
    {
        initConversationResult->InitConversationResult(userId);
    }

    m_mostRecentIntermediateRecoResult = nullptr;

    return result;
}

uint64_t CSpxAudioStreamSession::GetResultLatencyInMs(const ProcessedAudioTimestampPtr& audioTimestamp) const
{
    auto nowTime = system_clock::now();
    if (nowTime < audioTimestamp->chunkReceivedTime)
    {
        SPX_TRACE_ERROR("Unexpected error: result received time (%s) is earlier than audio received time (%s).",
            PAL::GetTimeInString(nowTime).c_str(), PAL::GetTimeInString(audioTimestamp->chunkReceivedTime).c_str());
        return 0;
    }

    uint64_t ticks = PAL::GetTicks(nowTime - audioTimestamp->chunkReceivedTime);
    if (GetStringValue(GetPropertyName(PropertyId::AudioConfig_AudioSource), "").compare(g_audioSourceMicrophone) == 0)
    {
        ticks += audioTimestamp->remainingAudioInTicks;
    }

    uint64_t latencyMs = (ticks + 5000) / 10000;
    return latencyMs;
}

void CSpxAudioStreamSession::FireAdapterResult_Intermediate(ISpxRecoEngineAdapter* adapter, uint64_t offset, shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    SPX_DBG_ASSERT(!IsState(SessionState::WaitForPumpSetFormatStart));

    bool recordHypothesisLatency = m_expectFirstHypothesis;
    if (m_expectFirstHypothesis)
    {
        m_expectFirstHypothesis = false;
    }

    auto buffer = m_audioBuffer;

    uint64_t offsetInResult = buffer ? buffer->ToAbsolute(offset) : offset;
    result->SetOffset(offsetInResult);
    UpdateAdapterResult_JsonResult(result);

    if (recordHypothesisLatency)
    {
        uint64_t latencyMs = 0;
        if (buffer)
        {
            auto audioTimestamp = buffer->GetTimestamp(offset + result->GetDuration());
            if (audioTimestamp != nullptr)
            {
                latencyMs = GetResultLatencyInMs(audioTimestamp);
            }
            else
            {
                SPX_TRACE_ERROR("FirstHypothesisLatency:(%ls): no audio timestamp available.", result->GetResultId().c_str());
            }
        }
        else
        {
            SPX_TRACE_ERROR("FirstHypothesisLatency:(%ls): audio buffer is empty, cannot get audio timestamp.", result->GetResultId().c_str());
        }
        // Write latency entry in result and telemetry.
        result->SetLatency(latencyMs);
        WriteTelemetryLatency(latencyMs, false);
    }

    FireResultEvent(GetSessionId(), result);
}

void CSpxAudioStreamSession::FireAdapterResult_KeywordResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result, bool isAccepted)
{
    UNUSED(adapter);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::WaitForPumpSetFormatStart), "ERROR! FireAdapterResult_KeywordResult was called with SessionState==WaitForPumpSetFormatStart");

    auto buffer = m_audioBuffer;
    offset = buffer ? buffer->ToAbsolute(offset) : offset;
    result->SetOffset(offset);

    if (isAccepted) {
        FireResultEvent(GetSessionId(), result);
    }
    else {
        // Drop all current audio since keyword is rejected
        if (buffer) {
            buffer->Drop();
        }

        WaitForRecognition_Complete(result);
    }
}

void CSpxAudioStreamSession::FireAdapterResult_FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::WaitForPumpSetFormatStart), "ERROR! FireAdapterResult_FinalResult was called with SessionState==WaitForPumpSetFormatStart");

    // This must be called before luAdapter processes the result, since ITN and Lexical properties are needed by ProcessResult.
    UpdateAdapterResult_JsonResult(result);

    // Only try and process the result with the LU Adapter if we have one (we won't have one, if nobody every added an IntentTrigger)
    auto luAdapter = m_luAdapter;
    if (luAdapter != nullptr)
    {
        luAdapter->ProcessResult(result);
    }

    m_expectFirstHypothesis = true;

    auto buffer = m_audioBuffer;
    uint64_t latencyMs = 0;
    if (buffer)
    {
        result->SetOffset(buffer->ToAbsolute(offset));
        auto audioTimeStamp = buffer->DiscardTill(offset + result->GetDuration());
        if (audioTimeStamp != nullptr)
        {
            latencyMs = GetResultLatencyInMs(audioTimeStamp);
        }
        else
        {
            SPX_TRACE_ERROR("ResultLatency:(%ls): no audio timestamp available.", result->GetResultId().c_str());
        }
    }
    // Write latency entry in result and telemetry.
    result->SetLatency(latencyMs);
    WriteTelemetryLatency(latencyMs, true);

    // For translation, this means that RecognizeAsync() only returns text result, but no audio result. Audio result
    // has to be received via callbacks.
    WaitForRecognition_Complete(result);
}

void CSpxAudioStreamSession::FireAdapterResult_ActivityReceived(ISpxRecoEngineAdapter*, std::string activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    FireEvent(EventType::ActivityReceivedEvent, nullptr, const_cast<wchar_t*>(GetSessionId().c_str()), 0, std::move(activity), audio);
}

void CSpxAudioStreamSession::FireAdapterResult_TranslationSynthesis(ISpxRecoEngineAdapter* adapter, std::shared_ptr<ISpxRecognitionResult> result)
{
    UNUSED(adapter);
    SPX_DBG_ASSERT_WITH_MESSAGE(!IsState(SessionState::WaitForPumpSetFormatStart));

    FireResultEvent(GetSessionId(), result);
}

#define JSON_KEY_NBEST  "NBest"
#define JSON_KEY_WORDS  "Words"
#define JSON_KEY_OFFSET  "Offset"
#define JSON_KEY_ITN     "ITN"
#define JSON_KEY_LEXICAL "Lexical"

void CSpxAudioStreamSession::UpdateAdapterResult_JsonResult(shared_ptr<ISpxRecognitionResult> result)
{
    auto buffer = m_audioBuffer;
    if (buffer == nullptr)
    {
        return;
    }

    auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
    auto jsonResult = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult));
    if (jsonResult.empty())
    {
        return;
    }

    SPX_DBG_TRACE_VERBOSE("%s: before update: json='%s'", __FUNCTION__, jsonResult.c_str());
    auto root = json::parse(jsonResult);
    bool valueChanged = false;
    uint64_t offset = 0;
    auto iteratorOffset = root.find(JSON_KEY_OFFSET);
    if (iteratorOffset != root.end())
    {
        uint64_t oldOffset = iteratorOffset->get<uint64_t>();
        uint64_t newOffset = buffer->ToAbsolute(oldOffset);
        if (oldOffset != newOffset)
        {
            root[JSON_KEY_OFFSET] = newOffset;
            valueChanged = true;
        }
    }
    auto iteratorNBest = root.find(JSON_KEY_NBEST);
    if (iteratorNBest != root.end() && iteratorNBest->is_array())
    {
        size_t nBestSize = iteratorNBest->size();
        for (size_t index = 0; index < nBestSize; index++)
        {
            auto item = iteratorNBest->at(index);
            if (index == 0)
            {
                auto iteratorItn = item.find(JSON_KEY_ITN);
                if (iteratorItn != item.end())
                {
                    namedProperties->SetStringValue("ITN", iteratorItn->get<string>().c_str());
                }
                auto iteratorLexical = item.find(JSON_KEY_LEXICAL);
                if ( iteratorLexical != item.end())
                {
                    namedProperties->SetStringValue("Lexical", iteratorLexical->get<string>().c_str());
                }
            }
            auto iteratorWords = item.find(JSON_KEY_WORDS);
            if (iteratorWords != item.end() && iteratorWords->is_array())
            {
                valueChanged = true;
                size_t wordListSize = iteratorWords->size();
                for (size_t wordIndex = 0; wordIndex < wordListSize; wordIndex++)
                {
                    auto word = iteratorWords->at(wordIndex);
                    auto iteratorWordOffset = word.find(JSON_KEY_OFFSET);
                    if (iteratorWordOffset != word.end())
                    {
                        offset = iteratorWordOffset->get<uint64_t>();
                        root[JSON_KEY_NBEST][index][JSON_KEY_WORDS][wordIndex][JSON_KEY_OFFSET] = buffer->ToAbsolute(offset);
                    }
                }
            }
        }
    }

    if (!namedProperties->HasStringValue("Lexical"))
    {
        auto iteratorLexical = root.find(JSON_KEY_LEXICAL);
        if (iteratorLexical != root.end())
        {
            namedProperties->SetStringValue("Lexical", iteratorLexical->get<string>().c_str());
        }
    }

    if (!namedProperties->HasStringValue("ITN"))
    {
        auto iteratorItn = root.find(JSON_KEY_ITN);
        if (iteratorItn != root.end())
        {
            namedProperties->SetStringValue("ITN", iteratorItn->get<string>().c_str());
        }
    }

    if (valueChanged)
    {
        string updatedJsonStr = root.dump();
        SPX_DBG_TRACE_VERBOSE("%s: after update: json='%s'", __FUNCTION__, updatedJsonStr.c_str());
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), updatedJsonStr.c_str());
    }
}

#undef JSON_KEY_NBEST
#undef JSON_KEY_WORDS
#undef JSON_KEY_OFFSET
#undef JSON_KEY_ITN
#undef JSON_KEY_LEXICAL

void CSpxAudioStreamSession::AdapterRequestingAudioMute(ISpxRecoEngineAdapter* /* adapter */, bool muteAudio)
{
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
    else if (IsState(SessionState::ProcessingAudioLeftovers))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Skipping audio mute for last portion of data. recoKind/sessionState: %d/%d", __FUNCTION__, m_recoKind, m_sessionState);
    }
    else
    {
        SPX_TRACE_ERROR("%s: Is this OK? recoKind/sessionState: %d/%d", __FUNCTION__, m_recoKind, m_sessionState);
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

// Called by audio pump thread after exceptions.
void CSpxAudioStreamSession::Error(const std::string& error)
{
    if (!error.empty())
    {
        auto task = CreateTask([=]() { CheckError(error); }, false);
        m_threadService->ExecuteAsync(move(task));
    }
}

void CSpxAudioStreamSession::CheckError(const string& error)
{
    if (!error.empty())
    {
        Error(nullptr, make_shared<SpxRecoEngineAdapterError>(false, CancellationReason::Error, CancellationErrorCode::RuntimeError, error));
    }
}

void CSpxAudioStreamSession::Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload)
{
    if (IsState(SessionState::Idle))
    {
        if (adapter != m_recoAdapter.get())
        {
            // Unexpected runtime error. However, this is called by USP worker thread, so
            // it should not throw exception here.
            SPX_TRACE_ERROR("Wrong adapter instance.");
            SPX_DBG_TRACE_VERBOSE("%s: Session is in idle state, ignore error events.", __FUNCTION__);
        }
        else
        {
            // We see an error from adapter, need to reset it when start next recognition.
            m_resetRecoAdapter = m_recoAdapter;
            SPX_DBG_TRACE_VERBOSE("%s: Reset adapter at the next recognition. Session is in idle state, ignore error events.", __FUNCTION__);
        }
        /* Since for DialogServiceConnector errors when Idle are not a rarity we want to report it*/
        if (IsRecognizerType<ISpxDialogServiceConnector>())
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
            auto error = factory->CreateFinalResult(nullptr, ResultReason::Canceled, NO_MATCH_REASON_NONE, payload->Reason(), payload->ErrorCode(), PAL::ToWString(payload->Info()).c_str(), 0, 0);
            FireResultEvent(GetSessionId(), error);
        }
    }
    // If it is a transport error and the connection was successfully before, we retry in continuous mode.
    // Otherwise report the error to the user, so that he can recreate a recognizer.
    else if (IsKind(RecognitionKind::Continuous) &&
             payload->IsTransportError() &&
             ( m_expectAdapterStoppedTurn || payload->ErrorCode() == CancellationErrorCode::ServiceRedirectTemporary || payload->ErrorCode() == CancellationErrorCode::ServiceRedirectPermanent) ) // We are in a turn.
    {
        if (payload->ErrorCode() == CancellationErrorCode::ServiceRedirectPermanent)
        {
            SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), payload->Info().c_str());
        }

        if (payload->ErrorCode() == CancellationErrorCode::ServiceRedirectTemporary)
        {
            SetStringValue("SPEECH-SingleUseEndpoint", payload->Info().c_str());
        }

        SPX_DBG_TRACE_VERBOSE("%s: Trying to reset the engine adapter", __FUNCTION__);
        auto finalResult = DiscardAudioUnderTransportErrors();
        if (finalResult != nullptr)
        {
            FireResultEvent(GetSessionId(), finalResult);
        }

        StartResetEngineAdapter();
    }
    else if (IsKind(RecognitionKind::Keyword) && payload->IsTransportError())
    {
        // We see an error from adapter, need to reset it when start next recognition.
        m_resetRecoAdapter = m_recoAdapter;
        SPX_DBG_TRACE_VERBOSE("%s: Reset adapter at the next recognition. We are in active keyword recognition mode, ignore error events.", __FUNCTION__);
    }
    else
    {
        SPX_DBG_TRACE_VERBOSE("%s: Creating/firing ResultReason::Canceled result", __FUNCTION__);
        auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
        auto error = factory->CreateFinalResult(nullptr, ResultReason::Canceled, NO_MATCH_REASON_NONE, payload->Reason(), payload->ErrorCode(), PAL::ToWString(payload->Info()).c_str(), 0, 0);
        WaitForRecognition_Complete(error);
        if (IsRecognizerType<ISpxDialogServiceConnector>())
        {
            /* For a DialogServiceConnector we raise a cancellation error but we set the session so we can keep using it. */
            m_resetRecoAdapter = m_recoAdapter;
            /* We drop the audio, this is the point that we would need to play with if we were to recover the lost turn */
            m_audioBuffer->Drop();
        }
        else if ((m_recoKind == RecognitionKind::Keyword || m_recoKind == RecognitionKind::KwsSingleShot) && CancellationReason::Error == payload->Reason())
        {
            // Audio buffers shall be dropped if keyword recognition is cancelled with an error to prevent extra keyword detections in the stopping phase.
            m_audioBuffer->Drop();
        }

        // We're not going to see a start or stop message. We're done. Canceled.
        m_expectAdapterStartedTurn = false;
        m_expectAdapterStoppedTurn = false;
    }
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::DiscardAudioUnderTransportErrors()
{
    bool discardAudio = PAL::ToBool(GetStringValue("DiscardAudioFromIntermediateRecoResult", PAL::BoolToString(false)));
    if (discardAudio == false)
    {
        return nullptr;
    }
    if (m_mostRecentIntermediateRecoResult == nullptr)
    {
        return nullptr;
    }
    SPX_DBG_TRACE_VERBOSE("Discarding audio after transportErrors");
    uint64_t offset = m_mostRecentIntermediateRecoResult->GetOffset();
    uint64_t duration = m_mostRecentIntermediateRecoResult->GetDuration();

    auto buffer = m_audioBuffer;
    if (buffer)
    {
        buffer->DiscardTill(offset + duration);
    }
    return CreateFakeFinalResult(m_mostRecentIntermediateRecoResult);
}

std::shared_ptr<ISpxRecognitionResult> CSpxAudioStreamSession::CreateFakeFinalResult(const std::shared_ptr<ISpxRecognitionResult>& intermediate)
{
    if (intermediate == nullptr)
    {
        return nullptr;
    }

    wstring text = intermediate->GetText();
    uint64_t offset = intermediate->GetOffset();
    uint64_t duration = intermediate->GetDuration();
    wstring userId{};
    auto ctsResult =  SpxQueryInterface<ISpxConversationTranscriptionResult>(intermediate);
    if (ctsResult != nullptr)
    {
        userId = ctsResult->GetUserId();
    }

    auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
    if (factory == nullptr)
    {
        return nullptr;
    }

    auto result = factory->CreateFinalResult(nullptr, ResultReason::RecognizedSpeech, NO_MATCH_REASON_NONE, REASON_CANCELED_NONE, CancellationErrorCode::NoError, text.c_str(), offset, duration, userId.c_str());
    return result;
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
    else if (PAL::stricmp(name, GetPropertyName(PropertyId::Speech_SessionId)) == 0)
    {
        return PAL::ToString(m_sessionId);
    }

    return ISpxPropertyBagImpl::GetStringValue(name, defaultValue);
}

void CSpxAudioStreamSession::SetStringValue(const char* name, const char* value)
{
    return ISpxPropertyBagImpl::SetStringValue(name, value);
}

std::shared_ptr<ISpxRecoEngineAdapter> CSpxAudioStreamSession::EnsureInitRecoEngineAdapter()
{
    SPX_DBG_TRACE_FUNCTION();

    if (m_recoAdapter == nullptr || m_resetRecoAdapter == m_recoAdapter)
    {
        EnsureResetEngineEngineAdapterComplete();
        InitRecoEngineAdapter();
        EnsureIntentRegionSet();
    }
    return m_recoAdapter;
}

std::shared_ptr < ISpxSpeechAudioProcessorAdapter> CSpxAudioStreamSession::EnsureInitSpeechProcessor()
{
    std::shared_ptr < ISpxSpeechAudioProcessorAdapter> retval = SpxCreateObjectWithSite<ISpxSpeechAudioProcessorAdapter>("CSpxSpeechAudioProcessor", this);
    m_speechProcessor = SpxQueryInterface<ISpxAudioProcessor>(retval);
    return retval;
}

void CSpxAudioStreamSession::InitRecoEngineAdapter()
{
    SPX_DBG_TRACE_FUNCTION();

    // determine which type (or types) of reco engine adapters we should try creating...
    bool tryUnidec = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Unidec", PAL::BoolToString(false)));
    bool tryMock = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Mock", PAL::BoolToString(false)));
    bool tryUsp = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Usp", PAL::BoolToString(false)));

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
    SPX_DBG_TRACE_FUNCTION();

    m_resetRecoAdapter = m_recoAdapter;
    if (IsKind(RecognitionKind::Continuous))
    {
        // If we were continuous, let's try and restart recognition, same kind...
        StartRecognizing(m_recoKind, nullptr);
    }
    else if (!IsKind(RecognitionKind::Idle))
    {
        // Otherwise... Let's stop the recognition
        StopRecognizing(m_recoKind);
    }
}

void CSpxAudioStreamSession::EnsureResetEngineEngineAdapterComplete()
{
    SPX_DBG_TRACE_FUNCTION();

    if (m_resetRecoAdapter != nullptr && m_resetRecoAdapter == m_recoAdapter)
    {
        // Let's term and clear our reco adapter...
        SPX_DBG_TRACE_VERBOSE("%s: resetting reco adapter (0x%8p)...", __FUNCTION__, (void*)m_resetRecoAdapter.get());
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
    SPX_DBG_TRACE_FUNCTION();

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
    auto intentRegion = this->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region), "");

    // Now ... let's check to see if we have a different region specified for intent...
    bool isIntentScenario = false;
    {
        unique_lock<mutex> lock(m_recognizersLock);
        if (m_recognizers.empty()) // The recognizer is being killed...
        {
            return;
        }

        SPX_DBG_ASSERT(m_recognizers.size() == 1); // we only support 1 recognizer today...
        auto intentRecognizer = SpxQueryInterface<ISpxIntentRecognizer>(m_recognizers.front().lock());
        isIntentScenario = intentRecognizer != nullptr;
    }

    if (isIntentScenario && m_luAdapter != nullptr)
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
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_IntentRegion), SpeechRegionFromIntentRegion(intentRegion).c_str());
}

std::string CSpxAudioStreamSession::SpeechRegionFromIntentRegion(const std::string& intentRegion)
{
    static std::pair<const char*, const char*> intentToSpeechRegion[] = {

        std::make_pair("West US",          "westus"),
        std::make_pair("US West",          "westus"),
        std::make_pair("uswest",           "westus"),

        std::make_pair("West US 2",        "westus2"),
        std::make_pair("US West 2",        "westus2"),
        std::make_pair("uswest2",          "westus2"),

        std::make_pair("South Central US", "southcentralus"),
        std::make_pair("US South Central", "southcentralus"),
        std::make_pair("ussouthcentral",   "southcentralus"),

        std::make_pair("West Central US",  "westcentralus"),
        std::make_pair("US West Central",  "westcentralus"),
        std::make_pair("uswestcentral",    "westcentralus"),

        std::make_pair("East US",          "eastus"),
        std::make_pair("US East",          "eastus"),
        std::make_pair("useast",           "eastus"),

        std::make_pair("East US 2",        "eastus2"),
        std::make_pair("US East 2",        "eastus2"),
        std::make_pair("useast2",          "eastus2"),

        std::make_pair("West Europe",      "westeurope"),
        std::make_pair("Europe West",      "westeurope"),
        std::make_pair("europewest",       "westeurope"),

        std::make_pair("North Europe",     "northeurope"),
        std::make_pair("Europe North",     "northeurope"),
        std::make_pair("europenorth",      "northeurope"),

        std::make_pair("Brazil South",     "southbrazil"),
        std::make_pair("South Brazil",     "southbrazil"),
        std::make_pair("brazilsouth",      "southbrazil"),

        std::make_pair("Australia East",   "eastaustralia"),
        std::make_pair("East Australia",   "eastaustralia"),
        std::make_pair("australiaeast",    "eastaustralia"),

        std::make_pair("Southeast Asia",   "southeastasia"),
        std::make_pair("Asia Southeast",   "southeastasia"),
        std::make_pair("asiasoutheast",    "southeastasia"),

        std::make_pair("East Asia",        "eastasia"),
        std::make_pair("Asia East",        "eastasia"),
        std::make_pair("asiaeast",         "eastasia"),
    };

    for (auto item : intentToSpeechRegion)
    {
        if (PAL::stricmp(item.first, intentRegion.c_str()) == 0)
        {
            return item.second;
        }
    }

    return intentRegion;
}

void CSpxAudioStreamSession::InitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::InitKwsEngineAdapter ...", (void*)this);

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
    SPX_IFTRUE_THROW_HR(m_kwsAdapter == nullptr, SPXERR_EXTENSION_LIBRARY_NOT_FOUND);
}

void CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused(std::shared_ptr<ISpxRecognitionResult> spottedKeywordResult)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused ...", (void*)this);

    // We need to do all this work on a background thread, because we can't guarantee it's safe
    // to spend any significant amount of time blocking this the KWS or Audio threads...
    auto task = CreateTask([=]() mutable {

        SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused kicked-off THREAD started ***", "*** CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused kicked-off THREAD stopped ***");
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused:  Task", (void*)this);

        // Keep track of the fact that we have a thread pending waiting to hear
        // what the final recognition result is, and then start/stop recognizing...
        if (m_singleShotInFlight != nullptr)
        {
            // There is another single shot in flight. Report an error.
            // TODO: Should queue in the future.
            SPX_THROW_HR(SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION);
        }

        auto singleShotInFlight = make_shared<Operation>(RecognitionKind::KwsSingleShot);
        singleShotInFlight->m_spottedKeywordResult = spottedKeywordResult;
        m_singleShotInFlight = singleShotInFlight;

        SPX_DBG_ASSERT(IsKind(RecognitionKind::KwsSingleShot) && IsState(SessionState::HotSwapPaused));
        this->HotSwapAdaptersWhilePaused(RecognitionKind::KwsSingleShot);

        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused - Task: m_audioPumpStoppedBeforeHotSwap %s", (void*)this, m_audioPumpStoppedBeforeHotSwap ? "true": "false");
        if (m_audioPumpStoppedBeforeHotSwap == true)
        {
            m_audioPumpStoppedBeforeHotSwap = false;

            // Process the buffered audio leftovers
            SPX_DBG_ASSERT(IsState(SessionState::ProcessingAudio));
            SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::HotSwapToKwsSingleShotWhilePaused:  Task - Processing audio leftovers after hot swap", (void*)this);

            ChangeState(SessionState::ProcessingAudio, SessionState::ProcessingAudioLeftovers);
            while (ProcessNextAudio())
            {
            }
            ChangeState(SessionState::ProcessingAudioLeftovers, SessionState::WaitForAdapterCompletedSetFormatStop);
            InformAdapterSetFormatStopping(SessionState::ProcessingAudio);
            EncounteredEndOfStream();
        }

        auto cancelTimer = CreateTask([=]()
        {
            auto status = singleShotInFlight->m_future.wait_for(0ms);
            if (status != future_status::ready &&
                m_singleShotInFlight &&
                m_singleShotInFlight->m_operationId == singleShotInFlight->m_operationId)
            {
                EnsureFireResultEvent();
            }
        });

        m_threadService->ExecuteAsync(std::move(cancelTimer), Operation::Timeout);
    });

    m_threadService->ExecuteAsync(std::move(task));
}

void CSpxAudioStreamSession::Ensure16kHzSampleRate()
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::Ensure16kHzSampleRate:  Pump: %p", (void*)this, (void*)m_audioPump.get());

    if (m_audioPump)
    {
        auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
        auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
        m_audioPump->GetFormat(waveformat.get(), cbFormat);

        // we only support 16kHz sampling rate for Keyword spot for now.
        if (waveformat->nSamplesPerSec != SAMPLES_PER_SECOND)
        {
            SPX_TRACE_ERROR("going to throw wrong sampling rate runtime_error");
            ThrowRuntimeError("Sampling rate '" + std::to_string(waveformat->nSamplesPerSec) + "' is not supported. 16kHz is the only sampling rate that is supported.");
        }
    }
}

void CSpxAudioStreamSession::StartAudioPump(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartAudioPump:  RecognitionKind %d | m_audioPump [%p]", (void*)this, (int)startKind, (void*)m_audioPump.get());
    SPX_DBG_ASSERT(IsState(SessionState::WaitForPumpSetFormatStart));

    // Tell everyone we're starting...
    if (startKind != RecognitionKind::Keyword)
    {
        FireSessionStartedEvent();
    }
    else
    {
        // Disable the fast lane for KWS.
        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());

        // It was spelled wrong originally. Check both until Carbon 2.0.
        if(!properties->HasStringValue("SPEECH-TransmitLengthBeforThrottleMs") && !properties->HasStringValue("SPEECH-TransmitLengthBeforeThrottleMs"))
        {
            properties->SetStringValue("SPEECH-TransmitLengthBeforeThrottleMs", "0");
        }
    }

    if (!m_audioPump)
    {
        // The session was terminated, exiting.
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartAudioPump: Audio pump was shutdown, exiting...", (void*)this);
        return;
    }

    auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    m_audioPump->GetFormat(waveformat.get(), cbFormat);

    if (!m_audioBuffer)
    {
        SetThrottleVariables(waveformat.get());

        m_audioBuffer = std::make_shared<PcmAudioBuffer>(*waveformat);
    }
    m_audioBuffer->NewTurn();
    m_currentTurnGlobalOffset = m_audioBuffer->GetAbsoluteOffset();

    // Prevent an invalid state transition during continuous recognitions
    // with offline Unidec. A better fix will need some refactoring.
    // Bug 2003347 Offline Unidec invalid state transition with KWS or Continuous reco
    bool useRecoEngineUnidec = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Unidec", PAL::BoolToString(false)));
    if (m_recoAdapter && useRecoEngineUnidec && !IsKindKeyword(startKind))
    {
        m_resetRecoAdapter = m_recoAdapter;
    }
    // Depending on the startKind, we'll either switch to the Kws Engine Adapter or the Reco Engine Adapter
    m_audioProcessor = IsKindKeyword(startKind)
        ? SpxQueryInterface<ISpxAudioProcessor>(EnsureInitKwsEngineAdapter(model))
        : SpxQueryInterface<ISpxAudioProcessor>(EnsureInitRecoEngineAdapter());
    m_kwsProcessorMode = KWSProcessorModeFromKind(startKind);

    // Should be conditional based on a property
    if (PAL::ToBool(GetStringValue("UseLocalSpeechDetection", PAL::BoolToString(false))))
    {
        uint32_t energy = stoul(GetStringValue("LocalSpeechDetectionThreshold", "180").c_str());
        uint32_t count = stoul(GetStringValue("LocalSpeechDetectionSilenceCount", "10").c_str());

        std::shared_ptr<ISpxSpeechAudioProcessorAdapter> speechAdapter = EnsureInitSpeechProcessor();
        if (speechAdapter)
        {
            speechAdapter->SetSpeechDetectionThreshold(energy);
            speechAdapter->SetSpeechDetectionSilenceCount(count);
        }
    }

    // Start pumping audio data from the pump, to the Audio Stream Session
    auto ptr = (ISpxAudioProcessor*)this;
    auto pISpxAudioProcessor = ptr->shared_from_this();

    auto audioPump = m_audioPump;
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::StartAudioPump: Starting pump[%p]...", (void*)this, (void*)audioPump.get());

    if (audioPump)
    {
        audioPump->StartPump(pISpxAudioProcessor);
    }
    // The call to StartPump (immediately above) will initiate calls from the pump to this::SetFormat() and then this::ProcessAudio()...
}

void CSpxAudioStreamSession::HotSwapAdaptersWhilePaused(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::HotSwapAdaptersWhilePaused: ...", (void*)this);
    SPX_DBG_ASSERT(IsKind(startKind) && IsState(SessionState::HotSwapPaused));

    auto oldAudioProcessor = m_audioProcessor;
    if (!m_audioPump)
    {
        // The session was terminated, exiting.
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::HotSwapAdaptersWhilePaused: Audio pump was shutdown, exiting...", (void*)this);
        return;
    }

    // Get the format in which the pump is currently sending audio data to the Session
    auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    m_audioPump->GetFormat(waveformat.get(), cbFormat);

    // Depending on the startKind, we'll either switch to the Kws Engine Adapter or the Reco Engine Adapter
    if (IsKindKeyword(startKind))
    {
        m_audioProcessor = SpxQueryInterface<ISpxAudioProcessor>(EnsureInitKwsEngineAdapter(model));
        m_kwsProcessorMode = KWSProcessorModeFromKind(startKind);
    }
    else
    {
        // Prevent an invalid state transition during continuous recognitions
        // with offline Unidec. A better fix will need some refactoring.
        // Bug 2003347 Offline Unidec invalid state transition with KWS or Continuous reco
        bool useRecoEngineUnidec = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseRecoEngine-Unidec", PAL::BoolToString(false)));
        if (m_recoAdapter && useRecoEngineUnidec)
        {
            m_resetRecoAdapter = m_recoAdapter;
        }
        m_audioProcessor = SpxQueryInterface<ISpxAudioProcessor>(EnsureInitRecoEngineAdapter());
        m_kwsProcessorMode = KWSProcessorMode::None;
        if (m_spottedKeyword)
        {
            // Switching from KWS to single shot, creating buffer from scratch with added keyword.
            auto currentBuffer = m_audioBuffer;
            m_audioBuffer = std::make_shared<PcmAudioBuffer>(*waveformat);
            SPX_DBG_TRACE_VERBOSE_IF(1, "%s: Size of spotted keyword %d", __FUNCTION__, m_spottedKeyword->size);
            m_audioBuffer->Add(m_spottedKeyword);
            currentBuffer->CopyNonAcknowledgedDataTo(m_audioBuffer);
            m_spottedKeyword = nullptr;
        }
    }

    // Tell the old Audio Processor that we've sent the last bit of audio data we're going to send
    SPX_DBG_TRACE_VERBOSE_IF(1, "%s: ProcessingAudio - size=%d", __FUNCTION__, 0);
    if (oldAudioProcessor)
    {
        oldAudioProcessor->ProcessAudio(std::make_shared<DataChunk>(nullptr, 0));
        // Then tell it we're finally done, by sending a nullptr SPXWAVEFORMAT
        oldAudioProcessor->SetFormat(nullptr);
    }
    m_adapterAudioMuted = false;

    if (m_audioBuffer)
    {
        m_audioBuffer->NewTurn();
        m_currentTurnGlobalOffset = m_audioBuffer->GetAbsoluteOffset();
    }

    // Inform the Audio Processor that we're starting...
    InformAdapterSetFormatStarting(waveformat.get());

    // The call to ProcessAudio(nullptr, 0) and SetFormat(nullptr) will instigate a call from that Adapter to this::AdapterCompletedSetFormatStop(adapter) shortly...
}

void CSpxAudioStreamSession::InformAdapterSetFormatStarting(const SPXWAVEFORMATEX* format)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + format->cbSize;
    {
        unique_lock<mutex> formatLock(m_formatMutex);
        m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
        memcpy(m_format.get(), format, sizeOfFormat);
    }

    auto recoAdapter = m_recoAdapter;
    if (recoAdapter != nullptr)
    {
        recoAdapter->SetAdapterMode(!IsKind(RecognitionKind::Continuous));
    }

    SPX_DBG_ASSERT(format != nullptr);
    if (m_audioProcessor)
    {
        m_audioProcessor->SetFormat(format);
    }

    if (m_speechProcessor)
    {
        m_speechProcessor->SetFormat(format);
    }
}

void CSpxAudioStreamSession::SetThrottleVariables(const SPXWAVEFORMATEX* format)
{
    const char* maxBufferDefault = IsRecognizerType<ISpxConversationTranscriber>() ? "240" : "60";

    // Look for runtime buffer overrides. These are set once when the buffer is created.
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    m_maxBufferedBeforeOverflow = seconds(stoi(properties->GetStringValue("SPEECH-MaxBufferSizeSeconds", maxBufferDefault)));

    // If the misspelled (old) value is set and the new (correct) one is not, use the old. Else use the new.
    const char *throttleFastLaneName = properties->HasStringValue("SPEECH-TransmitLengthBeforThrottleMs") && !properties->HasStringValue("SPEECH-TransmitLengthBeforeThrottleMs") ? "SPEECH-TransmitLengthBeforThrottleMs" : "SPEECH-TransmitLengthBeforeThrottleMs";
    m_maxTransmittedInFastLane = milliseconds(stoi(properties->GetStringValue(throttleFastLaneName, "5000")));

    m_avgBytesPerSecond = format->nAvgBytesPerSec;
    m_maxFastLaneSizeBytes = (m_avgBytesPerSecond / 1000) * m_maxTransmittedInFastLane.count();
    m_simulateRealtimePercentage = stoul(properties->GetStringValue("SPEECH-AudioThrottleAsPercentageOfRealTime", "200").c_str());

    SPX_DBG_TRACE_VERBOSE("%s: FastLane size %" PRIu64 " Throttle Max Speed: %lu", __FUNCTION__, m_maxFastLaneSizeBytes, m_simulateRealtimePercentage);

    // While this could be set once, if logging is enabled in a retail build the constructor isn't logged.
    if (std::ratio_greater<std::chrono::steady_clock::period, std::milli>::value)
    {
        SPX_DBG_TRACE_VERBOSE("%s - steady_clock resolution (%e) insufficient, falling back to duration based throttling.", __FUNCTION__,
            static_cast<double>(std::chrono::steady_clock::period::num) / static_cast<double>(std::chrono::steady_clock::period::den));
        m_useDurationBasedThrottle = true;
    }
}

void CSpxAudioStreamSession::InformAdapterSetFormatStopping(SessionState comingFromState)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    // If we transitioned --> WaitForAdapterCompletedSetFormatStop state because this Session was trying to stop the pump...
    // We need to tell the audio processor that we've sent the last bit of audio we're planning on sending
    SPX_DBG_ASSERT(comingFromState == SessionState::ProcessingAudio ||
                   comingFromState == SessionState::StoppingPump ||
                   comingFromState == SessionState::WaitForAdapterCompletedSetFormatStop);
    if (comingFromState == SessionState::StoppingPump)
    {
        if (m_audioProcessor)
        {
            SPX_TRACE_INFO("[%p]CSpxAudioStreamSession::InformAdapterSetFormatStoppingProcessingAudio - Send zero size audio.", (void*)this);
            m_audioProcessor->ProcessAudio(std::make_shared<DataChunk>(nullptr, 0));
        }
    }

    if (!m_expectAdapterStartedTurn && !m_expectAdapterStoppedTurn)
    {
        // Then we can finally tell it we're done, by sending a nullptr SPXWAVEFORMAT
        SPX_TRACE_INFO("[%p]CSpxAudioStreamSession::InformAdapterSetFormatStoppingSetFormat(nullptr)", (void*)this);
        if (m_audioProcessor)
        {
            m_audioProcessor->SetFormat(nullptr);
        }

        if (m_speechProcessor)
        {
            m_speechProcessor->SetFormat(nullptr);
        }

        m_adapterAudioMuted = false;
    }
}

void CSpxAudioStreamSession::EncounteredEndOfStream()
{
    SPX_DBG_TRACE_FUNCTION();

    m_sawEndOfStream = true;
    if (IsKind(RecognitionKind::Continuous) || IsKeywordKind())
    {
        m_fireEndOfStreamAtSessionStop = true;
    }
}

void CSpxAudioStreamSession::AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio doneAdapter)
{
    SPX_DBG_TRACE_SCOPE("*** CSpxAudioStreamSession::AdapterCompletedSetFormatStop kicked-off THREAD started ***", "*** CSpxAudioStreamSession::AdapterCompletedSetFormatStop kicked-off THREAD stopped ***");
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterCompletedSetFormatStop: kicked-off THREAD started ***", (void*)this);

    if (ChangeState(RecognitionKind::KwsSingleShot, SessionState::WaitForAdapterCompletedSetFormatStop, RecognitionKind::Keyword, SessionState::ProcessingAudio))
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterCompletedSetFormatStop: KwsSingleShot Waiting for done ... Done!! Switching back to Keyword/Processing", (void*)this);
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterCompletedSetFormatStop: Now Keyword/ProcessingAudio ...", (void*)this);

        FireSessionStoppedEvent();
    }
    else if (ChangeState(SessionState::HotSwapPaused, SessionState::ProcessingAudio))
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterCompletedSetFormatStop: Previous Adapter is done processing audio ... resuming Processing with the new adapter...", (void*)this);
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterCompletedSetFormatStop: Now ProcessingAudio ...", (void*)this);

        if (doneAdapter == AdapterDoneProcessingAudio::Keyword && (IsKind(RecognitionKind::KwsSingleShot) || IsKind(RecognitionKind::SingleShot)))
        {
            FireSessionStartedEvent();
        }
    }
    // Because next RecognizeAsync will be scheduled on the same thread, we can firstly set the state to idle and then fire session stop event.
    else if (ChangeState(SessionState::WaitForAdapterCompletedSetFormatStop, RecognitionKind::Idle, SessionState::Idle))
    {
        if (doneAdapter == AdapterDoneProcessingAudio::Speech)
        {
            // The Reco Engine adapter request to finish processing audio has completed, that signifies that the "session" has stopped
            FireSessionStoppedEvent();

            // Restart the keyword spotter if necessary...
            if (m_kwsModel != nullptr && ChangeState(SessionState::Idle, RecognitionKind::Keyword, SessionState::WaitForPumpSetFormatStart))
            {
                // Go ahead re-start the pump
                SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterCompletedSetFormatStop: Restart the keyword spotter - Now WaitForPumpSetFormatStart ...", (void*)this);

                StartAudioPump(RecognitionKind::Keyword, m_kwsModel);
            }
        }
        else if (doneAdapter == AdapterDoneProcessingAudio::Keyword)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
            auto result = factory->CreateFinalResult(nullptr, ResultReason::Canceled, NO_MATCH_REASON_NONE, CancellationReason::EndOfStream, CancellationErrorCode::NoError, nullptr, 0, 0);

            WaitForRecognition_Complete(result);
        }
    }
    else
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::AdapterCompletedSetFormatStop:  Is this OK? doneAdapter=%d; sessionState=%d **************************", (void*)this, doneAdapter, m_sessionState);
    }
}

void CSpxAudioStreamSession::SpeechStartDetected(uint64_t offset)
{
    FireSpeechStartDetectedEvent(offset);
}

void CSpxAudioStreamSession::SpeechEndDetected(uint64_t offset)
{
    FireSpeechEndDetectedEvent(offset);
}

bool CSpxAudioStreamSession::IsKind(RecognitionKind kind) const
{
    return m_recoKind == kind;
}

bool CSpxAudioStreamSession::IsState(SessionState state)
{
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
    if (m_sessionState == sessionStateFrom && m_recoKind == recoKindFrom)
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::ChangeState: recoKind/sessionState: %d/%d => %d/%d", (void*)this, recoKindFrom, sessionStateFrom, recoKindTo, sessionStateTo);

        // Protecting only the write access because m_cv is waiting,
        // all changes to the state can still happen only on the background thread.
        unique_lock<mutex> lock(m_stateMutex);
        m_sessionState = sessionStateTo;
        m_recoKind = recoKindTo;
        m_cv.notify_all();
        return true;
    }
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioStreamSession::ChangeState:  recoKind/sessionState: %d/%d != %d/%d", (void*)this, recoKindFrom, sessionStateFrom, m_recoKind, m_sessionState);

    return false;
}

void CSpxAudioStreamSession::EnsureInitLuEngineAdapter()
{
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

std::shared_ptr<ISpxThreadService> CSpxAudioStreamSession::InternalQueryService(const char* serviceName)
{
    if (!serviceName)
        SPX_THROW_HR(SPXERR_INVALID_ARG);

    if (PAL::stricmp(SpxTypeName(ISpxThreadService), serviceName) == 0)
    {
        return m_threadService;
    }

    return nullptr;
}

void CSpxAudioStreamSession::WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency)
{
    if (m_recoAdapter != nullptr)
    {
        m_recoAdapter->WriteTelemetryLatency(latencyInTicks, isPhraseLatency);
    }
    else
    {
        SPX_TRACE_ERROR("%s: m_recoAdapter is null.", __FUNCTION__);
    }
}

void CSpxAudioStreamSession::SendSpeechEventMessage(std::string&& payload)
{
    EnsureInitRecoEngineAdapter();
    m_recoAdapter->SendSpeechEventMessage(std::move(payload));
}

void CSpxAudioStreamSession::SendNetworkMessage(std::string&& path, std::string&& payload)
{
    SPX_DBG_TRACE_FUNCTION();
    auto keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    auto task = CreateTask([this, keepAlive, path, payload] () mutable {
        EnsureInitRecoEngineAdapter();
        m_recoAdapter->SendNetworkMessage(std::move(path), std::move(payload));
    });

    m_threadService->ExecuteAsync(move(task));
}

void CSpxAudioStreamSession::SendNetworkMessage(std::string&& path, std::vector<uint8_t>&& payload)
{
    SPX_DBG_TRACE_FUNCTION();
    auto keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    auto task = CreateTask([this, keepAlive, path, payload] () mutable {
        EnsureInitRecoEngineAdapter();
        m_recoAdapter->SendNetworkMessage(std::move(path), std::move(payload));
    });

    m_threadService->ExecuteAsync(move(task));
}

bool CSpxAudioStreamSession::IsStreaming()
{
    return  m_expectAdapterStoppedTurn || m_expectAdapterStartedTurn;
}

std::string CSpxAudioStreamSession::GetSpeechEventPayload(bool startMeeting)
{
    std::string payload;

    if (IsRecognizerType<ISpxConversationTranscriber>())
    {
        std::shared_ptr<ISpxConversation> conversation;
        {
            std::unique_lock<std::mutex> lock{ m_conversationLock };
            conversation = m_conversation.lock();
        }

        if (conversation != nullptr)
        {
            payload = conversation->GetSpeechEventPayload(startMeeting ? ISpxConversation::MeetingState::START : ISpxConversation::MeetingState::END);
        }
    }
    return payload;
}

CSpxStringMap CSpxAudioStreamSession::GetParametersFromUser(std::string&& path)
{
    CSpxStringMap parametersFromUser;
    std::shared_ptr<ISpxRecognizer> recognizer;
    {
        std::unique_lock<std::mutex> lock{ m_recognizersLock };
        SPX_DBG_ASSERT(m_recognizers.size() == 1); // we only support 1 recognizer today...
        recognizer = m_recognizers.front().lock();
    }
    auto ct = SpxQueryInterface<ISpxGetUspMessageParamsFromUser>(recognizer);
    if (ct != nullptr)
    {
        parametersFromUser = ct->GetParametersFromUser(move(path));
    }
    return parametersFromUser;
}

void CSpxAudioStreamSession::ForEachRecognizer(std::function<void(std::shared_ptr<ISpxRecognizer>)> fn)
{
    // Make a copy of the recognizers (under lock), to use to send events;
    // otherwise the underlying list could be modified while we're exeuting
    list<weak_ptr<ISpxRecognizer>> weakRecognizers;
    {
        unique_lock<mutex> lock(m_recognizersLock);
        weakRecognizers.assign(m_recognizers.begin(), m_recognizers.end());
    }

    string error;
    SPXAPI_TRY()
    {
        for (auto iter = weakRecognizers.begin(); iter != weakRecognizers.end(); iter++)
        {
            auto recognizer = iter->lock();
            if (recognizer != nullptr)
            {
                fn(iter->lock());
            }
        }
    }
    SPXAPI_CATCH_ONLY()
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

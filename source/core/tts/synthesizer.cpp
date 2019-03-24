#include "stdafx.h"
#include "synthesizer.h"
#include <future>
#include "handle_table.h"
#include "site_helpers.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "property_id_2_name_map.h"
#include "guid_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxSynthesizer::CSpxSynthesizer() :
    m_fEnabled(true)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxSynthesizer::~CSpxSynthesizer()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    Term();
}

void CSpxSynthesizer::Init()
{
    // NOTE: Due to current ownership model, and our late-into-the-cycle changes for SpeechConfig objects
    // the CSpxSynthesizer is sited to the CSpxApiFactory. This ApiFactory is not held by the
    // dev user at or above the CAPI. Thus ... we must hold it alive in order for the properties to be
    // obtainable via the standard ISpxNamedProperties mechanisms... It will be released in ::Term()
    m_siteKeepAlive = GetSite();

    EnsureTtsEngineAdapter();
}

void CSpxSynthesizer::Term()
{
    ReleaseTtsEngineAdapter();
    m_audioOutput->Close();
    SpxTermAndClear(m_siteKeepAlive);
}

bool CSpxSynthesizer::IsEnabled()
{
    return m_fEnabled;
}

void CSpxSynthesizer::Enable()
{
    m_fEnabled = true;
}

void CSpxSynthesizer::Disable()
{
    m_fEnabled = false;
}

void CSpxSynthesizer::SetOutput(std::shared_ptr<ISpxAudioOutput> output)
{
    m_audioOutput = output;
    m_ttsAdapter->SetOutput(output);
}

std::shared_ptr<ISpxSynthesisResult> CSpxSynthesizer::Speak(const std::string& text, bool isSsml)
{
    // Request ID is per speak, different events from same speak will share one request ID
    auto requestId = PAL::CreateGuidWithoutDashes();

    // Push request into queue
    PushRequestIntoQueue(requestId);

    // Wait until current request to be in front of the queue
    WaitUntilRequestInFrontOfQueue(requestId);

    // Fire SynthesisStarted event
    auto synthesisStartedResult = CreateResult(requestId, ResultReason::SynthesizingAudioStarted, nullptr, 0);
    FireResultEvent(synthesisStartedResult);

    // Speak
    auto synthesisDoneResult = m_ttsAdapter->Speak(text, isSsml, requestId);

    // Set events
    auto events = this->QueryInterfaceInternal<ISpxSynthesizerEvents>();
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(synthesisDoneResult);
    resultInit->SetEvents(events);

    // Fire SynthesisCompleted or SynthesisCanceled (depending on the result reason) event
    FireResultEvent(synthesisDoneResult);

    // Pop processed request from queue
    PopRequestFromQueue();

    return synthesisDoneResult;
}

CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>> CSpxSynthesizer::SpeakAsync(const std::string& text, bool isSsml)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxSynthesizer>(this);
    std::shared_future<std::shared_ptr<ISpxSynthesisResult>> waitForCompletion(std::async(std::launch::async, [this, keepAlive, text, isSsml]() {
        // Speak
        return Speak(text, isSsml);
    }));

    return CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>(waitForCompletion, AOS_Started);
}

std::shared_ptr<ISpxSynthesisResult> CSpxSynthesizer::StartSpeaking(const std::string& text, bool isSsml)
{
    // Request ID is per speak, different events from same speak will share one request ID
    auto requestId = PAL::CreateGuidWithoutDashes();

    // Push request into queue
    PushRequestIntoQueue(requestId);

    // Wait until current request to be in front of the queue
    WaitUntilRequestInFrontOfQueue(requestId);

    // Fire SynthesisStarted event
    auto synthesisStartedResult = CreateResult(requestId, ResultReason::SynthesizingAudioStarted, nullptr, 0);
    FireResultEvent(synthesisStartedResult);

    auto keepAlive = SpxSharedPtrFromThis<ISpxSynthesizer>(this);
    std::shared_future<std::shared_ptr<ISpxSynthesisResult>> waitForCompletion(std::async(std::launch::async, [this, keepAlive, requestId, text, isSsml]() {
        // Speak
        auto synthesisDoneResult = m_ttsAdapter->Speak(text, isSsml, requestId);

        // Set events
        auto events = this->QueryInterfaceInternal<ISpxSynthesizerEvents>();
        auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(synthesisDoneResult);
        resultInit->SetEvents(events);

        // Fire SynthesisCompleted or SynthesisCanceled (depending on the result reason) event
        FireResultEvent(synthesisDoneResult);

        // Pop processed request from queue
        PopRequestFromQueue();

        return synthesisDoneResult;
    }));

    auto asyncop = CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>(waitForCompletion, AOS_Started);
    auto futureResult = std::make_shared<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>>(std::move(asyncop));
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(synthesisStartedResult);
    resultInit->SetFutureResult(futureResult); // Assign asyncop to result to be brought out, otherwise it will be synchronous

    return synthesisStartedResult;
}

CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>> CSpxSynthesizer::StartSpeakingAsync(const std::string& text, bool isSsml)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxSynthesizer>(this);
    std::shared_future<std::shared_ptr<ISpxSynthesisResult>> waitForSpeakStart(std::async(std::launch::async, [this, keepAlive, text, isSsml]() {
        // Start speaking
        return StartSpeaking(text, isSsml);
    }));

    return CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>(waitForSpeakStart, AOS_Started);
}

void CSpxSynthesizer::Close()
{
    m_audioOutput->Close();
}

void CSpxSynthesizer::FireSynthesisStarted(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireEvent(SynthesisStarted, result);
}

void CSpxSynthesizer::FireSynthesizing(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireEvent(Synthesizing, result);
}

void CSpxSynthesizer::FireSynthesisCompleted(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireEvent(SynthesisCompleted, result);
}

void CSpxSynthesizer::FireSynthesisCanceled(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireEvent(SynthesisCanceled, result);
}

uint32_t CSpxSynthesizer::Write(ISpxTtsEngineAdapter* adapter, const std::wstring& requestId, uint8_t* buffer, uint32_t size)
{
    UNUSED(adapter);

    // Fire Synthesizing event
    auto result = CreateResult(requestId, ResultReason::SynthesizingAudio, buffer, size);
    FireResultEvent(result);

    // Write audio data to output
    return m_audioOutput->Write(buffer, size);
}

std::shared_ptr<ISpxNamedProperties> CSpxSynthesizer::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

void CSpxSynthesizer::PushRequestIntoQueue(const std::wstring& requestId)
{
    std::unique_lock<std::mutex> lock(m_queueOperationMutex);
    m_requestQueue.emplace(requestId);
    lock.unlock();
}

void CSpxSynthesizer::WaitUntilRequestInFrontOfQueue(const std::wstring& requestId)
{
    std::unique_lock<std::mutex> lock(m_requestWaitingMutex);

#ifdef _DEBUG
    while (!m_cv.wait_for(lock, std::chrono::milliseconds(100), [&] {
        std::unique_lock<std::mutex> lock(m_queueOperationMutex);
        return m_requestQueue.front() == requestId; }))
    {
        SPX_DBG_TRACE_VERBOSE("%s: waiting for processing speak request ...", __FUNCTION__);
    }
#else
    m_cv.wait(lock, [&] {
        std::unique_lock<std::mutex> lock(m_queueOperationMutex);
        return m_requestQueue.front() == requestId; });
#endif
}

void CSpxSynthesizer::PopRequestFromQueue()
{
    std::unique_lock<std::mutex> lock(m_queueOperationMutex);
    m_requestQueue.pop();
    m_cv.notify_all();
}

std::shared_ptr<ISpxSynthesisResult> CSpxSynthesizer::CreateResult(const std::wstring& requestId, ResultReason reason, uint8_t* audio_buffer, size_t audio_length)
{
    // Get output format
    auto audioStream = SpxQueryInterface<ISpxAudioStream>(m_audioOutput);
    auto requiredFormatSize = audioStream->GetFormat(nullptr, 0);
    auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    audioStream->GetFormat(format.get(), requiredFormatSize);

    // Build result
    auto result = SpxCreateObjectWithSite<ISpxSynthesisResult>("CSpxSynthesisResult", SpxSiteFromThis(this));
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);
    resultInit->InitSynthesisResult(requestId, reason, REASON_CANCELED_NONE, CancellationErrorCode::NoError,
        audio_buffer, audio_length, format.get(), audioStream->HasHeader());
    auto events = this->QueryInterfaceInternal<ISpxSynthesizerEvents>();
    resultInit->SetEvents(events);

    return result;
}

void CSpxSynthesizer::FireResultEvent(std::shared_ptr<ISpxSynthesisResult> result)
{
    auto reason = result->GetReason();
    switch (reason)
    {
    case Microsoft::CognitiveServices::Speech::ResultReason::SynthesizingAudioStarted:
        FireSynthesisStarted(result);
        break;

    case Microsoft::CognitiveServices::Speech::ResultReason::SynthesizingAudio:
        FireSynthesizing(result);
        break;

    case Microsoft::CognitiveServices::Speech::ResultReason::SynthesizingAudioCompleted:
        FireSynthesisCompleted(result);
        break;

    case Microsoft::CognitiveServices::Speech::ResultReason::Canceled:
        FireSynthesisCanceled(result);
        break;

    default:
        break;
    }
}

void CSpxSynthesizer::FireEvent(std::list<std::pair<void*, std::shared_ptr<SynthEvent_Type>>> events, std::shared_ptr<ISpxSynthesisResult> result)
{
    auto iterator = events.begin();
    while (iterator != events.end())
    {
        auto pevent = iterator->second;
        if (pevent != nullptr)
        {
            auto synthEvent = SpxCreateObjectWithSite<ISpxSynthesisEventArgs>("CSpxSynthesisEventArgs", SpxSiteFromThis(this));
            auto argsInit = SpxQueryInterface<ISpxSynthesisEventArgsInit>(synthEvent);
            argsInit->Init(result);

            pevent->Signal(synthEvent);
        }

        iterator++;
    }
}

void CSpxSynthesizer::EnsureTtsEngineAdapter()
{
    if (m_ttsAdapter == nullptr)
    {
        InitializeTtsEngineAdapter();
    }
}

void CSpxSynthesizer::InitializeTtsEngineAdapter()
{
    // determine which type (or types) of tts engine adapters we should try creating...
    bool tryMock = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseTtsEngine-Mock", PAL::BoolToString(false).c_str()));
    bool tryRest = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseTtsEngine-Rest", PAL::BoolToString(false).c_str()));
    bool tryLocal = PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseTtsEngine-Local", PAL::BoolToString(false).c_str()));

    // if nobody specified which type(s) of TTS engine adapters this session should use, we'll use the REST
    if (!tryMock && !tryRest && !tryLocal)
    {
        tryRest = true;
    }

    // try to create the REST API adapter...
    if (m_ttsAdapter == nullptr && tryRest)
    {
        m_ttsAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxRestTtsEngineAdapter", this);
    }

    // try to create the mock tts engine adapter...
    if (m_ttsAdapter == nullptr && tryMock)
    {
        m_ttsAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxMockTtsEngineAdapter", this);
    }

    // try to create the local tts engine adapter...
    if (m_ttsAdapter == nullptr && tryLocal)
    {
        m_ttsAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxLocalTtsEngineAdapter", this);
    }

    // if we still don't have an adapter... that's an exception
    SPX_IFTRUE_THROW_HR(m_ttsAdapter == nullptr, SPXERR_NOT_FOUND);
}

void CSpxSynthesizer::ReleaseTtsEngineAdapter()
{
    if (m_ttsAdapter != nullptr)
    {
        m_ttsAdapter->Term();
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

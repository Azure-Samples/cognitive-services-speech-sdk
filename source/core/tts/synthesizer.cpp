//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// synthesizer.cpp: Implementation definitions for CSpxSynthesizer C++ class
//

#include "stdafx.h"
#include "synthesizer.h"
#include <future>
#include "site_helpers.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "synthesis_helper.h"
#include "property_id_2_name_map.h"
#include "guid_utils.h"
#include "log_helpers.h"
#include "error_info.h"

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

    CheckLogFilename();

    EnsureTtsEngineAdapter();
}

void CSpxSynthesizer::Term()
{
    SpxTermAndClear(m_ttsAdapter);

    if (m_audioOutput)
    {
        m_audioOutput->Close();
    }

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
    auto requestId = PAL::ToString(PAL::CreateGuidWithoutDashes());

    // Push request into queue
    PushRequestIntoQueue(requestId);

    // Wait until current request to be in front of the queue
    WaitUntilRequestInFrontOfQueue(requestId);

    if (m_requestQueue.empty() || m_requestQueue.front() != requestId)
    {
        return CreateUserCancelledResult(requestId);
    }

    // Fire SynthesisStarted event
    auto synthesisStartedResult = CreateResult(requestId, ResultReason::SynthesizingAudioStarted, nullptr, 0);
    FireResultEvent(synthesisStartedResult);

    // check if stop is called during firing synthesis started event
    if (m_shouldStop)
    {
        PopRequestFromQueue(requestId);
        return CreateUserCancelledResult(requestId);
    }

    // Speak
    auto synthesisDoneResult = m_ttsAdapter->Speak(text, isSsml, requestId, true);

    // Wait for audio output to be done
    m_audioOutput->WaitUntilDone();

    // Set events
    auto events = this->QueryInterfaceInternal<ISpxSynthesizerEvents>();
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(synthesisDoneResult);
    resultInit->SetEvents(events);

    // Fire SynthesisCompleted or SynthesisCanceled (depending on the result reason) event
    FireResultEvent(synthesisDoneResult);

    // Pop processed request from queue
    PopRequestFromQueue(requestId);

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
    auto requestId = PAL::ToString(PAL::CreateGuidWithoutDashes());

    // Push request into queue
    PushRequestIntoQueue(requestId);

    // Wait until current request to be in front of the queue
    WaitUntilRequestInFrontOfQueue(requestId);

    if (m_requestQueue.empty() || m_requestQueue.front() != requestId)
    {
        return CreateUserCancelledResult(requestId);
    }

    // Fire SynthesisStarted event
    auto synthesisStartedResult = CreateResult(requestId, ResultReason::SynthesizingAudioStarted, nullptr, 0);
    FireResultEvent(synthesisStartedResult);

    // check if stop is called during firing synthesis started event
    if (m_shouldStop)
    {
        PopRequestFromQueue(requestId);
        return CreateUserCancelledResult(requestId);
    }

    auto keepAlive = SpxSharedPtrFromThis<ISpxSynthesizer>(this);
    std::shared_future<std::shared_ptr<ISpxSynthesisResult>> waitForCompletion(std::async(std::launch::async,
        [this, keepAlive, requestId, text, isSsml]() {
        // Speak
        auto synthesisDoneResult = m_ttsAdapter->Speak(text, isSsml, requestId, true);

        // Wait for audio output to be done
        m_audioOutput->WaitUntilDone();

        // Set events
        auto events = this->QueryInterfaceInternal<ISpxSynthesizerEvents>();
        auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(synthesisDoneResult);
        resultInit->SetEvents(events);

        // Fire SynthesisCompleted or SynthesisCanceled (depending on the result reason) event
        FireResultEvent(synthesisDoneResult);

        // Pop processed request from queue
        PopRequestFromQueue(requestId);

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

void CSpxSynthesizer::StopSpeaking()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    // clear request queue and push an empty request as a placeholder
    ClearRequestQueueAndKeepFront();

    m_shouldStop = true;

    // call stop method of adapter
    m_ttsAdapter->StopSpeaking();

    if (m_audioOutput)
    {
        m_audioOutput->ClearUnread();
    }

    WaitUntilRequestInFrontOfQueue(std::string());

    m_shouldStop = false;

    // Pop the empty request from queue
    PopRequestFromQueue();
}

CSpxAsyncOp<void> CSpxSynthesizer::StopSpeakingAsync()
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxSynthesizer>(this);
    std::shared_future<void> waitForSpeakStop(std::async(std::launch::async, [this, keepAlive]() {
        // stop speaking
        return StopSpeaking();
    }));

    return CSpxAsyncOp<void>(waitForSpeakStop, AOS_Started);
}

void CSpxSynthesizer::Close()
{
    m_audioOutput->Close();
}

void CSpxSynthesizer::ConnectSynthesisStartedCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesisStartedMutex);

    auto iterator = SynthesisStarted.begin();
    while (iterator != SynthesisStarted.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != SynthesisStarted.end())
    {
        iterator->second->Connect(callback);
    }
    else
    {
        auto eventSignal = std::make_shared<EventSignal<std::shared_ptr<ISpxSynthesisEventArgs>>>();
        eventSignal->Connect(callback);
        SynthesisStarted.emplace_front(object, eventSignal);
    }
}

void CSpxSynthesizer::ConnectSynthesizingCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesizingMutex);

    auto iterator = Synthesizing.begin();
    while (iterator != Synthesizing.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != Synthesizing.end())
    {
        iterator->second->Connect(callback);
    }
    else
    {
        auto eventSignal = std::make_shared<EventSignal<std::shared_ptr<ISpxSynthesisEventArgs>>>();
        eventSignal->Connect(callback);
        Synthesizing.emplace_front(object, eventSignal);
    }
}

void CSpxSynthesizer::ConnectSynthesisCompletedCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesisCompletedMutex);

    auto iterator = SynthesisCompleted.begin();
    while (iterator != SynthesisCompleted.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != SynthesisCompleted.end())
    {
        iterator->second->Connect(callback);
    }
    else
    {
        auto eventSignal = std::make_shared<EventSignal<std::shared_ptr<ISpxSynthesisEventArgs>>>();
        eventSignal->Connect(callback);
        SynthesisCompleted.emplace_front(object, eventSignal);
    }
}

void CSpxSynthesizer::ConnectSynthesisCanceledCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesisCanceledMutex);

    auto iterator = SynthesisCanceled.begin();
    while (iterator != SynthesisCanceled.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != SynthesisCanceled.end())
    {
        iterator->second->Connect(callback);
    }
    else
    {
        auto eventSignal = std::make_shared<EventSignal<std::shared_ptr<ISpxSynthesisEventArgs>>>();
        eventSignal->Connect(callback);
        SynthesisCanceled.emplace_front(object, eventSignal);
    }
}

void CSpxSynthesizer::DisconnectSynthesisStartedCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesisStartedMutex);

    auto iterator = SynthesisStarted.begin();
    while (iterator != SynthesisStarted.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != SynthesisStarted.end())
    {
        if (callback != nullptr)
        {
            iterator->second->Disconnect(callback);
        }
        else
        {
            iterator->second->DisconnectAll();
        }

        if (!iterator->second->IsConnected())
        {
            SynthesisStarted.remove(*iterator);
        }
    }
}

void CSpxSynthesizer::DisconnectSynthesizingCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesizingMutex);

    auto iterator = Synthesizing.begin();
    while (iterator != Synthesizing.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != Synthesizing.end())
    {
        if (callback != nullptr)
        {
            iterator->second->Disconnect(callback);
        }
        else
        {
            iterator->second->DisconnectAll();
        }

        if (!iterator->second->IsConnected())
        {
            Synthesizing.remove(*iterator);
        }
    }
}

void CSpxSynthesizer::DisconnectSynthesisCompletedCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesisCompletedMutex);

    auto iterator = SynthesisCompleted.begin();
    while (iterator != SynthesisCompleted.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != SynthesisCompleted.end())
    {
        if (callback != nullptr)
        {
            iterator->second->Disconnect(callback);
        }
        else
        {
            iterator->second->DisconnectAll();
        }

        if (!iterator->second->IsConnected())
        {
            SynthesisCompleted.remove(*iterator);
        }
    }
}

void CSpxSynthesizer::DisconnectSynthesisCanceledCallback(void* object, SynthesisCallbackFunction_Type callback)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_synthesisCanceledMutex);

    auto iterator = SynthesisCanceled.begin();
    while (iterator != SynthesisCanceled.end() && iterator->first != object)
    {
        ++iterator;
    }

    if (iterator != SynthesisCanceled.end())
    {
        if (callback != nullptr)
        {
            iterator->second->Disconnect(callback);
        }
        else
        {
            iterator->second->DisconnectAll();
        }

        if (!iterator->second->IsConnected())
        {
            SynthesisCanceled.remove(*iterator);
        }
    }
}

void CSpxSynthesizer::FireSynthesisStarted(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireSynthesisEvent(SynthesisStarted, result);
}

void CSpxSynthesizer::FireSynthesizing(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireSynthesisEvent(Synthesizing, result);
}

void CSpxSynthesizer::FireSynthesisCompleted(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireSynthesisEvent(SynthesisCompleted, result);
}

void CSpxSynthesizer::FireSynthesisCanceled(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    FireSynthesisEvent(SynthesisCanceled, result);
}

void CSpxSynthesizer::FireWordBoundary(uint64_t audioOffset, uint32_t textOffset, uint32_t wordLength)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    auto wordBoundaryEvent = SpxCreateObjectWithSite<ISpxWordBoundaryEventArgs>("CSpxWordBoundaryEventArgs", SpxSiteFromThis(this));
    auto argsInit = SpxQueryInterface<ISpxWordBoundaryEventArgsInit>(wordBoundaryEvent);
    argsInit->Init(audioOffset, textOffset, wordLength);
    WordBoundary.Signal(wordBoundaryEvent);
}

uint32_t CSpxSynthesizer::Write(ISpxTtsEngineAdapter* adapter, const std::string& requestId, uint8_t* buffer,
                                uint32_t size, std::shared_ptr<std::unordered_map<std::string, std::string>> properties)
{
    UNUSED(adapter);

    if (m_shouldStop)
    {
        return size;
    }

    // Fire Synthesizing event
    const auto result = CreateResult(requestId, ResultReason::SynthesizingAudio, buffer, size, properties);
    FireResultEvent(result);

    // Write audio data to output
    return m_audioOutput->Write(buffer, size);
}

std::shared_ptr<ISpxSynthesizerEvents> CSpxSynthesizer::GetEventsSite()
{
    std::shared_ptr<ISpxGenericSite> site = static_cast<ISpxGenericSite*>(this)->shared_from_this();
    return SpxQueryInterface<ISpxSynthesizerEvents>(site);
}

std::shared_ptr<ISpxSynthesisResult> CSpxSynthesizer::CreateEmptySynthesisResult()
{
    return SpxCreateObjectWithSite<ISpxSynthesisResult>("CSpxSynthesisResult", this);
}

std::shared_ptr<ISpxNamedProperties> CSpxSynthesizer::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

void CSpxSynthesizer::CheckLogFilename()
{
    SpxDiagLogSetProperties(GetParentProperties());
}

void CSpxSynthesizer::PushRequestIntoQueue(const std::string requestId)
{
    std::unique_lock<std::mutex> lock(m_queueOperationMutex);
    m_requestQueue.emplace(requestId);
    m_cv.notify_all();
}

void CSpxSynthesizer::WaitUntilRequestInFrontOfQueue(const std::string& requestId)
{
    std::unique_lock<std::mutex> lock(m_requestWaitingMutex);

#ifdef _DEBUG
    while (!m_cv.wait_for(lock, std::chrono::milliseconds(100), [&] {
        std::unique_lock<std::mutex> lock(m_queueOperationMutex);
        return m_requestQueue.empty() || m_requestQueue.front() == requestId || m_requestQueue.front().empty(); }))
    {
        SPX_DBG_TRACE_VERBOSE("%s: waiting for processing speak request ...", __FUNCTION__);
    }
#else
    m_cv.wait(lock, [&] {
        std::unique_lock<std::mutex> lock(m_queueOperationMutex);
        return m_requestQueue.empty() || m_requestQueue.front() == requestId || m_requestQueue.front().empty(); });
#endif
}

void CSpxSynthesizer::PopRequestFromQueue(const std::string& requestId)
{
    std::unique_lock<std::mutex> lock(m_queueOperationMutex);
    if (!m_requestQueue.empty() && m_requestQueue.front() == requestId)
    {
        m_requestQueue.pop();
    }
    m_cv.notify_all();
}

void CSpxSynthesizer::ClearRequestQueueAndKeepFront()
{
    std::unique_lock<std::mutex> lock(m_queueOperationMutex);
    std::queue<std::string> empty;
    if (!m_requestQueue.empty())
    {
        empty.emplace(m_requestQueue.front());
    }

    std::swap(m_requestQueue, empty);
    m_requestQueue.emplace("");
    m_cv.notify_all();
}

std::shared_ptr<ISpxSynthesisResult> CSpxSynthesizer::CreateResult(const std::string& requestId, ResultReason reason,
                                                                   uint8_t* audio_buffer, size_t audio_length,
                                                                   std::shared_ptr<std::unordered_map<std::string, std::string>> properties)
{
    // Get output format
    auto audioStream = SpxQueryInterface<ISpxAudioStream>(m_audioOutput);
    const auto requiredFormatSize = audioStream->GetFormat(nullptr, 0);
    const auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    audioStream->GetFormat(format.get(), requiredFormatSize);

    // Generate cancellation error information when relevant; keep it consistent with the USP version
    std::shared_ptr<ISpxErrorInformation> errorInfo;
    if (reason == ResultReason::Canceled)
    {
        errorInfo = ErrorInfo::FromHttpStatus(HttpStatusCode::CLIENT_CLOSED_REQUEST);
    }

    // Build result
    auto result = SpxCreateObjectWithSite<ISpxSynthesisResult>("CSpxSynthesisResult", SpxSiteFromThis(this));
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);
    resultInit->InitSynthesisResult(requestId, reason, REASON_CANCELED_NONE, errorInfo,
        audio_buffer, audio_length, format.get(), SpxQueryInterface<ISpxAudioOutputFormat>(m_audioOutput)->HasHeader());

    if (properties != nullptr)
    {
        auto resultProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        for (const auto &p : *properties)
        {
            resultProperties->SetStringValue(p.first.c_str(), p.second.c_str());
        }
    }

    auto events = this->QueryInterfaceInternal<ISpxSynthesizerEvents>();
    resultInit->SetEvents(events);

    return result;
}

std::shared_ptr<ISpxSynthesisResult> CSpxSynthesizer::CreateUserCancelledResult(const std::string& requestId)
{
    auto cancelledResult = CreateResult(requestId, ResultReason::Canceled, nullptr, 0);
    auto resultProperties = SpxQueryInterface<ISpxNamedProperties>(cancelledResult);
    return cancelledResult;
}

void CSpxSynthesizer::FireResultEvent(std::shared_ptr<ISpxSynthesisResult> result)
{
    switch (result->GetReason())
    {
    case ResultReason::SynthesizingAudioStarted:
        FireSynthesisStarted(result);
        break;

    case ResultReason::SynthesizingAudio:
        FireSynthesizing(result);
        break;

    case ResultReason::SynthesizingAudioCompleted:
        FireSynthesisCompleted(result);
        break;

    case ResultReason::Canceled:
        FireSynthesisCanceled(result);
        break;

    default:
        break;
    }
}

void CSpxSynthesizer::FireSynthesisEvent(std::list<std::pair<void*, std::shared_ptr<SynthEvent_Type>>> events, std::shared_ptr<ISpxSynthesisResult> result)
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

        ++iterator;
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
    bool tryRest = false, tryUsp = false;
    std::string endpointUrl = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), "");
    std::string hostUrl = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Host), "");
    std::string endpoint;

    if (!endpointUrl.empty())
    {
        endpoint = endpointUrl;
    }
    else if (!hostUrl.empty())
    {
        endpoint = hostUrl;
    }

#ifdef SPEECHSDK_HYBRID_TTS_ENABLED
    bool tryLocal = false, tryHybrid = false;

    const auto backend = GetStringValue("SPEECH-SynthBackend", "");
    if (backend == "hybrid")
    {
        tryHybrid = true;
    }
    else if (backend == "offline")
    {
        tryLocal = true;
    }
    else if (!endpoint.empty())
#else
    if (!endpoint.empty())
#endif
     {
        auto url = HttpUtils::ParseUrl(endpoint);
        switch (url.scheme)
        {
            case UriScheme::HTTP:
            case UriScheme::HTTPS:
                tryRest = true;
                break;

            case UriScheme::WS:
            case UriScheme::WSS:
                tryUsp = true;
                break;
        }
    }

    bool tryMock = PAL::ToBool(GetStringValue("SDK-INTERNAL-UseTtsEngine-Mock", PAL::BoolToString(false))) ||
                   PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseTtsEngine-Mock", PAL::BoolToString(false)));
    tryRest = tryRest || PAL::ToBool(GetStringValue("SDK-INTERNAL-UseTtsEngine-Rest", PAL::BoolToString(false))) ||
                   PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseTtsEngine-Rest", PAL::BoolToString(false)));
    tryUsp = tryUsp || PAL::ToBool(GetStringValue("SDK-INTERNAL-UseTtsEngine-Usp", PAL::BoolToString(false))) ||
                  PAL::ToBool(GetStringValue("CARBON-INTERNAL-UseTtsEngine-Usp", PAL::BoolToString(false)));

    // if nobody specified which type(s) of TTS engine adapters this session should use, we'll use the USP
#ifdef SPEECHSDK_HYBRID_TTS_ENABLED
    if (!tryMock && !tryRest && !tryUsp && !tryLocal && !tryHybrid)
#else
    if (!tryMock && !tryRest && !tryUsp)
#endif
    {
        tryUsp = true;
    }

    // try to create the USP adapter...
    if (m_ttsAdapter == nullptr && tryUsp)
    {
        m_ttsAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxUspTtsEngineAdapter", this);
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

#ifdef SPEECHSDK_HYBRID_TTS_ENABLED
    // try to create the local tts engine adapter...
    if (m_ttsAdapter == nullptr && tryHybrid)
    {
        m_ttsAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxHybridTtsEngineAdapter", this);
    }

    // try to create the local tts engine adapter...
    if (m_ttsAdapter == nullptr && tryLocal)
    {
        m_ttsAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxLocalTtsEngineAdapter", this);
    }
#endif

    // if we still don't have an adapter... that's an exception
    SPX_IFTRUE_THROW_HR(m_ttsAdapter == nullptr, SPXERR_NOT_FOUND);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

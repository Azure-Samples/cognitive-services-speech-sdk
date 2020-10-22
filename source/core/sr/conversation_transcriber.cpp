//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_transcriber.cpp: implementation declarations for conversation transcriber
//

#include "stdafx.h"
#include "json.h"
#include <sstream>
#include "conversation_transcriber.h"
#include "spx_namespace.h"
#include "usp.h"
#include "http_utils.h"
#include "http_request.h"
#include "http_response.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxConversationTranscriber::CSpxConversationTranscriber()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxConversationTranscriber::~CSpxConversationTranscriber()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxConversationTranscriber::JoinConversation(std::weak_ptr<ISpxConversation> conversation)
{
    m_conversation = conversation;
    m_has_participant = true;
}

// leave conversation should cut all the events firing., transcribing, transcribed,
// participant changed, TextMessageReceived
void CSpxConversationTranscriber::LeaveConversation()
{
    m_has_participant = false;
}

void CSpxConversationTranscriber::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    EnsureDefaultSession();
    CheckLogFilename();
    m_has_participant = true;
}

//todo:
void CSpxConversationTranscriber::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxConversationTranscriber::SetAudioConfig(std::weak_ptr<ISpxAudioConfig> audio_config)
{
    m_audioInput = audio_config;
}

std::shared_ptr<ISpxAudioConfig> CSpxConversationTranscriber::GetAudioConfig()
{
    return m_audioInput.lock();
}

void CSpxConversationTranscriber::FireSessionStarted(const std::wstring& sessionId)
{
    auto factory = SpxQueryService<ISpxEventArgsFactory>(CheckAndGetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStarted.Signal(sessionEvent);
}

void CSpxConversationTranscriber::FireSessionStopped(const std::wstring& sessionId)
{
    auto factory = SpxQueryService<ISpxEventArgsFactory>(CheckAndGetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStopped.Signal(sessionEvent);
}

void CSpxConversationTranscriber::FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechStartDetected, sessionId, nullptr, offset);
}

void CSpxConversationTranscriber::FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechEndDetected, sessionId, nullptr, offset);
}

void CSpxConversationTranscriber::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    ISpxRecognizerEvents::RecoEvent_Type* event = nullptr;
    auto reason = result->GetReason();
    bool dispatch_event = true;
    switch (reason)
    {
    case ResultReason::Canceled:
        event = &Canceled;
        break;

    case ResultReason::NoMatch:
    case ResultReason::RecognizedSpeech:
    case ResultReason::RecognizedKeyword:
        if (!m_has_participant)
        {
            dispatch_event = false;
        }
        event = &FinalResult;
        SPX_DBG_TRACE_VERBOSE_IF(!event->IsConnected(), "%s: No FinalResult event signal connected!! nobody listening...", __FUNCTION__);
        break;

    case ResultReason::RecognizingSpeech:
    case ResultReason::RecognizingKeyword:
        if (!m_has_participant)
        {
            dispatch_event = false;
        }
        event = &IntermediateResult;
        break;

    default:
        // This should be changed to throw exception. But currently it causes problem in lock.
        // Bug: https://msasg.visualstudio.com/Skyman/_workitems/edit/1314877
        SPX_DBG_ASSERT_WITH_MESSAGE(false, "The reason found in the result was unexpected.");
        break;
    }

    if (dispatch_event)
    {
        FireRecoEvent(event, sessionId, result);
    }
    else
    {
        SPX_TRACE_INFO("Not dispatching recognizing and recognized results due to participants left the conversation.");
    }
}

void CSpxConversationTranscriber::FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* event, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset)
{
    if (event != nullptr)
    {
        if (event->IsConnected())
        {
            auto factory = SpxQueryService<ISpxEventArgsFactory>(CheckAndGetSite());
            auto recoEvent = (result != nullptr)
                ? factory->CreateRecognitionEventArgs(sessionId, result)
                : factory->CreateRecognitionEventArgs(sessionId, offset);
            event->Signal(recoEvent);
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("No listener connected to event");
        }
    }
}

CSpxAsyncOp<void> CSpxConversationTranscriber::StartContinuousRecognitionAsync()
{
    CheckSite(GetSite().get());
    return BaseType::StartContinuousRecognitionAsync();
}

std::shared_ptr<ISpxRecognizerSite> CSpxConversationTranscriber::CheckAndGetSite()
{
    auto site = GetSite();
    CheckSite(site.get());
    return site;
}

void CSpxConversationTranscriber::CheckSite(const ISpxRecognizerSite * site)
{
    if (site == nullptr)
    {
        ThrowRuntimeError("Did you forget to call JoinConversationAsync before calling StartTranscribingAsync?");
    }
}
}}}} // Microsoft::CognitiveServices::Speech::Impl

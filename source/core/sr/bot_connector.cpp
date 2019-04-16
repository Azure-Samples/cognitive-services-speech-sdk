//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// bot_connector.cpp: Implementation definitions for CSpxSpeechBotConnector C++ class.
//
#include "stdafx.h"
#include "recognizer.h"
#include "handle_table.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "property_id_2_name_map.h"
#include "bot_connector.h"
#include "file_logger.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxSpeechBotConnector::CSpxSpeechBotConnector() :
    ISpxSpeechBotConnectorEvents(nullptr, nullptr),
    m_fEnabled(true)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxSpeechBotConnector::~CSpxSpeechBotConnector()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    TermDefaultSession();
}

void CSpxSpeechBotConnector::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    EnsureDefaultSession();
    CheckLogFilename();
}

void CSpxSpeechBotConnector::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxSpeechBotConnector::SetStringValue(const char* name, const char* value)
{
    SetStringValueInProperties(name, value);
}

CSpxAsyncOp<void> CSpxSpeechBotConnector::ConnectAsync()
{
    auto keep_alive = SpxSharedPtrFromThis<ISpxSpeechBotConnector>(this);

    std::shared_future<void> taskFuture = std::async(std::launch::async, [this, keep_alive]()
    {
        return m_defaultSession->OpenConnection(false);
    });
    return CSpxAsyncOp<void>{ taskFuture, AOS_Started};
}

CSpxAsyncOp<void> CSpxSpeechBotConnector::DisconnectAsync()
{
    auto keep_alive = SpxSharedPtrFromThis<ISpxSpeechBotConnector>(this);

    std::shared_future<void> taskFuture = std::async(std::launch::async, [this, keep_alive]()
    {
        return m_defaultSession->CloseConnection();
    });
    return CSpxAsyncOp<void>{ taskFuture, AOS_Started};
}

CSpxAsyncOp<std::string> CSpxSpeechBotConnector::SendActivityAsync(std::shared_ptr<ISpxActivity> activity)
{
    return m_defaultSession->SendActivityAsync(activity);
}

void CSpxSpeechBotConnector::SetRecoMode(const char* modeToSet)
{
    const char* reco_mode = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode = GetStringValueFromProperties(reco_mode, "");
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode), modeToSet);
    }
    else
    {
        // Since the mode is set during connection setup, no mode switch is allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(modeToSet) != 0), SPXERR_SWITCH_MODE_NOT_ALLOWED);;
    }
}

CSpxAsyncOp<void> CSpxSpeechBotConnector::StartContinuousListeningAsync()
{
    SetRecoMode(g_recoModeConversation);
    return m_defaultSession->StartContinuousRecognitionAsync();
}

CSpxAsyncOp<void> CSpxSpeechBotConnector::StopContinuousListeningAsync()
{
    return m_defaultSession->StopContinuousRecognitionAsync();
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxSpeechBotConnector::ListenOnceAsync()
{
    SetRecoMode(g_recoModeInteractive);
    return m_defaultSession->RecognizeAsync();
}

std::shared_ptr<ISpxSession> CSpxSpeechBotConnector::GetDefaultSession()
{
    EnsureDefaultSession();
    return SpxQueryInterface<ISpxSession>(m_defaultSession);
}

void CSpxSpeechBotConnector::FireSessionStarted(const std::wstring& sessionId)
{
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStarted.Signal(sessionEvent);
}

void CSpxSpeechBotConnector::FireSessionStopped(const std::wstring& sessionId)
{
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStopped.Signal(sessionEvent);
}

void CSpxSpeechBotConnector::FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechStartDetected, sessionId, nullptr, offset);
}

void CSpxSpeechBotConnector::FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechEndDetected, sessionId, nullptr, offset);
}

void CSpxSpeechBotConnector::FireActivityReceived(const std::wstring& sessionId, std::shared_ptr<ISpxActivity> activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    UNUSED(sessionId);
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto activityEvent = factory->CreateActivityEventArgs(activity, audio);
    ActivityReceived.Signal(activityEvent);
}

void CSpxSpeechBotConnector::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    ISpxRecognizerEvents::RecoEvent_Type* event = nullptr;
    auto reason = result->GetReason();
    switch (reason)
    {
    case ResultReason::Canceled:
        event = &Canceled;
        break;

    case ResultReason::NoMatch:
    case ResultReason::RecognizedSpeech:
    case ResultReason::RecognizedKeyword:
        event = &FinalResult;
        SPX_DBG_TRACE_VERBOSE_IF(!event->IsConnected(), "%s: No FinalResult event signal connected!! nobody listening...", __FUNCTION__);
        break;

    case ResultReason::RecognizingSpeech:
    case ResultReason::RecognizingKeyword:
        event = &IntermediateResult;
        break;

    default:
        // This should be changed to throw exception. But currently it causes problem in lock.
        // Bug: https://msasg.visualstudio.com/Skyman/_workitems/edit/1314877
        SPX_DBG_ASSERT_WITH_MESSAGE(false, "The reason found in the result was unexpected.");
        break;
    }

    FireRecoEvent(event, sessionId, result);
}

void CSpxSpeechBotConnector::FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* event, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset)
{
    if (event != nullptr && event->IsConnected())
    {
        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
        auto recoEvent = (result != nullptr)
            ? factory->CreateRecognitionEventArgs(sessionId, result)
            : factory->CreateRecognitionEventArgs(sessionId, offset);
        event->Signal(recoEvent);
    }
}

void CSpxSpeechBotConnector::SetStringValueInProperties(const char* name, const char* value)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);

    // only allow authorization token to be set again.
    if (PAL::stricmp(name, GetPropertyName(PropertyId::SpeechServiceAuthorization_Token)) != 0)
    {
        SPX_IFTRUE_THROW_HR(namedProperties->HasStringValue(name), SPXERR_ALREADY_INITIALIZED); // throw if it's already been set
    }
    namedProperties->SetStringValue(name, value);
}

std::string CSpxSpeechBotConnector::GetStringValueFromProperties(const char* name, const char* defaultValue)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    return namedProperties->GetStringValue(name, defaultValue);
}

void CSpxSpeechBotConnector::EnsureDefaultSession()
{
    if (m_defaultSession == nullptr)
    {
        SPX_DBG_ASSERT(GetSite());
        m_defaultSession = GetSite()->GetDefaultSession();
    }
}

void CSpxSpeechBotConnector::TermDefaultSession()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_defaultSession != nullptr)
    {
        SpxQueryInterface<ISpxSession>(m_defaultSession)->RemoveRecognizer(this);
    }
    SpxTermAndClear(m_defaultSession);
}

void CSpxSpeechBotConnector::OnIsEnabledChanged()
{
    // no op currently
}

std::shared_ptr<ISpxNamedProperties> CSpxSpeechBotConnector::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

void CSpxSpeechBotConnector::CheckLogFilename()
{
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    auto filename = namedProperties->GetStringValue(GetPropertyName(PropertyId::Speech_LogFilename), "");
    if (!filename.empty())
    {
        FileLogger::Instance().SetFilename(std::move(filename));
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

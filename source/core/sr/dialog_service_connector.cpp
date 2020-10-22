//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// dialog_service_connector.cpp: Implementation definitions for CSpxDialogServiceConnector C++ class.
//
#include "stdafx.h"
#include "recognizer.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "property_id_2_name_map.h"
#include "dialog_service_connector.h"
#include "log_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxDialogServiceConnector::CSpxDialogServiceConnector() :
    ISpxDialogServiceConnectorEvents(nullptr, nullptr),
    m_fEnabled(true)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxDialogServiceConnector::~CSpxDialogServiceConnector()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    TermDefaultSession();
}

void CSpxDialogServiceConnector::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    EnsureDefaultSession();
    CheckLogFilename();
}

void CSpxDialogServiceConnector::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxDialogServiceConnector::SetStringValue(const char* name, const char* value)
{
    SetStringValueInProperties(name, value);
}

CSpxAsyncOp<void> CSpxDialogServiceConnector::ConnectAsync()
{
    auto keep_alive = SpxSharedPtrFromThis<ISpxDialogServiceConnector>(this);

    std::shared_future<void> taskFuture = std::async(std::launch::async, [this, keep_alive]()
    {
        return m_defaultSession->OpenConnection(false);
    });
    return CSpxAsyncOp<void>{ taskFuture, AOS_Started};
}

CSpxAsyncOp<void> CSpxDialogServiceConnector::DisconnectAsync()
{
    auto keep_alive = SpxSharedPtrFromThis<ISpxDialogServiceConnector>(this);

    std::shared_future<void> taskFuture = std::async(std::launch::async, [this, keep_alive]()
    {
        return m_defaultSession->CloseConnection();
    });
    return CSpxAsyncOp<void>{ taskFuture, AOS_Started};
}

CSpxAsyncOp<std::string> CSpxDialogServiceConnector::SendActivityAsync(std::string activity)
{
    SetRecoMode(g_recoModeInteractive);
    return m_defaultSession->SendActivityAsync(std::move(activity));
}

void CSpxDialogServiceConnector::SetRecoMode(const char* modeToSet)
{
    const char* recoModePropertyName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode = GetStringValueFromProperties(recoModePropertyName, "");
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(recoModePropertyName, modeToSet);
    }
    else
    {
        // Since the mode is set during connection setup, no mode switch is allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(modeToSet) != 0), SPXERR_SWITCH_MODE_NOT_ALLOWED);;
    }
}

CSpxAsyncOp<void> CSpxDialogServiceConnector::StartContinuousListeningAsync()
{
    SetRecoMode(g_recoModeInteractive);
    return m_defaultSession->StartContinuousRecognitionAsync();
}

CSpxAsyncOp<void> CSpxDialogServiceConnector::StopContinuousListeningAsync()
{
    return m_defaultSession->StopContinuousRecognitionAsync();
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxDialogServiceConnector::ListenOnceAsync()
{
    SetRecoMode(g_recoModeInteractive);
    return m_defaultSession->RecognizeAsync();
}

std::shared_ptr<ISpxSession> CSpxDialogServiceConnector::GetDefaultSession()
{
    EnsureDefaultSession();
    return SpxQueryInterface<ISpxSession>(m_defaultSession);
}

void CSpxDialogServiceConnector::FireSessionStarted(const std::wstring& sessionId)
{
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStarted.Signal(sessionEvent);
}

void CSpxDialogServiceConnector::FireSessionStopped(const std::wstring& sessionId)
{
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStopped.Signal(sessionEvent);
}

void CSpxDialogServiceConnector::FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechStartDetected, sessionId, nullptr, offset);
}

void CSpxDialogServiceConnector::FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechEndDetected, sessionId, nullptr, offset);
}

void CSpxDialogServiceConnector::FireActivityReceived(const std::wstring& sessionId, std::string activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    UNUSED(sessionId);
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto activityEvent = factory->CreateActivityEventArgs(std::move(activity), audio);
    ActivityReceived.Signal(activityEvent);
}

void CSpxDialogServiceConnector::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
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

void CSpxDialogServiceConnector::FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* event, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset)
{
    if (event != nullptr)
    {
        if(event->IsConnected())
        {
        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
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

void CSpxDialogServiceConnector::SetStringValueInProperties(const char* name, const char* value)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);

    // only allow authorization token and activity template to be set again.
    if (PAL::stricmp(name, GetPropertyName(PropertyId::SpeechServiceAuthorization_Token)) != 0 ||
        PAL::stricmp(name, GetPropertyName(PropertyId::Conversation_Speech_Activity_Template)) != 0)
    {
        SPX_IFTRUE_THROW_HR(namedProperties->HasStringValue(name), SPXERR_ALREADY_INITIALIZED); // throw if it's already been set
    }

    namedProperties->SetStringValue(name, value);
}

std::string CSpxDialogServiceConnector::GetStringValueFromProperties(const char* name, const char* defaultValue)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    return namedProperties->GetStringValue(name, defaultValue);
}

void CSpxDialogServiceConnector::EnsureDefaultSession()
{
    if (m_defaultSession == nullptr)
    {
        SPX_DBG_ASSERT(GetSite());
        m_defaultSession = GetSite()->GetDefaultSession();
    }
}

void CSpxDialogServiceConnector::TermDefaultSession()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_defaultSession != nullptr)
    {
        SpxQueryInterface<ISpxSession>(m_defaultSession)->RemoveRecognizer(this);
    }
    SpxTermAndClear(m_defaultSession);
}

void CSpxDialogServiceConnector::OnIsEnabledChanged()
{
    // no op currently
}

std::shared_ptr<ISpxNamedProperties> CSpxDialogServiceConnector::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

void CSpxDialogServiceConnector::CheckLogFilename()
{
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    SpxDiagLogSetProperties(namedProperties);
}

CSpxAsyncOp<void> CSpxDialogServiceConnector::StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model)
{
    const char* recoModePropertyName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode = GetStringValueFromProperties(recoModePropertyName, "");

    // currently, kws uses recoModeInteractive as default, but takes the passed mode, if configured
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(recoModePropertyName, g_recoModeInteractive);
    }
    return m_defaultSession->StartKeywordRecognitionAsync(model);
}

CSpxAsyncOp<void> CSpxDialogServiceConnector::StopKeywordRecognitionAsync()
{
    return m_defaultSession->StopKeywordRecognitionAsync();
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

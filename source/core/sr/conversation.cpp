//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation.cpp: implementation declarations for conversation
//

#include "stdafx.h"
#include "json.h"
#include <sstream>
#include "conversation.h"
#include "spx_namespace.h"
#include "usp.h"
#include "http_utils.h"
#include "http_request.h"
#include "http_response.h"
#include "create_object_helpers.h"

#include "participants_mgr.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxConversation::CSpxConversation()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxConversation::~CSpxConversation()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    // conversation object may die before transcribers, after that Carbon still needs session. So, can't kill session here.
}

void CSpxConversation::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    std::shared_ptr<ISpxRecognizerSite> site = GetSite();
    SPX_IFTRUE_THROW_HR(site == nullptr, SPXERR_UNINITIALIZED);

    auto genericSite = site->QueryInterface<ISpxGenericSite>();
    SPX_IFTRUE_THROW_HR(genericSite == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    auto thread_service = SpxQueryService<ISpxThreadService>(site);
    SPX_IFTRUE_THROW_HR(thread_service == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    m_keepSessionAlive = site;

    // Initialize websocket platform
    Microsoft::CognitiveServices::Speech::USP::PlatformInit(nullptr, 0, nullptr, nullptr);

    auto properties = SpxQueryService<ISpxNamedProperties>(site);
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    auto need_participant_mgr = properties->GetStringValue("ConversationTranscriptionInRoomAndOnline");
    if (PAL::ToBool(need_participant_mgr))
    {
        m_impl = std::make_shared<CSpxParticipantMgrImpl>(thread_service, m_keepSessionAlive);
        SPX_DBG_TRACE_INFO("Created a CSpxParticipantMgrImpl for manager participants in a meeting.");
    }
    else
    {
        m_impl = SpxCreateObjectWithSite<ISpxConversation>("CSpxConversationImpl", genericSite);
        SPX_DBG_TRACE_INFO("Created a CSpxParticipantMgrImpl for the conversation translator service.");
    }

    SPX_DBG_ASSERT(m_impl != nullptr);
    SetRecoMode();
}

std::shared_ptr<ISpxSession> CSpxConversation::InternalQueryService(const char* serviceName)
{
    if (!serviceName)
    {
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    if (PAL::stricmp(PAL::GetTypeName<ISpxSession>().c_str(), serviceName) == 0)
    {
        return SpxQueryService<ISpxSession>(GetSite());
    }

    return nullptr;
}

void CSpxConversation::SetRecoMode()
{
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    std::string currentRecoMode = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode));

    // Conversation transcriber uses CONTINUOUS mode.
    if (currentRecoMode.empty())
    {
        properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode), g_recoModeConversation);
    }
}

void CSpxConversation::Term()
{
    if (m_impl != nullptr)
    {
        auto objectInit = m_impl->QueryInterface<ISpxObjectInit>();
        if (objectInit != nullptr)
        {
            objectInit->Term();
        }

        m_impl = nullptr;
    }
}

void CSpxConversation::UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant> participant)
{
    ValidateImpl();
    m_impl->UpdateParticipant(add, userId, participant);
}

void CSpxConversation::UpdateParticipant(bool add, const std::string& userId)
{
    ValidateImpl();
    m_impl->UpdateParticipant(add, userId);
}

void CSpxConversation::UpdateParticipants(bool add, std::vector<ParticipantPtr>&& participants)
{
    ValidateImpl();
    m_impl->UpdateParticipants(add, std::move(participants));
}

void CSpxConversation::SetConversationId(const std::string& id)
{
    ValidateImpl();
    m_impl->SetConversationId(id);
}

const std::string CSpxConversation::GetConversationId() const
{
    ValidateImpl();
    return m_impl->GetConversationId();
}

std::string  CSpxConversation::GetSpeechEventPayload(ISpxConversation::MeetingState state)
{
    ValidateImpl();
    return m_impl->GetSpeechEventPayload(state);
}

void CSpxConversation::CreateConversation(const std::string & nickname)
{
    ValidateImpl();
    m_impl->CreateConversation(nickname);
}

void CSpxConversation::DeleteConversation()
{
    ValidateImpl();
    m_impl->DeleteConversation();
}

void CSpxConversation::StartConversation()
{
    ValidateImpl();
    m_impl->StartConversation();
}

void CSpxConversation::EndConversation()
{
    ValidateImpl();
    m_impl->EndConversation();
}

void CSpxConversation::SetLockConversation(bool lock)
{
    ValidateImpl();
    m_impl->SetLockConversation(lock);
}

void CSpxConversation::SetMuteAllParticipants(bool mute)
{
    ValidateImpl();
    m_impl->SetMuteAllParticipants(mute);
}

void CSpxConversation::SetMuteParticipant(bool mute, const std::string & participantId)
{
    ValidateImpl();
    m_impl->SetMuteParticipant(mute, participantId);
}

void CSpxConversation::SetStringValue(const char * name, const char * value)
{
    auto properties = std::dynamic_pointer_cast<ISpxNamedProperties>(m_impl);
    if (properties)
    {
        return properties->SetStringValue(name, value);
    }
}

bool CSpxConversation::HasStringValue(const char * name) const
{
    auto properties = std::dynamic_pointer_cast<ISpxNamedProperties>(m_impl);
    if (properties)
    {
        return properties->HasStringValue(name);
    }
    return false;
}

void CSpxConversation::Copy(ISpxNamedProperties* from)
{
    auto properties = std::dynamic_pointer_cast<ISpxNamedProperties>(m_impl);
    if (properties)
    {
        properties->Copy(from);
    }
}

std::string CSpxConversation::GetStringValue(const char* name, const char* defaultValue /* = "" */) const
{
    auto properties = std::dynamic_pointer_cast<ISpxNamedProperties>(m_impl);
    if (properties)
    {
        return properties->GetStringValue(name, defaultValue);
    }
    return defaultValue;
}

void CSpxConversation::ValidateImpl() const
{
    if (m_impl == nullptr)
    {
        ThrowRuntimeError("Called CSpxConversation method without initializing the impl!");
    }
}
}}}} // Microsoft::CognitiveServices::Speech::Impl

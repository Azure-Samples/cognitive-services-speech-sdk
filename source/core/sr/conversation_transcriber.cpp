//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_transcriber.cpp: implementation declarations for conversation transcriber
//

#include "stdafx.h"
#include "json.h"
#include "conversation_transcriber.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using json = nlohmann::json;

CSpxConversationTranscriber::CSpxConversationTranscriber()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    m_action = ActionType::NONE;
}

CSpxConversationTranscriber::~CSpxConversationTranscriber()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    m_participants_so_far.clear();
    m_current_participants.clear();
}

void CSpxConversationTranscriber::Init()
{
    BaseType::Init();

    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);

    m_threadService = SpxQueryService<ISpxThreadService>(GetSite());
    SPX_IFTRUE_THROW_HR(m_threadService == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    SetRecoMode();
}

void CSpxConversationTranscriber::SetRecoMode()
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

void CSpxConversationTranscriber::Term()
{
    BaseType::Term();
}

void CSpxConversationTranscriber::EndConversation()
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversationTranscriber>(this);
    std::packaged_task<void()> task([=]() {

        //todo: need a way to verify whether the conversation has started or not.
        m_action = ActionType::END_CONVERSATION;
        SendSpeehEventMessageInternal();
    });
    m_threadService->ExecuteSync(move(task));
}

void CSpxConversationTranscriber::UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant> participant)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversationTranscriber>(this);
    std::packaged_task<void()> task([this, keepAlive, userId=userId, add=add, participant=std::move(participant)]() {

        if (participant == nullptr)
        {
            ThrowInvalidArgumentException("Invalid participant pointer!");
        }
        UpdateParticipantInternal(add, userId, participant->GetPreferredLanguage(), participant->GetVoiceSignature());
    });
    m_threadService->ExecuteSync(move(task));
}

void CSpxConversationTranscriber::UpdateParticipant(bool add, const std::string& userId)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversationTranscriber>(this);
    std::packaged_task<void()> task([=]() {

        UpdateParticipantInternal(add, userId);
    });
    m_threadService->ExecuteSync(move(task));
}

void CSpxConversationTranscriber::UpdateParticipants(bool add, std::vector<ParticipantPtr>&& participants)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversationTranscriber>(this);
    std::packaged_task<void()> task([this, keepAlive, add, participants]() mutable {
        UpdateParticipantsInternal(add, move(participants));
    });
    m_threadService->ExecuteSync(move(task));

}

void CSpxConversationTranscriber::SetConversationId(const std::string& id)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversationTranscriber>(this);
    //capture conversation_id by reference and others by value.
    std::packaged_task<void()> task([keepAlive= keepAlive, &conversation_id = m_conversation_id, id=id]() {

        conversation_id = id;
    });
    m_threadService->ExecuteSync(move(task));
}

void CSpxConversationTranscriber::GetConversationId(std::string& id)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversationTranscriber>(this);
    std::packaged_task<void()> task([keepAlive=keepAlive,&conversation_id= m_conversation_id,&id=id]() mutable {
        id = conversation_id;
        SPX_DBG_TRACE_INFO("id inside task is %s", id.c_str());
    });
    m_threadService->ExecuteSync(move(task));
}

std::string  CSpxConversationTranscriber::GetSpeechEventPayload(bool atStartPumping)
{
    return CreateSpeechEventPayload(atStartPumping);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private functions
void CSpxConversationTranscriber::StartUpdateParticipants()
{
    m_current_participants.clear();
    m_action = ActionType::NONE;
}

void CSpxConversationTranscriber::DoneUpdateParticipants()
{
    SendSpeehEventMessageInternal();

    if (m_action == ActionType::ADD_PARTICIPANT)
    {
        m_participants_so_far.insert(end(m_participants_so_far), begin(m_current_participants), end(m_current_participants));
    }
    else if (m_action == ActionType::REMOVE_PARTICIPANT)
    {
        for (auto& cur : m_current_participants)
        {
            m_participants_so_far.erase(std::find_if(begin(m_participants_so_far), end(m_participants_so_far), [&cur](Participant& p) { return p.id == cur.id; }));
        }
    }
}

void CSpxConversationTranscriber::SendSpeehEventMessageInternal()
{
    auto default_session = GetDefaultSession();
    if (default_session)
    {
        if (default_session->IsStreaming())
        {
            auto payload = CreateSpeechEventPayload(false); // not starting

            try
            {
                default_session->SendSpeechEventMessage(move(payload));
            }
            catch (const std::exception& e)
            {
                SPX_TRACE_ERROR("%s", e.what());
            }
            catch (...)
            {
                SPX_TRACE_ERROR("Exception occurred in SendSpeechEventMessage");
            }
        }
    }
}

void CSpxConversationTranscriber::UpdateParticipantInternal(bool add, const std::string& id, const std::string& preferred_language, const std::string& voice_signature)
{
    StartUpdateParticipants();

    Participant person{ id, preferred_language, voice_signature };

    m_action = add ? ActionType::ADD_PARTICIPANT : ActionType::REMOVE_PARTICIPANT;

    SanityCheckParticipants(id, person);

    m_current_participants.push_back(person);

    DoneUpdateParticipants();
}

// Add a group of participants in a single speech event message.
void CSpxConversationTranscriber::UpdateParticipantsInternal(bool add, std::vector<ParticipantPtr>&& participants)
{
    if (participants.size() == 0)
    {
        ThrowInvalidArgumentException("Nothing to do in updateparticipantsInternal.");
    }

    StartUpdateParticipants();

    m_action = add ? ActionType::ADD_PARTICIPANT : ActionType::REMOVE_PARTICIPANT;

    for (auto& participant : participants)
    {
        Participant person(participant.get());

        SanityCheckParticipants(person.id, person);

        m_current_participants.emplace_back(std::move(person));
    }

    DoneUpdateParticipants();
}

void CSpxConversationTranscriber::SanityCheckParticipants(const std::string& id, const Participant& person)
{
    // Can't remove if we don't have the participant in the conversation.
    if (m_action == ActionType::REMOVE_PARTICIPANT)
    {
        auto it_prev = std::find_if(begin(m_participants_so_far), end(m_participants_so_far), [&id](const Participant& p) { return p.id == id; });
        if (it_prev == end(m_participants_so_far))
        {
            std::ostringstream os;
            os << id << " has not been added before. So, it can't be removed this time!";
            ThrowInvalidArgumentException(os.str());
        }
    }

    // if we are adding/removing it the second time, erase the first one.
    auto it = std::find_if(begin(m_current_participants), end(m_current_participants), [&person](const auto& p) { return p.id == person.id; });
    if (it != end(m_current_participants))
    {
        m_current_participants.erase(it);
    }

    // check if we exceed the max limit
    auto total_participants = m_participants_so_far.size() + m_current_participants.size();
    if (total_participants >= m_max_number_of_participants)
    {
        std::ostringstream os;
        os << "The number of participants in the conversation '" << m_conversation_id << "' is " << total_participants << ". Max allowed is " << m_max_number_of_participants;
        ThrowInvalidArgumentException(os.str());
    }
}

void to_json(json& j, const CSpxConversationTranscriber::CSpxVoiceSignature& voice)
{
    j = json{ {"Version", voice.Version}, {"Tag", voice.Tag}, {"Data", voice.Data} };
}

void to_json(json& j, const CSpxConversationTranscriber::Participant& p)
{
    j = json{ {"id", p.id}, {"preferredLanguage", p.preferred_language},  {"voice", p.voice} };
}

std::string CSpxConversationTranscriber::CreateSpeechEventPayload(bool atStartAudioPumping)
{
    if (m_conversation_id.empty())
    {
        ThrowInvalidArgumentException("conversation id is empty! Please set a conversation id before adding participants.");
    }

    json speech_event;
    speech_event["id"] = "meeting";
    speech_event["meeting"]["id"] = m_conversation_id.c_str();

    std::string name;

    if (atStartAudioPumping)
    {
        name = "start";
    }
    else if (m_action == ActionType::ADD_PARTICIPANT)
    {
        name = "join";
    }
    else if (m_action == ActionType::END_CONVERSATION)
    {
        name = "end";
    }
    else
    {
        name = "leave";
    }

    speech_event["name"] = name;
    speech_event["meeting"]["attendees"] = atStartAudioPumping? m_participants_so_far: m_current_participants;

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    auto cal_uid = properties->GetStringValue("iCalUid");
    if(!cal_uid.empty())
    {
        speech_event["iCalUid"] = cal_uid;
    }
    auto call_id = properties->GetStringValue("callId");
    if (!call_id.empty())
    {
        speech_event["callId"] = call_id;
    }
    auto organizer = properties->GetStringValue("organizer");
    if (!organizer.empty())
    {
        speech_event["organizer"] = organizer;
    }
    auto recording_on = properties->GetStringValue("audiorecording");
    speech_event["record"] = recording_on.compare("on") == 0 ? "true" : "false";

    auto flac_encoded = properties->GetStringValue("FLAC");
    if (!flac_encoded.empty())
    {
        speech_event["meeting"]["FLAC"] = 1;
    }

    return speech_event.dump();
}


}}}} // Microsoft::CognitiveServices::Speech::Impl

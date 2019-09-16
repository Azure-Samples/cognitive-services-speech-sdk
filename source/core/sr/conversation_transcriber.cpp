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

    // Initialize websocket platform
    Microsoft::CognitiveServices::Speech::USP::PlatformInit(nullptr, 0, nullptr, nullptr);

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    m_subscriptionKey = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), "");
    m_endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), "");
    m_sentEndMeeting = false;

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

void CSpxConversationTranscriber::EndConversation(bool destroy)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversationTranscriber>(this);
    std::packaged_task<void()> task([=]() {
        if (destroy)
        {
            HttpSendEndMeetingRequest();
        }
        else
        {
            SPX_DBG_TRACE_INFO("EndConversation did not trigger a HTTP request due to destroy is %s.", destroy ? "true" : "false");
        }
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

std::string  CSpxConversationTranscriber::GetSpeechEventPayload(ISpxConversationTranscriber::MeetingState state)
{
    return CreateSpeechEventPayload(state);
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
    SendSpeechEventMessageInternal();

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

void CSpxConversationTranscriber::SendSpeechEventMessageInternal()
{
    auto default_session = GetDefaultSession();
    if (default_session)
    {
        if (default_session->IsStreaming())
        {
            auto payload = CreateSpeechEventPayload(MeetingState::GOING);

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
    auto total_participants = (int)(m_participants_so_far.size() + m_current_participants.size());
    int max_allowed_participants = GetMaxAllowedParticipants();

    if (total_participants >= max_allowed_participants)
    {
        std::ostringstream os;
        os << "The number of participants in the conversation '" << m_conversation_id << "' is " << total_participants << ". Max allowed is " << max_allowed_participants;
        ThrowInvalidArgumentException(os.str());
    }
}

int CSpxConversationTranscriber::GetMaxAllowedParticipants()
{
    int max_allowed_participants = -1;

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());

    auto max_participants_set_by_client = properties->GetStringValue("Conversation-MaximumAllowedParticipants");

    if (!max_participants_set_by_client.empty())
    {
        try
        {
            max_allowed_participants = std::stoi(max_participants_set_by_client, nullptr, 10);
        }
        catch (const std::invalid_argument& ia)
        {
            UNUSED(ia);
            SPX_TRACE_WARNING("Invalid maximum number of participants set. Defaulting to %d", m_max_number_of_participants);
        }
    }
    //In case of any error in parsing max allowed participants, We are defaulting to m_max_number_of_participants which is 50
    if (max_allowed_participants <= 0)
    {
        max_allowed_participants = m_max_number_of_participants;
    }
    return max_allowed_participants;
}

void to_json(json& j, const CSpxConversationTranscriber::CSpxVoiceSignature& voice)
{
    j = json{ {"Version", voice.Version}, {"Tag", voice.Tag}, {"Data", voice.Data} };
}

void to_json(json& j, const CSpxConversationTranscriber::Participant& p)
{
    j = json{ {"id", p.id}, {"preferredLanguage", p.preferred_language},  {"voice", p.voice} };
}

std::string CSpxConversationTranscriber::CreateSpeechEventPayload(MeetingState state)
{
    if (m_conversation_id.empty())
    {
        ThrowInvalidArgumentException("conversation id is empty! Please set a conversation id before adding participants.");
    }

    json speech_event;
    speech_event["id"] = "meeting";
    speech_event["meeting"]["id"] = m_conversation_id.c_str();

    std::string name;

    switch (state)
    {
        case MeetingState::START:
            name = "start";
            m_sentEndMeeting = false;
            break;

        case MeetingState::END:
            name = "end";
            break;

        case MeetingState::GOING:
            if (m_action == ActionType::ADD_PARTICIPANT)
            {
                name = "join";
            }
            else if (m_action == ActionType::REMOVE_PARTICIPANT)
            {
                name = "leave";
            }
            else
            {
                ThrowLogicError("The participant is not joining or leaving a meeting! " + std::to_string((int)m_action));
            }
            break;

        default:
            ThrowLogicError("Unsupported Meeting state " + std::to_string((int)state));
    }

    speech_event["name"] = name;
    speech_event["meeting"]["attendees"] = state  == MeetingState::START ? m_participants_so_far: m_current_participants;

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    auto cal_uid = properties->GetStringValue("iCalUid");
    if(!cal_uid.empty())
    {
        speech_event["meeting"]["iCalUid"] = cal_uid;
    }
    auto call_id = properties->GetStringValue("callId");
    if (!call_id.empty())
    {
        speech_event["meeting"]["callId"] = call_id;
    }
    auto organizer = properties->GetStringValue("organizer");
    if (!organizer.empty())
    {
        speech_event["meeting"]["organizer"] = organizer;
    }
    auto recording_on = properties->GetStringValue("audiorecording");
    speech_event["meeting"]["record"] = recording_on.compare("on") == 0 ? "true" : "false";

    auto flac_encoded = properties->GetStringValue("FLAC");
    if (!flac_encoded.empty())
    {
        speech_event["meeting"]["FLAC"] = 1;
    }

    return speech_event.dump();
}

void CSpxConversationTranscriber::HttpAddHeaders(HttpRequest& request)
{
    // we use either subscription key or auth token.
    bool done = false;
    if (!m_subscriptionKey.empty())
    {
        request.SetRequestHeader(SUBSCRIPTION_KEY_NAME, m_subscriptionKey);
        done = true;
    }
    else
    {
        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

        auto authorizationToken = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token), "");
        SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);
        if (authorizationToken.empty())
        {
            ThrowRuntimeError("The authorization token is empty");
        }
        request.SetRequestHeader(AUTHORIZATION_TOKEN_KEY_NAME, (std::string("Bearer ") + authorizationToken).data());
        done = true;
    }

    if (!done)
    {
        ThrowRuntimeError("A valid subscription key or an authorization token is needed in a HTTP request.");
    }
}

void CSpxConversationTranscriber::HttpAddQueryParams(HttpRequest& request)
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    auto cal_uid = properties->GetStringValue(m_iCalUid);
    auto call_id = properties->GetStringValue(m_callId);

    bool hasId = false;
    // we need to add both if we have them.
    if (!cal_uid.empty())
    {
        request.AddQueryParameter(m_iCalUid, cal_uid);
        hasId = true;
    }
    if (!call_id.empty())
    {
        request.AddQueryParameter(m_callId, call_id);
        hasId = true;
    }
    // it is an error, if we have none.
    if (!hasId)
    {
        ThrowRuntimeError("iCalUid or callId must be provided in sending an end meeting request.");
    }
}

void CSpxConversationTranscriber::HttpSendEndMeetingRequest()
{
    if (m_sentEndMeeting)
    {
        std::string message = "Already sent an end meeting request to service for meeting id ";
        message += m_conversation_id;
        ThrowRuntimeError(message);
        return;
    }

    try
    {
        // the host name and path are parsed from the endpoint. The path has /meetings appended to it.
        auto url = HttpUtils::ParseUrl(m_endpoint);
        HttpRequest  request(url.host);
        auto pathStartWithForwardSlash = "/" + url.path + "/meetings";
        request.SetPath(pathStartWithForwardSlash);

        HttpAddHeaders(request);
        HttpAddQueryParams(request);
        std::unique_ptr<HttpResponse> response = request.SendRequest(HTTPAPI_REQUEST_DELETE);
        // throws HttpException in the case of errors. You can also use IsSuccess(), or GetStatusCode() if you don't want exceptions
        response->EnsureSuccess();
    }
    catch (HttpException& e)
    {
        std::string message = "Error in send end meeting request." + std::to_string(e.statusCode());
        ThrowRuntimeError(message);
    }
    catch (...)
    {
        throw;
    }
    SPX_TRACE_INFO("Sent a HTTP DELETE request to destroy the meeting resources in service.");
    m_sentEndMeeting = true;
}

}}}} // Microsoft::CognitiveServices::Speech::Impl

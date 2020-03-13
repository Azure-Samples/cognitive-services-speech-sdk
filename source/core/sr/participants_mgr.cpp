//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// participants_mgr.cpp: implementation declarations for ISpxConversation.
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
#include "service_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using json = nlohmann::json;

CSpxParticipantMgrImpl::CSpxParticipantMgrImpl(std::shared_ptr<ISpxThreadService> thread_service, std::shared_ptr<ISpxRecognizerSite> site_in)
    :m_action{ ActionType::NONE },
    m_threadService{ thread_service },
    m_site { site_in}
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    auto site = GetSite();

    auto properties = SpxQueryService<ISpxNamedProperties>(site);
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

    m_subscriptionKey = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), "");
    m_endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), "");

    if ((bool)m_threadService == false)
    {
        ThrowRuntimeError("Thread Service has not started yet!");
    }
    if ((bool)site == false)
    {
        ThrowRuntimeError("Passed an nullptr as site to ParticipantMgrImpl.");
    }
}

CSpxParticipantMgrImpl::~CSpxParticipantMgrImpl()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    m_participants_so_far.clear();
    m_current_participants.clear();
}

void CSpxParticipantMgrImpl::UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant> participant)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversation>(this);
    std::packaged_task<void()> task([this, keepAlive, userId = userId, add = add, participant = std::move(participant)]() {

        if (participant == nullptr)
        {
            ThrowInvalidArgumentException("Invalid participant pointer!");
        }
        UpdateParticipantInternal(add, userId, participant->GetPreferredLanguage(), participant->GetVoiceSignature());
    });
    m_threadService->ExecuteSync(move(task));
}

void CSpxParticipantMgrImpl::UpdateParticipant(bool add, const std::string& userId)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversation>(this);
    std::packaged_task<void()> task([this, keepAlive, add, id=userId]() {

        UpdateParticipantInternal(add, id);
    });
    m_threadService->ExecuteSync(move(task));
}

void CSpxParticipantMgrImpl::UpdateParticipants(bool add, std::vector<ParticipantPtr>&& participants)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversation>(this);
    std::packaged_task<void()> task([this, keepAlive, add, participants]() mutable {
        UpdateParticipantsInternal(add, move(participants));
    });
    m_threadService->ExecuteSync(move(task));
}

void CSpxParticipantMgrImpl::SetConversationId(const std::string& id)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversation>(this);
    //capture conversation_id by reference and others by value.
    std::packaged_task<void()> task([keepAlive = keepAlive, &conversation_id = m_conversation_id, id = id]() {

        conversation_id = id;
    });
    m_threadService->ExecuteSync(move(task));
}

const std::string CSpxParticipantMgrImpl::GetConversationId() const
{
    auto keepAlive = ISpxInterfaceBase::shared_from_this();
    std::string id;
    std::packaged_task<void()> task([keepAlive = keepAlive, this, &id]() {
        id = m_conversation_id;
        SPX_DBG_TRACE_INFO("id inside task is %s", id.c_str());
    });
    m_threadService->ExecuteSync(move(task));

    return id;
}

void CSpxParticipantMgrImpl::GetQueryParams()
{
    // calendar id never changes during a meeting
    m_calendar_uid_value = GetStringValue(m_calendar_uid_key, "");

    // call id can change during a conversation.
    m_call_id_value = GetStringValue(m_call_id_key, "");
    SPX_DBG_TRACE_INFO("Retrieved calendar id as %s and call id as %s", m_calendar_uid_value.c_str(), m_call_id_value.c_str());
}


void CSpxParticipantMgrImpl::EndConversation()
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxConversation>(this);
    std::packaged_task<void()> task([keepAlive, this]()
    {
        SPX_TRACE_INFO("Going to send a HTTP DELETE request.");
        try
        {
            GetQueryParams();
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
    });

    m_threadService->ExecuteSync(std::move(task));
}


std::string CSpxParticipantMgrImpl::GetSpeechEventPayload(ISpxConversation::MeetingState state)
{
    return CreateSpeechEventPayload(state);
}

void CognitiveServices::Speech::Impl::CSpxParticipantMgrImpl::CreateConversation(const std::string &)
{
    // nothing to do here
}

void CSpxParticipantMgrImpl::StartUpdateParticipants()
{
    m_current_participants.clear();
    m_action = ActionType::NONE;
}

void CSpxParticipantMgrImpl::DoneUpdateParticipants()
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

void CSpxParticipantMgrImpl::SendSpeechEventMessageInternal()
{
    auto site = GetSite();

    auto default_session = site->GetDefaultSession();

    if (default_session)
    {
        if (!default_session->IsStreaming())
        {
            SPX_TRACE_INFO("The speech event is not being sent due to the audio session is idle");
            return;
        }

        auto payload = CreateSpeechEventPayload(MeetingState::ONGOING);
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

void CSpxParticipantMgrImpl::UpdateParticipantInternal(bool add, const std::string& id, const std::string& preferred_language, const std::string& voice_signature)
{
    StartUpdateParticipants();

    Participant person{ id, preferred_language, voice_signature };

    m_action = add ? ActionType::ADD_PARTICIPANT : ActionType::REMOVE_PARTICIPANT;

    SanityCheckParticipants(id, person);

    m_current_participants.push_back(person);
    SPX_TRACE_INFO("Added participant id='%s'", id.c_str());

    DoneUpdateParticipants();
}

// Add a group of participants in a single speech event message.
void CSpxParticipantMgrImpl::UpdateParticipantsInternal(bool add, std::vector<ParticipantPtr>&& participants)
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

void CSpxParticipantMgrImpl::SanityCheckParticipants(const std::string& id, const Participant& person)
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

int CSpxParticipantMgrImpl::GetMaxAllowedParticipants()
{
    int max_allowed_participants = -1;

    auto max_participants_set_by_client = GetStringValue("Conversation-MaximumAllowedParticipants", "");

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

void to_json(json& j, const CSpxParticipantMgrImpl::CSpxVoiceSignature& voice)
{
    j = json{ {"Version", voice.Version}, {"Tag", voice.Tag}, {"Data", voice.Data} };
}

void to_json(json& j, const CSpxParticipantMgrImpl::Participant& p)
{
    j = json{ {"id", p.id}, {"preferredLanguage", p.preferred_language},  {"voice", p.voice} };
}

std::string CSpxParticipantMgrImpl::CreateSpeechEventPayload(MeetingState state)
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
        break;

    case MeetingState::END:
        name = "end";
        break;

    case MeetingState::ONGOING:
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
    speech_event["meeting"]["attendees"] = state == MeetingState::START ? m_participants_so_far : m_current_participants;

    auto cal_uid = GetStringValue("iCalUid", "");
    if (!cal_uid.empty())
    {
        speech_event["meeting"]["iCalUid"] = cal_uid;
    }
    auto call_id = GetStringValue("callId", "");
    if (!call_id.empty())
    {
        speech_event["meeting"]["callId"] = call_id;
    }
    auto organizer = GetStringValue("organizer", "");
    if (!organizer.empty())
    {
        speech_event["meeting"]["organizer"] = organizer;
    }
    auto recording_on = GetStringValue("audiorecording", "");
    speech_event["meeting"]["record"] = recording_on.compare("on") == 0 ? "true" : "false";

    auto mri = GetStringValue("Organizer_Mri", "");
    if (!mri.empty())
    {
        speech_event["meeting"]["Organizer_Mri"] = mri;
    }

    auto threadid = GetStringValue("Threadid", "");
    if (!threadid.empty())
    {
        speech_event["meeting"]["Threadid"] = threadid;
    }
    auto flac_encoded = GetStringValue("FLAC", "");
    if (!flac_encoded.empty())
    {
        speech_event["meeting"]["FLAC"] = 1;
    }

    return speech_event.dump();
}

void CSpxParticipantMgrImpl::HttpAddHeaders(HttpRequest& request)
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
        auto authorizationToken = GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token), "");
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

void CSpxParticipantMgrImpl::HttpAddQueryParams(HttpRequest& request)
{
    bool hasId = false;
    // we need to add both if we have them.
    if (!m_calendar_uid_value.empty())
    {
        request.AddQueryParameter(m_calendar_uid_key, m_calendar_uid_value);
        hasId = true;
    }
    if (!m_call_id_value.empty())
    {
        request.AddQueryParameter(m_call_id_key, m_call_id_value);
        hasId = true;
    }
    // it is an error, if we have none.
    if (!hasId)
    {
        ThrowRuntimeError("iCalUid or callId must be provided in sending an end meeting request.");
    }
}

std::shared_ptr<ISpxRecognizerSite> CSpxParticipantMgrImpl::GetSite() const
{
    auto site = m_site.lock();
    SPX_IFTRUE_THROW_HR(site == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);
    return site;
}

std::shared_ptr<ISpxNamedProperties> CSpxParticipantMgrImpl::GetParentProperties() const
{
    return SpxQueryInterface<ISpxNamedProperties>(GetSite());
}

}}}}

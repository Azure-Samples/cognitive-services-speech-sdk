//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// participants_mgr.h : Private implementation declarations for ISpxConversation.
//

#pragma once
#include "recognizer.h"
#include "thread_service.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class HttpResponse;
class HttpRequest;

class CSpxParticipantMgrImpl : public ISpxConversation, public ISpxPropertyBagImpl
{
public:
    struct CSpxVoiceSignature
    {
        CSpxVoiceSignature() : Version("-1"), Tag(std::string{}), Data(std::string{})
        {}

        std::string Version;
        std::string Tag;
        std::string Data;
    };

    struct Participant
    {
        Participant(const ISpxParticipant* participant)
        {
            if (participant == nullptr)
            {
                ThrowInvalidArgumentException("participant pointer is null");
            }
            id = participant->GetId();
            preferred_language = participant->GetPreferredLanguage();
            auto voice_raw_string = participant->GetVoiceSignature();
            ParseVoiceSignature(voice_raw_string);
        }

        Participant(const std::string& id, const std::string& language, const std::string& voice_raw_string)
            :id{ id }, preferred_language{ language }
        {
            ParseVoiceSignature(voice_raw_string);
        }

        void ParseVoiceSignature(const std::string& voice_raw_string)
        {
            if (!voice_raw_string.empty())
            {
                try
                {
                    auto voice_json = nlohmann::json::parse(voice_raw_string);
                    auto version = voice_json["Version"];
                    if (version.type() == nlohmann::json::value_t::number_unsigned)
                    {
                        voice.Version = std::to_string(version.get<int>());
                    }
                    else
                    {
                        voice.Version = version.get<std::string>();
                    }

                    voice.Tag = voice_json["Tag"].get<std::string>();
                    voice.Data = voice_json["Data"].get<std::string>();
                }
                catch (nlohmann::json::parse_error& e)
                {
                    std::string message = "Voice signature format is invalid, " + std::string(e.what());
                    ThrowInvalidArgumentException(message);
                }
            }
        }

        std::string id;
        std::string preferred_language;
        CSpxVoiceSignature voice;
    };

    CSpxParticipantMgrImpl(std::shared_ptr<ISpxThreadService> thread_service, std::shared_ptr<ISpxRecognizerSite> site);
    virtual ~CSpxParticipantMgrImpl();

    void UpdateParticipant(bool add, const std::string& userId) override;
    void UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant> participant) override;
    void UpdateParticipants(bool add, std::vector<ParticipantPtr>&& participants) override;
    void SetConversationId(const std::string& id) override;
    const std::string GetConversationId() const override;
    std::string GetSpeechEventPayload(MeetingState state) override;
    virtual void EndConversation() override;
    virtual void CreateConversation(const std::string &) override;

    virtual void DeleteConversation() override { ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR); }
    virtual void StartConversation() override { ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR); }
    virtual void SetLockConversation(bool) override { ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR); }
    virtual void SetMuteAllParticipants(bool) override { ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR); }
    virtual void SetMuteParticipant(bool, const std::string &) override { ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR); }

    virtual std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;

private:
    static constexpr int m_max_number_of_participants = 50;

    enum class ActionType
    {
        NONE,
        ADD_PARTICIPANT,
        REMOVE_PARTICIPANT,
    };

    std::string CreateSpeechEventPayload(MeetingState state);
    void UpdateParticipantInternal(bool add, const std::string& id, const std::string& preferred_language = {}, const std::string& voice_signature = {});
    void UpdateParticipantsInternal(bool add, std::vector<ParticipantPtr>&& participants);
    void SanityCheckParticipants(const std::string& id, const Participant& person);
    void SendSpeechEventMessageInternal();

    void StartUpdateParticipants();
    void DoneUpdateParticipants();

    int GetMaxAllowedParticipants();

    void HttpAddQueryParams(HttpRequest& request);
    void HttpAddHeaders(HttpRequest& request);
    void GetQueryParams();

    std::shared_ptr<ISpxRecognizerSite> GetSite() const;

private:

    std::vector<Participant> m_current_participants;
    std::vector<Participant> m_participants_so_far;
    std::string m_conversation_id;
    ActionType m_action;

    std::shared_ptr<ISpxThreadService>  m_threadService;
    std::weak_ptr<ISpxRecognizerSite> m_site;

    std::string m_subscriptionKey;
    std::string m_endpoint;

    static auto constexpr m_calendar_uid_key = "iCalUid";
    static auto constexpr m_call_id_key = "callId";
    std::string m_calendar_uid_value;
    std::string m_call_id_value;

    DISABLE_COPY_AND_MOVE(CSpxParticipantMgrImpl);
};
}}}}

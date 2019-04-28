//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_transcriber.h: Private implementation declarations for conversation transcriber.
//

#pragma once
#include "recognizer.h"
#include "thread_service.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConversationTranscriber :
    public CSpxRecognizer,
    public ISpxConversationTranscriber
{
public:
    using BaseType = CSpxRecognizer;

    struct CSpxVoiceSignature
    {
        int Version;
        std::string Tag;
        std::string Data;
    };

    struct Participant
    {
        Participant(const ISpxParticipant* participant)
        {
            if (participant == nullptr)
            {
                ThrowInvalidArgumentException("participant point is null");
            }
            id = participant->GetId();
            preferred_language = participant->GetPreferredLanguage();
            auto voice_raw_string = participant->GetVoiceSignature();
            auto voice_json = nlohmann::json::parse(voice_raw_string);

            voice.Version = voice_json["Version"].get<int>();
            voice.Tag = voice_json["Tag"].get<std::string>();
            voice.Data = voice_json["Data"].get<std::string>();
        }

        Participant(const std::string& id, const std::string& language, const std::string& voice_raw_string)
            :id{ id }, preferred_language{ language }
        {
            auto voice_json = nlohmann::json::parse(voice_raw_string);
            voice.Version = voice_json["Version"].get<int>();
            voice.Tag = voice_json["Tag"].get<std::string>();
            voice.Data = voice_json["Data"].get<std::string>();
        }

        std::string id;
        std::string preferred_language;
        CSpxVoiceSignature voice;
    };

    CSpxConversationTranscriber();
    virtual ~CSpxConversationTranscriber();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammarList)
        SPX_INTERFACE_MAP_ENTRY(ISpxConversationTranscriber)
    SPX_INTERFACE_MAP_END()

      // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxConversationTranscriber
    void UpdateParticipant(bool add, const std::string& userId) override;
    void UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant> participant) override;
    void UpdateParticipants(bool add, std::vector<ParticipantPtr>&& participants) override;
    void SetConversationId(const std::string& id) override;
    void GetConversationId(std::string& id) override;
    void EndConversation() override;
    std::string GetSpeechEventPayload(bool atStartAudioPumping) override;

private:
    static constexpr int m_max_number_of_participants = 50;

    enum class ActionType
    {
        NONE,
        ADD_PARTICIPANT,
        REMOVE_PARTICIPANT,
        END_CONVERSATION
    };

    std::string CreateSpeechEventPayload(bool rightBeforeAudioPumping);
    void UpdateParticipantInternal(bool add, const std::string& id, const std::string& preferred_language = {}, const std::string& voice_signature = {});
    void UpdateParticipantsInternal(bool add, std::vector<ParticipantPtr>&& participants);
    void SanityCheckParticipants(const std::string& id, const Participant& person);
    void SendSpeehEventMessageInternal();

    void StartUpdateParticipants();
    void DoneUpdateParticipants();

    void SetRecoMode();

private:

    std::vector<Participant> m_current_participants;
    std::vector<Participant> m_participants_so_far;
    std::string m_conversation_id;
    ActionType m_action;

    std::shared_ptr<ISpxThreadService>  m_threadService;

    DISABLE_COPY_AND_MOVE(CSpxConversationTranscriber);
};

} } } } // Microsoft::CognitiveServices::Speech::Impl

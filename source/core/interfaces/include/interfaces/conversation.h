//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>
#include <memory>

#include <interfaces/base.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxParticipant : public ISpxInterfaceBaseFor<ISpxParticipant>
{
public:
    virtual void SetPreferredLanguage(std::string&& preferredLanguage) = 0;
    virtual void SetVoiceSignature(std::string&& voiceSignature) = 0;

    virtual std::string GetPreferredLanguage() const = 0;
    virtual std::string GetVoiceSignature() const = 0;
    virtual std::string GetId() const = 0;
};

using ParticipantPtr = std::shared_ptr<ISpxParticipant>;

class ISpxConversation : public ISpxInterfaceBaseFor<ISpxConversation>
{
public:

    enum class MeetingState
    {
        START,
        ONGOING,
        END
    };

    virtual void UpdateParticipant(bool add, const std::string& userId) = 0;
    virtual void UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant> participant) = 0;
    virtual void UpdateParticipants(bool add, std::vector<ParticipantPtr>&& participants) = 0;
    virtual void SetConversationId(const std::string& id) = 0;
    virtual const std::string GetConversationId() const = 0;
    virtual std::string GetSpeechEventPayload(MeetingState state) = 0;
    virtual void EndConversation() = 0;

    virtual void CreateConversation(const std::string& nickname = "") = 0;
    virtual void DeleteConversation() = 0;
    virtual void StartConversation() = 0;
    virtual void SetLockConversation(bool locked) = 0;
    virtual void SetMuteAllParticipants(bool mute) = 0;
    virtual void SetMuteParticipant(bool mute, const std::string& participantId) = 0;
};

}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

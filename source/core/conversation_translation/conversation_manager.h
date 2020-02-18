//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_manager.h: Implementation declarations for CSpxConversationManager C++ class
//

#pragma once

#include <http_endpoint_info.h>
#include "interface_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    enum class TextToSpeechFormat
    {
        Wav,
        Siren
    };

    struct CreateConversationArgs
    {
        std::string Nickname;
        std::string LanguageCode;
        std::string TtsVoiceCode;
        TextToSpeechFormat TtsFormat = TextToSpeechFormat::Wav;
        std::string CorrelationId;
        std::vector<std::string> TranslateTo;
        std::string CognitiveSpeechSubscriptionKey;
        std::string CognitiveSpeechRegion;
        std::string CognitiveSpeechAuthenticationToken;
        std::string ClientAppId;
    };

    struct ConversationArgs
    {
        std::string Avatar;
        std::string CognitiveSpeechAuthToken;
        std::string CognitiveSpeechRegion;
        std::string CorrelationId;
        bool IsApproved;
        bool IsMuted;
        std::string RoomCode;
        std::string SessionToken;

        std::string Nickname;
        std::string ParticipantId;
        std::string RequestId;
    };

    class ConversationManager : public std::enable_shared_from_this<ConversationManager>
    {
    public:
        static std::shared_ptr<ConversationManager> Create(const HttpEndpointInfo& endpointInfo)
        {
            return std::shared_ptr<ConversationManager>(new ConversationManager(endpointInfo));
        }

        virtual ~ConversationManager();

        ConversationArgs CreateOrJoin(const CreateConversationArgs& args, const std::string& roomCode, const std::string& roomPin = "");
        void Leave(const std::string& sessionToken);

    private:
        ConversationManager(const HttpEndpointInfo& endpointInfo);

    private:
        const HttpEndpointInfo m_endpointInfo;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

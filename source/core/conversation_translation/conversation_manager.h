//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_manager.h: Implementation declarations for CSpxConversationManager C++ class
//

#pragma once

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

    class CSpxConversationManager : public ISpxObjectInit
    {
    public:
        CSpxConversationManager(const std::string& host);
        virtual ~CSpxConversationManager();

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_END()

        // --- ISpxObjectInit
        void Init() override;
        void Term() override;

        void SetPort(int port);
        void SetSecure(bool secure);
        void SetPath(const std::string& path);
        void SetAutomaticProxy(bool autoProxy);
        void SetProxy(const std::string& proxy, const int port, const std::string& username = "", const std::string& password = "");

        ConversationArgs CreateOrJoin(const CreateConversationArgs& args, const std::string& roomCode, const std::string& roomPin = "");
        void Leave(const std::string& sessionToken);

    private:
        const std::string m_host;
        int m_port;
        bool m_secure;
        std::string m_path;
        bool m_autoProxy;
        std::string m_proxyHost;
        int m_proxyPort;
        std::string m_proxyUsername;
        std::string m_proxyPassword;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

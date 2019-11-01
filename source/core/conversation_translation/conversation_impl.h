//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <ispxinterfaces.h>
#include <property_bag_impl.h>
#include <interface_helpers.h>
#include <service_helpers.h>
#include "ISpxConversationInterfaces.h"
#include "conversation_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    struct ConversationArgs;
    class ConversationConnection;
    class CSpxConversationManager;

    class CSpxConversationImpl :
        public ISpxConversation,
        public ISpxServiceProvider,
        public ISpxPropertyBagImpl,
        public ISpxConversationInternals,
        public ThreadingHelpers
    {
    public:
        CSpxConversationImpl();
        virtual ~CSpxConversationImpl();

        // --- ISpxObjectInit
        virtual void Init() override;
        virtual void Term() override;

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
            SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
            SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
            SPX_INTERFACE_MAP_ENTRY(ISpxConversation)
            SPX_INTERFACE_MAP_ENTRY(ISpxConversationInternals)
        SPX_INTERFACE_MAP_END()

        SPX_SERVICE_MAP_BEGIN()
            SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
            SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
        SPX_SERVICE_MAP_END()

        // Inherited via ISpxConversation
        virtual const std::string GetConversationId() const override;
        virtual void SetConversationId(const std::string& conversationId) override;
        virtual void CreateConversation(const std::string& nickname) override;
        virtual void StartConversation() override;
        virtual void EndConversation() override;
        virtual void DeleteConversation() override;
        virtual void SetLockConversation(bool lock) override;
        virtual void SetMuteAllParticipants(bool mute) override;
        virtual void SetMuteParticipant(bool mute, const std::string& participantId) override;

        virtual void UpdateParticipant(bool add, const std::string &userId) override;
        virtual void UpdateParticipant(bool, const std::string&, std::shared_ptr<ISpxParticipant>) override;

        virtual void UpdateParticipants(bool, std::vector<ParticipantPtr>&&) override { Impl::ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR); }
        virtual std::string GetSpeechEventPayload(MeetingState) override { Impl::ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR); return ""; }

        // Inherited via ISpxConversationInternals
        virtual bool IsConnected() const override;
        virtual std::shared_ptr<ConversationArgs> GetConversationArgs() const override;
        virtual std::shared_ptr<CSpxConversationManager> GetConversationManager() const override;
        virtual std::shared_ptr<ConversationConnection> GetConversationConnection() const override;

    private:
        void EndConversationInternal();
        void DeleteConversationInternal();

        template<class... TArgs>
        inline void SendSynchronously(void (ConversationConnection::* func)(TArgs...), TArgs&&... args)
        {
            RunSynchronously([this, func, &args...]()
            {
                SPX_IFFALSE_THROW_HR(IsConnected(), SPXERR_INVALID_STATE);
                (m_connection.get()->*func)(std::forward<TArgs>(args)...);
            });
        }

    private:
        std::string m_conversationId;
        std::shared_ptr<ConversationArgs> m_args;
        std::shared_ptr<CSpxConversationManager> m_manager;
        std::unique_ptr<ConversationClient> m_client;
        std::shared_ptr<ConversationConnection> m_connection;

        /// <summary>
        /// A flag indicating whether or not this is a permanent room. This is for future
        /// use.
        /// </summary>
        bool m_permanentRoom;
    };


}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

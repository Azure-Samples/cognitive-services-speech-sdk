//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_translator.h: Private implementation declarations for conversation translator.
//

#pragma once

#include <atomic>
#include <exception.h>
#include <property_id_2_name_map.h>
#include "conversation_connection.h"
#include "conversation_manager.h"
#include "conversation_utils.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    /// <summary>
    /// A conversation translator that uses the conversation service. This enables a connected experience
    /// where participants can use their own devices to see everyone else's recognitions and IMs in their
    /// own languages, as well as speak and send IMs to others.
    ///
    /// Beneath the hood, this uses a translation speech recognizer for doing speech recognitions for the
    /// local participant. We need to use the translator recognizer as this enables the backend
    /// communication between the speech recognition service and the conversation service. It will also
    /// maintain a separate connection to the conversation service as well. This is used to receive events
    /// for all other participants in the room.
    /// </summary>
    class CSpxConversationTranslator :
        public ISpxServiceProvider,
        public ISpxNamedProperties,
        public ISpxSessionFromRecognizer,
        public ISpxGenericSite,
        public ISpxConnectionFromRecognizer,
        public ISpxConversationTranslator,
        public ISpxObjectWithAudioConfig,
        public ThreadingHelpers,
        public ConversationCallbacks
    {
    public:
        /// <summary>
        /// Possible states for the conversation translator
        /// </summary>
        enum class ConversationState
        {
            /// <summary>
            /// A terminal error was encountered that caused a disconnection
            /// </summary>
            Failed = -1,
            /// <summary>
            /// The initial state
            /// </summary>
            Initial = 0,
            /// <summary>
            /// The connections are now closed. This is the final state
            /// </summary>
            Closed,
            /// <summary>
            /// In the process of closing
            /// </summary>
            Closing,
            /// <summary>
            /// If you are host, the conversation is currently being created. If you are the participant,
            /// you are in the process of registering with the conversation service to join an existing
            /// conversation
            /// </summary>
            CreatingOrJoining,
            /// <summary>
            /// The host has successfully created the conversation, or the participant has successfully
            /// registered to join an existing conversation. At this point, we do not yet have an open
            /// web socket connection to the conversation service nor the speech service.
            CreatedOrJoined,
            /// <summary>
            /// In the process of opening a web socket connection to the conversation service
            /// </summary>
            Opening,
            /// <summary>
            /// You are currently connected to the conversation service
            /// </summary>
            Open,
        };

        /// <summary>
        /// Creates a new instance
        /// </summary>
        CSpxConversationTranslator();

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~CSpxConversationTranslator();

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
            SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
            SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
            SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
            SPX_INTERFACE_MAP_ENTRY(ISpxConnectionFromRecognizer)
            SPX_INTERFACE_MAP_ENTRY(ISpxConversationTranslator)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithAudioConfig)
        SPX_INTERFACE_MAP_END()

        // --- ISpxObjectInit
        void Init() override;
        void Term() override;

        // --- ISpxConversationTranslator
        const std::vector<std::shared_ptr<ISpxConversationParticipant>> GetParticipants() override;
        std::string GetAuthorizationToken() const override;
        void SetAuthorizationToken(const std::string& authToken, const std::string& region = "") override;

        void JoinConversation(std::shared_ptr<ISpxConversation> conversation, const std::string& nickname, bool isHost) override;
        void Connect() override;
        void StartTranscribing() override;
        void StopTranscribing() override;
        void SendTextMsg(const std::string& message) override;
        void Disconnect() override;
        void LeaveConversation() override;
        bool CanJoin() const override;

        // --- IServiceProvider
        SPX_SERVICE_MAP_BEGIN()
            SPX_SERVICE_MAP_ENTRY_FUNC(QueryServiceInternal)
            SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
        SPX_SERVICE_MAP_END()

        // -- ISpxNamedProperties
        std::string GetStringValue(const char* name, const char* defaultValue = "") const override;
        void SetStringValue(const char* name, const char* value) override;
        bool HasStringValue(const char* name) const override;
        void Copy(ISpxNamedProperties* from) override;

        // --- ISpxSessionFromRecognizer
        std::shared_ptr<ISpxSession> GetDefaultSession() override;

        // --- ISpxConnectionFromRecognizer
        std::shared_ptr<ISpxConnection> GetConnection() override;

        // -- ISpxObjectWithAudioConfig
        virtual void SetAudioConfig(std::weak_ptr<ISpxAudioConfig> audio_config) override;
        virtual std::shared_ptr<ISpxAudioConfig> GetAudioConfig() override;

        // ConversationCallbacks
        virtual void OnConnected() override;
        virtual void OnDisconnected(const USP::WebSocketDisconnectReason reason, const string& message, bool serverRequested) override;
        virtual void OnSpeechRecognition(const ConversationSpeechRecognitionMessage& reco) override;
        virtual void OnInstantMessage(const ConversationTranslatedMessage& im) override;
        virtual void OnParticipantChanged(const ConversationParticipantAction action, const std::vector<ConversationParticipant>& participants) override;
        virtual void OnRoomExpirationWarning(const int32_t minutesLeft) override;
        virtual void OnError(const std::shared_ptr<ISpxErrorInformation>& error) override;
        virtual void OnUpdatedAuthorizationToken(
            const std::string& authToken, const std::string& region, const std::chrono::system_clock::time_point& expiresAt) override;

    protected:
        ConversationState SetState(const ConversationState next)
        {
            return m_state_.exchange(next);
        }

        ConversationState GetState() const { return m_state_.load(); }

        struct EventsToSend;

        void SendStateEvents(const EventsToSend& evt);
        EventsToSend GetStateExitEvents();

        void ToFailedState(bool isRecognizer, const std::shared_ptr<ISpxErrorInformation>& error);
        void ToClosedState();
        void ToClosingState();
        void ToCreatingOrJoiningState();
        void ToCreatedOrJoinedState(const std::shared_ptr<ISpxErrorInformation>& error);
        void ToOpeningState();
        void ToOpenState();

        std::shared_ptr<ISpxInterfaceBase> QueryServiceInternal(const char * serviceName)
        {
            if (PAL::stricmp(SpxTypeName(ISpxNamedProperties), serviceName) == 0
                && m_recognizer != nullptr)
            {
                return m_recognizer->QueryInterface<ISpxNamedProperties>();
            }

            return nullptr;
        }

    private:
        DISABLE_COPY_AND_MOVE(CSpxConversationTranslator);

        void ConnectConversation(std::shared_ptr<ISpxConversation> conv, bool owned);
        bool StopConversation();
        void DisconnectConversation(bool waitForEvents);

        void ConnectRecognizer(std::shared_ptr<ISpxRecognizer> recognizer);
        bool StopRecognizer();
        void DisconnectRecognizer(bool waitForEvents);

        static inline bool IsConsideredOpen(ConversationState state);
        inline const std::string GetSessionId() const;
        inline const std::string GetParticipantId() const;
        inline bool IsConversationConnected() const;
        inline bool IsMultiChannelAudio() const;

        void OnConversationDeleted();

        void OnRecognizerSessionStarted(shared_ptr<ISpxSessionEventArgs>);
        void OnRecognizerSessionStopped(shared_ptr<ISpxSessionEventArgs>);
        void OnRecognizerConnected(shared_ptr<ISpxConnectionEventArgs>);
        void OnRecognizerDisconnected(shared_ptr<ISpxConnectionEventArgs>);
        void OnRecognizerResult(shared_ptr<ISpxRecognitionEventArgs>);
        void OnRecognizerCanceled(shared_ptr<ISpxRecognitionEventArgs>);

        void LogTranslationError(shared_ptr<ISpxRecognitionResult> recoResult);

        void SetAuthorizationTokenInternal(const std::string& authToken, const std::string& region);

        template<typename I>
        inline static shared_ptr<I> SafeQueryInterface(std::shared_ptr<ISpxInterfaceBase> ptr)
        {
            return ptr == nullptr
                ? shared_ptr<I>()
                : ptr->QueryInterface<I>();
        }

        inline std::string GetStringValue(PropertyId id)
        {
            return GetStringValue(GetPropertyName(id));
        }

        inline void SetStringValue(PropertyId id, const char * str)
        {
            SetStringValue(GetPropertyName(id), str);
        }

    private:
        std::atomic<ConversationState> m_state_;
        std::shared_ptr<ISpxConversation> _m_conv;
        std::weak_ptr<ISpxConversationInternals> m_convInternals;
        bool m_isHost;
        std::shared_ptr<ISpxRecognizer> m_recognizer;
        std::atomic_bool m_recognizerConnected;
        std::string m_speechLang;
        std::weak_ptr<ISpxAudioConfig> m_audioInput;
        std::string m_utteranceId;
        size_t m_convDeletedEvtHandlerId;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

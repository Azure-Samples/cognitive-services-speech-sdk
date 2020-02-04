//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_translator.h: Private implementation declarations for conversation translator.
//

#pragma once

#include <atomic>
#include <exception.h>
#include "ISpxConversationInterfaces.h"
#include "conversation_connection.h"
#include "conversation_manager.h"
#include "recognizer.h"
#include "conversation_utils.h"

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
            /// Not currently connected
            /// </summary>
            Closed = 0,
            /// <summary>
            /// In the process of disconnecting
            /// </summary>
            Closing,
            /// <summary>
            /// In the process of connecting
            /// </summary>
            Opening,
            /// <summary>
            /// One of the two underlying connections is not open yet
            /// </summary>
            PartiallyOpen,
            /// <summary>
            /// The connections to the service are open
            /// </summary>
            Open
        };

        /// <summary>
        /// Creates a new instance
        /// </summary>
        CSpxConversationTranslator();

        /// <summary>
        /// Destructor
        /// </summary>
        ~CSpxConversationTranslator();

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
            SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
            SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
            SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSiteInitImpl<ISpxRecognizerSite>)
            SPX_INTERFACE_MAP_ENTRY(ISpxConnectionFromRecognizer)
            SPX_INTERFACE_MAP_ENTRY(ISpxConversationTranslator)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithAudioConfig)
        SPX_INTERFACE_MAP_END()

        // --- ISpxObjectInit
        void Init() override;
        void Term() override;

        // --- ISpxConversationTranslator
        const std::vector<std::shared_ptr<ISpxConversationParticipant>> GetParticipants() override;
        void JoinConversation(std::shared_ptr<ISpxConversation> conversation, const std::string& nickname, bool endConversationOnLeave) override;
        void StartTranscribing() override;
        void StopTranscribing() override;
        void SendTextMsg(const std::string& message) override;
        void LeaveConversation() override;

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
        virtual void OnDisconnected(const USP::WebSocketDisconnectReason reason, const string& message) override;
        virtual void OnSpeechRecognition(const ConversationSpeechRecognitionMessage& reco) override;
        virtual void OnInstantMessage(const ConversationTranslatedMessage& im) override;
        virtual void OnParticipantChanged(const ConversationParticipantAction action, const std::vector<ConversationParticipant>& participants) override;
        virtual void OnRoomExpirationWarning(const int32_t minutesLeft) override;
        virtual void OnError(const bool isWebSocket, const ConversationErrorCode error, const std::string& message) override;

    protected:
        void SetState(const ConversationState next)
        {
            m_state_ = next;
        }

        void ChangeState(const ConversationState from, const ConversationState next)
        {
            ConversationState previous = m_state_.exchange(next);
            SPX_DBG_ASSERT_WITH_MESSAGE(
                previous == from,
                "Unexpected state transition for ConversationTranslator %p, From: %d, To: %d, Actual: %d",
                (void *)this, from, next, previous);

            UNUSED(from); // unused in release builds
            UNUSED(previous);
        }

        ConversationState GetState() const { return m_state_; }

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
        /** \brief Disable copy constructor */
        CSpxConversationTranslator(const CSpxConversationTranslator&) = delete;
        /** \brief Disable copy assignment */
        CSpxConversationTranslator& operator=(const CSpxConversationTranslator&) = delete;
        /** \brief Disable move constructor */
        CSpxConversationTranslator(CSpxConversationTranslator&&) = delete;
        /** \brief Disable move assignment */
        CSpxConversationTranslator& operator=(CSpxConversationTranslator&&) = delete;

        void ConnectConversation(std::shared_ptr<ISpxConversation> conv, bool owned);
        void DisconnectConversation();

        void ConnectRecognizer(std::shared_ptr<ISpxRecognizer> recognizer);
        void DisconnectTranslationRecognizer();

        inline bool IsConsideredOpen() const;
        inline const std::string GetSessionId() const;
        inline const std::string GetParticipantId() const;
        inline bool IsConversationConnected() const;
        inline bool IsMultiChannelAudio() const;

        void OnRecognizerSessionStarted(shared_ptr<ISpxSessionEventArgs>);
        void OnRecognizerSessionStopped(shared_ptr<ISpxSessionEventArgs>);
        void OnRecognizerConnected(shared_ptr<ISpxConnectionEventArgs>);
        void OnRecognizerDisconnected(shared_ptr<ISpxConnectionEventArgs>);
        void OnRecognizerResult(shared_ptr<ISpxRecognitionEventArgs>);
        void OnRecognizerCanceled(shared_ptr<ISpxRecognitionEventArgs>);

        void ToFailedState(bool isRecognizer, CancellationErrorCode error, const std::string& message);

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
        bool m_endConversationOnLeave;
        std::shared_ptr<ISpxRecognizer> m_recognizer;
        std::atomic_bool m_recognizerConnected;
        std::string m_speechLang;
        std::weak_ptr<ISpxAudioConfig> m_audioInput;
        std::string m_utteranceId;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

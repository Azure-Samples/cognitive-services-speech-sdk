//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <ispxinterfaces.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    /// <summary>
    /// Interface for a conversation participant
    /// </summary>
    class ISpxConversationParticipant :
        public virtual ISpxParticipant,
        public virtual ISpxInterfaceBaseFor<ISpxConversationParticipant>
    {
    public:
        using HandleType = SPXPARTICIPANTHANDLE;
        using HandleInterfaceType = ISpxParticipant;

        /// <summary>
        /// Gets the colour of the user's avatar as an HTML hex string (e.g. FF0000 for red)
        /// </summary>
        virtual std::string GetAvatar() const = 0;

        /// <summary>
        /// The participant's display name. Please note that there may be more than one participant
        /// with the same name. You can use the GetId() method to tell them apart
        /// </summary>
        virtual std::string GetDisplayName() const = 0;

        /// <summary>
        /// Gets whether or not the participant is using Text To Speech (TTS)
        /// </summary>
        virtual bool IsUsingTts() const = 0;

        /// <summary>
        /// Gets whether or not this participant is muted
        /// </summary>
        virtual bool IsMuted() const = 0;

        /// <summary>
        /// Gets whether or not this participant is the host
        /// </summary>
        virtual bool IsHost() const = 0;
    };

    /// <summary>
    /// Base interface for conversation translator transcriptions
    /// </summary>
    class ISpxConversationTranslationResult :
        public virtual ISpxRecognitionResult,
        public virtual ISpxTranslationRecognitionResult,
        public virtual ISpxConversationTranscriptionResult,
        public virtual ISpxInterfaceBaseFor<ISpxConversationTranslationResult>
    {
    public:
        using HandleType = SPXRESULTHANDLE;
        using HandleInterfaceType = ISpxRecognitionResult;
        
        std::shared_ptr<ISpxAudioDataStream> GetAudioDataStream() override { return nullptr;  }

        /// <summary>
        /// Gets the language that the original recognition or text message is in
        /// </summary>
        virtual std::wstring GetOriginalLanguage() const = 0;
    };

    /// <summary>
    /// Event arguments that indicate the start or end of an event session
    /// </summary>
    class ISpxConversationSessionEventArgs :
        public virtual ISpxSessionEventArgs,
        public virtual ISpxInterfaceBaseFor<ISpxConversationSessionEventArgs>
    {
    public:
        using HandleType = SPXEVENTHANDLE;
        using HandleInterfaceType = ISpxSessionEventArgs;
    };

    /// <summary>
    /// Event arguments that indicate when the session connects or disconnects
    /// </summary>
    class ISpxConversationConnectionEventArgs :
        public virtual ISpxConnectionEventArgs,
        public virtual ISpxInterfaceBaseFor<ISpxConversationConnectionEventArgs>
    {
    public:
        using HandleType = SPXEVENTHANDLE;
        using HandleInterfaceType = ISpxConnectionEventArgs;
    };

    /// <summary>
    /// Event arguments that indicate how many minutes are left until the conversation expires
    /// </summary>
    class ISpxConversationExpirationEventArgs :
        public virtual ISpxConversationEventArgs,
        public virtual ISpxInterfaceBaseFor<ISpxConversationExpirationEventArgs>
    {
    public:
        using HandleType = SPXEVENTHANDLE;
        using HandleInterfaceType = ISpxConversationEventArgs;

        /// <summary>
        /// Gets how many minutes are left
        /// </summary>
        virtual int32_t GetMinutesLeft() const = 0;
    };

    /// <summary>
    /// Event arguments for the participant changed event
    /// </summary>
    class ISpxConversationParticipantChangedEventArgs :
        public virtual ISpxConversationEventArgs,
        public virtual ISpxInterfaceBaseFor<ISpxConversationParticipantChangedEventArgs>
    {
    public:
        using HandleType = SPXEVENTHANDLE;
        using HandleInterfaceType = ISpxConversationEventArgs;

        /// <summary>
        /// Gets why the participant changed event was raised
        /// </summary>
        virtual Transcription::ParticipantChangedReason GetReason() const = 0;

        /// <summary>
        /// Gets the participants that are affected
        /// </summary>
        virtual const std::vector<std::shared_ptr<ISpxConversationParticipant>>& GetParticipants() const = 0;
    };

    /// <summary>
    /// Event arguments for the transcription events
    /// </summary>
    class ISpxConversationTranslationEventArgs :
        public virtual ISpxRecognitionEventArgs,
        public virtual ISpxInterfaceBaseFor<ISpxConversationTranslationEventArgs>
    {
    public:
        using HandleType = SPXEVENTHANDLE;
        using HandleInterfaceType = ISpxRecognitionEventArgs;

        /// <summary>
        /// Gets the conversation translation result
        /// </summary>
        virtual std::shared_ptr<ISpxConversationTranslationResult> GetConversationTranslationResult() const = 0;
    };

    /// <summary>
    /// Interface for a conversation translator enables a connected experience where participants
    /// can use their own devices to see everyone else's recognitions, and IMs in their own
    /// languages, as well as speak and send IMs to others.
    /// </summary>
    class ISpxConversationTranslator : public ISpxInterfaceBaseFor<ISpxConversationTranslator>
    {
    public:
        using HandleType = SPXCONVERSATIONTRANSLATORHANDLE;
        using HandleInterfaceType = ISpxConversationTranslator;

        using SessionEvent_Type = EventSignal<std::shared_ptr<ISpxConversationSessionEventArgs>>;
        using ConnectionEvent_Type = EventSignal<std::shared_ptr<ISpxConversationConnectionEventArgs>>;
        using ConversationExpiration_Type = EventSignal<std::shared_ptr<ISpxConversationExpirationEventArgs>>;
        using ParticipantsChanged_Type = EventSignal<std::shared_ptr<ISpxConversationParticipantChangedEventArgs>>;
        using Transcription_Type = EventSignal<std::shared_ptr<ISpxConversationTranslationEventArgs>>;

        /// <summary>
        /// Raised when the session has started and you can start sending audio if needed
        /// </summary>
        SessionEvent_Type SessionStarted;

        /// <summary>
        /// Raised when the session has stopped
        /// </summary>
        SessionEvent_Type SessionStopped;

        /// <summary>
        /// Raised whenever a connection to the service happens. Due to retry and internal logic,
        /// the underlying connection may disconnect and reconnect more than once during a single
        /// session
        /// </summary>
        ConnectionEvent_Type Connected;

        /// <summary>
        /// Raised when the a disconnection from the service happens. Due to retry and internal
        /// logic, the underlying connection may disconnect and reconnect more than once during
        /// a single session. In the case where it is no longer possible to reconnect, a Canceled
        /// event detailing the error, followed by a SessionStopped event will be raised
        /// </summary>
        ConnectionEvent_Type Disconnected;

        /// <summary>
        /// Raised in two cases:
        /// <list type="bullet">
        /// <item>
        ///     <term>Errors</term>
        ///     <description>An error occurred that could not be recovered from.</description>
        /// </item>
        /// <item>
        ///     <term>End of file input</term>
        ///     <description>
        ///         In the case you are sending audio from a file, this is raised
        ///         when you reach the end and there is no more audio to send
        ///     </description>
        /// </item>
        /// </list>
        /// Raised when an error is encountered (e.g. network is no longer available).
        /// </summary>
        Transcription_Type Canceled;

        /// <summary>
        /// Raised when the participants in the conversation have changed (e.g. a participant
        /// updated their nickname, or a participant left)
        /// </summary>
        ParticipantsChanged_Type ParticipantsChanged;

        /// <summary>
        /// Raised when the conversation service sends a notice detailing how many minutes are
        /// left before the conversation is closed. All conversations have an upper limit on
        /// their duration
        /// </summary>
        ConversationExpiration_Type ConversationExpiration;

        /// <summary>
        /// Raised whenever there is a partial recognition of an utterance
        /// </summary>
        Transcription_Type Transcribing;

        /// <summary>
        /// Raised when there is a final recognition for an utterance
        /// </summary>
        Transcription_Type Transcribed;

        /// <summary>
        /// Raised when a text message is received from the conversation
        /// </summary>
        Transcription_Type TextMessageReceived;

        /// <summary>
        /// Gets the current participants in the conversation
        /// </summary>
        virtual const std::vector<std::shared_ptr<ISpxConversationParticipant>> GetParticipants() = 0;

        /// <summary>
        /// Joins a conversation
        /// </summary>
        /// <param name="conversation">The conversation instance to use. This instance can be used by the
        /// host to manage the conversation</param>
        /// <param name="nickname">The display name to use for the current participant in the conversation</param>
        /// <param name="endConversationOnLeave">Flag indicating whether or not we should call conversation->EndConversation()
        /// method on the passed in conversation instance when user's call the LeaveConversationAsync() method</param>
        /// <returns>Asynchronous operation</returns>
        virtual void JoinConversation(std::shared_ptr<ISpxConversation> conversation, const std::string& nickname, bool endConversationOnLeave) = 0;

        /// <summary>
        /// Connects and/or reconnects to the conversation service
        /// </summary>
        virtual void Connect() = 0;

        /// <summary>
        /// Starts sending audio to the conversation service for speech recognition
        /// </summary>
        /// <returns>Asynchronous operation</returns>
        virtual void StartTranscribing() = 0;

        /// <summary>
        /// Stops sending audio to the conversation service
        /// </summary>
        /// <returns>Asynchronous operation</returns>
        virtual void StopTranscribing() = 0;

        /// <summary>
        /// Sends an instant message to all participants in the conversation. This instant message
        /// will be translated into each participant's text language.
        /// </summary>
        /// <param name="message">The instant message to send</param>
        /// <returns>Asynchronous operation</returns>
        virtual void SendTextMsg(const std::string& message) = 0;

        /// <summary>
        /// Disconnects from the conversation service. After this you can rejoin within a limited time window
        /// using the Connect() method
        /// </summary>
        virtual void Disconnect() = 0;

        /// <summary>
        /// Leaves the current conversation. After this is called, you will no longer receive any
        /// events
        /// </summary>
        /// <returns>Asynchronous operation</returns>
        virtual void LeaveConversation() = 0;

        /// <summary>
        /// Determines whether or not we can join using this instance. This will be true until you
        /// call one of the Join methods a first time
        /// </summary>
        /// <returns>True if you can call join, false otherwise</returns>
        virtual bool CanJoin() const = 0;
    };

    /// <summary>
    /// Interface used to initialize a conversation translator connection instance
    /// </summary>
    class ISpxConversationTranslatorConnection : public ISpxInterfaceBaseFor<ISpxConversationTranslatorConnection>
    {
    public:
        /// <summary>
        /// Initialises the conversation translator connection
        /// </summary>
        /// <param name="convTrans">The conversation translator instance this connection is for</param>
        /// <param name="recognizer">The recognizer this connection is for</param>
        virtual void Init(std::weak_ptr<ISpxConversationTranslator> convTrans, std::weak_ptr<ISpxRecognizer> recognizer) = 0;

        /// <summary>
        /// Gets the conversation translator instance
        /// </summary>
        /// <returns>The instance, or a nullptr if the instance is no longer valid</returns>
        virtual std::shared_ptr<ISpxConversationTranslator> GetConversationTranslator() = 0;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

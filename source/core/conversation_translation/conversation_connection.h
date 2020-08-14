//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <ispxinterfaces.h>
#include <event.h>
#include "i_web_socket.h"
#include "conversation_messages.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    #define _CONVERSATION_ERROR_CODE_VALUES \
        AuthenticationError, \
        BadRequest, \
        TooManyRequests, \
        Forbidden, \
        ConnectionError, \
        ServiceUnavailable, \
        ServiceError, \
        RuntimeError
    /// <summary>
    /// Defines possible conversation error codes
    /// </summary>
    DEFINE_ENUM(ConversationErrorCode, _CONVERSATION_ERROR_CODE_VALUES)

    #define _CONVERSATION_PARTICIPANT_ACTION_ENUM_VALUES \
        Join, \
        Leave, \
        Update
    /// <summary>
    /// Why the ConversationConnection::OnParticipantsUpdated event was raised
    /// </summary>
    DEFINE_ENUM(ConversationParticipantAction, _CONVERSATION_PARTICIPANT_ACTION_ENUM_VALUES)

    template<typename T>
    using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;


    /// <summary>
    /// The Callbacks type represents an application-defined structure used to register callbacks for conversation events.
    /// The callbacks are invoked during the processing of the request, an application should spend as little time as possible
    /// in the callback functions.
    /// </summary>
    struct ConversationCallbacks
    {
        /// <summary>
        /// A callback function that will be invoked when the connection to service is established.
        /// </summary>
        virtual void OnConnected() {}

        /// <summary>
        /// A callback function that will be invoked when the connection to service is lost.
        /// </summary>
        /// <param name="reason">The web socket disconnect reason</param>
        /// <param name="message">Any additional message explaining the disconnect (can be an empty string)</param>
        virtual void OnDisconnected(const USP::WebSocketDisconnectReason, const string&) { }

        /// <summary>
        /// A callback function that will be invoked when a partial or final recognition is received.
        /// This is raised for your own recognitions, as well as all other participants in the
        /// conversation.
        /// </summary>
        /// <param name="reco">The received recognition</param>
        virtual void OnSpeechRecognition(const ConversationSpeechRecognitionMessage& reco) { (void)reco; }

        /// <summary>
        /// Event raised when we receive an instant message. This is raised for your own recognitions,
        /// as well as all other participants in the conversation
        /// </summary>
        /// <param name="im">The received instant message</param>
        virtual void OnInstantMessage(const ConversationTranslatedMessage& im) { (void)im; }

        /// <summary>
        /// A callback function that will be invoked when a participant changes. For example
        /// a participant has joined or left the conversation, or the participant has changed
        /// (e.g. muted/unmuted)
        /// </summary>
        /// <param name="action">What changed (e.g. participant left the conversation)</param>
        /// <param name="participant">The details for the participants that have changed</param>
        virtual void OnParticipantChanged(const ConversationParticipantAction action, const std::vector<ConversationParticipant>& participants)
        {
            (void)action;
            (void)participants;
        }

        /// <summary>
        /// A callback function that will be invoked to indicate how much time there is left
        /// in the conversation before it reaches its max duration and expires.
        /// </summary>
        /// <param name="minutesLeft">How many minutes are left</param>
        virtual void OnRoomExpirationWarning(const int32_t minutesLeft) { (void)minutesLeft; }

        /// <summary>
        /// A callback function that will be invoked when an error occurs in handling communication with service.
        /// </summary>
        /// <param> The error raised on the connection. </param>
        virtual void OnError(const std::shared_ptr<ISpxErrorInformation>&)
        {
        }

        /// <summary>
        /// A callback function invoked when we receive an updated Cognitive Speech authorization
        /// token to use from the conversation service
        /// </summary>
        /// <param name="authToken">Authorization token</param>
        /// <param name="region">The Azure region (e.g. westus)</param>
        /// <param name="expiredAt">When the authorization token expires</param>
        virtual void OnUpdatedAuthorizationToken(
            const std::string& authToken, const std::string& region, const std::chrono::system_clock::time_point& expiresAt)
        {
            (void)authToken;
            (void)region;
            (void)expiresAt;
        }
    };


    /// <summary>
    /// A conversation web socket connection. This enables you to receive recognitions, instant messages,
    /// and events from the conversation for other participants. It also allows you to send text
    /// messages
    /// </summary>
    class ConversationConnection : public std::enable_shared_from_this<ConversationConnection>
    {
    public:
        /// <summary>
        /// Creates a new instance of the conversation connection
        /// </summary>
        /// <param name="endpoint">The conversation web socket endpoint</param>
        /// <param name="threadService">The thread service to use for sending/receiving data on the web socket</param>
        /// <param name="affinity">Which affinity to use when sending/receiving data on the web socket. Default is background</param>
        /// <param name="sessionId">(Optional) the unique identifier for the session</param>
        /// <param name="pollingInterval">How often to poll for new received data on the web socket connection. Default is 10ms</param>
        /// <returns>The conversation connection instance</returns>
        static std::shared_ptr<ConversationConnection> Create(
            const HttpEndpointInfo& endpoint,
            std::shared_ptr<ISpxThreadService> threadService,
            ISpxThreadService::Affinity affinity = ISpxThreadService::Affinity::Background,
            const std::string& sessionId = "",
            std::chrono::milliseconds pollingInterval = 10ms)
        {
            return std::shared_ptr<ConversationConnection>(new ConversationConnection(endpoint, threadService, affinity, sessionId, pollingInterval));
        }

        /// <summary>
        /// Destructor
        /// </summary>
        ~ConversationConnection();

        /// <summary>
        /// Returns true if the status is connected. 
        /// </summary>
        bool IsConnected() const;

        /// <summary>
        /// Gets whether or not you have been muted by the host. When this happens
        /// you cannot send up audio for speech recognitions, nor can you send
        /// instant messages
        /// </summary>
        bool IsMutedByHost() const;

        /// <summary>
        /// Gets the participants in the conversation
        /// </summary>
        const std::vector<ConversationParticipant> GetParticipants() const;

        /// <summary>
        /// Connects the web socket connection
        /// </summary>
        /// <param name="participantId">The identifier for the current participant</param>
        /// <param name="conversationToken">The token to use to identify the conversation to connect to</param>
        void Connect(const std::string& participantId, const std::string& conversationToken);

        /// <summary>
        /// Disconnects the web socket connection
        /// </summary>
        void Disconnect();

        /// <summary>
        /// Sends a text message to other participants in the room
        /// </summary>
        /// <param name="msg">The text message to send</param>
        void SendTextMessage(const std::string& msg);

        /// <summary>
        /// Enables or disables TTS in the conversation. This will only work
        /// if you enabled TTS while creating/joining the conversation
        /// </summary>
        /// <param name="enabled">True to enable TTS, false to disable it</param>
        void SetTtsEnabled(bool enabled);

        /// <summary>
        /// [HOST ONLY] Sets the locked room state. When a room is locked, no new participants may
        /// join the room.
        /// </summary>
        /// <param name="locklocked">True to lock the room, false otherwise</param>
        void SetRoomLocked(bool lock);

        /// <summary>
        /// [HOST ONLY] Mutes all participants in the conversation except for the host.
        /// </summary>
        /// <param name="mute">True to mute everyone else, false to unmute and allow others
        /// to speak</param>
        void SetMuteEveryoneElse(bool mute);

        /// <summary>
        /// [HOST ONLY] Mutes a particular participant. 
        /// </summary>
        /// <param name="participantId">The identifier of the participant</param>
        /// <param name="mute">True to mute, false to unmute</param>
        void SetMuteParticipant(const std::string& participantId, bool mute);
        
        /// <summary>
        /// [HOST ONLY] Removes a participant from the conversation.
        /// </summary>
        /// <param name="participantId">The identifier of the participant</param>
        void EjectParticipant(const std::string& participantId);

        /// <summary>
        /// Changes your nickname in the conversation
        /// </summary>
        /// <param name="nickname">The nickname to set</param>
        void SetNickname(const std::string& nickname);

        /// <summary>
        /// [HOST ONLY] Updates the Cognitive Speech authorization token for all participants
        /// in the conversation
        /// </summary>
        /// <param name="authToken">The authorization token</param>
        /// <param name="region">The Azure region (e.g. westus)</param>
        void SetAuthorizationToken(const std::string& authToken, const std::string& region);

        /// <summary>
        /// Sets the callback functions to use
        /// </summary>
        /// <param name="callbacks">The new callbacks to use</param>
        void SetCallbacks(std::shared_ptr<ConversationCallbacks> callbacks);

    private:
        ConversationConnection(
            const HttpEndpointInfo& endpoint,
            std::shared_ptr<ISpxThreadService> threadService,
            ISpxThreadService::Affinity affinity,
            const std::string& sessionId,
            std::chrono::milliseconds pollingInterval);

        /// <summary>
        /// Checks if we are connected, and whether or not we can send messages
        /// over the web socket connection to the conversation service
        /// </summary>
        inline void CheckCanSend();

        /// <summary>
        /// Same as CheckCanSend() but also check if you are the host
        /// </summary>
        inline void CheckHostCanSend();

        void HandleConnected();
        void HandleDisconnected(USP::WebSocketDisconnectReason reason, const std::string& message);
        void HandleTextData(const std::string& text);
        void HandleBinaryData(const uint8_t* data, const size_t length);
        void HandleError(const std::shared_ptr<ISpxErrorInformation>& error);

        void HandleInfoMessage(const ConversationInfoMessage* info);
        void HandleParticipantList(const ConversationParticipantListMessage* participantList);
        void HandleCommand(const ConversationCommandMessage* command);
        void HandleRecognition(const ConversationSpeechRecognitionMessage* reco);
        void HandleInstantMessage(const ConversationTranslatedMessage* im);
        void HandleParticipantCommand(const ConversationParticipantCommandMessage* command);

        inline const ConversationParticipant& CurrentParticipant() const
        {
            return m_participants.at(m_currentParticipantId);
        }

    private:
        HttpEndpointInfo m_webSocketEndpoint;
        std::string m_sessionId;
        std::shared_ptr<ConversationCallbacks> m_callbacks;
        std::shared_ptr<USP::IWebSocket> m_webSocket;
        std::shared_ptr<ISpxThreadService> m_threadService;
        const ISpxThreadService::Affinity m_affinity;
        std::chrono::milliseconds m_pollingIntervalMs;
        std::string m_roomId;
        std::unordered_map<std::string, ConversationParticipant> m_participants;
        std::string m_currentParticipantId;
        std::atomic_bool m_receivedParticipantsList;
        std::atomic_bool m_mutedByHost;
        std::vector<size_t> m_callbackIds;
        std::promise<bool> m_connectionOpenPromise;
        std::shared_future<bool> m_connectionOpen;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::ConversationTranslation

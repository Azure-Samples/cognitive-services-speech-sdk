//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "ispxinterfaces.h"
#include <event.h>
#include "IWebSocket.h"
#include "conversation_messages.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    #define _CONVERSATION_ERROR_CODE_VALUES \
        AuthenticationError = 1, \
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
        virtual void OnDisconnected(const USP::WebSocketDisconnectReason reason, const string& message)
        {
            (void)reason;
            (void)message;
        }

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
        /// <param name="isWebSocket">True if the error was caused by the web socket layer</param>
        /// <param name="error">The error code</param>
        /// <param name="message">(Optional) A message describing the error</param>
        virtual void OnError(const bool isWebSocket, const ConversationErrorCode error, const std::string& message)
        {
            (void)isWebSocket;
            (void)error;
            (void)message;
        }
    };

    using ConversationCallbacksPtr = std::shared_ptr<ConversationCallbacks>;
    class ConversationConnection;

    /// <summary>
    /// A client that can be used to create ConversationConnections
    /// </summary>
    class ConversationClient
    {
    public:
        /// <summary>
        /// Creates a conversation client.
        /// </summary>
        /// <param name="callbacks">The struct defines callback functions that will be invoked when various events occur.</param>
        /// <param name="connectionId">Connection id that will be passed to the service, and can be used for diagnostics.</param>
        /// <param name="threadService">The thread service. This is used to send/receive on the web socket in the background.</param>
        ConversationClient(ConversationCallbacksPtr callbacks, const std::string& connectionId, const std::shared_ptr<ISpxThreadService>& threadService);

        /// <summary>
        /// Default copy constructor
        /// </summary>
        ConversationClient(const ConversationClient&) = default;

        /// <summary>
        /// Destructor
        /// </summary>
        ~ConversationClient() = default;

        /// <summary>
        /// Sets the identifier for the participant for whom we are creating the connection to the conversation service
        /// </summary>
        /// <param name="particpantId">The participant identifier (a GUID)</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetParticipantId(const std::string& particpantId);

        /// <summary>
        /// Sets the conversation service endpoint. It should contain the host name, resource path and all query parameters needed.
        /// </summary>
        /// <param name="endpointUrl">The endpoint URL to use</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetEndpointUrl(const std::string& endpointUrl);

        /// <summary>
        /// Adds some user specified query parameters. These will be appended to the URL used to connect to the service
        /// </summary>
        /// <param name="queryParameters">The query parameters string to include. Should not start with ? or &</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetUserDefinedQueryParameters(const std::string& queryParameters);

        /// <summary>
        /// Sets the proxy server information, which is used to configure the connection to go through a proxy server.
        /// </summary>
        /// <param name="proxyHost">The proxy host name</param>
        /// <param name="proxyPort">The port the proxy is listening on</param>
        /// <param name="proxyUsername">The proxy username</param>
        /// <param name="proxyPassword">The proxy password</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetProxyServerInfo(const char* proxyHost, int proxyPort, const char* proxyUsername = nullptr, const char* proxyPassword = nullptr);

        /// <summary>
        /// When using OpenSSL only: sets a single trusted cert, optionally w/o CRL checks.
        /// This is meant to be used in a firewall setting with potential lack of
        /// CRLs (particularly on the leaf).
        /// </summary>
        /// <param name="trustedCert">The certificate to trust (PEM format)</param>
        /// <param name="disable_crl_check">Whether to also disable CRL checks</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetSingleTrustedCert(const std::string& trustedCert, bool disable_crl_check);

        /// <summary>
        /// Sets the authentication token to use for the conversation. This is returned in response to a call to
        /// CSpxConversationManager::CreateOrJoin. Please note that this is different from the speech service
        /// authentication tokens
        /// </summary>
        /// <param name="authToken">The conversation authentication token to use</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetAuthenticationToken(const std::string& authToken);

        /// <summary>
        /// Sets query parameters
        /// </summary>
        /// <param name="name">The query parameter name</param>
        /// <param name="value">The query value</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetQueryParameter(const std::string& name, const std::string& value);

        /// <summary>
        /// Sets the polling interval the client will use. The default is 10ms
        /// </summary>
        /// <param name="pollingIntervalInMs">The polling interval in ms to use</param>
        /// <returns>Reference to self</returns>
        ConversationClient& SetPollingInterval(const std::uint32_t pollingIntervalInMs);

        /// <summary>
        /// Establishes a web socket connection to the conversation service
        /// </summary>
        /// <returns>A pointer to the connection</returns>
        std::shared_ptr<ConversationConnection> Connect();

    private:
        friend class ConversationConnection;

        USP::WebSocketParams m_params;
        ConversationCallbacksPtr m_callbacks;
        std::shared_ptr<ISpxThreadService> m_threadService;
        std::uint32_t m_pollingIntervalMs = 10;
        std::string m_userDefinedQueryParameters;
        std::string m_participantId;
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
        /// Sets the callback functions to use
        /// </summary>
        /// <param name="callbacks">The new callbacks to use</param>
        void SetCallbacks(ConversationCallbacksPtr callbacks);

    private:
        friend class ConversationClient;

        ConversationConnection(const ConversationClient& client);

        void Connect();

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
        void HandleError(USP::WebSocketError error, int code, const std::string& message);

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
        ConversationCallbacksPtr m_callbacks;
        USP::WebSocketParams m_params;
        std::shared_ptr<USP::IWebSocket> m_webSocket;
        std::shared_ptr<ISpxThreadService> m_threadService;
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

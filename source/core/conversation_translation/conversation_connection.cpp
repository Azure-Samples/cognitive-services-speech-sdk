//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "common.h"
#include <sstream>
#include <azure_c_shared_utility_xlogging_wrapper.h>
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include <string_utils.h>
#include <http_utils.h>
#include "conversation_connection.h"
#include "conversation_utils.h"
#include "web_socket.h"

using namespace Microsoft::CognitiveServices::Speech::USP;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    using StringUtils = PAL::StringUtils;

    DEFINE_ENUM_STRINGS(ConversationErrorCode, _CONVERSATION_ERROR_CODE_VALUES)

    /// <summary>
    /// Returns the value if it is not empty or returns the default value
    /// </summary>
    /// <param name="value">The value</param>
    /// <param name="defaultValue">The default to use if the value is empty</param>
    /// <returns>The value or the default</returns>
    static inline std::string ValueOrDefault(const string& value, const string& defaultValue)
    {
        return value.empty() ? defaultValue : value;
    }

    static inline void WaitForConnect(std::shared_future<bool> future)
    {
        auto status = future.wait_for(90s);
        switch (status)
        {
            case std::future_status::ready:
            {
                // observe any exception and return
                bool opened = future.get();
                if (!opened)
                {
                    SPX_THROW_HR(SPXERR_ABORT);
                }
                return;
            }

            default:
            case std::future_status::deferred:
            case std::future_status::timeout:
                SPX_THROW_HR(SPXERR_TIMEOUT);
                break;
        }
    }

    ConversationConnection::ConversationConnection(
            const HttpEndpointInfo& endpoint,
            std::shared_ptr<ISpxThreadService> threadService,
            ISpxThreadService::Affinity affinity,
            const std::string& sessionId,
            std::chrono::milliseconds pollingInterval)
        : m_webSocketEndpoint(endpoint),
        m_sessionId(sessionId),
        m_callbacks(),
        m_webSocket(),
        m_threadService(threadService),
        m_affinity(affinity),
        m_pollingIntervalMs(pollingInterval),
        m_roomId(),
        m_participants(),
        m_currentParticipantId(),
        m_receivedParticipantsList(false),
        m_mutedByHost(false),
        m_callbackIds(),
        m_connectionOpenPromise(),
        m_connectionOpen()
    {
    }

    ConversationConnection::~ConversationConnection()
    {
        // prevent callbacks to this code by removing event handlers
        for (const size_t callbackId : m_callbackIds)
        {
            m_webSocket->OnConnected.remove(callbackId);
            m_webSocket->OnDisconnected.remove(callbackId);
            m_webSocket->OnTextData.remove(callbackId);
            m_webSocket->OnBinaryData.remove(callbackId);
            m_webSocket->OnError.remove(callbackId);
        }

        m_callbacks = nullptr;
        m_webSocket = nullptr;
        m_threadService = nullptr;
        m_receivedParticipantsList = false;
        m_mutedByHost = false;
        m_callbackIds.clear();
    }

    void ConversationConnection::Connect(const std::string& participantId, const std::string& conversationToken)
    {
        if (m_webSocket != nullptr)
        {
            switch (m_webSocket->GetState())
            {
                case WebSocketState::CONNECTED:
                case WebSocketState::INITIAL:
                case WebSocketState::OPENING:
                    // the web socket is open, or in the process of opening. We just need to make sure
                    // we have received the participant list
                    WaitForConnect(m_connectionOpen);
                    return;

                default:
                case WebSocketState::CLOSED:
                case WebSocketState::DESTROYING:
                    // we need to open the web socket
                    break;
            }
        }

        if (m_webSocket == nullptr)
        {
            m_webSocket = WebSocket::Create(m_threadService, m_affinity, m_pollingIntervalMs);

            auto ptr = shared_from_this();

            // register handlers
            m_callbackIds.push_back(m_webSocket->OnConnected.add(ptr, &ConversationConnection::HandleConnected));
            m_callbackIds.push_back(m_webSocket->OnDisconnected.add(ptr, &ConversationConnection::HandleDisconnected));
            m_callbackIds.push_back(m_webSocket->OnTextData.add(ptr, &ConversationConnection::HandleTextData));
            m_callbackIds.push_back(m_webSocket->OnBinaryData.add(ptr, &ConversationConnection::HandleBinaryData));
            m_callbackIds.push_back(m_webSocket->OnError.add(ptr, &ConversationConnection::HandleError));

            m_webSocketEndpoint.SetQueryParameter(ConversationQueryParameters::ApiVersion, ConversationConstants::ApiVersion);

            m_webSocketEndpoint.SetHeader(ConversationQueryParameters::AuthTokenHeader, conversationToken);
            if (!m_sessionId.empty())
            {
                m_webSocketEndpoint.SetHeader(ConversationConstants::ClientTraceIdHeader, m_sessionId);
            }

            m_currentParticipantId = participantId;
        }

        m_connectionOpenPromise = std::promise<bool>();
        m_connectionOpen = m_connectionOpenPromise.get_future().share();
        m_receivedParticipantsList = false;
        m_mutedByHost = false;
        m_roomId.clear();
        unordered_map<string, ConversationParticipant> empty;
        m_participants.swap(empty);

        m_webSocket->Connect(m_webSocketEndpoint);

        // wait for the web socket to connect and to receive the list of participants from the service.
        WaitForConnect(m_connectionOpen);
    }

    bool ConversationConnection::IsConnected() const
    {
        return m_webSocket && m_webSocket->IsConnected() && m_receivedParticipantsList;
    }

    bool ConversationConnection::IsMutedByHost() const
    {
        return m_mutedByHost && !CurrentParticipant().IsHost;
    }

    const std::vector<ConversationParticipant> ConversationConnection::GetParticipants() const
    {
        std::vector<ConversationParticipant> p;

        for (const auto& entry : m_participants)
        {
            p.push_back(entry.second);
        }

        return p;
    }

    void ConversationConnection::Disconnect()
    {
        if (m_webSocket == nullptr)
        {
            return;
        }

        switch (m_webSocket->GetState())
        {
            case WebSocketState::CLOSED:
            case WebSocketState::DESTROYING:
                // nothing to do
                break;

            case WebSocketState::CONNECTED:
            case WebSocketState::INITIAL:
            case WebSocketState::OPENING:
            default:
                m_webSocket->Disconnect();
                break;
        }
    }

    void ConversationConnection::SendTextMessage(const std::string & msg)
    {
        if (msg.empty())
        {
            return;
        }

        CheckCanSend();

        if (!CurrentParticipant().IsHost && m_mutedByHost)
        {
            ThrowLogicError("You have been muted by the host");
        }

        ConversationInstantMessage im(m_roomId, CurrentParticipant().Nickname, CurrentParticipant().ParticipantId, msg);
        m_webSocket->SendTextData(im.ToJsonString());
    }

    void ConversationConnection::SetTtsEnabled(bool enabled)
    {
        CheckCanSend();

        ConversationParticipantCommandMessage msg(
            m_roomId,
            CurrentParticipant().Nickname,
            CurrentParticipant().ParticipantId,
            ParticipantCommandType::SetUseTTS,
            enabled);

        m_webSocket->SendTextData(msg.ToJsonString());
    }

    void ConversationConnection::SetRoomLocked(bool lock)
    {
        CheckHostCanSend();

        ConversationParticipantCommandMessage msg(
            m_roomId,
            CurrentParticipant().Nickname,
            CurrentParticipant().ParticipantId,
            ParticipantCommandType::SetLockState,
            lock);

        m_webSocket->SendTextData(msg.ToJsonString());
    }

    void ConversationConnection::SetMuteEveryoneElse(bool mute)
    {
        CheckHostCanSend();

        ConversationParticipantCommandMessage msg(
            m_roomId,
            CurrentParticipant().Nickname,
            CurrentParticipant().ParticipantId,
            ParticipantCommandType::SetMuteAll,
            mute);

        m_webSocket->SendTextData(msg.ToJsonString());
    }

    void ConversationConnection::SetMuteParticipant(const std::string & participantId, bool mute)
    {
        CheckHostCanSend();

        ConversationParticipantCommandMessage msg(
            m_roomId,
            m_participants[participantId].Nickname,
            participantId,
            ParticipantCommandType::SetMute,
            mute);

        m_webSocket->SendTextData(msg.ToJsonString());
    }

    void ConversationConnection::EjectParticipant(const std::string & participantId)
    {
        CheckHostCanSend();

        ConversationParticipantCommandMessage msg(
            m_roomId,
            CurrentParticipant().Nickname,
            CurrentParticipant().ParticipantId,
            ParticipantCommandType::EjectParticipant,
            participantId);

        m_webSocket->SendTextData(msg.ToJsonString());
    }

    void ConversationConnection::SetNickname(const std::string & nickname)
    {
        CheckCanSend();

        ConversationParticipantCommandMessage msg(
            m_roomId,
            CurrentParticipant().Nickname,
            CurrentParticipant().ParticipantId,
            ParticipantCommandType::ChangeNickname,
            nickname);

        m_webSocket->SendTextData(msg.ToJsonString());
    }

    void ConversationConnection::SetAuthorizationToken(const std::string& authToken, const std::string& region)
    {
        CheckHostCanSend();

        ConversationAuthorizationTokenMessage msg(authToken, region);

        m_webSocket->SendTextData(msg.ToJsonString());
    }

    void ConversationConnection::SetCallbacks(std::shared_ptr<ConversationCallbacks> callbacks)
    {
        m_callbacks = callbacks;
    }

    inline void ConversationConnection::CheckCanSend()
    {
        if (m_webSocket == nullptr)
        {
            ThrowLogicError("You are not connected (null web socket)");
        }

        auto state = m_webSocket->GetState();
        if (state != WebSocketState::CONNECTED)
        {
            ThrowLogicError("You are not connected. Current state: " + to_string((int)state));
        }

        if (!m_receivedParticipantsList)
        {
            ThrowLogicError("You are connected but have not yet received the participants list message. Please wait and try again later");
        }
    }

    inline void ConversationConnection::CheckHostCanSend()
    {
        CheckCanSend();
        if (!CurrentParticipant().IsHost)
        {
            ThrowLogicError("Only the host can send this command");
        }
    }

    void ConversationConnection::HandleConnected()
    {
        // we should not send this event until we receive the participant list command on the web socket.
        // Sending messages on the web socket before this happens will result in the conversation service closing
        // the web socket connection for a protocol violation
        if (!m_receivedParticipantsList)
        {
            return;
        }

        if (m_callbacks != nullptr)
        {
            m_callbacks->OnConnected();
        }
    }

    void ConversationConnection::HandleDisconnected(USP::WebSocketDisconnectReason reason, const std::string & message, bool serverRequested)
    {
        m_receivedParticipantsList = false;
        m_mutedByHost = false;

        try
        {
            m_connectionOpenPromise.set_value(false);
        }
        catch (std::future_error&)
        {
            // promise could already be fulfilled and that's OK
        }

        // NOTE: Do not try to reset the web socket connection here. Doing so will result in
        //       code trying to access destroyed members. This is invoked through the web
        //       socket code.

        if (m_callbacks != nullptr)
        {
            m_callbacks->OnDisconnected(reason, message, serverRequested);
        }
    }

    void ConversationConnection::HandleTextData(const std::string& text)
    {
        if (text.empty())
        {
            LogInfo("Received empty message. ConversationConnection: %p", this);
            return;
        }

        // Let's try to parse the message
        unique_ptr<ConversationMessageBase> parsed = ConversationMessageBase::ParseJsonString(text);
        if (parsed == nullptr)
        {
            LogError("Failed to parse received message. ConversationConnection: %p, Message: '%s'", this, text.c_str());
            return;
        }

        switch (parsed->Type)
        {
            case MessageType::Command:
                HandleCommand(dynamic_cast<const ConversationCommandMessage*>(parsed.get()));
                break;

            case MessageType::Info:
                HandleInfoMessage(dynamic_cast<const ConversationInfoMessage*>(parsed.get()));
                break;

            case MessageType::Final:
            case MessageType::Partial:
                HandleRecognition(dynamic_cast<const ConversationSpeechRecognitionMessage*>(parsed.get()));
                break;

            case MessageType::ParticipantCommand:
                HandleParticipantCommand(dynamic_cast<const ConversationParticipantCommandMessage*>(parsed.get()));
                break;

            case MessageType::TranslatedMessage:
                HandleInstantMessage(dynamic_cast<const ConversationTranslatedMessage*>(parsed.get()));
                break;

            case MessageType::InstantMessage:
                // These types of messages should never be received from the service
                LogError(
                    "Unexpected received message from conversation service. ConversationConnection: %p, Type: %s, Message: '%s'",
                    this, MessageTypeStrings(parsed->Type), text.c_str());
                break;

            case Unknown:
            default:
                LogError(
                    "Unsupported or unknown received message. ConversationConnection: %p, Type: %s, Message: '%s'",
                    this, MessageTypeStrings(parsed->Type), text.c_str());
                break;
        }
    }

    void ConversationConnection::HandleBinaryData(const uint8_t * data, const size_t length)
    {
        (void)data;
        (void)length;

        // TODO ralphe: parse TTS data here
    }

    void ConversationConnection::HandleError(WebSocketError error, int code, const std::string & message)
    {
        LogError("WebSocketError received. ConversationConnection: %p, Error: %d, Code: %d, Message: %s",
            this, error, code, message.c_str());

        ConversationErrorCode conversationErrorCode = ConversationErrorCode::ConnectionError;
        std::string errorDescription = message;
        std::string errorCodeInString = to_string(code);

        switch (error)
        {
            case WebSocketError::REMOTE_CLOSED:
                conversationErrorCode = ConversationErrorCode::ConnectionError;
                errorDescription = "Connection was closed by the remote host. Error code: " + errorCodeInString + ". Error details: " + message;
                break;

            case WebSocketError::CONNECTION_FAILURE:
                conversationErrorCode = ConversationErrorCode::ConnectionError;
                errorDescription = "Connection failed (no connection to the remote host). Internal error: " + errorCodeInString
                    + ". Error details: " + message
                    + ". Please check network connection, firewall setting, and the region name used.";
                break;

            case WebSocketError::WEBSOCKET_UPGRADE:
                switch (static_cast<HttpStatusCode>(code))
                {
                    case HttpStatusCode::BAD_REQUEST:
                        conversationErrorCode = ConversationErrorCode::BadRequest;
                        errorDescription = "WebSocket Upgrade failed with a bad request (400). Please check the query parameters and headers are set correctly.";
                        break;
                    case HttpStatusCode::FORBIDDEN:
                    case HttpStatusCode::UNAUTHORIZED:
                        conversationErrorCode = ConversationErrorCode::AuthenticationError;
                        errorDescription = "WebSocket Upgrade failed with an authentication error ("
                            + errorCodeInString
                            + "). Please check that the correct authorization query parameters and/or headers are set.";
                        break;
                    case HttpStatusCode::TOO_MANY_REQUESTS:
                        conversationErrorCode = ConversationErrorCode::TooManyRequests;
                        errorDescription = "WebSocket Upgrade failed with too many requests error (429).";
                        break;
                    case HttpStatusCode::INTERNAL_ERROR:
                        conversationErrorCode = ConversationErrorCode::ServiceError;
                        errorDescription = "WebSocket Upgrade failed with an internal error (500). If the issue persists, please contact support.";
                        break;
                    case HttpStatusCode::NOT_FOUND:
                        conversationErrorCode = ConversationErrorCode::BadRequest;
                        errorDescription = "WebSocket Upgrade failed with not found (404). Please double check that the WebSocket URL is correct.";
                        break;
                    case HttpStatusCode::SERVICE_UNAVAILABLE:
                        conversationErrorCode = ConversationErrorCode::ServiceError;
                        errorDescription = "WebSocket Upgrade failed with service unavailable (503). Please try again later.";
                        break;
                    default:
                        conversationErrorCode = ConversationErrorCode::ConnectionError;
                        errorDescription = "WebSocket Upgrade failed with HTTP status code: " + errorCodeInString;
                        break;
                }
                break;

            case WebSocketError::WEBSOCKET_SEND_FRAME:
                conversationErrorCode = ConversationErrorCode::ConnectionError;
                errorDescription = "Failure while sending a frame over the WebSocket connection. Internal error: " + errorCodeInString
                    + ". Error details: " + message;
                break;

            case WebSocketError::WEBSOCKET_ERROR:
                conversationErrorCode = ConversationErrorCode::ConnectionError;
                errorDescription = "WebSocket operation failed. Internal error: " + errorCodeInString
                    + ". Error details: " + message;
                break;

            case WebSocketError::DNS_FAILURE:
                conversationErrorCode = ConversationErrorCode::ConnectionError;
                errorDescription = "DNS connection failed (the remote host did not respond). Internal error: " + errorCodeInString;
                break;

            default:
            case WebSocketError::UNKNOWN:
                conversationErrorCode = ConversationErrorCode::ConnectionError;
                errorDescription = "Unknown transport error. ErrorDetails: " + message;
                break;
        }

        LogError("Parsed web socket error. ErrorCode: '%s', Description: '%s'",
            ConversationErrorCodeStrings(conversationErrorCode), errorDescription.c_str());

        try
        {
            std::ostringstream oss;
            oss << "Web socket error '" << ConversationErrorCodeStrings(conversationErrorCode) << "': " << errorDescription;

            std::string exceptionString = oss.str();
            auto ex_ptr = std::make_exception_ptr(ExceptionWithCallStack(exceptionString, SPXERR_RUNTIME_ERROR));
            m_connectionOpenPromise.set_exception(ex_ptr);
        }
        catch (std::future_error&)
        {
            // promise could already be fulfilled and that's OK
        }

        if (m_callbacks != nullptr)
        {
            m_callbacks->OnError(true, conversationErrorCode, errorDescription);
        }
    }

    void ConversationConnection::HandleInfoMessage(const ConversationInfoMessage* info)
    {
        if (info == nullptr)
        {
            return;
        }

        if (info->Command == "token")
        {
            auto tokenMessage = dynamic_cast<const ConversationAuthorizationTokenMessage*>(info);

            if (tokenMessage != nullptr && m_callbacks != nullptr)
            {
                m_callbacks->OnUpdatedAuthorizationToken(
                    tokenMessage->AuthToken,
                    tokenMessage->Region,
                    tokenMessage->ValidUntil);
            }
        }
        else
        {
            HandleParticipantList(dynamic_cast<const ConversationParticipantListMessage*>(info));
        }
    }

    void ConversationConnection::HandleParticipantList(const ConversationParticipantListMessage * participantList)
    {
        if (participantList == nullptr)
        {
            return;
        }

        // This message is always received when we first connect so send out an "Joined" event.
        // We cannot send messages to the service until we have received this message.
        bool oldReceived = m_receivedParticipantsList.exchange(true);
        m_roomId = participantList->ConversationId;
        m_connectionOpenPromise.set_value(true);

        // Send out the connected event
        if (!oldReceived)
        {
            HandleConnected();
        }

        // update existing participants list
        for (const ConversationParticipant& p : participantList->Participants)
        {
            m_participants[p.ParticipantId] = p;
        }

        if (m_callbacks != nullptr)
        {
            m_callbacks->OnParticipantChanged(ConversationParticipantAction::Join, participantList->Participants);
        }
    }

    void ConversationConnection::HandleCommand(const ConversationCommandMessage * command)
    {
        if (command == nullptr)
        {
            return;
        }

        const ConversationParticipant existing = m_participants[command->ParticipantId];

        ConversationParticipant participant;
        participant.Avatar = ValueOrDefault(command->Avatar, existing.Avatar);
        participant.IsHost = command->IsHost;
        participant.IsMuted = command->IsMuted;
        participant.Locale = ValueOrDefault(command->Locale, existing.Locale);
        participant.Nickname = command->Nickname;
        participant.ParticipantId = command->ParticipantId;
        participant.UseTTS = command->UseTTS;

        ConversationParticipantAction action;

        switch (command->Command)
        {
            case CommandType::DisconnectSession:
                // ignore for this to avoid sending double left events
                return;

            case CommandType::LeaveSession:
                action = ConversationParticipantAction::Leave;
                m_participants.erase(participant.ParticipantId);

                // some fields are not set so use the values from the existing one (if any)
                participant.IsMuted = existing.IsMuted;
                participant.UseTTS = existing.UseTTS;
                break;

            case CommandType::JoinSession:
                action = ConversationParticipantAction::Join;
                m_participants[participant.ParticipantId] = participant;
                break;

            default:
                LogError("Unsupported command type. ConversationConnection: %p, Command: %d", this, command->Type);
                return;
        }

        if (m_callbacks != nullptr)
        {
            m_callbacks->OnParticipantChanged(action, std::vector<ConversationParticipant> { participant });
        }
    }

    void ConversationConnection::HandleRecognition(const ConversationSpeechRecognitionMessage * reco)
    {
        if (reco != nullptr && m_callbacks != nullptr)
        {
            m_callbacks->OnSpeechRecognition(*reco);
        }
    }

    void ConversationConnection::HandleInstantMessage(const ConversationTranslatedMessage * im)
    {
        if (im != nullptr && m_callbacks != nullptr)
        {
            m_callbacks->OnInstantMessage(*im);
        }
    }

    void ConversationConnection::HandleParticipantCommand(const ConversationParticipantCommandMessage * command)
    {
        if (command == nullptr)
        {
            return;
        }

        switch (command->Command)
        {
            case ParticipantCommandType::ChangeNickname:
            case ParticipantCommandType::SetMute:
                {
                    // since the message doesn't have the full details for the participant, we'll need to retrieve the existing
                    // one (if any)
                    string participantId = command->ParticipantId;

                    const auto iter = m_participants.find(participantId);
                    if (iter == m_participants.end())
                    {
                        // participant doesn't exist so let's add them with as much information as we have
                        ConversationParticipant part;
                        part.Avatar = command->Avatar;
                        part.IsHost = false;
                        part.IsMuted = false;
                        part.Nickname = command->Nickname;
                        part.ParticipantId = command->ParticipantId;
                        part.UseTTS = false;

                        m_participants[participantId] = std::move(part);
                    }

                    ConversationParticipant & participant = m_participants[participantId];
                    if (command->Command == ParticipantCommandType::ChangeNickname)
                    {
                        participant.Nickname = command->Value.StringVal();
                    }
                    else
                    {
                        participant.IsMuted = command->Value.BoolValue();
                    }

                    if (m_callbacks != nullptr)
                    {
                        m_callbacks->OnParticipantChanged(ConversationParticipantAction::Update, vector<ConversationParticipant> { participant });
                    }
                }
                break;

            case ParticipantCommandType::SetMuteAll:
                {
                    vector<ConversationParticipant> modified;
                    m_mutedByHost = command->Value.BoolValue();

                    // update state for all participants except host
                    for (auto& entry : m_participants)
                    {
                        if (!entry.second.IsHost)
                        {
                            entry.second.IsMuted = true;
                            modified.push_back(entry.second);
                        }
                    }

                    if (m_callbacks != nullptr)
                    {
                        m_callbacks->OnParticipantChanged(ConversationParticipantAction::Update, modified);
                    }
                }
                break;

            case ParticipantCommandType::RoomExpirationWarning:
                if (m_callbacks != nullptr)
                {
                    m_callbacks->OnRoomExpirationWarning(command->Value.IntValue());
                }
                break;

            case ParticipantCommandType::SetLockState:
                // TODO ralphe: do we need to do anything with this?
                break;

            default:
                // unexpected participant command from service
                LogError(
                    "Unsupported received participant command message from conversation service. ConversationConnection: %p, Type: %s",
                    this, ParticipantCommandTypeStrings(command->Command));
                break;
        }
    }

}}}}} // Microsoft::CognitiveServices::Speech::Impl::ConversationTranslation


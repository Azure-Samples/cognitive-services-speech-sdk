//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// IWebSocket.h: The interface that defines what a web socket implementation should look like
//

#pragma once

#include <ispxinterfaces.h>
#include <future>
#include <http_endpoint_info.h>
#include <event.h>
#include <i_web_socket_state.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    /// <summary>
    /// Enumeration of message types
    /// </summary>
    enum class MetricMessageType : int8_t
    {
        INTERNAL_TRANSPORT_ERROR_PARSEERROR = -1,
        INTERNAL_TRANSPORT_UNHANDLEDRESPONSE = -2,
        INTERNAL_TRANSPORT_INVALIDSTATE = -3,
        INTERNAL_CORTANA_EVENT_COALESCED = -4,
        METRIC_MESSAGE_TYPE_INVALID = 0,
        METRIC_MESSAGE_TYPE_DEVICECONTEXT = 1,
        METRIC_MESSAGE_TYPE_AUDIO_START = 2,
        METRIC_MESSAGE_TYPE_AUDIO_LAST = 3,
        METRIC_MESSAGE_TYPE_TELEMETRY = 4,
        METRIC_TRANSPORT_STATE_DNS = 5,
        METRIC_TRANSPORT_STATE_DROPPED = 6,
        METRIC_TRANSPORT_STATE_CLOSED = 7,
        METRIC_TRANSPORT_STATE_CANCELLED = 8,
        METRIC_TRANSPORT_STATE_RESET = 9
    };

    /// <summary>
    /// The interface for web socket messages
    /// </summary>
    struct IWebSocketMessage
    {
        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~IWebSocketMessage() = default;

        /// <summary>
        /// Gets the telemetry type of the web socket message
        /// </summary>
        /// <returns>The message type</returns>
        virtual USP::MetricMessageType MetricMessageType() const = 0;

        /// <summary>
        /// The type of web socket frame (e.g. text, or binary)
        /// </summary>
        /// <returns>The web socket frame type</returns>
        virtual uint8_t FrameType() const = 0;

        /// <summary>
        /// The total size in bytes of the message
        /// </summary>
        /// <returns>The message size in bytes</returns>
        virtual size_t Size() const = 0;

        /// <summary>
        /// Serializes this web socket message
        /// </summary>
        /// <param name="buffer">The buffer to initialise and write to</param>
        /// <returns>The number of bytes written to the buffer</returns>
        virtual size_t Serialize(std::shared_ptr<uint8_t>& buffer) = 0;

        /// <summary>
        /// Sets that the message has been sent
        /// </summary>
        /// <param>True if the message was sent successfully, false otherwise</param>
        virtual void SetMessageSent(bool success) = 0;

        /// <summary>
        /// Sets an exception when sending the message.
        /// </summary>
        /// <param>The exception when sending the message.</param>
        virtual void SetMessageSentException(std::exception_ptr eptr) = 0;

        /// <summary>
        /// Constructor
        /// </summary>
        IWebSocketMessage() = default;

        IWebSocketMessage(const IWebSocketMessage&) = delete;
        IWebSocketMessage(IWebSocketMessage&&) = default;
        IWebSocketMessage& operator=(const IWebSocketMessage&) = delete;
        IWebSocketMessage& operator=(IWebSocketMessage&&) = default;

    };

    /// <summary>
    /// The interface that web socket implementations should implement
    /// </summary>
    class IWebSocket
    {
    public:
        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~IWebSocket() = default;

        /// <summary>
        /// Connects to the web socket connection
        /// </summary>
        /// <param name="params">The connection parameters to use</param>
        /// <param name="connectionId">(Optional) The connection ID associated with this web socket connection which is used for telemetry</param>
        virtual void Connect(const Impl::HttpEndpointInfo& webSocketEndpoint, const std::string& connectionId = "") = 0;

        /// <summary>
        /// Disconnects the web socket connection
        /// </summary>
        virtual void Disconnect() = 0;

        /// <summary>
        /// Sends text data to the server
        /// </summary>
        /// <param name="text">The text data to send</param>
        virtual void SendTextData(const std::string& text) = 0;

        /// <summary>
        /// Sends binary data to the server
        /// </summary>
        /// <param name="data">The data to send</param>
        /// <param name="size">The size of the data to send</param>
        virtual void SendBinaryData(const uint8_t* data, const size_t size) = 0;

        /// <summary>
        /// Sends a web socket message to the server
        /// </summary>
        /// <param name="message">The message to send</param>
        virtual void SendData(std::unique_ptr<IWebSocketMessage> message) = 0;

        /// <summary>
        /// Gets the web socket connection state
        /// </summary>
        virtual WebSocketState GetState() const = 0;

        /// <summary>
        /// Checks if the web socket is connected. This will return if and only if the current
        /// state is Connected
        /// </summary>
        virtual bool IsConnected()
        {
            return GetState() == WebSocketState::CONNECTED;
        }

        /// <summary>
        /// Event raised when the web socket connects
        /// </summary>
        Impl::event<> OnConnected;

        /// <summary>
        /// Event raised when the socket is disconnected. The first parameter will be the reason
        /// we were disconnected. The second will either be the message the server sent to, or
        /// an internally generated message in the case of errors. The third parameter is set
        /// to true if the server requested the web socket be disconnected.
        /// </summary>
        Impl::event<WebSocketDisconnectReason, const std::string&, bool> OnDisconnected;

        /// <summary>
        /// Event raised when we receive text data from the server
        /// </summary>
        Impl::event<const std::string&> OnTextData;

        /// <summary>
        /// Event raised when we receive binary data from the server. The first parameter will
        /// be the pointer to the data, the second will be the size
        /// </summary>
        Impl::event<const uint8_t*, const size_t> OnBinaryData;

        /// <summary>
        /// Event raised when we encounter an error.
        /// </summary>
        Impl::event<const std::shared_ptr<Impl::ISpxErrorInformation>&> OnError;
    };

} } } }

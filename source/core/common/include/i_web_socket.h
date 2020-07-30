//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// IWebSocket.h: The interface that defines what a web socket implementation should look like
//

#pragma once

#include <future>
#include <http_endpoint_info.h>
#include <event.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    /// <summary>
    /// Possible web socket errors
    /// </summary>
    enum class WebSocketError
    {
        /// <summary>
        /// An unknown error was encountered (e.g. unexpected exception)
        /// </summary>
        UNKNOWN,
        /// <summary>
        /// The server we are connected to closed the connection
        /// </summary>
        REMOTE_CLOSED,
        /// <summary>
        /// There was an error connecting to the web server. The error code returned will
        /// be the internal WS_OPEN_RESULT value
        /// </summary>
        CONNECTION_FAILURE,
        /// <summary>
        /// We got an HTTP error in response to our upgrade request from the server. The
        /// error code will contain the HTTP status code returned by the server
        /// </summary>
        WEBSOCKET_UPGRADE,
        /// <summary>
        /// There was an error while trying to send text/binary data to the server over
        /// the web socket connection
        /// </summary>
        WEBSOCKET_SEND_FRAME,
        /// <summary>
        /// There was an unexpected error while connecting to the web socket. The error
        /// code will contain the WS_ERROR value
        /// </summary>
        WEBSOCKET_ERROR,
        /// <summary>
        /// There was an error with doing a DNS lookup. This is only happens when you are
        /// using a DNS cache. The error code will be the DNS cache error value
        /// </summary>
        DNS_FAILURE
    };


    /// <summary>
    /// Possible HTTP status codes returned by the service for upgrade requests
    /// </summary>
    enum class HttpStatusCode
    {
        /// <summary>
        /// Success response
        /// </summary>
        OK = 200,
        /// <summary>
        /// This and all future requests should be directed to URL returned in the Location header in the response. HTTP method
        /// may change here.
        /// </summary>
        MOVED_PERMANENTLY = 301,
        /// <summary>
        /// This request should directed to the URL returned in the Location header in the response. Future requests may go to the
        /// original URL. HTTP method is not allowed to change here.
        /// </summary>
        TEMP_REDIRECT = 307,
        /// <summary>
        /// This request should directed to the URL returned in the Location header in the response. Future requests may go to the
        /// original URL. HTTP method is not allowed to change here.
        /// </summary>
        PERM_REDIRECT = 308,
        /// <summary>
        /// The request sent is invalid. Please double check you have all the required
        /// </summary>
        BAD_REQUEST = 400,
        /// <summary>
        /// The request is not authorized. Check the server response to see what authentication headers/query
        /// parameters are needed.
        /// </summary>
        UNAUTHORIZED = 401,
        /// <summary>
        /// The request is forbidden by the server. Please double check that you have the correct
        /// credentials
        /// </summary>
        FORBIDDEN = 403,
        /// <summary>
        /// The path used could not be found
        /// </summary>
        NOT_FOUND = 404,
        /// <summary>
        /// You have been throttled by the service. You should wait some time and try again
        /// </summary>
        TOO_MANY_REQUESTS = 429,
        /// <summary>
        /// The server crashed while processing the upgrade request. You should retry again later
        /// </summary>
        INTERNAL_ERROR = 500,
        /// <summary>
        /// The service is down or unavailable. You should retry again
        /// </summary>
        SERVICE_UNAVAILABLE = 503
    };


    /// <summary>
    /// Possible states for the web socket connection
    /// </summary>
    enum class WebSocketState
    {
        /// <summary>
        /// The web socket is closed
        /// </summary>
        CLOSED = 0,
        /// <summary>
        /// We start opening in the web socket connection here
        /// </summary>
        INITIAL,
        /// <summary>
        /// The web socket connection is opening
        /// </summary>
        OPENING,
        /// <summary>
        /// The web socket is connected
        /// </summary>
        CONNECTED,
        /// <summary>
        /// The web socket connection is being closed
        /// </summary>
        DESTROYING
    };


    /// <summary>
    /// Enumerations of reasons why the web socket was disconnected. These are as defined in RFC 6455
    /// section 7.4.1
    /// </summary>
    enum class WebSocketDisconnectReason : uint16_t
    {
        /// <summary>
        /// Unknown disconnect reason
        /// </summary>
        Unknown = UINT16_MAX,
        /// <summary>
        /// Normal closure. This could be requested by the client or the server
        /// </summary>
        Normal = 1000,
        /// <summary>
        /// Indicates an endpoint is being removed. Either the server or client will become unavailable.
        /// </summary>
        EndpointUnavailable = 1001,
        /// <summary>
        /// The client or server is terminating the connection because of a protocol error.
        /// </summary>
        ProtocolError = 1002,
        /// <summary>
        /// Endpoint is terminating the connection because it has received a type of data it cannot accept
        /// (e.g., an endpoint that understands only text data MAY send this if it receives a binary message).
        /// </summary>
        CannotAcceptDataType = 1003,
        /// <summary>
        /// The client or server is terminating the connection because it has received data inconsistent with
        /// the message type.
        /// </summary>
        InvalidPayloadData = 1007,
        /// <summary>
        /// The connection will be closed because an endpoint has received a message that violates its policy.
        /// </summary>
        PolicyViolation = 1008,
        /// <summary>
        /// Endpoint is terminating the connection because it has received a message that is too big for it
        /// to process.
        /// </summary>
        MessageTooBig = 1009,
        /// <summary>
        /// Client is terminating the connection because it has expected the server to negotiate one or more
        /// extensions, but the server didn't return them in the response message of the WebSocket handshake
        /// </summary>
        UnexpectedCondition = 1010,
        /// <summary>
        /// The connection will be closed by the server because of an error on the server.
        /// </summary>
        InternalServerError = 1011
    };

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
        /// Get the future that is completed when the message has been sent
        /// </summary>
        /// <returns>The future. A true result indicates success, false indicates failure</returns>
        virtual std::future<bool> MessageSent() = 0;

        /// <summary>
        /// Sets that the message has been sent
        /// </summary>
        /// <param>True if the message was sent successfully, false otherwise</param>
        virtual void MessageSent(bool success) = 0;

    protected:
        /// <summary>
        /// Constructor
        /// </summary>
        IWebSocketMessage() = default;

    private:
        DISABLE_COPY_AND_MOVE(IWebSocketMessage);
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
        /// Event raised when we encounter an error. The parameters in order are:
        /// * The web socket error encountered
        /// * An additional error code
        ///   - This can be the HTTP status code returned for the upgrade request
        ///   - This can be the internal uws_client error code
        /// * A string description of the error
        /// </summary>
        Impl::event<WebSocketError, int, const std::string&> OnError;
    };

} } } }

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <stdint.h>

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
enum class WebSocketDisconnectReason
{
    /// <summary>
    /// Unknown disconnect reason
    /// </summary>
    Unknown = std::numeric_limits<uint16_t>::max(),
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
    InternalServerError = 1011,
    /// <summary>
    /// Client is attempting to provide a locale that is not supported for the requested scenario.
    /// </summary>
    UnsupportedLocale = 4000,
};

}}}}

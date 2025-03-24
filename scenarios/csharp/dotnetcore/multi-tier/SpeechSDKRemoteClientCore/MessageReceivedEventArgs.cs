//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Event arguments raised when a message not handled by the client framework is received from the service.
    /// </summary>
    /// <typeparam name="MessageType">The type of message received from the service.
    /// This allows for different message formats based on the transport implementation
    /// (e.g., JSON objects for WebSocket, Protocol Buffers for gRPC).</typeparam>
    /// <remarks>
    /// This class is used to propagate service-specific messages that are not part of the core
    /// audio streaming protocol. These messages might include recognition results, status updates,
    /// or other service-specific notifications.
    /// </remarks>
    public sealed class MessageReceivedEventArgs<MessageType> : EventArgs
    {
        /// <summary>
        /// Gets the message received from the service.
        /// </summary>
        /// <remarks>
        /// The message type and format depend on the service implementation and transport protocol being used.
        /// Consumers should handle the message according to their specific service requirements.
        /// </remarks>
        public MessageType Message { get; }

        /// <summary>
        /// Initializes a new instance of the MessageReceivedEventArgs class.
        /// </summary>
        /// <param name="message">The message payload received from the service.</param>
        /// <exception cref="ArgumentNullException">Thrown when message is null.</exception>
        public MessageReceivedEventArgs(MessageType message)
        {
            Message = message ?? throw new ArgumentNullException(nameof(message));
        }
    }
}

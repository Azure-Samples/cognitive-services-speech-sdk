//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using SpeechSDKSamples.Remote.Core;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Represents the current state of a transport connection.
    /// </summary>
    public enum ConnectionState
    {
        /// <summary>
        /// The transport is not connected and no connection attempt is in progress.
        /// </summary>
        Disconnected,

        /// <summary>
        /// The transport is attempting to establish a connection.
        /// </summary>
        Connecting,

        /// <summary>
        /// The transport is connected and ready for audio transmission.
        /// </summary>
        Connected,

        /// <summary>
        /// The transport is in the process of closing the connection.
        /// </summary>
        Closing
    }

    /// <summary>
    /// Represents a transport layer that can send audio data and receive responses from a remote endpoint.
    /// </summary>
    /// <typeparam name="ResponseMessageType">The type of messages expected from the service.
    /// This allows for different message formats based on the transport implementation (e.g., JSON for WebSocket, Protocol Buffers for gRPC).</typeparam>
    public interface IStreamTransport<ResponseMessageType> : IDisposable
    {
        /// <summary>
        /// Gets the current connection state of the transport.
        /// </summary>
        ConnectionState State { get; }

        /// <summary>
        /// Gets whether the transport is ready to send audio data.
        /// </summary>
        /// <returns>True if the audio format is set and the connection is established.</returns>
        bool IsReadyForSending { get; }

        /// <summary>
        /// Sends audio data to the remote endpoint.
        /// </summary>
        /// <param name="data">The audio data to send. Can be empty to signal end of stream.</param>
        /// <param name="cancellationToken">Optional token to cancel the operation.</param>
        /// <returns>A task that completes when the data has been sent.</returns>
        /// <exception cref="ArgumentNullException">Thrown when data is null.</exception>
        /// <exception cref="InvalidOperationException">Thrown when audio format is not set or connection is not established.</exception>
        /// <exception cref="OperationCanceledException">Thrown when the operation is canceled.</exception>
        /// <exception cref="ConnectionFailedException">Thrown when the connection fails during send.</exception>
        Task SendAudioAsync(ReadOnlyMemory<byte> data, CancellationToken cancellationToken = default);

        /// <summary>
        /// Sets the audio format for the stream.
        /// </summary>
        /// <param name="format">The audio format specification.</param>
        /// <exception cref="ArgumentNullException">Thrown when format is null.</exception>
        /// <exception cref="ArgumentException">Thrown when format contains invalid values.</exception>
        /// <exception cref="InvalidOperationException">Thrown when called after streaming has started.</exception>
        void SetAudioFormat(AudioFormat format);

        /// <summary>
        /// Occurs when a message is received from the remote endpoint.
        /// </summary>
        /// <remarks>
        /// Subscribers should handle this event quickly and avoid long-running operations
        /// as they may block message processing. Consider dispatching to a separate task
        /// for time-consuming operations.
        /// </remarks>
        event EventHandler<MessageReceivedEventArgs<ResponseMessageType>> MessageReceived;

        /// <summary>
        /// Occurs when audio data has been acknowledged by the remote endpoint.
        /// </summary>
        /// <remarks>
        /// This event indicates that the remote endpoint has successfully processed
        /// the audio up to a certain point. This can be used to manage buffer cleanup
        /// and ensure reliable transmission.
        /// </remarks>
        event EventHandler<AudioAcknowledgedEventArgs> AudioAcknowledged;

        /// <summary>
        /// Occurs when the transport has stopped sending audio.
        /// </summary>
        /// <remarks>
        /// This event may be triggered due to normal completion, an error condition,
        /// or an explicit stop request. Check the StoppedEventArgs for the specific reason.
        /// </remarks>
        event EventHandler<StoppedEventArgs> Stopped;

        /// <summary>
        /// Establishes connection to the remote endpoint.
        /// </summary>
        /// <param name="cancellationToken">Optional token to cancel the operation.</param>
        /// <returns>A task that completes when the connection is established.</returns>
        /// <exception cref="ConnectionFailedException">Thrown when connection cannot be established.</exception>
        /// <exception cref="OperationCanceledException">Thrown when the operation is canceled.</exception>
        /// <exception cref="InvalidOperationException">Thrown when already connected or connecting.</exception>
        Task ConnectAsync(CancellationToken cancellationToken = default);

        /// <summary>
        /// Gracefully closes the connection to the remote endpoint.
        /// </summary>
        /// <param name="cancellationToken">Optional token to cancel the operation.</param>
        /// <returns>A task that completes when the connection is closed.</returns>
        /// <exception cref="InvalidOperationException">Thrown when not connected.</exception>
        /// <exception cref="OperationCanceledException">Thrown when the operation is canceled.</exception>
        Task CloseAsync(CancellationToken cancellationToken = default);
    }
}

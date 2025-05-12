//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.Core;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Represents a client that can send audio to a remote service.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This class is responsible for managing the connection to the remote service, and sending audio to it.
    /// It also contains a buffer for audio that has been sent but not yet acknowledged by the service.
    /// </para>
    /// <para>
    /// In the event the connection to the service encounters an error, the client will attempt to reconnect and resend audio that has not been acknowledged.
    /// </para>
    /// <para>
    /// The type of response messages are orthogonal to the classes use of them as this class is designed to work with multiple transport implementations.
    /// </para>
    /// </remarks>
    /// <typeparam name="ResponseMessageType">The type of the response messages from the service.</typeparam>
    public class SpeechSDKRemoteClient<ResponseMessageType> : IStreamTransport<ResponseMessageType>
    {
        /// <summary>
        /// A callback from the consumer code to property create a new instance of the underlying transport.
        /// </summary>
        private readonly Func<IStreamTransport<ResponseMessageType>> _streamTransportFactory;

        /// <summary>
        /// A createed instance of the underlying transport.
        /// </summary>
        private IStreamTransport<ResponseMessageType> _streamTransport;

        /// <summary>
        /// Lock for class wide operations on the underlying transport.
        /// </summary>
        private object _streamTransportLock = new object();

        /// <summary>
        /// A semaphore to manage establishment of a connection to the service.
        /// </summary>
        private SemaphoreSlim _connectionLock = new SemaphoreSlim(1);

        /// <summary>
        /// The last error that occurred when attempting to connect to the service.
        /// </summary>
        private Exception _lastConnectionError = null;
        private bool isConnected = false;

        /// <summary>
        /// Default values for connection attempts and backoff.
        /// </summary>
        private int _connectionsAttempted = 0;
        private int _maxConectionAttempts = 4;
        private int _connectionBackoffIntervalMS = 250;

        /// <summary>
        /// Used to stop the audio send loop.
        /// </summary>
        private CancellationTokenSource _transportSendLoopCancellation;
        private Task _transportSendLoop;

        /// <summary>
        /// Audio format for the source audio.
        /// </summary>
        private AudioFormat _audioFormat;

        /// <summary>
        /// Audio replay buffer.
        /// </summary>
        private PCMAudioBuffer _audioBuffer;

        public ConnectionState State => _streamTransport.State;

        public bool IsReadyForSending => _streamTransport.IsReadyForSending;

        /// <summary>
        /// Event raised when a message from the service isn't handled by this client.
        /// </summary>
        public event EventHandler<MessageReceivedEventArgs<ResponseMessageType>> MessageReceived;

        /// <summary>
        /// And event that is raised when the service acknowledges audio processing is complete.
        /// </summary>
        public event EventHandler<AudioAcknowledgedEventArgs> AudioAcknowledged;

        /// <summary>
        /// Event raised when recogniton has stopped.
        /// </summary>
        /// <remarks>
        /// Inspect the event arguments to see the reason for recognition stopping.
        /// </remarks>
        public event EventHandler<StoppedEventArgs> Stopped;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="streamTransportFactory"></param>
        public SpeechSDKRemoteClient(Func<IStreamTransport<ResponseMessageType>> streamTransportFactory)
        {
            _streamTransportFactory = streamTransportFactory;
        }

        /// <summary>
        /// Event handler for when the underlying transport stops.
        /// </summary>
        /// <param name="sender">Underlying transport</param>
        /// <param name="e">Event args</param>
        /// <remarks>
        /// <para>This method handles stop signals from the underlying transport. If the stop was due to an error, the client will attempt to reconnect and resend audio that has not been acknowledged.
        /// The reconnection logic owns counting the number of attempts and determining if the connection should be retried.
        /// </para>
        /// <para>If the reason for the underlying transport stopping was not and error, or an error occurs during the reconnect process the Stopped event will be raised.</para>
        /// </remarks>
        private void _streamTransport_OnStopped(object sender, StoppedEventArgs e)
        {
            if (e.Reason != RecognitionStoppedReason.Error)
            {
                Stopped?.Invoke(this, e);
                return;
            }

            isConnected = false;

            // We need to reconnect.
            // To avoid deadlocking with ourselves, we need to run the restart on a separate task.

            _transportSendLoopCancellation?.Cancel();

            Task.Run(() =>
            {
                try
                {
                    _transportSendLoop?.Wait();
                    lock (_streamTransportLock)
                    {
                        _streamTransport?.Dispose();
                        _streamTransport = null;
                    }
                    EnsureAudioBuffer();
                    _audioBuffer.NewTurn();
                    EnsureTransportRunning(default).Wait();
                }
                catch (Exception ex)
                {
                    Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.Error, ex));
                }
            });
        }

        /// <summary>
        /// Event handler for when data is received from the underlying transport.
        /// </summary>
        /// <param name="sender">The underlying transport</param>
        /// <param name="e">Event args</param>
        /// <remarks>
        /// <para>This handler will process and AcknowledgedAudio message, and for any other message pass it to the caller via the MessageReceived event.</para>
        /// </remarks>
        private void _streamTransport_OnDataReceived(object sender, MessageReceivedEventArgs<ResponseMessageType> e) => MessageReceived?.Invoke(this, e);

        /// <summary>
        /// Event handler for when the underlying transport acknowledges audio.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">Event args containing a TimeSpan with the acknowledged audio length.</param>
        private void _streamTransport_AudioAcknowledged(object sender, AudioAcknowledgedEventArgs e)
        {
            _audioBuffer.DiscardTill(e.AcknowledgedAudio);
            _connectionsAttempted = 0;
        }

        /// <summary>
        /// Sends an audio chunk to the remote service.
        /// </summary>
        /// <param name="data">The audio to send.</param>
        /// <param name="cancellationToken">A cancellation token for this call.</param>
        /// <returns>On queueing of the audio data</returns>
        /// <remarks>
        /// Since this method returns when the data has been queued, it is not a gaurentee that the audio has been sent, or that it has been
        /// received or processed.
        /// </remarks>
        public async Task SendAudioAsync(ReadOnlyMemory<byte> data, CancellationToken cancellationToken = default)
        {
            EnsureAudioBuffer();

            _audioBuffer.Add(new DataChunk(data, DateTime.UtcNow));

            await EnsureTransportRunning(cancellationToken).ConfigureAwait(false);
        }

        /// <summary>
        /// Connects to the remote service.
        /// </summary>
        /// <param name="cancellationToken">Cancellation Token</param>
        /// <returns>A task that completes when a connection has been opened.</returns>
        public async Task ConnectAsync(CancellationToken cancellationToken = default)
        {
            _connectionsAttempted = 0;
            _lastConnectionError = null;
            await EnsureStreamConnectedAsync(cancellationToken).ConfigureAwait(false);
        }

        /// <summary>
        /// Closes the connection to the remote service.
        /// </summary>
        /// <param name="cancellationToken">A CancellationToken</param>
        /// <returns></returns>
        public Task CloseAsync(CancellationToken cancellationToken = default)
        {
            return _streamTransport.CloseAsync(cancellationToken);
        }

        /// <summary>
        /// Ensures that a stream transport has been created and the proper events are wired up.
        /// </summary>
        private void EnsureStreamTransport()
        {
            lock (_streamTransportLock)
            {
                if (_streamTransport == null)
                {
                    _streamTransport = _streamTransportFactory();
                    _streamTransport.MessageReceived += _streamTransport_OnDataReceived;
                    _streamTransport.Stopped += _streamTransport_OnStopped;
                    _streamTransport.AudioAcknowledged += _streamTransport_AudioAcknowledged;
                }
            }
        }

        /// <summary>
        /// Ensures the underlying transport is connected.
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        /// <exception cref="ConnectionFailedException">Thrown when the number of connections attempts without forward progress exceeds settings.</exception>
        private async Task EnsureStreamConnectedAsync(CancellationToken cancellationToken)
        {
            EnsureStreamTransport();

            await _connectionLock.WaitAsync(cancellationToken).ConfigureAwait(false);

            try
            {
                if (isConnected)
                {
                    return;
                }

                while (_connectionsAttempted++ < _maxConectionAttempts)
                {
                    try
                    {
                        _lastConnectionError = null;
                        await _streamTransport.ConnectAsync(cancellationToken).ConfigureAwait(false);
                        isConnected = true;
                        break;
                    }
                    catch (Exception e)
                    {
                        _lastConnectionError = e;
                    }

                    await Task.Delay(TimeSpan.FromMilliseconds(_connectionBackoffIntervalMS * _connectionsAttempted), cancellationToken).ConfigureAwait(false);
                }

                if (_connectionsAttempted > _maxConectionAttempts)
                {
                    throw new ConnectionFailedException($"{_connectionsAttempted} connection attempts failed.", _lastConnectionError);
                }
            }
            finally
            {
                _connectionLock.Release();
            }
        }

        /// <summary>
        /// Ensures that the transport layer is connected and running a message loop.
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns>A task the completes once the messaging layer is running.</returns>
        private async Task EnsureTransportRunning(CancellationToken cancellationToken)
        {
            if (null == _transportSendLoop || _transportSendLoop.IsCompleted == true)
            {
                await EnsurePreAudioMessages(cancellationToken).ConfigureAwait(false);

                _transportSendLoopCancellation = new CancellationTokenSource();
                _transportSendLoop = Task.Run(async () =>
                {
                    try
                    {
                        while (!_transportSendLoopCancellation.IsCancellationRequested)
                        {
                            DataChunk data = null;
                            if (null != (data = _audioBuffer.GetNext()))
                            {
                                await _streamTransport.SendAudioAsync(data.Data, _transportSendLoopCancellation.Token).ConfigureAwait(false);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    catch (OperationCanceledException)
                    {
                        await _streamTransport.CloseAsync();
                    }
                    catch (Exception ex)
                    {
                        _streamTransport_OnStopped(this, new StoppedEventArgs(RecognitionStoppedReason.Error, ex));
                    }
                });
            }
        }

        /// <summary>
        /// Ensures that the audio format has been sent to the remote service.
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns>A <see cref="Task"/> that completes when the audio format has been sent.</returns>
        private async Task EnsurePreAudioMessages(CancellationToken cancellationToken)
        {
            await EnsureStreamConnectedAsync(cancellationToken).ConfigureAwait(false);
            _streamTransport.SetAudioFormat(_audioFormat);
        }

        private void EnsureAudioBuffer()
        {
            if (_audioBuffer == null)
            {
                _audioBuffer = new PCMAudioBuffer(_audioFormat);
            }
        }

        /// <summary>
        /// Sets the audio format sent to the service.
        /// </summary>
        /// <param name="format"></param>
        public void SetAudioFormat(AudioFormat format)
        {
            _audioFormat = format;
        }

        public void Dispose()
        {
            _streamTransport.Dispose();
        }
    }
}

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.WebSocket.Shared;
using System;
using System.Collections.Generic;
using System.Net.WebSockets;
using System.Threading;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Client.WebSocket
{
    /// <summary>
    /// A WebSocket based implementation of the IStreamTransport interface.
    /// </summary>
    public class WebSocketTransport : IStreamTransport<IMessage>, IDisposable
    {
        /// <inheritdoc/>
        public ConnectionState State
        {
            get
            {
                if (_clientWebSocket == null)
                    return ConnectionState.Disconnected;

                switch (_clientWebSocket.State)
                {
                    case WebSocketState.Connecting:
                        return ConnectionState.Connecting;
                    case WebSocketState.Open:
                        return ConnectionState.Connected;
                    case WebSocketState.CloseSent:
                    case WebSocketState.CloseReceived:
                        return ConnectionState.Closing;
                    default:
                        return ConnectionState.Disconnected;
                }
            }
        }

        /// <inheritdoc/>
        public bool IsReadyForSending => _format.BitsPerSample > 0 && _format.SamplesPerSecond > 0 && _format.ChannelCount > 0 && State == ConnectionState.Connected;

        private Uri _endpoint;
        private ClientWebSocket _clientWebSocket;
        private AudioFormat _format;
        private bool _formatSent = false;
        private CancellationTokenSource _stopTokenSource = new CancellationTokenSource();

        private Task _receiveTask;
        private bool disposedValue;

        public WebSocketTransport(Uri endpoint)
        {
            _endpoint = endpoint;
        }

        /// <inheritdoc/>
        public event EventHandler<MessageReceivedEventArgs<IMessage>> MessageReceived;

        /// <inheritdoc/>
        public event EventHandler<StoppedEventArgs> Stopped;

        /// <inheritdoc/>
        public event EventHandler<AudioAcknowledgedEventArgs> AudioAcknowledged;

        /// <inheritdoc/>
        public async Task CloseAsync(CancellationToken cancellationToken)
        {
            await _clientWebSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Closing", cancellationToken);
            _stopTokenSource.Cancel();
            _clientWebSocket = null;
            _formatSent = false;
        }

        /// <inheritdoc/>
        public async Task ConnectAsync(CancellationToken cancellationToken)
        {
            _clientWebSocket = new ClientWebSocket();
            await _clientWebSocket.ConnectAsync(_endpoint, cancellationToken);

            _receiveTask = Task.Run(ReceiveLoop);
        }

        private async Task ReceiveLoop()
        {
            var buffer = new byte[1024];
            var messageBuffer = new List<byte>();

            try
            {
                while (_clientWebSocket.State == WebSocketState.Open)
                {
                    WebSocketReceiveResult result;
                    do
                    {
                        result = await _clientWebSocket.ReceiveAsync(new ArraySegment<byte>(buffer), _stopTokenSource.Token);
                        if (result.MessageType == WebSocketMessageType.Close)
                        {
                            if (_clientWebSocket.State == WebSocketState.CloseReceived)
                            {
                                await _clientWebSocket.CloseOutputAsync(WebSocketCloseStatus.NormalClosure, "", CancellationToken.None);
                                Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.Error));
                            }
                            return;
                        }

                        messageBuffer.AddRange(new ArraySegment<byte>(buffer, 0, result.Count));
                    } while (!result.EndOfMessage);

                    var messageArray = messageBuffer.ToArray();
                    messageBuffer.Clear();

                    var message = MessageUtility.DeserializeMessage(messageArray);
                    switch (message)
                    {
                        case AudioAcknowledgedMessage audioAcknowledgedMessage:
                            AudioAcknowledged?.Invoke(this, new AudioAcknowledgedEventArgs(audioAcknowledgedMessage.AcknowledgedAudio));
                            break;
                        case RecognitionStoppedMessage recognitionStoppedMessage:
                            var stoppedArgs = new StoppedEventArgs(recognitionStoppedMessage.Reason);
                            Stopped?.Invoke(this, stoppedArgs);
                            break;
                        default:
                            MessageReceived?.Invoke(this, new MessageReceivedEventArgs<IMessage>(message));
                            break;
                    }
                }
            }
            catch (WebSocketException ex)
            {
                // Log the exception or handle it as needed
                Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.Error, ex));
            }
            catch (OperationCanceledException)
            {
                // This exception is expected when the operation is canceled.
            }
            catch (Exception ex)
            {
                // Catch any other unexpected exceptions
                Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.Error, ex));
            }
            finally
            {
                if (_clientWebSocket != null && _clientWebSocket.State == WebSocketState.Open || _clientWebSocket.State == WebSocketState.CloseReceived)
                {
                    await _clientWebSocket.CloseAsync(WebSocketCloseStatus.InternalServerError, "Closing due to error", CancellationToken.None);
                }
            }
        }

        /// <inheritdoc/>
        public async Task SendAudioAsync(ReadOnlyMemory<byte> data, CancellationToken cancellationToken)
        {
            if (!_formatSent)
            {
                await SendAudioFormatAsync(_format, cancellationToken);
                _formatSent = true;
            }

            var audioMessage = new AudioMessage(data);
            var message = await MessageUtility.SerializeMessageAsync(audioMessage);
            await _clientWebSocket.SendAsync(message, WebSocketMessageType.Binary, true, cancellationToken);
        }

        /// <inheritdoc/>
        public void SetAudioFormat(AudioFormat format)
        {
            _format = format;
        }

        private async Task SendAudioFormatAsync(AudioFormat format, CancellationToken cancellationToken)
        {
            var audioFormatMessage = new AudioFormatMessage()
            {
                BitsPerSample = format.BitsPerSample,
                ChannelCount = format.ChannelCount,
                SamplesPerSecond = format.SamplesPerSecond
            };

            var message = await MessageUtility.SerializeMessageAsync(audioFormatMessage);

            await _clientWebSocket.SendAsync(message, WebSocketMessageType.Text, true, cancellationToken);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {

                    if (_clientWebSocket?.State == WebSocketState.Open)
                    {
                        _clientWebSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Closing", CancellationToken.None).Wait();
                    }

                    _stopTokenSource?.Cancel();
                    _receiveTask?.Wait();
                    _receiveTask?.Dispose();
                    _stopTokenSource?.Dispose();
                    _clientWebSocket?.Dispose();
                }

                disposedValue = true;
            }
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
    }
}

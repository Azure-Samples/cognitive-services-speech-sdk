//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Grpc.Core;
using Grpc.Net.Client;
using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.Service.GRPC;
using System;
using System.Net;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Client.GRPC
{
    /// <summary>
    /// A GRPC based implementation of the IStreamTransport interface.
    /// </summary>
    public class GRPCTransport : IStreamTransport<SpeechSDKRemoteResponse>, IDisposable
    {
        /// <inheritdoc/>
        public ConnectionState State
        {
            get
            {
                if (_channel == null)
                    return ConnectionState.Disconnected;
                if (_call == null)
                    return ConnectionState.Disconnected;

                try
                {
                    var state = _call.GetStatus();
                    if (state.StatusCode == StatusCode.OK)
                        return ConnectionState.Connected;
                    return ConnectionState.Disconnected;
                }
                catch
                {
                    return ConnectionState.Disconnected;
                }
            }
        }

        /// <inheritdoc/>
        public bool IsReadyForSending => _format.BitsPerSample > 0 && _format.SamplesPerSecond > 0 && _format.ChannelCount > 0 && State == ConnectionState.Connected;

        private Uri _endpoint;
        private GrpcChannel _channel = null;
        private SpeechSDKRemoteService.SpeechSDKRemoteServiceClient _client;
        private AsyncDuplexStreamingCall<SpeechSDKRemoteRequest, SpeechSDKRemoteResponse> _call;
        private AudioFormat _format;
        private bool _formatSent = false;
        private CancellationTokenSource _stopTokenSource = new CancellationTokenSource();
        private Task _receiveTask;
        private bool disposedValue;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="endpoint"></param>
        public GRPCTransport(Uri endpoint)
        {
            _endpoint = endpoint;
        }

        /// <summary>
        /// An event that is raised when a message is received from the remote service that is not handled / processed by this class.
        /// </summary>
        public event EventHandler<MessageReceivedEventArgs<SpeechSDKRemoteResponse>> MessageReceived;

        /// <summary>
        /// An event that is raised when the recognition is stopped.
        /// </summary>
        public event EventHandler<StoppedEventArgs> Stopped;

        /// <summary>
        /// An event that is raised when the remote service has acknowledged audio up to a certain offset.
        /// </summary>
        public event EventHandler<AudioAcknowledgedEventArgs> AudioAcknowledged;

        /// <summary>
        /// Closes the underling connection to the endpoint.
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        /// <exception cref="NotImplementedException"></exception>
        public Task CloseAsync(CancellationToken cancellationToken)
        {
            _stopTokenSource.Cancel();
            return Task.CompletedTask;
        }

        /// <summary>
        /// Connects to the remote endpoint.
        /// </summary>
        /// <param name="cancellation">Cancellation Token</param>
        /// <returns>A task that completes when the connection has been established.</returns>
        /// <remarks>
        /// ConnectAsync will open a connection to the remote endpoint and prepare to listen for response messages. If existing the 
        /// </remarks>
        public Task ConnectAsync(CancellationToken cancellation)
        {
            if (_channel == null)
            {
                _channel = GrpcChannel.ForAddress(_endpoint, new GrpcChannelOptions
                {
                    HttpClient = new HttpClient(new HttpClientHandler
                    {
                        AutomaticDecompression = DecompressionMethods.None
                    }),
                    DisposeHttpClient = true
                });
            }

            _client = new SpeechSDKRemoteService.SpeechSDKRemoteServiceClient(_channel);
            _call = _client.ProcessRemoteSpeechAudio(null, null, _stopTokenSource.Token);
            _receiveTask = Task.Run(async () =>
            {
                try
                {
                    await foreach (var response in _call.ResponseStream.ReadAllAsync(_stopTokenSource.Token))
                    {
                        switch (response.ResponseCase)
                        {
                            case SpeechSDKRemoteResponse.ResponseOneofCase.AcknowledgedAudio:
                                AudioAcknowledged?.Invoke(this, new AudioAcknowledgedEventArgs(response.AcknowledgedAudio.AcknowledgedAudio.ToTimeSpan()));
                                break;

                            case SpeechSDKRemoteResponse.ResponseOneofCase.RecognitionStopped:
                                var message = response.RecognitionStopped;
                                if (null != message)
                                {
                                    switch (message.Reason)
                                    {
                                        case recognitionStoppedReason.StoppRequested:
                                            Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.StopRequested));
                                            break;
                                        case recognitionStoppedReason.EndOfStream:
                                            Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.EndOfStream));
                                            break;
                                        case recognitionStoppedReason.Error:
                                            Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.Error, new Exception(message.ErrorMessage)));
                                            break;
                                        default:
                                            Stopped?.Invoke(this, new StoppedEventArgs(RecognitionStoppedReason.Error, new Exception("Unknown reason")));
                                            break;
                                    }
                                }
                                break;
                            default:
                                MessageReceived?.Invoke(this, new MessageReceivedEventArgs<SpeechSDKRemoteResponse>(response));
                                break;
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                }
            });

            return Task.CompletedTask;
        }

        /// <summary>
        /// Sends a chunk of audio to the remote endpoint.
        /// </summary>
        /// <param name="data">Audio data to send</param>
        /// <param name="cancellationToken">Cancellation Token</param>
        /// <returns>A Task that completes when the audio chunk has been sent.</returns>
        public async Task SendAudioAsync(ReadOnlyMemory<byte> data, CancellationToken cancellationToken)
        {
            if (!_formatSent)
            {
                await SendAudioFormatAsync(cancellationToken);
                _formatSent = true;
            }

            var message = new SpeechSDKRemoteRequest()
            {
                AudioPayload = Google.Protobuf.ByteString.CopyFrom(data.Span),
            };

            await _call.RequestStream.WriteAsync(message, cancellationToken);
        }

        private async Task SendAudioFormatAsync(CancellationToken cancellationToken)
        {
            var formatMessage = new audioFormat()
            {
                BitsPerSample = Convert.ToUInt32(_format.BitsPerSample),
                ChannelCount = Convert.ToUInt32(_format.ChannelCount),
                SamplesPerSecond = Convert.ToUInt32(_format.SamplesPerSecond)
            };

            var message = new SpeechSDKRemoteRequest()
            {
                AudioFormat = formatMessage,
            };

            await _call.RequestStream.WriteAsync(message, cancellationToken);
        }

        /// <summary>
        /// Sets the audio format that this connection will use.
        /// </summary>
        /// <param name="format">An <see cref="AudioFormat"/> object that describes the audio format.</param>
        public void SetAudioFormat(AudioFormat format)
        {
            _format = format;
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    _call?.RequestStream.CompleteAsync().Wait();
                    _channel?.ShutdownAsync().Wait();
                    _receiveTask?.Wait();
                    _call?.Dispose();
                    _channel?.Dispose();
                    _stopTokenSource?.Cancel();
                    
                    _receiveTask?.Dispose();
                    _stopTokenSource?.Dispose();

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

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Client.GRPC;
using SpeechSDKSamples.Remote.Service.GRPC;
using SpeechSDKSamples.Remote.Client.WebSocket;
using SpeechSDKSamples.Remote.WebSocket.Shared;
using SpeechSDKSamples.Remote.Core;

namespace SpeechSDKSamples.Remote.Client.App
{
    /// <summary>
    /// A client application that streams audio files to the service for speech recognition.
    /// </summary>
    /// <typeparam name="ResponseMessageType">The type of response message expected from the service. 
    /// Use <see cref="int"/> for WebSocket transport or <see cref="SpeechSDKRemoteResponse"/> for gRPC transport.</typeparam>
    /// <remarks>
    /// This generic class supports multiple transport types for the sample. You can use the included WebSocket or gRPC transports,
    /// or implement your own by creating a new transport that implements <see cref="IStreamTransport{T}"/>.
    /// </remarks>
    internal class AudioStreamSource<ResponseMessageType> : IAudioStreamTransport, IDisposable
    {
        private bool _disposed;
        private readonly TaskCompletionSource _completionSource;
        private IStreamTransport<ResponseMessageType> _streamTransport;

        public AudioStreamSource()
        {
            _completionSource = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
        }

        /// <summary>
        /// Sends the specified audio file to the specified endpoint using the configured transport.
        /// </summary>
        /// <param name="fileName">Path to the audio file to send.</param>
        /// <param name="endpoint">Service endpoint URL.</param>
        /// <returns>A task that completes when the file has been fully processed.</returns>
        /// <exception cref="ArgumentNullException">Thrown when fileName or endpoint is null.</exception>
        /// <exception cref="ArgumentException">Thrown when fileName is empty or endpoint is invalid.</exception>
        /// <exception cref="FileNotFoundException">Thrown when the audio file does not exist.</exception>
        /// <exception cref="InvalidOperationException">Thrown when the audio file format is invalid or when transport creation fails.</exception>
        /// <exception cref="ObjectDisposedException">Thrown when this instance has been disposed.</exception>
        public async Task SendAudio(string fileName, Uri endpoint)
        {
            if (_disposed)
                throw new ObjectDisposedException(nameof(AudioStreamSource<ResponseMessageType>));
            
            if (string.IsNullOrEmpty(fileName))
                throw new ArgumentException("File name cannot be empty", nameof(fileName));
            
            if (endpoint == null)
                throw new ArgumentNullException(nameof(endpoint));

            if (!File.Exists(fileName))
                throw new FileNotFoundException("Audio file not found", fileName);

            try
            {
                _streamTransport = new SpeechSDKRemoteClient<ResponseMessageType>(() => CreateTransport(endpoint));

                _streamTransport.MessageReceived += (sender, e) =>
                {
                    // Handle the message received from the service.
                    Console.WriteLine($"Received message: {e.Message}");
                };

                _streamTransport.Stopped += (sender, e) =>
                {
                    var message = "Recognition stopped due to ";

                    message += e.Reason switch
                    {
                        RecognitionStoppedReason.EndOfStream => "end of the file.",
                        RecognitionStoppedReason.Error => $"an error occurred. {e.Exception}",
                        RecognitionStoppedReason.StopRequested => "stop was requested.",
                        _ => "an unknown reason."
                    };

                    Console.WriteLine(message);
                };

                await _streamTransport.ConnectAsync().ConfigureAwait(false);

                try
                {
                    using (var audioFileReader = new BinaryReader(File.OpenRead(fileName)))
                    {
                        var audioFormat = WavFileUtilities.ReadWaveHeader(audioFileReader);
                        if (audioFormat.AverageBytesPerSecond <= 0)
                            throw new InvalidOperationException("Failed to read WAV file header");

                        // Calculate buffer size to represent 100ms of audio
                        int bufferSize = (int)(audioFormat.AverageBytesPerSecond * 0.1);
                        byte[] buffer = new byte[bufferSize];

                        int bytesRead;
                        while ((bytesRead = audioFileReader.Read(buffer, 0, buffer.Length)) > 0)
                        {
                            if (_disposed)
                                throw new ObjectDisposedException(nameof(AudioStreamSource<ResponseMessageType>));

                            var audioData = bytesRead < buffer.Length ? buffer.AsMemory(0, bytesRead) : buffer;
                            await _streamTransport.SendAudioAsync(audioData).ConfigureAwait(false);

                            // Delay to simulate real-time microphone input
                            await Task.Delay(TimeSpan.FromMilliseconds(100)).ConfigureAwait(false);
                        }
                    }

                    // Signal completion when the entire file has been processed
                    _completionSource.TrySetResult();
                    await _completionSource.Task.ConfigureAwait(false);
                }
                catch (Exception ex)
                {
                    _completionSource.TrySetException(ex);
                    throw;
                }
                finally
                {
                    if (_streamTransport != null)
                        await _streamTransport.CloseAsync().ConfigureAwait(false);
                }
            }
            catch
            {
                _completionSource.TrySetCanceled();
                throw;
            }
        }

        /// <summary>
        /// Creates a transport instance based on the response message type.
        /// </summary>
        /// <param name="endpoint">The service endpoint URL.</param>
        /// <returns>An initialized transport instance.</returns>
        /// <exception cref="ArgumentException">Thrown when the response type doesn't match a supported transport.</exception>
        private IStreamTransport<ResponseMessageType> CreateTransport(Uri endpoint)
        {
            if (typeof(ResponseMessageType) == typeof(int))
            {
                return (IStreamTransport<ResponseMessageType>)new WebSocketTransport(endpoint);
            }
            else if (typeof(ResponseMessageType) == typeof(SpeechSDKRemoteResponse))
            {
                return (IStreamTransport<ResponseMessageType>)new GRPCTransport(endpoint);
            }
            else
            {
                throw new ArgumentException($"Unsupported response type: {typeof(ResponseMessageType).Name}");
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (_disposed)
                return;

            if (disposing)
            {
                if (_streamTransport is IDisposable disposable)
                {
                    disposable.Dispose();
                }
                _completionSource.TrySetCanceled();
            }

            _disposed = true;
        }

        ~AudioStreamSource()
        {
            Dispose(false);
        }
    }
}

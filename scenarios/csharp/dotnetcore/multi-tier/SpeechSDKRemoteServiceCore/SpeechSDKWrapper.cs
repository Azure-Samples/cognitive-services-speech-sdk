//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.Extensions.Logging;
using SpeechSDKSamples.Remote.Core;
using System;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Service.Core
{
/// <summary>
/// Provides a wrapper around the Microsoft Cognitive Services Speech SDK for continuous speech recognition
/// with support for streaming audio input and event-based recognition status updates.
/// </summary>
public class SpeechSDKWrapper : IDisposable
    {
        private SpeechRecognizer _speechRecognizer;
        private PushAudioInputStream _inputStream;
        private SpeechConfig _speechConfig;
        private AudioFormat _format;
        private IResponseTransport _responseTransport;
        private ILogger _logger;

        private bool _disposed;

        /// <summary>
        /// Occurs when speech is being recognized before final result is determined
        /// </summary>
        public event EventHandler<SpeechRecognitionEventArgs> SDKRecognizing;

        /// <summary>
        /// Occurs when speech recognition is finalized with a result
        /// </summary>
        public event EventHandler<SpeechRecognitionEventArgs> SDKRecognized;

        /// <summary>
        /// Occurs when a recognition session starts
        /// </summary>
        public event EventHandler<SessionEventArgs> SDKSessionStarted;

        /// <summary>
        /// Occurs when a recognition session stops
        /// </summary>
        public event EventHandler<SessionEventArgs> SDKSessionStopped;

        /// <summary>
        /// Occurs when recognition is canceled
        /// </summary>
        public event EventHandler<SpeechRecognitionCanceledEventArgs> SDKCanceled;

        /// <summary>
        /// Occurs when speech is detected in the audio stream
        /// </summary>
        public event EventHandler<RecognitionEventArgs> SDKSpeechStartDetected;

        /// <summary>
        /// Occurs when the end of speech is detected in the audio stream
        /// </summary>
        public event EventHandler<RecognitionEventArgs> SDKSpeechEndDetected;

        public SpeechSDKWrapper(SpeechConfig config, AudioFormat format, IResponseTransport responseTransport, ILogger logger)
        {
            _speechConfig = config ?? throw new ArgumentNullException(nameof(config));
            _format = format;
            _responseTransport = responseTransport ?? throw new ArgumentNullException(nameof(responseTransport));
            _logger = logger;

            if (format.SamplesPerSecond <= 0)
                throw new ArgumentException("SamplesPerSecond must be greater than 0", nameof(format));
            if (format.BitsPerSample <= 0)
                throw new ArgumentException("BitsPerSample must be greater than 0", nameof(format));
            if (format.ChannelCount <= 0)
                throw new ArgumentException("ChannelCount must be greater than 0", nameof(format));

            var sdkAudioFormat = AudioStreamFormat.GetWaveFormatPCM(Convert.ToUInt32(_format.SamplesPerSecond),
                                                                    Convert.ToByte(_format.BitsPerSample),
                                                                    Convert.ToByte(_format.ChannelCount));

            _inputStream = new PushAudioInputStream(sdkAudioFormat);

            var audioConfig = AudioConfig.FromStreamInput(_inputStream);

            _speechRecognizer = new SpeechRecognizer(_speechConfig, audioConfig);
            _speechRecognizer.Canceled += _speechRecognizer_Canceled;
            _speechRecognizer.Recognized += _speechRecognizer_Recognized;
            _speechRecognizer.Recognizing += _speechRecognizer_Recognizing;
            _speechRecognizer.SessionStarted += _speechRecognizer_SessionStarted;
            _speechRecognizer.SessionStopped += _speechRecognizer_SessionStopped;
            _speechRecognizer.SpeechStartDetected += _speechRecognizer_SpeechStartDetected;
            _speechRecognizer.SpeechEndDetected += _speechRecognizer_SpeechEndDetected;
        }

        /// <summary>
        /// Processes received audio data by writing it to the input stream for recognition.
        /// </summary>
        /// <param name="data">The audio data to process</param>
        /// <exception cref="ArgumentNullException">Thrown when data is null</exception>
        public void OnAudioReceived(ReadOnlyMemory<byte> data)
        {
            if (_disposed)
                throw new ObjectDisposedException(nameof(SpeechSDKWrapper));

            try
            {
                _inputStream.Write(data.ToArray());
            }
            catch (Exception ex)
            {
                throw new InvalidOperationException("Failed to write audio data to input stream", ex);
            }
        }

        private void _speechRecognizer_SpeechStartDetected(object sender, RecognitionEventArgs e)
        {
            SDKSpeechStartDetected?.Invoke(this, e);
        }

        private void _speechRecognizer_SpeechEndDetected(object sender, RecognitionEventArgs e)
        {
            SDKSpeechEndDetected?.Invoke(this, e);
        }

        private void _speechRecognizer_SessionStopped(object sender, SessionEventArgs e)
        {
            _logger?.LogInformation("Session stopped");
            SDKSessionStopped?.Invoke(this, e);
        }

        private void _speechRecognizer_SessionStarted(object sender, SessionEventArgs e)
        {
            SDKSessionStarted?.Invoke(this, e);
        }

        private void _speechRecognizer_Recognizing(object sender, SpeechRecognitionEventArgs e)
        {
            SDKRecognizing?.Invoke(this, e);
        }

        private void _speechRecognizer_Recognized(object sender, SpeechRecognitionEventArgs e)
        {
            var offset = TimeSpan.FromTicks(e.Result.OffsetInTicks) + e.Result.Duration;

            _responseTransport.AcknowledgeAudioAsync(offset);

            SDKRecognized?.Invoke(this, e);
        }

        private void _speechRecognizer_Canceled(object sender, SpeechRecognitionCanceledEventArgs e)
        {
            RecognitionStoppedReason reason;
            string errorMessage = string.Empty;
            int errorCode = 0;

            switch (e.Reason)
            {
                case CancellationReason.CancelledByUser:
                    reason = RecognitionStoppedReason.StopRequested;
                    break;
                case CancellationReason.Error:
                    reason = RecognitionStoppedReason.Error;
                    var details = CancellationDetails.FromResult(e.Result);
                    errorCode = (int)details.ErrorCode;
                    errorMessage = details.ErrorDetails;
                    break;
                case CancellationReason.EndOfStream:
                    reason = RecognitionStoppedReason.EndOfStream;
                    break;
                default:
                    reason = RecognitionStoppedReason.Unknown;
                    errorMessage = $"Unknown stop reason {e.Reason}";
                    break;
            }

            _logger?.LogInformation($"Recognition Stopped due to {reason} {errorMessage}");
            _responseTransport.StopRecognitionAsync(reason, errorCode, errorMessage).ConfigureAwait(false).GetAwaiter().GetResult();
            SDKCanceled?.Invoke(this, e);
        }

        /// <summary>
        /// Starts continuous speech recognition.
        /// </summary>
        /// <returns>A task that completes when recognition has started</returns>
        public async Task StartRecognitionAsync()
        {
            if (_disposed)
                throw new ObjectDisposedException(nameof(SpeechSDKWrapper));

            await _speechRecognizer.StartContinuousRecognitionAsync();
        }

        /// <summary>
        /// Stops the continuous speech recognition and notifies the client.
        /// </summary>
        /// <returns>A task that completes when recognition has stopped and client has been notified</returns>
        public async Task StopRecognition()
        {
            if (_disposed)
                throw new ObjectDisposedException(nameof(SpeechSDKWrapper));

            var stopTask = _speechRecognizer.StopContinuousRecognitionAsync();
            var informTask = _responseTransport.StopRecognitionAsync(RecognitionStoppedReason.StopRequested, 0, string.Empty);

            await Task.WhenAll(stopTask, informTask).ConfigureAwait(false);
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
                _speechRecognizer?.Dispose();
                _inputStream?.Dispose();
            }

            _disposed = true;
        }

        ~SpeechSDKWrapper()
        {
            Dispose(false);
        }
    }
}

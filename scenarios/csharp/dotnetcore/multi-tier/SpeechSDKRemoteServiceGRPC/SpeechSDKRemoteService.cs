//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Google.Protobuf;
using Grpc.Core;
using Microsoft.CognitiveServices.Speech;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.Service.Core;

namespace SpeechSDKSamples.Remote.Service.GRPC.Services
{
    /// <summary>
    /// A GRPC service that will process audio streams and send them to the Speech SDK for processing.
    /// </summary>
    public class SpeechSDKService : SpeechSDKRemoteService.SpeechSDKRemoteServiceBase
    {
        private readonly ILogger<SpeechSDKService> _logger;
        private readonly IConfigurationService _configService;
        private readonly CancellationTokenSource _cancellationSource = new CancellationTokenSource();
        private readonly IHostApplicationLifetime _lifetime;

        public SpeechSDKService(ILogger<SpeechSDKService> logger,
                                IConfigurationService configService,
                                IHostApplicationLifetime hostLifetime)
        {
            _logger = logger;
            _configService = configService;
            _lifetime = hostLifetime;
        }

        /// <summary>
        /// GRPC method to process audio streams and send them to the Speech SDK for processing.
        /// </summary>
        /// <param name="requestStream">The incoming message stream.</param>
        /// <param name="responseStream">The response stream</param>
        /// <param name="context"></param>
        /// <returns>A <see cref="Task"/> that completes when processing is complete.</returns>
        public override async Task ProcessRemoteSpeechAudio(IAsyncStreamReader<SpeechSDKRemoteRequest> requestStream, IServerStreamWriter<SpeechSDKRemoteResponse> responseStream, ServerCallContext context)
        {
            try
            {
                // An auth mechanism would be a really good idea here....

                // Our protocol by convention is to expect the first message in the stream to be a audio format message.
                // Alternate implementations could either use a fixed audio format, or have a default value incase
                // one isn't specified.
                var audioFormat = await GetAudioFormatForStreamAsync(requestStream, _cancellationSource.Token).ConfigureAwait(false);

                var config = GetSpeechConfig();

                using (var sdkWrapper = new SpeechSDKWrapper(config, audioFormat, new GRPCResponseTransport(responseStream), _logger))
                {
                    // Got business logic that leverages the inferances from the Speech SDK?
                    // This is where it goes:

                    // As a sample we're just going to write out to the console for now...
                    sdkWrapper.SDKSessionStarted += (s, e) => _logger.LogInformation("SESSION STARTED (GRPC)");
                    sdkWrapper.SDKSessionStopped += (s, e) => _logger.LogInformation("SESSION STOPPED");
                    sdkWrapper.SDKSpeechStartDetected += (s, e) => _logger.LogInformation("SPEECH START");
                    sdkWrapper.SDKSpeechEndDetected += (s, e) => _logger.LogInformation("SPEECH END");

                    // We'll also demonstrate how to send the recognizing and recognized text back to the client for
                    // display.

                    var sendClientText = async (object s, SpeechRecognitionEventArgs e, recognitionType rt) =>
                    {
                        try
                        {
                            var result = new recognitionResult()
                            {
                                Text = e.Result.Text,
                                Type = rt
                            };

                            var message = new SpeechSDKRemoteResponse()
                            {
                                RecognitionResult = result
                            };

                            await responseStream.WriteAsync(message);
                        }
                        catch (Exception ex)
                        {
                            _logger.LogError(ex, "Error sending recognition result to client.");
                        };
                    };

                    sdkWrapper.SDKRecognizing += (s, e) => sendClientText(s, e, recognitionType.Intermediate);
                    sdkWrapper.SDKRecognized += (s, e) => sendClientText(s, e, recognitionType.Final);

                    // Start the SR...
                    await sdkWrapper.StartRecognitionAsync();

                    await foreach (var request in requestStream.ReadAllAsync(_lifetime.ApplicationStopping))
                    {

                        if (request.HasAudioPayload)
                        {
                            sdkWrapper.OnAudioReceived(request.AudioPayload.Memory);
                        }
                    }
                    GC.KeepAlive(sdkWrapper);
                }
            }
            catch (OperationCanceledException cex)
            {
                _logger.LogInformation(cex, "We're being canceled. So leave.");
            }

            _logger.LogInformation("Processing of audio stream complete.");

        }

        private async Task<AudioFormat> GetAudioFormatForStreamAsync(IAsyncStreamReader<SpeechSDKRemoteRequest> stream,
            CancellationToken cancellationToken)
        {
            if (!await stream.MoveNext(cancellationToken))
            {
                return default;
            }

            var message = stream.Current;

            if (message.RequestCase != SpeechSDKRemoteRequest.RequestOneofCase.AudioFormat)
            {
                // What?
                throw new InvalidDataException($"Expected {SpeechSDKRemoteRequest.RequestOneofCase.AudioFormat}, but got {message.RequestCase}");
            }

            var audioMessage = message.AudioFormat;

            if (null == audioMessage)
            {
                throw new InvalidDataException("Message did not deserialize into an AudioFormatMessage");
            }

            return new AudioFormat()
            {
                BitsPerSample = Convert.ToInt32(audioMessage.BitsPerSample),
                ChannelCount = Convert.ToInt32(audioMessage.ChannelCount),
                SamplesPerSecond = Convert.ToInt32(audioMessage.SamplesPerSecond)
            };
        }

        private SpeechConfig GetSpeechConfig()
        {
            if (!string.IsNullOrEmpty(_configService.SpeechServiceRegion))
            {
                return SpeechConfig.FromSubscription(_configService.SpeechServiceRegion, _configService.SpeechServiceRegion);
            }
            else
            {
                return SpeechConfig.FromEndpoint(_configService.SpeechServiceEndpoint);
            }
        }
    }
}

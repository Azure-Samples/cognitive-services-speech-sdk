//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.Service.Core;
using SpeechSDKSamples.Remote.WebSocket.Shared;
using System.Net.WebSockets;
using System.Text;

namespace SpeechSDKSamples.Remote.Service.WebSocket.Services.Controllers
{
    public class SpeechSDKRemoteService
    {
        private readonly IHostApplicationLifetime _lifetime;
        private readonly IConfigurationService _configService;
        private ILogger<SpeechSDKRemoteService> _logger;

        public SpeechSDKRemoteService(ILogger<SpeechSDKRemoteService> logger,
                                     IConfigurationService configService,
                                     IHostApplicationLifetime appLifetime)
        {
            _lifetime = appLifetime;
            _configService = configService;
            _logger = logger;
        }

        public async Task HandleWebSocketConnectionAsync(System.Net.WebSockets.WebSocket webSocket)
        {
            try
            {
                var buffer = new byte[1024];
                var messageBuffer = new List<byte>();

                WebSocketReceiveResult result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), _lifetime.ApplicationStopping);

                // Our protocol by convention is to expect the first message in the stream to be an audio format message.
                // Alternate implementations could either use a fixed audio format or have a default value in case
                // one isn't specified.
                var mem = new ReadOnlyMemory<byte>(buffer);

                var audioFormat = GetAudioFormatForStream(result, mem.Slice(0, result.Count), _lifetime.ApplicationStopping);

                var message = Encoding.UTF8.GetString(buffer, 0, result.Count);

                // An auth mechanism would be a really good idea here....

                var config = GetSpeechConfig();

                var sdkWrapper = new SpeechSDKWrapper(config, audioFormat, new WebSocketResponseTransport(webSocket, _logger), _logger);

                // Got business logic that leverages the inferences from the Speech SDK?
                // This is where it goes:

                // As a sample we're just going to write out to the console for now...
                sdkWrapper.SDKSessionStarted += (s, e) => _logger.LogInformation("SESSION STARTED (WEBSOCKET)");
                sdkWrapper.SDKSessionStopped += (s, e) => _logger.LogInformation("SESSION STOPPED");
                sdkWrapper.SDKSpeechStartDetected += (s, e) => _logger.LogInformation("SPEECH START");
                sdkWrapper.SDKSpeechEndDetected += (s, e) => _logger.LogInformation("SPEECH END");

                // We'll also demonstrate how to send the recognizing and recognized text back to the client for
                // display.

                var sendClientText = async (SpeechRecognitionEventArgs e, DisplayType rt) =>
                {
                    try
                    {
                        var result = new DisplayText()
                        {
                            Text = e.Result.Text,
                            DisplayType = rt,
                        };

                        var message = await MessageUtility.SerializeMessageAsync(result, _lifetime.ApplicationStopping);

                        await webSocket.SendAsync(message, WebSocketMessageType.Text, true, _lifetime.ApplicationStopping).ConfigureAwait(false);
                    }
                    catch (Exception ex)
                    {
                        _logger.LogError(ex, "Error sending message to client");
                    };
                };

                sdkWrapper.SDKRecognizing += (s, e) => sendClientText(e, DisplayType.Intermedaite);
                sdkWrapper.SDKRecognized += (s, e) => sendClientText(e, DisplayType.Final);

                // Start the SR...
                await sdkWrapper.StartRecognitionAsync();

                while (!webSocket.CloseStatus.HasValue)
                {
                    messageBuffer.Clear();
                    do
                    {
                        result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), _lifetime.ApplicationStopping);
                        if (result.MessageType == WebSocketMessageType.Close)
                        {
                            _logger.LogInformation("CLOSE RECEIVED!");
                            await webSocket.CloseOutputAsync(WebSocketCloseStatus.NormalClosure, "", CancellationToken.None);
                            return;
                        }

                        messageBuffer.AddRange(new ArraySegment<byte>(buffer, 0, result.Count));
                    } while (!result.EndOfMessage);

                    var messageArray = messageBuffer.ToArray();
                    if (result.MessageType == WebSocketMessageType.Binary)
                    {
                        var iMessage = MessageUtility.DeserializeMessage(messageArray);
                        switch (iMessage)
                        {
                            case AudioMessage audioMessage:
                                sdkWrapper.OnAudioReceived(audioMessage.AudioData);
                                break;
                        }

                    }
                }
            }
            catch (OperationCanceledException cex)
            {
                if (webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived)
                {
                    await webSocket.CloseOutputAsync(WebSocketCloseStatus.NormalClosure, "Operation canceled", CancellationToken.None);
                }
                _logger.LogInformation(cex, "We're being canceled. So leave.");
            }
        }
        private AudioFormat GetAudioFormatForStream(WebSocketReceiveResult messageResult,
                    ReadOnlyMemory<byte> message,
                    CancellationToken cancellationToken)
        {
            if (messageResult.MessageType != WebSocketMessageType.Text)
            {
                // What?
                throw new InvalidDataException($"Expected {WebSocketMessageType.Text}, but got {messageResult.MessageType}");
            }

            var audioMessage = MessageUtility.DeserializeMessageByType<AudioFormatMessage>(message);

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

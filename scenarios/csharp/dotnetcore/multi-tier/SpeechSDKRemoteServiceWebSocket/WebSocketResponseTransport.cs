//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.Service.Core;
using SpeechSDKSamples.Remote.WebSocket.Shared;

namespace SpeechSDKSamples.Remote.Service.WebSocket
{
    internal class WebSocketResponseTransport: IResponseTransport
    {
        private System.Net.WebSockets.WebSocket _ws;
        private ILogger _logger;

        public WebSocketResponseTransport(System.Net.WebSockets.WebSocket ws, ILogger logger)
        {
            _ws = ws;
            _logger = logger;
        }

        public async Task AcknowledgeAudioAsync(TimeSpan offset)
        {
            var ackMessage = new AudioAcknowledgedMessage();
            ackMessage.AcknowledgedAudio = offset;

            var message = await MessageUtility.SerializeMessageAsync(ackMessage).ConfigureAwait(false);

            _logger.LogInformation("Sending audio acknowledged message: {0}", ackMessage);

            await _ws.SendAsync(message.ToArray(), System.Net.WebSockets.WebSocketMessageType.Text, true, default).ConfigureAwait(false);
        }

        public async Task StopRecognitionAsync(RecognitionStoppedReason reason, int errorCode, string errorMessage)
        {
            var stopMessage = new RecognitionStoppedMessage()
            {
                Reason = reason,
                ErrorCode = errorCode,
                ErrorMessage = errorMessage
            };

            _logger.LogInformation("Sending stop message: {0}", stopMessage);

            var message = await MessageUtility.SerializeMessageAsync(stopMessage).ConfigureAwait(false);

            await _ws.SendAsync(message.ToArray(), System.Net.WebSockets.WebSocketMessageType.Text, true, default).ConfigureAwait(false);
        }
    }
}

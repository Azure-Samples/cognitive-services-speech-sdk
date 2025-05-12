//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.AspNetCore.Builder;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Client.WebSocket;
using SpeechSDKSamples.Remote.WebSocket.Shared;
using SpeechSDKSamples.Remote.Service;
using SpeechSDKSamples.Remote.Service.Core;
using System;
using System.Linq;
using System.Threading.Tasks;

namespace SpeechSDKSamples.RemoteClient.Tests
{
    [TestClass]
    public class EndToEndWebSocket : EndToEndBase<IMessage>
    {
        private WebApplication _wsApp;
        private readonly int _port = 5001;

        protected override async Task ServiceInitialize()
        {
            var config = new BasicConfigurationService()
            {
                SpeechServiceEndpoint = _mockSpeechServer.EndpointUri
            };

            _wsApp = SpeechSDKSamples.Remote.Service.WebSocket.Services.Program.CreateWebApp(config, _port);

            await _wsApp.StartAsync(TimeoutToken).ConfigureAwait(false);
        }

        protected override IStreamTransport<IMessage> CreateClientTransport()
        {
            var uri = new Uri($"ws://localhost:{_port}/ws/SpeechSDKRemoteService");

            return new WebSocketTransport(uri);
        }

        [TestCleanup]
        public async Task TearDown()
        {
            await _wsApp.StopAsync(TimeoutToken).ConfigureAwait(false);
        }

        protected override DisplayType GetDisplayTypeForMessage(IMessage message)
        {
            var displayMessage = message as DisplayText;
            return displayMessage.DisplayType;

        }
    }
}

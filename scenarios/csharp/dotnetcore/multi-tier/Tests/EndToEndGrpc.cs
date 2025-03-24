//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.AspNetCore.Builder;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Client.GRPC;
using SpeechSDKSamples.Remote.Service;
using SpeechSDKSamples.Remote.Service.Core;
using SpeechSDKSamples.Remote.Service.GRPC;
using System;
using System.Threading.Tasks;

namespace SpeechSDKSamples.RemoteClient.Tests
{
    [TestClass]
    public class EndToEndGrpc : EndToEndBase<SpeechSDKRemoteResponse>
    {
        private WebApplication _grpcApp;
        private readonly int _port = 5001;

        protected override async Task ServiceInitialize()
        {
            var config = new BasicConfigurationService()
            {
                SpeechServiceEndpoint = _mockSpeechServer.EndpointUri
            };

            _grpcApp = SpeechSDKSamples.Remote.Service.GRPC.Services.Program.CreateWebApp(config, _port);

            await _grpcApp.StartAsync(TimeoutToken).ConfigureAwait(false);
        }

        protected override IStreamTransport<SpeechSDKRemoteResponse> CreateClientTransport()
        {
            var grpcEndpoint = $"http://localhost:{_port}";
            return new GRPCTransport(new Uri(grpcEndpoint));
        }

        [TestCleanup]
        public async Task TearDown()
        {
            await _grpcApp.StopAsync(TimeoutToken).ConfigureAwait(false);
        }

        protected override DisplayType GetDisplayTypeForMessage(SpeechSDKRemoteResponse message)
        {
            return message.RecognitionResult.Type switch
            { recognitionType.Final => DisplayType.Final, recognitionType.Intermediate => DisplayType.Intermedaite, _ => throw new ArgumentException() };
        }
    }
}

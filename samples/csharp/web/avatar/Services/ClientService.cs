//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Avatar.Models;
using Microsoft.Extensions.Options;
using System.Collections.Concurrent;

namespace Avatar.Services
{
    public class ClientService(IOptions<ClientSettings> clientSettings, ClientContext clientContext) : IClientService
    {
        private readonly ClientContext _clientContext = clientContext;

        private readonly ConcurrentDictionary<Guid, ClientContext> _clientContexts = new();

        private readonly ClientSettings _clientSettings = clientSettings.Value;

        public Guid InitializeClient()
        {
            var clientId = Guid.NewGuid();
            var clientContext = _clientContext; 

            // set ClientContext property value
            clientContext.AzureOpenAIDeploymentName = _clientSettings.AzureOpenAIDeploymentName;
            clientContext.CognitiveSearchIndexName = _clientSettings.CognitiveSearchIndexName;
            clientContext.TtsVoice = ClientSettings.DefaultTtsVoice;
            clientContext.CustomVoiceEndpointId = null;
            clientContext.PersonalVoiceSpeakerProfileId = null;
            clientContext.SpeechSynthesizer = null;
            clientContext.SpeechSynthesizerConnected = false;
            clientContext.SpeechToken = null;
            clientContext.IceToken = null;
            clientContext.ChatInitiated = false;
            clientContext.Messages = [];
            clientContext.IsSpeaking = false;
            clientContext.SpeakingText = null;
            clientContext.SpokenTextQueue = new LinkedList<string>();
            clientContext.SpeakingThread = null;
            clientContext.LastSpeakTime = null;

            _clientContexts[clientId] = clientContext;

            return clientId;
        }

        public ClientContext GetClientContext(Guid clientId)
        {
            if (!_clientContexts.TryGetValue(clientId, out var context))
            {
                throw new KeyNotFoundException($"Client context for ID {clientId} was not found.");
            }

            return context;
        }

        public void RemoveClient(Guid clientId)
        {
            _clientContexts.TryRemove(clientId, out _);
        }
    }
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure.Core;
using Azure.Identity;
using OpenAI.Chat;
using Microsoft.Extensions.Options;

namespace Avatar.Models
{
    public class ClientContext
    {
        private readonly DefaultAzureCredential _credential;

        private readonly ClientSettings _clientSettings;

        public string? AzureOpenAIDeploymentName { get; set; }

        public string? CognitiveSearchIndexName { get; set; }

        public string? TtsVoice { get; set; }

        public string? CustomVoiceEndpointId { get; set; }

        public string? PersonalVoiceSpeakerProfileId { get; set; }

        public object? SpeechSynthesizer { get; set; }

        public object? SpeechSynthesizerConnection { get; set; }

        public bool SpeechSynthesizerConnected { get; set; }

        public string? SpeechToken { get; set; }

        public string? IceToken { get; set; }

        public bool ChatInitiated { get; set; }

        public List<ChatMessage> Messages { get; set; } = [];

        public bool IsSpeaking { get; set; }

        public string? SpeakingText { get; set; }

        public LinkedList<string>? SpokenTextQueue { get; set; } = new LinkedList<string>();

        public Thread? SpeakingThread { get; set; }

        public DateTime? LastSpeakTime { get; set; }

        public ClientContext(IOptions<ClientSettings> clientSettings)
        {
            _clientSettings = clientSettings.Value;
            _credential = new DefaultAzureCredential(new DefaultAzureCredentialOptions
            {
                ManagedIdentityClientId = _clientSettings.UserAssignedManagedIdentityClientId
            });
        }
        public async Task<string> GetAzureTokenAsync()
        {
            var tokenRequestContext = new TokenRequestContext(["https://cognitiveservices.azure.com/.default"]);
            var token = await _credential.GetTokenAsync(tokenRequestContext);
            return token.Token;
        }
    }
}
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Text.RegularExpressions;

namespace Avatar.Models
{
    public class ClientSettings
    {
        public static readonly List<string> SentenceLevelPunctuations = new List<string> { ".", "?", "!", ":", ";", "。", "？", "！", "：", "；" };

        public static readonly List<string> QuickReplies = new List<string> { "Let me take a look.", "Let me check.", "One moment, please." };

        public static readonly Regex OydDocRegex = new Regex(@"\[doc(\d+)\]");

        public static readonly bool RepeatSpeakingSentenceAfterReconnection = true;

        public static readonly string DefaultTtsVoice = "en-US-JennyNeural";

        public static readonly bool EnableQuickReply = false;

        public static readonly bool EnableDisplayTextAlignmentWithSpeech = false;

        public static readonly bool EnableAudioAudit = false;

        public string? SpeechRegion { get; set; }

        public string? SpeechKey { get; set; }

        public string? SpeechPrivateEndpoint { get; set; }

        public string? SpeechResourceUrl { get; set; }

        public string? UserAssignedManagedIdentityClientId { get; set; }

        public string? AzureOpenAIEndpoint { get; set; }

        public string? AzureOpenAIAPIKey { get; set; }

        public string? AzureOpenAIDeploymentName { get; set; }

        public string? CognitiveSearchEndpoint { get; set; }

        public string? CognitiveSearchAPIKey { get; set; }

        public string? CognitiveSearchIndexName { get; set; }

        public string? IceServerUrl { get; set; }

        public string? IceServerUrlRemote { get; set; }

        public string? IceServerUsername { get; set; }

        public string? IceServerPassword { get; set; }
    }
}

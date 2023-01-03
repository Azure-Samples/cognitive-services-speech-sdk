// <copyright file="ConversationItem.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class ConversationItem
    {
        [JsonProperty("participantId")]
        public string ParticipantId { get; set; }

        [JsonProperty("id")]
        public string Id { get; set; }

        [JsonProperty("text")]
        public string Text { get; set; }

        [JsonProperty("lexical")]
        public string Lexical { get; set; }

        [JsonProperty("itn")]
        public string Itn { get; set; }

        [JsonProperty("maskedItn")]
        public string MaskedItn { get; set; }

        [JsonProperty("wordLevelTimings")]
        public IEnumerable<WordLevelAudioTiming> AudioTimings { get; set; }

        [JsonProperty("conversationItemLevelTiming")]
        public AudioTiming ConversationItemLevelTiming { get; set; }

        [JsonProperty("role")]
        public string Role { get; set; }
    }
}
// <copyright file="Conversation.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    public class Conversation
    {
        [JsonProperty("id")]
        public string Id { get; set; }

        [JsonProperty("language")]
        public string Language { get; set; }

        [JsonProperty("modality")]
        [JsonConverter(typeof(StringEnumConverter))]
        public Modality Modality { get; set; }

        [JsonProperty("conversationItems")]
#pragma warning disable CA2227 // Collection properties should be read only
        public IList<ConversationItem> ConversationItems { get; set; }
#pragma warning restore CA2227 // Collection properties should be read only
    }
}

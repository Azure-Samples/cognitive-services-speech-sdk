// <copyright file="Item.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System;
    using JsonSubTypes;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    [JsonConverter(typeof(JsonSubtypes), nameof(Kind))]
    [JsonSubtypes.KnownSubType(typeof(ConversationPiiItem), AnalyzeConversationsTaskResultKind.conversationalPIIResults)]
    [JsonSubtypes.KnownSubType(typeof(ConversationSummarizationItem), AnalyzeConversationsTaskResultKind.conversationalSummarizationResults)]
    public abstract class Item
    {
        [JsonProperty("kind")]
        [JsonConverter(typeof(StringEnumConverter))]
        public AnalyzeConversationsTaskResultKind Kind { get; set; }

        [JsonProperty("taskName")]
        public string TaskName { get; set; }

        [JsonProperty("lastUpdateDateTime")]
        public DateTimeOffset LastUpdateDateTime { get; set; }

        [JsonProperty("status")]
        public string Status { get; set; }
    }
}
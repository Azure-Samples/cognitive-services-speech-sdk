// <copyright file="ConversationsSummaryResult.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class ConversationsSummaryResult
    {
        [JsonProperty(PropertyName = "id")]
        public string Id { get; set; }

        [JsonProperty(PropertyName = "summaries")]
        public IEnumerable<SummaryResultItem> Summaries { get; set; }

        [JsonProperty("warnings")]
        public IEnumerable<Warning> Warnings { get; set; }
    }
}

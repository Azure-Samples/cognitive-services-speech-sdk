// <copyright file="AnalyzeConversationsResult.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System;
    using System.Collections.Generic;
    using Connector.Serializable.TextAnalytics;
    using Newtonsoft.Json;

    public class AnalyzeConversationsResult
    {
        [JsonProperty("jobId")]
        public Guid JobId { get; set; }

        [JsonProperty("lastUpdatedDateTime")]
        public DateTimeOffset LastUpdatedDateTime { get; set; }

        [JsonProperty("createdDateTime")]
        public DateTimeOffset CreatedDateTime { get; set; }

        [JsonProperty("expirationDateTime")]
        public DateTimeOffset ExpirationDateTime { get; set; }

        [JsonProperty("status")]
        public string Status { get; set; }

        [JsonProperty("errors")]
        public IEnumerable<ErrorEntity> Errors { get; private set; }

        [JsonProperty("displayName")]
        public string DisplayName { get; set; }

        [JsonProperty("tasks")]
        public Tasks Tasks { get; set; }
    }
}

// <copyright file="AnalyzeConversationResultsBase.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Connector.Serializable.TextAnalytics;
    using Newtonsoft.Json;

    public abstract class AnalyzeConversationResultsBase
    {
        [JsonProperty("errors")]
        public IEnumerable<ErrorEntity> Errors { get; set; }

        [JsonProperty("modelVersion")]
        public string ModelVersion { get; set; }
    }
}
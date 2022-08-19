// <copyright file="AnalyzeConversationsRequest.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class AnalyzeConversationsRequest
    {
        [JsonProperty("displayName")]
        public string DisplayName { get; set; }

        [JsonProperty("analysisInput")]
        public AnalysisInput AnalysisInput { get; set; }

        [JsonProperty("tasks")]
#pragma warning disable CA2227 // Collection properties should be read only
        public IList<AnalyzeConversationsTask> Tasks { get; set; }
#pragma warning restore CA2227 // Collection properties should be read only
    }
}

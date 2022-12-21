// <copyright file="ConversationAnalyticsResults.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using Connector.Serializable.Language.Conversations;
    using Newtonsoft.Json;

    public class ConversationAnalyticsResults
    {
        [JsonProperty("conversationPiiResults")]
        public AnalyzeConversationPiiResults AnalyzeConversationPiiResults { get; set; }

        [JsonProperty("conversationSummarizationResults")]
        public AnalyzeConversationSummarizationResults AnalyzeConversationSummarizationResults { get; set; }
    }
}
// <copyright file="AnalyzeConversationPiiResults.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class AnalyzeConversationPiiResults : AnalyzeConversationResultsBase
    {
        [JsonProperty("combinedRedactedContent")]
        public IEnumerable<CombinedConversationPiiResult> CombinedRedactedContent { get; set; }

        [JsonProperty("conversations")]
        public IEnumerable<ConversationPiiResult> Conversations { get; set; }
    }
}
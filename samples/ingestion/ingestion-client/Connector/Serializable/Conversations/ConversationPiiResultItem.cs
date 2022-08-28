// <copyright file="ConversationPiiResultItem.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Connector.Serializable.TextAnalytics;
    using Newtonsoft.Json;

    public class ConversationPiiResultItem
    {
        /// <summary>
        /// Gets or sets the id of the conversation item.
        /// </summary>
        [JsonProperty(PropertyName = "id")]
        public string Id
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the participant id of the conversation item.
        /// </summary>
        [JsonProperty(PropertyName = "channel")]
        public string Channel
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the offset of the conversation item.
        /// </summary>
        [JsonProperty(PropertyName = "offset")]
        public string Offset
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the redacted content for transcript.
        /// </summary>
        [JsonProperty(PropertyName = "redactedContent")]
        public RedactedTranscriptContent RedactedContent
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the PII entities for transcript.
        /// </summary>
        [JsonProperty(PropertyName = "entities")]
        public IEnumerable<TextAnalyticsEntity> Entities
        {
            get;
            set;
        }
    }
}

// <copyright file="ConversationPiiResult.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class ConversationPiiResult
    {
        [JsonProperty(PropertyName = "id")]
        public string Id
        {
            get;
            set;
        }

        [JsonProperty(PropertyName = "conversationItems", ItemConverterType = typeof(ConversationPiiCustomConverter))]
        public IEnumerable<ConversationPiiResultItem> ConversationItems
        {
            get;
            set;
        }

        [JsonProperty("warnings")]
        public IEnumerable<Warning> Warnings { get; set; }
    }
}

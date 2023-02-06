// <copyright file="ItemizedSummaryContext.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using Newtonsoft.Json;

    public class ItemizedSummaryContext
    {
        [JsonProperty(PropertyName = "conversationItemId")]
        public string ConversationItemId { get; set; }

        [JsonProperty(PropertyName = "offset")]
        public int Offset { get; set; }

        [JsonProperty(PropertyName = "length")]
        public int Length { get; set; }
    }
}

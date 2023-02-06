// <copyright file="SummaryResultItem.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;

    using Newtonsoft.Json;

    public class SummaryResultItem
    {
        [JsonProperty("aspect")]
        public string Aspect { get; set; }

        [JsonProperty("text")]
        public string Text { get; set; }

        [JsonProperty("contexts")]
        public IEnumerable<ItemizedSummaryContext> Contexts { get; set; }
    }
}

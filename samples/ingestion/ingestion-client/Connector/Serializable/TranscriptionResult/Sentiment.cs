// <copyright file="Sentiment.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using Newtonsoft.Json;

    public class Sentiment
    {
        [JsonProperty("negative")]
        public double Negative { get; set; }

        [JsonProperty("neutral")]
        public double Neutral { get; set; }

        [JsonProperty("positive")]
        public double Positive { get; set; }
    }
}

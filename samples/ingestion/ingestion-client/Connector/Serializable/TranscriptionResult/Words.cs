// <copyright file="Words.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using Newtonsoft.Json;

    public class Words
    {
        public Words(string word, string offset, string duration, double offsetInTicks, double durationInTicks, double confidence)
        {
            this.Word = word;
            this.Offset = offset;
            this.Duration = duration;
            this.OffsetInTicks = offsetInTicks;
            this.DurationInTicks = durationInTicks;
            this.Confidence = confidence;
        }

        [JsonProperty("word")]
        public string Word { get; set; }

        [JsonProperty("offset")]
        public string Offset { get; set; }

        [JsonProperty("duration")]
        public string Duration { get; set; }

        [JsonProperty("offsetInTicks")]
        public double OffsetInTicks { get; set; }

        [JsonProperty("durationInTicks")]
        public double DurationInTicks { get; set; }

        [JsonProperty("confidence")]
        public double Confidence { get; set; }
    }
}

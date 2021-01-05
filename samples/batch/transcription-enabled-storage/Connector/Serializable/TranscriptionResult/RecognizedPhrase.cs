// <copyright file="RecognizedPhrase.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class RecognizedPhrase
    {
        public RecognizedPhrase(string recognitionStatus, int channel, int speaker, string offset, string duration, double offsetInTicks, double durationInTicks, IEnumerable<NBest> nBest)
        {
            RecognitionStatus = recognitionStatus;
            Channel = channel;
            Speaker = speaker;
            Offset = offset;
            Duration = duration;
            OffsetInTicks = offsetInTicks;
            DurationInTicks = durationInTicks;
            NBest = nBest;
        }

        [JsonProperty("recognitionStatus")]
        public string RecognitionStatus { get; }

        [JsonProperty("channel")]
        public int Channel { get; }

        [JsonProperty("speaker")]
        public int Speaker { get; }

        [JsonProperty("offset")]
        public string Offset { get; }

        [JsonProperty("duration")]
        public string Duration { get; }

        [JsonProperty("offsetInTicks")]
        public double OffsetInTicks { get; }

        [JsonProperty("durationInTicks")]
        public double DurationInTicks { get; }

        [JsonProperty("nBest")]
        public IEnumerable<NBest> NBest { get; set; }
    }
}

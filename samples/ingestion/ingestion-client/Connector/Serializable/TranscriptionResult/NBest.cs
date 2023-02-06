// <copyright file="NBest.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class NBest
    {
        public NBest(double confidence, string lexical, string itn, string maskedITN, string display, Sentiment sentiment, IEnumerable<Words> words)
        {
            this.Confidence = confidence;
            this.Lexical = lexical;
            this.ITN = itn;
            this.MaskedITN = maskedITN;
            this.Display = display;
            this.Sentiment = sentiment;
            this.Words = words;
        }

        [JsonProperty("confidence")]
        public double Confidence { get; }

        [JsonProperty("lexical")]
        public string Lexical { get; set; }

        [JsonProperty("itn")]
        public string ITN { get; set; }

        [JsonProperty("maskedITN")]
        public string MaskedITN { get; set; }

        [JsonProperty("display")]
        public string Display { get; set; }

        [JsonProperty("sentiment")]
        public Sentiment Sentiment { get; set; }

        [JsonProperty("words")]
        public IEnumerable<Words> Words { get; set; }
    }
}

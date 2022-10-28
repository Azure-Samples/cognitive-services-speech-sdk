// <copyright file="CombinedRecognizedPhrase.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using Newtonsoft.Json;

    public class CombinedRecognizedPhrase
    {
        public CombinedRecognizedPhrase(int channel, string lexical, string itn, string maskedItn, string display, Sentiment sentiment)
        {
            this.Channel = channel;
            this.Lexical = lexical;
            this.ITN = itn;
            this.MaskedITN = maskedItn;
            this.Display = display;
            this.Sentiment = sentiment;
        }

        [JsonProperty("channel")]
        public int Channel { get; set; }

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
    }
}

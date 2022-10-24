// <copyright file="SpeechTranscript.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;

    using Connector.Serializable.Language.Conversations;

    using Newtonsoft.Json;

    public class SpeechTranscript
    {
        public SpeechTranscript(string source, string timestamp, long durationInTicks, string duration, IEnumerable<CombinedRecognizedPhrase> combinedRecognizedPhrases, IEnumerable<RecognizedPhrase> recognizedPhrases)
        {
            this.Source = source;
            this.Timestamp = timestamp;
            this.DurationInTicks = durationInTicks;
            this.Duration = duration;
            this.CombinedRecognizedPhrases = combinedRecognizedPhrases;
            this.RecognizedPhrases = recognizedPhrases;
        }

        [JsonProperty("source")]
        public string Source { get; }

        [JsonProperty("timeStamp")]
        public string Timestamp { get; }

        [JsonProperty("durationInTicks")]
        public long DurationInTicks { get; }

        [JsonProperty("duration")]
        public string Duration { get; }

        [JsonProperty("combinedRecognizedPhrases")]
        public IEnumerable<CombinedRecognizedPhrase> CombinedRecognizedPhrases { get; }

        [JsonProperty("recognizedPhrases")]
        public IEnumerable<RecognizedPhrase> RecognizedPhrases { get; }

        [JsonProperty("conversationAnalyticsResults")]
        public ConversationAnalyticsResults ConversationAnalyticsResults { get; set; }
    }
}

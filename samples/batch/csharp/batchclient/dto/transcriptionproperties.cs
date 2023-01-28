//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class TranscriptionProperties
    {
        [JsonProperty(PropertyName = "diarizationEnabled")]
        public bool IsDiarizationEnabled { get; set; }

        [JsonProperty(PropertyName = "wordLevelTimestampsEnabled")]
        public bool IsWordLevelTimestampsEnabled { get; set; }

        [JsonProperty(PropertyName = "displayFormWordLevelTimestampsEnabled")]
        public bool IsDisplayFormWordLevelTimestampsEnabled { get; set; }

        public string Email { get; set; }

        public IEnumerable<int> Channels { get; set; }

        public PunctuationMode PunctuationMode { get; set; }

        public ProfanityFilterMode ProfanityFilterMode { get; set; }

        [JsonConverter(typeof(TimeSpanConverter))]
        public TimeSpan Duration { get; set; }

        public Uri DestinationContainerUrl { get; set; }

        [JsonConverter(typeof(TimeSpanConverter))]
        public TimeSpan TimeToLive { get; set; }

        public DiarizationProperties Diarization { get; set; }

        public LanguageIdentificationProperties LanguageIdentification { get; set; }

        public EntityError Error { get; set; }
    }
}

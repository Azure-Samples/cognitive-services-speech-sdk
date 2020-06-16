//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using Newtonsoft.Json;
    using System;
    using System.Collections.Generic;

    public class RecognitionResults
    {
        public string Source { get; set; }

        public DateTime Timestamp { get; set; }

        public long DurationInTicks { get; set; }

        [JsonConverter(typeof(TimeSpanConverter))]
        public TimeSpan Duration { get; set; }

        public IEnumerable<CombinedResults> CombinedRecognizedPhrases { get; set; }

        public IEnumerable<RecognitionResult> RecognizedPhrases { get; set; }
    }
}

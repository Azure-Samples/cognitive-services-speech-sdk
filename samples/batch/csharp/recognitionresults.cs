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
        public string Source { get; private set; }

        public DateTime Timestamp { get; private set; }

        public long DurationInTicks { get; private set; }

        [JsonConverter(typeof(TimespanConverter))]
        public TimeSpan Duration { get; private set; }

        public IEnumerable<CombinedResults> CombinedRecognizedPhrases { get; private set; }

        public IEnumerable<RecognitionResult> RecognizedPhrases { get; private set; }
    }
}

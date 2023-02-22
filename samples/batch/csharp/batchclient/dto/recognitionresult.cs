//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class RecognitionResult
    {
        public string RecognitionStatus { get; set; }

        public int Channel { get; set; }

        public int? Speaker { get; set; }

        [JsonConverter(typeof(TimeSpanConverter))]
        public TimeSpan Offset { get; set; }

        [JsonConverter(typeof(TimeSpanConverter))]
        public TimeSpan Duration { get; set; }

        public long OffsetInTicks { get; set; }

        public long DurationInTicks { get; set; }

        public IEnumerable<NBest> NBest { get; set; }

        public string Locale { get; set; }
    }
}

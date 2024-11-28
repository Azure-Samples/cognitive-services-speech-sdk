//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using Newtonsoft.Json;

    public class WordDetails
    {
        public string Word { get; set; }

        [JsonConverter(typeof(TimeSpanConverter))]
        public TimeSpan Offset { get; set; }

        [JsonConverter(typeof(TimeSpanConverter))]
        public TimeSpan Duration { get; set; }

        public double OffsetInTicks { get; set; }

        public double DurationInTicks { get; set; }

        public long OffsetMilliseconds { get; set; }

        public long DurationMilliseconds { get; set; }

        public float Confidence { get; set; }
    }
}

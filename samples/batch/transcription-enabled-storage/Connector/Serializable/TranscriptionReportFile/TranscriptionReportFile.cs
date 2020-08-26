// <copyright file="TranscriptionReportFile.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class TranscriptionReportFile
    {
        public TranscriptionReportFile(string successfulTranscriptionsCount, string failedTranscriptionsCount, IEnumerable<TranscriptionReportFileDetail> details)
        {
            SuccessfulTranscriptionsCount = successfulTranscriptionsCount;
            FailedTranscriptionsCount = failedTranscriptionsCount;
            Details = details;
        }

        [JsonProperty("SuccessfulTranscriptionsCount")]
        public string SuccessfulTranscriptionsCount { get; }

        [JsonProperty("FailedTranscriptionsCount")]
        public string FailedTranscriptionsCount { get; }

        [JsonProperty("Details")]
        public IEnumerable<TranscriptionReportFileDetail> Details { get; }
    }
}

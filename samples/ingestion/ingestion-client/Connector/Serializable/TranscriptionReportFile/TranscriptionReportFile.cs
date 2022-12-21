// <copyright file="TranscriptionReportFile.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;

    public class TranscriptionReportFile
    {
        public TranscriptionReportFile(string successfulTranscriptionsCount, string failedTranscriptionsCount, IEnumerable<TranscriptionReportFileDetail> details)
        {
            this.SuccessfulTranscriptionsCount = successfulTranscriptionsCount;
            this.FailedTranscriptionsCount = failedTranscriptionsCount;
            this.Details = details;
        }

        public string SuccessfulTranscriptionsCount { get; }

        public string FailedTranscriptionsCount { get; }

        public IEnumerable<TranscriptionReportFileDetail> Details { get; }
    }
}

// <copyright file="TranscriptionReportFileDetail.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using Newtonsoft.Json;

    public class TranscriptionReportFileDetail
    {
        public TranscriptionReportFileDetail(string status, string errorKind, string errorMessage, string sourceUrl)
        {
            Status = status;
            ErrorKind = errorKind;
            ErrorMessage = errorMessage;
            SourceUrl = sourceUrl;
        }

        [JsonProperty("status")]
        public string Status { get; }

        [JsonProperty("errorKind")]
        public string ErrorKind { get; }

        [JsonProperty("errorMessage")]
        public string ErrorMessage { get; }

        [JsonProperty("sourceUrl")]
        public string SourceUrl { get; }
    }
}

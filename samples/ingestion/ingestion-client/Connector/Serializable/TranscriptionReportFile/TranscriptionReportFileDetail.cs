// <copyright file="TranscriptionReportFileDetail.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using Newtonsoft.Json;

    public class TranscriptionReportFileDetail
    {
        public TranscriptionReportFileDetail(string status, string errorKind, string errorMessage, string source)
        {
            this.Status = status;
            this.ErrorKind = errorKind;
            this.ErrorMessage = errorMessage;
            this.Source = source;
        }

        [JsonProperty("status")]
        public string Status { get; set; }

        [JsonProperty("errorKind")]
        public string ErrorKind { get; set; }

        [JsonProperty("errorMessage")]
        public string ErrorMessage { get; set; }

        [JsonProperty("source")]
        public string Source { get; set; }

        [JsonProperty("sourceUrl")]
        private string SourceUrl
        {
            set { this.Source = value; }
        }
    }
}

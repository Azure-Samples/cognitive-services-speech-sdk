// <copyright file="AudioFileInfo.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionStartedServiceBusMessage
{
    using System;
    using Connector.Serializable.TranscriptionStartedMessage.AzureOpenAI;

    public class AudioFileInfo
    {
        public AudioFileInfo(string fileUrl, int retryCount, TextAnalyticsRequests textAnalyticsRequests, AzureOpenAIRequests azureOpenAIRequests)
        {
            this.FileUrl = fileUrl;
            this.RetryCount = retryCount;
            this.TextAnalyticsRequests = textAnalyticsRequests;
            this.AzureOpenAIRequests = azureOpenAIRequests;
        }

        public string FileUrl { get; set; }

        public string FileName => StorageConnector.GetFileNameFromUri(new Uri(this.FileUrl));

        public int RetryCount { get; set; }

        public TextAnalyticsRequests TextAnalyticsRequests { get; set; }

        public AzureOpenAIRequests AzureOpenAIRequests { get; set; }
    }
}

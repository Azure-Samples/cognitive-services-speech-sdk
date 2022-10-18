// <copyright file="TextAnalyticsRequest.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionStartedServiceBusMessage
{
    public class TextAnalyticsRequest
    {
        public TextAnalyticsRequest(string id, TextAnalyticsRequestStatus status)
        {
            this.Id = id;
            this.Status = status;
        }

        public enum TextAnalyticsRequestStatus
        {
            None = 0,
            Running,
            Completed
        }

        public string Id { get; set; }

        public TextAnalyticsRequestStatus Status { get; set; }
    }
}

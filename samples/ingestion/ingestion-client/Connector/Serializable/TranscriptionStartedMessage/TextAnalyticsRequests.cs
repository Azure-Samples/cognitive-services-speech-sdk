// <copyright file="TextAnalyticsRequests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionStartedServiceBusMessage
{
    using System.Collections.Generic;

    public class TextAnalyticsRequests
    {
        public TextAnalyticsRequests(
            IEnumerable<TextAnalyticsRequest> utteranceLevelRequests,
            IEnumerable<TextAnalyticsRequest> audioLevelRequests,
            IEnumerable<TextAnalyticsRequest> conversationRequests)
        {
            this.UtteranceLevelRequests = utteranceLevelRequests ?? new List<TextAnalyticsRequest>();
            this.AudioLevelRequests = audioLevelRequests ?? new List<TextAnalyticsRequest>();
            this.ConversationRequests = conversationRequests ?? new List<TextAnalyticsRequest>();
        }

        public IEnumerable<TextAnalyticsRequest> UtteranceLevelRequests { get; set; }

        public IEnumerable<TextAnalyticsRequest> AudioLevelRequests { get; set; }

        public IEnumerable<TextAnalyticsRequest> ConversationRequests { get; set; }
    }
}

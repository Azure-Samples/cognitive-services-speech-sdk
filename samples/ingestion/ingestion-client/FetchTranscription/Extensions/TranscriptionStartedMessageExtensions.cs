// <copyright file="TranscriptionStartedMessageExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using System.Linq;
    using System.Threading.Tasks;

    using Connector;
    using Connector.Enums;

    using Language;
    using Microsoft.Extensions.Logging;

    using TextAnalytics;

    public static class TranscriptionStartedMessageExtensions
    {
        public static async Task<TranscriptionAnalyticsJobStatus> GetTranscriptionAnalyticsJobsStatusAsync(
            this TranscriptionStartedMessage transcriptionStartedMessage,
            TextAnalyticsProvider textAnalyticsProvider,
            AnalyzeConversationsProvider analyzeConversationsProvider,
            ILogger logger)
        {
            _ = transcriptionStartedMessage ?? throw new ArgumentNullException(nameof(transcriptionStartedMessage));

            if (textAnalyticsProvider == null && analyzeConversationsProvider == null)
            {
                return TranscriptionAnalyticsJobStatus.None;
            }

            var textAnalyticsRequestCompleted = textAnalyticsProvider != null ? await textAnalyticsProvider.TextAnalyticsRequestsCompleted(transcriptionStartedMessage.AudioFileInfos).ConfigureAwait(false) : true;
            var conversationalAnalyticsRequestCompleted = analyzeConversationsProvider != null ? await analyzeConversationsProvider.ConversationalRequestsCompleted(transcriptionStartedMessage.AudioFileInfos).ConfigureAwait(false) : true;

            if (!textAnalyticsRequestCompleted || !conversationalAnalyticsRequestCompleted)
            {
                return TranscriptionAnalyticsJobStatus.Running;
            }

            // if the message already contains text analytics requests and all are in terminal state, treat jobs as completed:
            var containsTextAnalyticsRequest = transcriptionStartedMessage.AudioFileInfos.Where(audioFileInfo => audioFileInfo.TextAnalyticsRequests != null).Any();
            if (containsTextAnalyticsRequest)
            {
                return TranscriptionAnalyticsJobStatus.Completed;
            }

            return TranscriptionAnalyticsJobStatus.NotStarted;
        }
    }
}

// <copyright file="TranscriptionAnalyticsOrchestrator.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription.TranscriptionAnalytics
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;

    using Connector;
    using Connector.Enums;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    using FetchTranscriptionFunction;
    using Microsoft.Extensions.Logging;

    using static Connector.Serializable.TranscriptionStartedServiceBusMessage.TextAnalyticsRequest;

    public sealed class TranscriptionAnalyticsOrchestrator
    {
        private readonly TextAnalyticsProvider textAnalyticsProvider;
        private readonly AnalyzeConversationsProvider analyzeConversationsProvider;

        public TranscriptionAnalyticsOrchestrator(
            string locale,
            ILogger logger)
        {
            var textAnalyticsKey = FetchTranscriptionEnvironmentVariables.TextAnalyticsKey;
            var textAnalyticsEndpoint = FetchTranscriptionEnvironmentVariables.TextAnalyticsEndpoint;
            var textAnalyticsInfoProvided = !string.IsNullOrEmpty(textAnalyticsKey) && !string.IsNullOrEmpty(textAnalyticsEndpoint);

            this.analyzeConversationsProvider = textAnalyticsInfoProvided ? new AnalyzeConversationsProvider(locale, textAnalyticsKey, textAnalyticsEndpoint, logger) : null;
            this.textAnalyticsProvider = textAnalyticsInfoProvided ? new TextAnalyticsProvider(locale, textAnalyticsKey, textAnalyticsEndpoint, logger) : null;
        }

        public async Task<TranscriptionAnalyticsJobStatus> GetTranscriptionAnalyticsJobsStatusAsync(TranscriptionStartedMessage transcriptionStartedMessage)
        {
            _ = transcriptionStartedMessage ?? throw new ArgumentNullException(nameof(transcriptionStartedMessage));

            if (this.textAnalyticsProvider == null && this.analyzeConversationsProvider == null)
            {
                return TranscriptionAnalyticsJobStatus.None;
            }

            var textAnalyticsRequestCompleted = this.textAnalyticsProvider != null ? await this.textAnalyticsProvider.TextAnalyticsRequestsCompleted(transcriptionStartedMessage.AudioFileInfos).ConfigureAwait(false) : true;
            var conversationalAnalyticsRequestCompleted = this.analyzeConversationsProvider != null ? await this.analyzeConversationsProvider.ConversationalRequestsCompleted(transcriptionStartedMessage.AudioFileInfos).ConfigureAwait(false) : true;

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

            return TranscriptionAnalyticsJobStatus.NotSubmitted;
        }

        public async Task<IEnumerable<string>> AddTranscriptionAnalyticsResultsToTranscripts(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings)
        {
            _ = speechTranscriptMappings ?? throw new ArgumentNullException(nameof(speechTranscriptMappings));

            var errors = new List<string>();
            foreach (var speechTranscriptMapping in speechTranscriptMappings)
            {
                var speechTranscript = speechTranscriptMapping.Value;
                var audioFileInfo = speechTranscriptMapping.Key;
                var fileName = audioFileInfo.FileName;
                if (FetchTranscriptionEnvironmentVariables.PiiRedactionSetting != PiiRedactionSetting.None)
                {
                    speechTranscript.RecognizedPhrases.ToList().ForEach(phrase =>
                    {
                        if (phrase.NBest != null && phrase.NBest.Any())
                        {
                            var firstNBest = phrase.NBest.First();
                            phrase.NBest = new[] { firstNBest };
                        }
                    });
                }

                var textAnalyticsErrors = new List<string>();

                if (audioFileInfo.TextAnalyticsRequests.AudioLevelRequests?.Any() == true)
                {
                    var audioLevelErrors = await this.textAnalyticsProvider.AddAudioLevelEntitiesAsync(audioFileInfo.TextAnalyticsRequests.AudioLevelRequests.Select(request => request.Id), speechTranscript).ConfigureAwait(false);
                    textAnalyticsErrors.AddRange(audioLevelErrors);
                }

                if (audioFileInfo.TextAnalyticsRequests.UtteranceLevelRequests?.Any() == true)
                {
                    var utteranceLevelErrors = await this.textAnalyticsProvider.AddUtteranceLevelEntitiesAsync(audioFileInfo.TextAnalyticsRequests.UtteranceLevelRequests.Select(request => request.Id), speechTranscript).ConfigureAwait(false);
                    textAnalyticsErrors.AddRange(utteranceLevelErrors);
                }

                if (audioFileInfo.TextAnalyticsRequests.ConversationRequests?.Any() == true)
                {
                    var conversationalAnalyticsErrors = await this.analyzeConversationsProvider.AddConversationalEntitiesAsync(audioFileInfo.TextAnalyticsRequests.ConversationRequests.Select(request => request.Id), speechTranscript).ConfigureAwait(false);
                    textAnalyticsErrors.AddRange(conversationalAnalyticsErrors);
                }

                if (textAnalyticsErrors.Any())
                {
                    var distinctErrors = textAnalyticsErrors.Distinct();
                    var errorMessage = $"File {(string.IsNullOrEmpty(fileName) ? "unknown" : fileName)}:\n{string.Join('\n', distinctErrors)}";
                    errors.Add(errorMessage);
                }
            }

            return errors;
        }

        public async Task<IEnumerable<string>> SubmitTranscriptionAnalyticsJobsAndAddToAudioFileInfos(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings)
        {
            _ = speechTranscriptMappings ?? throw new ArgumentNullException(nameof(speechTranscriptMappings));

            var errors = new List<string>();
            foreach (var speechTranscriptMapping in speechTranscriptMappings)
            {
                var speechTranscript = speechTranscriptMapping.Value;
                var audioFileInfo = speechTranscriptMapping.Key;

                var fileName = audioFileInfo.FileName;

                if (speechTranscript.RecognizedPhrases != null && speechTranscript.RecognizedPhrases.All(phrase => phrase.RecognitionStatus.Equals("Success", StringComparison.Ordinal)))
                {
                    var textAnalyticsErrors = new List<string>();

                    (var utteranceLevelJobIds, var utteranceLevelErrors) = await this.textAnalyticsProvider.SubmitUtteranceLevelRequests(
                        speechTranscript,
                        FetchTranscriptionEnvironmentVariables.SentimentAnalysisSetting).ConfigureAwait(false);

                    var utteranceLevelRequests = utteranceLevelJobIds?.Select(jobId => new TextAnalyticsRequest(jobId, TextAnalyticsRequestStatus.Running));
                    textAnalyticsErrors.AddRange(utteranceLevelErrors);

                    (var audioLevelJobIds, var audioLevelErrors) = await this.textAnalyticsProvider.SubmitAudioLevelRequests(
                        speechTranscript,
                        FetchTranscriptionEnvironmentVariables.SentimentAnalysisSetting,
                        FetchTranscriptionEnvironmentVariables.PiiRedactionSetting).ConfigureAwait(false);

                    var audioLevelRequests = audioLevelJobIds?.Select(jobId => new TextAnalyticsRequest(jobId, TextAnalyticsRequestStatus.Running));
                    textAnalyticsErrors.AddRange(audioLevelErrors);

                    (var conversationJobIds, var conversationErrors) = await this.analyzeConversationsProvider.SubmitAnalyzeConversationsRequestAsync(speechTranscript).ConfigureAwait(false);

                    var conversationalRequests = conversationJobIds?.Select(jobId => new TextAnalyticsRequest(jobId, TextAnalyticsRequestStatus.Running));
                    textAnalyticsErrors.AddRange(conversationErrors);

                    audioFileInfo.TextAnalyticsRequests = new TextAnalyticsRequests(utteranceLevelRequests, audioLevelRequests, conversationalRequests);

                    if (textAnalyticsErrors.Any())
                    {
                        var distinctErrors = textAnalyticsErrors.Distinct();
                        var errorMessage = $"File {(string.IsNullOrEmpty(fileName) ? "unknown" : fileName)}:\n{string.Join('\n', distinctErrors)}";
                        errors.Add(errorMessage);
                    }
                }
            }

            return errors;
        }
    }
}

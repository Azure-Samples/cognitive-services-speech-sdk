// <copyright file="TranscriptionAnalyticsOrchestrator.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;

    using Connector;
    using Connector.Enums;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    using Microsoft.Extensions.Logging;

    public sealed class TranscriptionAnalyticsOrchestrator
    {
        private readonly List<ITranscriptionAnalyticsProvider> providers;

        public TranscriptionAnalyticsOrchestrator(
            string locale,
            ILogger logger)
        {
            var textAnalyticsKey = FetchTranscriptionEnvironmentVariables.TextAnalyticsKey;
            var textAnalyticsEndpoint = FetchTranscriptionEnvironmentVariables.TextAnalyticsEndpoint;
            var textAnalyticsInfoProvided = !string.IsNullOrEmpty(textAnalyticsKey) && !string.IsNullOrEmpty(textAnalyticsEndpoint);

            this.providers = new List<ITranscriptionAnalyticsProvider>();

            if (textAnalyticsInfoProvided)
            {
                this.providers.Add(new TextAnalyticsProvider(locale, textAnalyticsKey, textAnalyticsEndpoint, logger));
                this.providers.Add(new AnalyzeConversationsProvider(locale, textAnalyticsKey, textAnalyticsEndpoint, logger));
            }
        }

        public async Task<TranscriptionAnalyticsJobStatus> GetTranscriptionAnalyticsJobsStatusAsync(TranscriptionStartedMessage transcriptionStartedMessage)
        {
            _ = transcriptionStartedMessage ?? throw new ArgumentNullException(nameof(transcriptionStartedMessage));

            if (this.providers.Count == 0)
            {
                return TranscriptionAnalyticsJobStatus.None;
            }

            foreach (var provider in this.providers)
            {
                var providerStatus = await provider.GetTranscriptionAnalyticsJobStatusAsync(transcriptionStartedMessage.AudioFileInfos).ConfigureAwait(false);

                // if any is not submitted, we can safely return here since we submit all requests at the same time - therefore all other providers should not have any running requests
                if (providerStatus == TranscriptionAnalyticsJobStatus.NotSubmitted)
                {
                    return TranscriptionAnalyticsJobStatus.NotSubmitted;
                }

                // if any is running, return and ma
                if (providerStatus == TranscriptionAnalyticsJobStatus.Running)
                {
                    return TranscriptionAnalyticsJobStatus.Running;
                }
            }

            return TranscriptionAnalyticsJobStatus.Completed;
        }

        public async Task<IEnumerable<string>> AddTranscriptionAnalyticsResultsToTranscripts(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings)
        {
            _ = speechTranscriptMappings ?? throw new ArgumentNullException(nameof(speechTranscriptMappings));

            var errors = new List<string>();

            foreach (var provider in this.providers)
            {
                var providerErros = await provider.AddTranscriptionAnalyticsResultsToTranscriptsAsync(speechTranscriptMappings).ConfigureAwait(false);
                errors.AddRange(providerErros);
            }

            return errors;
        }

        public async Task<IEnumerable<string>> SubmitTranscriptionAnalyticsJobsAndAddToAudioFileInfos(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings)
        {
            _ = speechTranscriptMappings ?? throw new ArgumentNullException(nameof(speechTranscriptMappings));

            var errors = new List<string>();

            foreach (var provider in this.providers)
            {
                var providerErros = await provider.SubmitTranscriptionAnalyticsJobsAsync(speechTranscriptMappings).ConfigureAwait(false);
                errors.AddRange(providerErros);
            }

            return errors;
        }
    }
}

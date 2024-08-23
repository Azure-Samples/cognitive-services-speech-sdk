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
    using Microsoft.Extensions.Options;

    public sealed class TranscriptionAnalyticsOrchestrator
    {
        private readonly List<ITranscriptionAnalyticsProvider> providers;

        private readonly AppConfig appConfig;

        public TranscriptionAnalyticsOrchestrator(
            string locale,
            ILogger logger,
            IOptions<AppConfig> appConfig)
        {
            this.appConfig = appConfig?.Value;
            this.providers = new List<ITranscriptionAnalyticsProvider>();

            if (!string.IsNullOrEmpty(this.appConfig.TextAnalyticsKey) && !string.IsNullOrEmpty(this.appConfig.TextAnalyticsEndpoint))
            {
                this.providers.Add(new TextAnalyticsProvider(locale, this.appConfig.TextAnalyticsKey, this.appConfig.TextAnalyticsEndpoint, logger, Options.Create(this.appConfig)));
                this.providers.Add(new AnalyzeConversationsProvider(locale, this.appConfig.TextAnalyticsKey, this.appConfig.TextAnalyticsEndpoint, logger, Options.Create(this.appConfig)));
            }
        }

        /// <summary>
        /// Gets the merged status of all transcription analytics jobs.
        /// </summary>
        /// <param name="transcriptionStartedMessage">The transcription started service bus message.</param>
        /// <returns>The merged job status.</returns>
        public async Task<TranscriptionAnalyticsJobStatus> GetTranscriptionAnalyticsJobsStatusAsync(TranscriptionStartedMessage transcriptionStartedMessage)
        {
            _ = transcriptionStartedMessage ?? throw new ArgumentNullException(nameof(transcriptionStartedMessage));

            foreach (var provider in this.providers)
            {
                var providerStatus = await provider.GetTranscriptionAnalyticsJobStatusAsync(transcriptionStartedMessage.AudioFileInfos).ConfigureAwait(false);

                // if any is not submitted, we can safely return here since we submit all requests at the same time - therefore all other providers should not have any running requests.
                if (providerStatus == TranscriptionAnalyticsJobStatus.NotSubmitted)
                {
                    return TranscriptionAnalyticsJobStatus.NotSubmitted;
                }

                // if any is running, we set the status to running and fetch it again after some time.
                if (providerStatus == TranscriptionAnalyticsJobStatus.Running)
                {
                    return TranscriptionAnalyticsJobStatus.Running;
                }
            }

            return TranscriptionAnalyticsJobStatus.Completed;
        }

        /// <summary>
        /// Submit transcription analytics jobs and adds their IDs to the audio file infos, so that they can get fetched the next time the transcription job status is polled.
        /// </summary>
        /// <param name="speechTranscriptMappings">The mapping from audio file infos to speech transcripts.</param>
        /// <returns>The errors if any.</returns>
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

        /// <summary>
        /// Adds the result of all transcription analytics jobs to the corresponding speech transcript.
        /// </summary>
        /// <param name="speechTranscriptMappings">The mapping from audio file infos to speech transcripts.</param>
        /// <returns>The errors if any.</returns>
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
    }
}

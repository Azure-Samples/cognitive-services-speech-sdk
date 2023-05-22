// <copyright file="BatchCompletionsClient.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net.Http;
    using System.Text;
    using System.Threading.Tasks;

    using Connector;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    using Microsoft.Extensions.Logging;

    using Newtonsoft.Json;

    public sealed class BatchCompletionsClient
    {
        private readonly StorageConnector storageConnector;
        private readonly HttpClient httpClient;
        private readonly string endpoint;
        private readonly string inputContainer;
        private readonly string targetContainer;
        private readonly string key;

        public BatchCompletionsClient(
            HttpClient httpClient,
            string key,
            string endpoint,
            string inputContainer,
            string targetContainer,
            StorageConnector storageConnector)
        {
            this.httpClient = httpClient;
            this.key = key;
            this.endpoint = endpoint;
            this.inputContainer = inputContainer;
            this.targetContainer = targetContainer;
            this.storageConnector = storageConnector;
            /////this.httpClient.DefaultRequestHeaders.Add("Content-Type", "application/json");
            /////this.httpClient.DefaultRequestHeaders.Add("api-key", key);
        }

        public async Task<string> SubmitBatchCompletionJobs(SpeechTranscript speechTranscript, ILogger logger)
        {
            _ = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));
            var inputFileName = StorageConnector.GetFileNameFromUri(new Uri(speechTranscript.Source));
            var transcriptBuilder = new StringBuilder();

            var speakerIdentifierFunc = GetSpeakerIdentifierFunc(speechTranscript);

            var orderedPhrases = speechTranscript.RecognizedPhrases.OrderBy(r => r.OffsetInTicks).Where(r => r.NBest?.FirstOrDefault() != null);

            foreach (var recognizedPhrase in orderedPhrases)
            {
                var line = $"{speakerIdentifierFunc((recognizedPhrase.Channel, recognizedPhrase.Speaker))}: {recognizedPhrase.NBest.First().Lexical}\r\n";
                transcriptBuilder.Append(line);
            }

            var transcript = transcriptBuilder.ToString();

            var requestFileBuilder = new StringBuilder();

            var generalCallCenterLine = $"{{\"prompt\": \"{Prompts.BuildGeneralCallCenterPrompt(transcript)}\",\"max_tokens\":1024}}";
            requestFileBuilder.AppendLine(generalCallCenterLine);

            var namedEntityRecognitionLine = $"{{\"prompt\": \"{Prompts.BuildNamedEntityRecognitionPrompt(transcript)}\",\"max_tokens\":1024}}";
            requestFileBuilder.AppendLine(namedEntityRecognitionLine);

            var inputFileUrl = this.storageConnector.GetFullBlobUrl(this.inputContainer, inputFileName);
            await this.storageConnector.WriteTextFileToBlobAsync(requestFileBuilder.ToString(), this.inputContainer, inputFileName, logger).ConfigureAwait(false);

            using var httpRequest = new HttpRequestMessage(HttpMethod.Post, new Uri($"{this.endpoint}/openai/completions:submit-batch?api-version=2023-05-04-preview"));
            httpRequest.Headers.Add("api-key", this.key);
            httpRequest.Headers.Add("Content-Type", "application/json");

            var requestBody = new
            {
                model = "gpt-35-turbo",
                blob_prefix = "jvandervegtetest01",
                content_url = inputFileUrl.ToString(),
                target_container = "https://jvandervegteopenaitest.blob.core.windows.net/results"
            };

            httpRequest.Content = new StringContent(JsonConvert.SerializeObject(requestBody), Encoding.UTF8, "application/json");

        }

        public async Task<bool> BatchCompletionJobsCompleted(IEnumerable<AudioFileInfo> audioFileInfos)
        {
            if (audioFileInfos == null || !audioFileInfos.Where(audioFileInfo => audioFileInfo.TextAnalyticsRequests != null).Any())
            {
                return true;
            }
        }

        private static Func<(int channelId, int speakerId), string> GetSpeakerIdentifierFunc(SpeechTranscript speechTranscript)
        {
            var channelCount = speechTranscript.RecognizedPhrases.DistinctBy(r => r.Channel).Count();
            var speakerCount = speechTranscript.RecognizedPhrases.DistinctBy(r => r.Speaker).Count();

            if (channelCount > 1 && speakerCount > 1)
            {
                return x => $"Channel{x.channelId}_Speaker{x.speakerId}";
            }
            else if (channelCount > 1)
            {
                return x => $"Channel_{x.channelId}";
            }
            else if (speakerCount > 1)
            {
                return x => $"Speaker_{x.speakerId}";
            }

            return x => "Speaker";
        }

    }
}

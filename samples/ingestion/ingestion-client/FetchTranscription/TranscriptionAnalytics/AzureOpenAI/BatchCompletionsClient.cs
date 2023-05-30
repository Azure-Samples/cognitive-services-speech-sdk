// <copyright file="BatchCompletionsClient.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Collections.Generic;
    using System.Data;
    using System.Linq;
    using System.Net.Http;
    using System.Text;
    using System.Threading.Tasks;

    using Connector;
    using Connector.Enums;
    using Connector.Serializable.TranscriptionStartedMessage.AzureOpenAI;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    using Microsoft.Extensions.Logging;

    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    using static Connector.Serializable.TranscriptionStartedMessage.AzureOpenAI.BatchCompletionRequest;

    public sealed class BatchCompletionsClient : ITranscriptionAnalyticsProvider
    {
        private readonly StorageConnector storageConnector;
        private readonly HttpClient httpClient;
        private readonly string endpoint;
        private readonly string inputContainer;
        private readonly string targetContainer;
        private readonly string key;
        private readonly ILogger logger;

        public BatchCompletionsClient(
            HttpClient httpClient,
            string key,
            string endpoint,
            string inputContainer,
            string targetContainer,
            StorageConnector storageConnector,
            ILogger logger)
        {
            this.httpClient = httpClient;
            this.key = key;
            this.endpoint = endpoint;
            this.inputContainer = inputContainer;
            this.targetContainer = targetContainer;
            this.storageConnector = storageConnector;
            this.logger = logger;
        }

        public async Task<TranscriptionAnalyticsJobStatus> GetTranscriptionAnalyticsJobStatusAsync(IEnumerable<AudioFileInfo> audioFileInfos)
        {
            if (!audioFileInfos.Where(audioFileInfo => audioFileInfo.AzureOpenAIRequests != null).Any())
            {
                return TranscriptionAnalyticsJobStatus.NotSubmitted;
            }

            var runningRequests = audioFileInfos.Where(audioFileInfo => audioFileInfo.AzureOpenAIRequests?.BatchCompletionRequest != null &&
                !IsTerminatedBatchCompletionRequestStatus(audioFileInfo.AzureOpenAIRequests.BatchCompletionRequest.Status))
                .Select(audioFileInfo => audioFileInfo.AzureOpenAIRequests.BatchCompletionRequest);

            var status = TranscriptionAnalyticsJobStatus.Completed;

            foreach (var batchCompletionRequest in runningRequests)
            {
                var responseContent = await this.GetOperationResponseAsync(batchCompletionRequest.OperationLocation).ConfigureAwait(false);
                var responseStatus = BatchCompletionRequestStatus.Failed;

                if (responseContent != null &&
                    responseContent.ContainsKey("status") &&
                    Enum.TryParse<BatchCompletionRequestStatus>((string)responseContent["status"], out var parsedStatus))
                {
                    responseStatus = parsedStatus;
                }

                batchCompletionRequest.Status = responseStatus;

                if (!IsTerminatedBatchCompletionRequestStatus(responseStatus))
                {
                    status = TranscriptionAnalyticsJobStatus.Running;
                }
            }

            return status;
        }

        public async Task<IEnumerable<string>> SubmitTranscriptionAnalyticsJobsAsync(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings)
        {
            _ = speechTranscriptMappings ?? throw new ArgumentNullException(nameof(speechTranscriptMappings));

            var errors = new List<string>();
            foreach (var speechTranscriptMapping in speechTranscriptMappings)
            {
                var speechTranscript = speechTranscriptMapping.Value;
                var audioFileInfo = speechTranscriptMapping.Key;

                var innerErrors = await this.SubmitBatchCompletionRequestsAndAddToAudioFileInfoAsync(speechTranscript, audioFileInfo).ConfigureAwait(false);
                errors.AddRange(innerErrors);
            }

            return errors;
        }

        public async Task<IEnumerable<string>> AddTranscriptionAnalyticsResultsToTranscriptsAsync(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings)
        {
            _ = speechTranscriptMappings ?? throw new ArgumentNullException(nameof(speechTranscriptMappings));

            var errors = new List<string>();
            foreach (var speechTranscriptMapping in speechTranscriptMappings)
            {
                var speechTranscript = speechTranscriptMapping.Value;
                var audioFileInfo = speechTranscriptMapping.Key;
                var fileName = audioFileInfo.FileName;

                var innerErrors = await this.AddBatchCompletionResultsAync(speechTranscript, audioFileInfo).ConfigureAwait(false);
                var errorMessage = $"File {(string.IsNullOrEmpty(fileName) ? "unknown" : fileName)}:\n{string.Join('\n', innerErrors)}";
                errors.Add(errorMessage);
            }

            return errors;
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

        private async Task<JObject> GetOperationResponseAsync(string operationLocation)
        {
            using var httpRequest = new HttpRequestMessage(HttpMethod.Get, new Uri(operationLocation));
            httpRequest.Headers.Add("api-key", this.key);
            var response = await this.httpClient.SendAsync(httpRequest).ConfigureAwait(false);

            response.EnsureSuccessStatusCode();
            var contentString = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
            var responseContent = JObject.Parse(contentString);

            return responseContent;
        }

        private async Task<IEnumerable<string>> SubmitBatchCompletionRequestsAndAddToAudioFileInfoAsync(SpeechTranscript speechTranscript, AudioFileInfo audioFileInfo)
        {
            var errors = new List<string>();
            var inputFileName = audioFileInfo.FileName;
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
            await this.storageConnector.WriteTextFileToBlobAsync(requestFileBuilder.ToString(), this.inputContainer, inputFileName, this.logger).ConfigureAwait(false);

            using var httpRequest = new HttpRequestMessage(HttpMethod.Post, new Uri($"{this.endpoint}/openai/completions:submit-batch?api-version=2023-05-04-preview"));
            httpRequest.Headers.Add("api-key", this.key);
            httpRequest.Headers.Add("Content-Type", "application/json");

            var requestBody = new
            {
                model = "gpt-35-turbo",
                blob_prefix = inputFileName,
                content_url = inputFileUrl.ToString(),
                target_container = this.storageConnector.GetFullContainerUrl(this.targetContainer).ToString(),
            };

            httpRequest.Content = new StringContent(JsonConvert.SerializeObject(requestBody), Encoding.UTF8, "application/json");

            var response = await this.httpClient.SendAsync(httpRequest).ConfigureAwait(false);
            response.EnsureSuccessStatusCode();
            var operationLocation = response.Headers.GetValues("operation-location").First();

            if (audioFileInfo.AzureOpenAIRequests == null)
            {
                audioFileInfo.AzureOpenAIRequests = new AzureOpenAIRequests(new BatchCompletionRequest(operationLocation, BatchCompletionRequestStatus.Running));
            }
            else
            {
                audioFileInfo.AzureOpenAIRequests.BatchCompletionRequest = new BatchCompletionRequest(operationLocation, BatchCompletionRequestStatus.Running);
            }

            return errors;
        }

        private async Task<IEnumerable<string>> AddBatchCompletionResultsAync(SpeechTranscript speechTranscript, AudioFileInfo audioFileInfo)
        {
            var errors = new List<string>();

            if (audioFileInfo?.AzureOpenAIRequests?.BatchCompletionRequest == null)
            {
                return errors;
            }

            var responseContent = await this.GetOperationResponseAsync(audioFileInfo.AzureOpenAIRequests.BatchCompletionRequest.OperationLocation).ConfigureAwait(false);
            var responseStatus = BatchCompletionRequestStatus.None;

            if (responseContent != null &&
                responseContent.ContainsKey("status") &&
                Enum.TryParse<BatchCompletionRequestStatus>((string)responseContent["status"], out var parsedStatus))
            {
                responseStatus = parsedStatus;
            }

            if (responseStatus != BatchCompletionRequestStatus.Completed)
            {
                errors.Add($"Unexpected or missing status code: {responseStatus}");
                return errors;
            }

            if (!responseContent.ContainsKey("result") || responseContent["result"].Value<string>("content_url") == null)
            {
                errors.Add($"Unexpected or missing property result.content_url");
                return errors;
            }

            var contentUrl = new Uri(responseContent["result"].Value<string>("content_url"));

            var containerName = StorageConnector.GetContainerNameFromUri(contentUrl);
            var fileName = StorageConnector.GetFileNameFromUri(contentUrl);

            var resultBytes = await this.storageConnector.DownloadFileFromContainer(containerName, fileName).ConfigureAwait(false);
            var resultString = Encoding.UTF8.GetString(resultBytes);

            var batchCompletionResults = new List<JObject>();

            foreach (var line in resultString.Split(Environment.NewLine))
            {
                if (string.IsNullOrEmpty(line))
                {
                    continue;
                }

                try
                {
                    var parsedLine = JObject.Parse(line);
                    batchCompletionResults.Add(parsedLine);
                }
                catch (JsonReaderException e)
                {
                    var errorMessage = $"Parsing response failed with exception: {e}";
                    this.logger.LogError(errorMessage);
                    errors.Add(errorMessage);
                }
            }

            speechTranscript.AzureOpenAIResults = new AzureOpenAIResults()
            {
                BatchCompletionResults = batchCompletionResults,
            };

            return errors;
        }
    }
}

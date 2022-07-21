// <copyright file="AnalyzeConversationsProvider.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription.Language
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text.Json;
    using System.Threading.Tasks;

    using Azure;
    using Azure.AI.Language.Conversations;
    using Azure.Core;
    using Connector;
    using Connector.Serializable;
    using Connector.Serializable.Language.Conversations;
    using FetchTranscriptionFunction;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    /// <summary>
    /// Analyze Conversations async client.
    /// </summary>
    public class AnalyzeConversationsProvider
    {
        private static readonly TimeSpan RequestTimeout = TimeSpan.FromMinutes(3);

        private readonly ConversationAnalysisClient ConversationAnalysisClient;

        private readonly string Locale;

        private readonly ILogger Log;

        public AnalyzeConversationsProvider(string locale, string subscriptionKey, string region, ILogger log)
        {
            ConversationAnalysisClient = new ConversationAnalysisClient(new Uri($"https://{region}.api.cognitive.microsoft.com"), new AzureKeyCredential(subscriptionKey));

            Locale = locale;
            Log = log;
        }

        public bool IsConversationalPiiEnabled()
        {
            return Locale.Contains("en", StringComparison.OrdinalIgnoreCase) && FetchTranscriptionEnvironmentVariables.ConversationPiiSetting != Connector.Enums.ConversationPiiSetting.None;
        }

        /// <summary>
        /// API to submit an analyzeConversations async Request.
        /// </summary>
        /// <param name="speechTranscript">Instance of the speech transcript.</param>
        /// <returns>An enumerable of the jobs IDs and errors if any.</returns>
        public async Task<(IEnumerable<string> jobIds, IEnumerable<string> errors)> SubmitAnalyzeConversationsRequestAsync(SpeechTranscript speechTranscript)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            var data = new AnalyzeConversationsRequest
            {
                DisplayName = "IngestionClient",
                AnalysisInput = new AnalysisInput(new[]
                {
                    new Conversation
                    {
                        Id = Guid.NewGuid().ToString(),
                        Language = Locale,
                        Modality = Modality.transcript,
                        ConversationItems = speechTranscript.RecognizedPhrases.Where(r => !string.IsNullOrEmpty(r.NBest.First().Lexical))
                            .Select(item => new ConversationItem()
                            {
                                Text = item.NBest.First().Display,
                                Lexical = item.NBest.First().Lexical,
                                Itn = item.NBest.First().ITN,
                                MaskedItn = item.NBest.First().MaskedITN,
                                Id = $"{item.Channel}_{item.Offset}",
                                ParticipantId = $"{item.Channel}",
                                AudioTimings = item.NBest.First().Words
                                    .Select(word => new WordLevelAudioTiming
                                    {
                                        Word = word.Word,
                                        Duration = (long)word.DurationInTicks,
                                        Offset = (long)word.OffsetInTicks
                                    })
                            }).ToList()
                    }
                }),
                Tasks = new[]
                {
                    new AnalyzeConversationsTask
                    {
                        TaskName = "Conversation PII task",
                        Kind = AnalyzeConversationsTaskKind.ConversationalPIITask,
                        Parameters = new Dictionary<string, object>
                        {
                            {
                                "piiCategories",    FetchTranscriptionEnvironmentVariables.ConversationPiiCategories.Select(s => s.ToString()).ToList()
                            },
                            {
                                "redactionSource", "lexical"
                            },
                            {
                                "includeAudioRedaction", FetchTranscriptionEnvironmentVariables.ConversationPiiSetting == Connector.Enums.ConversationPiiSetting.IncludeAudioRedaction
                            }
                        }
                    }
                }
            };

            return await SubmitConversationsAsync(data).ConfigureAwait(false);
        }

        /// <summary>
        /// API to get the job result of all analyze conversation jobs.
        /// </summary>
        /// <param name="jobIds">Enumerable of jobIds.</param>
        /// <returns>Enumerable of results of conversation PII redaction and errors encountered if any.</returns>
        public async Task<(IEnumerable<AnalyzeConversationPiiResults> piiResults, IEnumerable<string> errors)> GetConversationsOperationsResult(IEnumerable<string> jobIds)
        {
            var piiResults = new List<AnalyzeConversationPiiResults>();

            var errors = new List<string>();

            if (!jobIds.Any())
            {
                return (null, errors);
            }

            var tasks = jobIds.Select(async jobId => await GetConversationsOperationResults(jobId).ConfigureAwait(false));

            var results = await Task.WhenAll(tasks).ConfigureAwait(false);
            foreach (var result in results)
            {
                piiResults.AddRange(result.piiResults);

                var piiErrors = piiResults.Where(s => s.Errors != null || s.Errors.Any());
                if (piiErrors.Any())
                {
                    errors.AddRange(piiErrors.SelectMany(s => s.Errors).Select(s => $"Error thrown for conversation : {s.Id}"));
                }
            }

            return (piiResults, errors);
        }

        /// <summary>
        /// Checks for all conversational analytics requests that were marked as running if they have completed and sets a new state accordingly.
        /// </summary>
        /// <param name="conversationRequests">Enumerable for conversationRequests.</param>
        /// <returns>True if all requests completed, else false.</returns>
        public async Task<bool> ConversationalRequestsCompleted(IEnumerable<Connector.Serializable.TranscriptionStartedServiceBusMessage.TextAnalyticsRequest> conversationRequests)
        {
            if (!IsConversationalPiiEnabled())
            {
                return true;
            }

            if (conversationRequests == null || !conversationRequests.Any())
            {
                return true;
            }

            var runningJobsCount = 0;

            foreach (var textAnalyticsJob in conversationRequests)
            {
                var response = await ConversationAnalysisClient.GetAnalyzeConversationJobStatusAsync(Guid.Parse(textAnalyticsJob.Id)).ConfigureAwait(false);

                if (response.IsError)
                {
                    continue;
                }

                var analysisResult = JsonConvert.DeserializeObject<AnalyzeConversationsResult>(response.Content.ToString());

                if (analysisResult.Tasks.InProgress == 0)
                {
                    // some jobs are still running.
                    runningJobsCount++;
                }
            }

            return runningJobsCount == 0;
        }

        private async Task<(IEnumerable<string> jobId, IEnumerable<string> errors)> SubmitConversationsAsync(dynamic data)
        {
            var errors = new List<string>();

            try
            {
                Log.LogInformation($"Sending language conversation request.");

                var operation = await ConversationAnalysisClient.AnalyzeConversationAsync(WaitUntil.Started, RequestContent.Create(JsonConvert.SerializeObject(data))).ConfigureAwait(false);

                var response = await operation.UpdateStatusAsync().ConfigureAwait(false);

                using JsonDocument result = JsonDocument.Parse(response.ContentStream);
                var jobResults = result.RootElement;
                var jobId = jobResults.GetProperty("jobId");

                return (new List<string> { jobId.GetString() }, errors);
            }
            catch (OperationCanceledException)
            {
                throw new TimeoutException($"The operation has timed out after {RequestTimeout.TotalSeconds} seconds.");
            }

            // do not catch throttling errors, rather throw and retry
            catch (RequestFailedException e) when (e.Status != 429)
            {
                errors.Add($"Text analytics request failed with error: {e.Message}");
            }

            return (null, errors);
        }

        private async Task<(IEnumerable<AnalyzeConversationPiiResults> piiResults, IEnumerable<string> errors)> GetConversationsOperationResults(string jobId)
        {
            var errors = new List<string>();
            try
            {
                Log.LogInformation($"Sending text analytics request for jobid {jobId}.");

                var response = await ConversationAnalysisClient.GetAnalyzeConversationJobStatusAsync(Guid.Parse(jobId)).ConfigureAwait(false);

                if (response.IsError)
                {
                    errors.Add($"Conversation analysis failed with error.");
                }

                var analysisResult = JsonConvert.DeserializeObject<AnalyzeConversationsResult>(response.Content.ToString());

                if (analysisResult.Tasks.InProgress == 0)
                {
                    // all tasks completed.
                    return (analysisResult.Tasks
                        .Items.Where(item => item.Kind == AnalyzeConversationsTaskResultKind.conversationalPIIResults)
                        .Select(s => s.Results), errors);
                }
            }
            catch (OperationCanceledException)
            {
                throw new TimeoutException($"The operation has timed out after {RequestTimeout.TotalSeconds} seconds.");
            }

            // do not catch throttling errors, rather throw and retry
            catch (RequestFailedException e) when (e.Status != 429)
            {
                errors.Add($"Conversation analysis request failed with error: {e.Message}");
            }

            return (null, errors);
        }
    }
}

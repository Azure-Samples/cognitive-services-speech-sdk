// <copyright file="TextAnalyticsProvider.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace TextAnalytics
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading;
    using System.Threading.Tasks;

    using Azure;
    using Azure.AI.TextAnalytics;

    using Connector;
    using Connector.Enums;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    using FetchTranscriptionFunction;

    using Language;

    using Microsoft.Extensions.Logging;

    using static Connector.Serializable.TranscriptionStartedServiceBusMessage.TextAnalyticsRequest;

    /// <summary>
    /// The text analytics provide.
    ///
    /// General overview of text analytics request processing:
    ///
    /// For a succeded transcription, check if transcription has text analytics job info.
    ///     if true:
    ///         Check if text analytics job terminated.
    ///         if true:
    ///             Add text analytics results to transcript, write transcript to storage.
    ///         if false:
    ///             Re-enqueue job, check again after X minutes.
    ///     if false:
    ///         Check if text analytics is requested
    ///         if true:
    ///             Add text analytics job info to transcription. Re-enqueue job, check again after X minutes.
    ///         if false:
    ///             Write transcript to storage.
    /// </summary>
    public class TextAnalyticsProvider
    {
        private const int MaxRecordsPerRequest = 25;

        private static readonly TimeSpan RequestTimeout = TimeSpan.FromMinutes(3);

        private readonly TextAnalyticsClient TextAnalyticsClient;

        private readonly string Locale;

        private readonly ILogger Log;

        public TextAnalyticsProvider(string locale, string subscriptionKey, string region, ILogger log)
        {
            TextAnalyticsClient = new TextAnalyticsClient(new Uri($"https://{region}.api.cognitive.microsoft.com"), new AzureKeyCredential(subscriptionKey));
            Locale = locale;
            Log = log;
        }

        /// <summary>
        /// Checks for all text analytics requests that were marked as running if they have completed and sets a new state accordingly.
        /// <param name="audioFileInfos"></param>
        /// <returns>True if all requests completed, else false.</returns>
        /// </summary>
        public async Task<bool> TextAnalyticsRequestsCompleted(IEnumerable<AudioFileInfo> audioFileInfos)
        {
            var runningTextAnalyticsRequests = new List<TextAnalyticsRequest>();

            if (audioFileInfos.Where(audioFileInfo => audioFileInfo.TextAnalyticsRequests.AudioLevelRequests != null).Any())
            {
                runningTextAnalyticsRequests.AddRange(audioFileInfos.SelectMany(audioFileInfo => audioFileInfo.TextAnalyticsRequests.AudioLevelRequests).Where(text => text.Status == TextAnalyticsRequestStatus.Running));
            }

            if (audioFileInfos.Where(audioFileInfo => audioFileInfo.TextAnalyticsRequests.UtteranceLevelRequests != null).Any())
            {
                runningTextAnalyticsRequests.AddRange(audioFileInfos.SelectMany(audioFileInfo => audioFileInfo.TextAnalyticsRequests.UtteranceLevelRequests).Where(text => text.Status == TextAnalyticsRequestStatus.Running));
            }

            var textAnalyticsRequestCompleted = true;

            foreach (var textAnalyticsJob in runningTextAnalyticsRequests)
            {
                var operation = new AnalyzeActionsOperation(textAnalyticsJob.Id, TextAnalyticsClient);

                using var cts = new CancellationTokenSource();
                cts.CancelAfter(RequestTimeout);
                await operation.UpdateStatusAsync(cts.Token).ConfigureAwait(false);

                if (operation.HasCompleted)
                {
                    textAnalyticsJob.Status = TextAnalyticsRequestStatus.Completed;
                }
                else
                {
                    textAnalyticsRequestCompleted = false;
                }
            }

            return textAnalyticsRequestCompleted;
        }

        /// <summary>
        /// Submits text analytics requests depending on the settings (sentimentAnalysisSetting). Every utterance will be submitted independently to text analytics.
        /// (This means for instance that every utterance will have a separate sentiment score).
        /// </summary>
        /// <param name="speechTranscript">The speech transcript object.</param>
        /// <param name="sentimentAnalysisSetting">The sentiment analysis setting.</param>
        /// <returns>The job ids and errors, if any were found.</returns>
        public async Task<(IEnumerable<string> jobIds, IEnumerable<string> errors)> SubmitUtteranceLevelRequests(
            SpeechTranscript speechTranscript,
            SentimentAnalysisSetting sentimentAnalysisSetting)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            if (sentimentAnalysisSetting != SentimentAnalysisSetting.UtteranceLevel)
            {
                return (new List<string>(), new List<string>());
            }

            var documents = speechTranscript.RecognizedPhrases.Where(r => r.NBest.FirstOrDefault() != null && !string.IsNullOrEmpty(r.NBest.First().Display)).Select(r => new TextDocumentInput($"{r.Channel}_{r.Offset}", r.NBest.First().Display) { Language = Locale });

            var actions = new TextAnalyticsActions
            {
                DisplayName = "IngestionClient",
                AnalyzeSentimentActions = new List<AnalyzeSentimentAction>() { new AnalyzeSentimentAction() }
            };

            return await SubmitDocumentsAsync(documents, actions).ConfigureAwait(false);
        }

        /// <summary>
        /// Submits text analytics requests depending on the settings (sentimentAnalysisSetting). The whole transcript (per channel) will be submitted in a single request.
        /// (This means for instance that one single sentiment score will be generated per channel).
        /// </summary>
        /// <param name="speechTranscript">The speech transcript object.</param>
        /// <param name="sentimentAnalysisSetting">The sentiment analysis setting.</param>
        /// <param name="piiRedactionSetting">The PII redaction setting.</param>
        /// <returns>The job ids and errors, if any were found.</returns>
        public async Task<(IEnumerable<string> jobIds, IEnumerable<string> errors)> SubmitAudioLevelRequests(
            SpeechTranscript speechTranscript,
            SentimentAnalysisSetting sentimentAnalysisSetting,
            PiiRedactionSetting piiRedactionSetting)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            if (sentimentAnalysisSetting != SentimentAnalysisSetting.AudioLevel && piiRedactionSetting != PiiRedactionSetting.UtteranceAndAudioLevel)
            {
                return (new List<string>(), new List<string>());
            }

            var documents = speechTranscript.CombinedRecognizedPhrases.Where(r => !string.IsNullOrEmpty(r.Display)).Select(r => new TextDocumentInput($"{r.Channel}", r.Display) { Language = Locale });

            var actions = new TextAnalyticsActions
            {
                DisplayName = "IngestionClient"
            };

            if (sentimentAnalysisSetting == SentimentAnalysisSetting.AudioLevel)
            {
                actions.AnalyzeSentimentActions = new List<AnalyzeSentimentAction>() { new AnalyzeSentimentAction() };
            }

            if (piiRedactionSetting == PiiRedactionSetting.UtteranceAndAudioLevel)
            {
                var action = new RecognizePiiEntitiesAction();

                if (!string.IsNullOrEmpty(FetchTranscriptionEnvironmentVariables.PiiCategories))
                {
                    var piiEntityCategories = FetchTranscriptionEnvironmentVariables.PiiCategories.Split(",").Select(c => new PiiEntityCategory(c));

                    foreach (var category in piiEntityCategories)
                    {
                        action.CategoriesFilter.Add(category);
                    }
                }

                actions.RecognizePiiEntitiesActions = new List<RecognizePiiEntitiesAction>() { action };
            }

            return await SubmitDocumentsAsync(documents, actions).ConfigureAwait(false);
        }

        /// <summary>
        /// Gets the (utterance-level) results from text analytics, adds the results to the speech transcript.
        /// </summary>
        /// <param name="jobIds">The text analytics job ids.</param>
        /// <param name="speechTranscript">The speech transcript object.</param>
        /// <returns>The errors, if any.</returns>
        public async Task<IEnumerable<string>> AddUtteranceLevelEntitiesAsync(
            IEnumerable<string> jobIds,
            SpeechTranscript speechTranscript)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));
            var errors = new List<string>();

            if (jobIds == null || !jobIds.Any())
            {
                return errors;
            }

            var (sentimentResults, piiResults, requestErrors) = await this.GetOperationsResultsAsync(jobIds).ConfigureAwait(false);
            errors.AddRange(requestErrors);

            foreach (var recognizedPhrase in speechTranscript.RecognizedPhrases)
            {
                var index = $"{recognizedPhrase.Channel}_{recognizedPhrase.Offset}";
                var firstNBest = recognizedPhrase.NBest.FirstOrDefault();

                if (sentimentResults != null)
                {
                    // utterance level requests can only contain sentiment results at the moment
                    var sentimentResult = sentimentResults.Where(s => s.Id == index).FirstOrDefault();

                    if (firstNBest != null)
                    {
                        firstNBest.Sentiment = new Sentiment()
                        {
                            Negative = sentimentResult?.DocumentSentiment.ConfidenceScores.Negative ?? 0.0,
                            Positive = sentimentResult?.DocumentSentiment.ConfidenceScores.Positive ?? 0.0,
                            Neutral = sentimentResult?.DocumentSentiment.ConfidenceScores.Neutral ?? 0.0,
                        };
                    }
                }
            }

            return errors;
        }

        /// <summary>
        /// Gets the (audio-level) results from text analytics, adds the results to the speech transcript.
        /// </summary>
        /// <param name="jobIds">The text analytics job ids.</param>
        /// <param name="speechTranscript">The speech transcript object.</param>
        /// <returns>The errors, if any.</returns>
        public async Task<IEnumerable<string>> AddAudioLevelEntitiesAsync(
            IEnumerable<string> jobIds,
            SpeechTranscript speechTranscript)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));
            var errors = new List<string>();

            if (jobIds == null || !jobIds.Any())
            {
                return errors;
            }

            var (sentimentResults, piiResults, requestErrors) = await this.GetOperationsResultsAsync(jobIds).ConfigureAwait(false);
            errors.AddRange(requestErrors);

            foreach (var combinedRecognizedPhrase in speechTranscript.CombinedRecognizedPhrases)
            {
                var channel = combinedRecognizedPhrase.Channel;
                var sentimentResult = sentimentResults.Where(document => document.Id.Equals($"{channel}", StringComparison.OrdinalIgnoreCase)).SingleOrDefault();

                if (sentimentResult != null)
                {
                    combinedRecognizedPhrase.Sentiment = new Sentiment()
                    {
                        Negative = sentimentResult.DocumentSentiment.ConfidenceScores.Negative,
                        Positive = sentimentResult.DocumentSentiment.ConfidenceScores.Positive,
                        Neutral = sentimentResult.DocumentSentiment.ConfidenceScores.Neutral,
                    };
                }

                if (!AnalyzeConversationsProvider.IsConversationalPiiEnabled())
                {
                    var piiResult = piiResults.Where(document => document.Id.Equals($"{channel}", StringComparison.OrdinalIgnoreCase)).SingleOrDefault();
                    if (piiResult != null)
                    {
                        var redactedText = piiResult.Entities.RedactedText;

                        combinedRecognizedPhrase.Display = redactedText;
                        combinedRecognizedPhrase.ITN = string.Empty;
                        combinedRecognizedPhrase.MaskedITN = string.Empty;
                        combinedRecognizedPhrase.Lexical = string.Empty;

                        var phrases = speechTranscript.RecognizedPhrases.Where(phrase => phrase.Channel == channel);

                        var startIndex = 0;
                        foreach (var phrase in phrases)
                        {
                            var firstNBest = phrase.NBest.FirstOrDefault();

                            if (firstNBest != null && !string.IsNullOrEmpty(firstNBest.Display))
                            {
                                firstNBest.Display = redactedText.Substring(startIndex, firstNBest.Display.Length);
                                firstNBest.ITN = string.Empty;
                                firstNBest.MaskedITN = string.Empty;
                                firstNBest.Lexical = string.Empty;

                                startIndex += firstNBest.Display.Length + 1;
                            }
                        }
                    }
                }
            }

            return errors;
        }

        private async Task<(IEnumerable<string> jobIds, IEnumerable<string> errors)> SubmitDocumentsAsync(
            IEnumerable<TextDocumentInput> documents,
            TextAnalyticsActions actions)
        {
            if (!documents.Any())
            {
                return (new List<string>(), new List<string>());
            }

            // Chunk documents to avoid running into text analytics #documents limit
            var chunkedDocuments = new List<List<TextDocumentInput>>();
            var totalDocuments = documents.Count();

            for (int i = 0; i < totalDocuments; i += MaxRecordsPerRequest)
            {
                var chunk = documents.Skip(i).Take(Math.Min(MaxRecordsPerRequest, totalDocuments - i)).ToList();
                chunkedDocuments.Add(chunk);
            }

            var tasks = new List<Task<(
                string jobId,
                IEnumerable<string> errors)>>();

            var counter = 0;
            foreach (var documentChunk in chunkedDocuments)
            {
                var index = counter;
                tasks.Add(SubmitDocumentsChunkAsync(index, documentChunk, actions));
                counter++;
            }

            var results = await Task.WhenAll(tasks).ConfigureAwait(false);

            var jobIds = results.Select(t => t.jobId);
            var errors = results.SelectMany(t => t.errors);

            return (jobIds, errors);
        }

        private async Task<(string jobId, IEnumerable<string> errors)> SubmitDocumentsChunkAsync(int chunkId, List<TextDocumentInput> documentChunk, TextAnalyticsActions actions)
        {
            var errors = new List<string>();

            try
            {
                Log.LogInformation($"Sending text analytics request for document chunk with id {chunkId}.");
                using var cts = new CancellationTokenSource();
                cts.CancelAfter(RequestTimeout);

                var operation = await TextAnalyticsClient.StartAnalyzeActionsAsync(documentChunk, actions, cancellationToken: cts.Token).ConfigureAwait(false);
                return (operation.Id, errors);
            }
            catch (OperationCanceledException operationCanceledException)
            {
                throw new TransientFailureException($"Operation was canceled after {RequestTimeout.TotalSeconds} seconds.", operationCanceledException);
            }

            // do not catch throttling errors, rather throw and retry
            catch (RequestFailedException e) when (e.Status != 429)
            {
                errors.Add($"Text analytics request failed with error: {e.Message}");
            }

            return (null, errors);
        }

        private async Task<(IEnumerable<AnalyzeSentimentResult> sentimentResults, IEnumerable<RecognizePiiEntitiesResult> piiResults, IEnumerable<string> errors)> GetOperationsResultsAsync(IEnumerable<string> jobIds)
        {
            var errors = new List<string>();
            var sentimentResults = new List<AnalyzeSentimentResult>();
            var piiResults = new List<RecognizePiiEntitiesResult>();

            if (!jobIds.Any())
            {
                return (sentimentResults, piiResults, errors);
            }

            var tasks = new List<Task<(
                IEnumerable<AnalyzeSentimentResult> sentimentResults,
                IEnumerable<RecognizePiiEntitiesResult> piiResults,
                IEnumerable<string> errors)>>();

            var counter = 0;
            foreach (var jobId in jobIds)
            {
                var index = counter;
                tasks.Add(GetOperationResults(index, jobId));
                counter++;
            }

            var results = await Task.WhenAll(tasks).ConfigureAwait(false);

            foreach (var result in results)
            {
                sentimentResults.AddRange(result.sentimentResults);
                piiResults.AddRange(result.piiResults);
                errors.AddRange(result.errors);
            }

            return (sentimentResults, piiResults, errors);
        }

        private async Task<(IEnumerable<AnalyzeSentimentResult> sentimentResults, IEnumerable<RecognizePiiEntitiesResult> piiResults, IEnumerable<string> errors)> GetOperationResults(int index, string operationId)
        {
            var errors = new List<string>();
            var sentimentResults = new List<AnalyzeSentimentResult>();
            var piiResults = new List<RecognizePiiEntitiesResult>();

            try
            {
                Log.LogInformation($"Sending text analytics request for document chunk with id {index}.");
                using var cts = new CancellationTokenSource();
                cts.CancelAfter(RequestTimeout);

                var textAnalyticsOperation = new AnalyzeActionsOperation(operationId, TextAnalyticsClient);

                await textAnalyticsOperation.UpdateStatusAsync().ConfigureAwait(false);

                if (!textAnalyticsOperation.HasCompleted)
                {
                    throw new InvalidOperationException("Tried to get result of not completed text analytics request.");
                }

                await foreach (var documentsInPage in textAnalyticsOperation.GetValuesAsync())
                {
                    foreach (var piiResult in documentsInPage.RecognizePiiEntitiesResults)
                    {
                        if (piiResult.HasError)
                        {
                            errors.Add($"PII recognition failed with error: {piiResult.Error.Message}");
                        }

                        piiResults.AddRange(piiResult.DocumentsResults);
                    }

                    foreach (var sentimentResult in documentsInPage.AnalyzeSentimentResults)
                    {
                        if (sentimentResult.HasError)
                        {
                            errors.Add($"Sentiment analysis failed with error: {sentimentResult.Error.Message}");
                        }

                        sentimentResults.AddRange(sentimentResult.DocumentsResults);
                    }
                }
            }
            catch (OperationCanceledException operationCanceledException)
            {
                throw new TransientFailureException($"Operation was canceled after {RequestTimeout.TotalSeconds} seconds.", operationCanceledException);
            }

            // do not catch throttling errors, rather throw and retry
            catch (RequestFailedException e) when (e.Status != 429)
            {
                errors.Add($"Text analytics request failed with error: {e.Message}");
            }

            return (sentimentResults, piiResults, errors);
        }
    }
}

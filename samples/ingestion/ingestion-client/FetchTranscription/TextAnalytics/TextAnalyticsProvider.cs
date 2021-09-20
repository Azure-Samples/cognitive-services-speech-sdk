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
    using FetchTranscriptionFunction;
    using Microsoft.Extensions.Logging;

    public class TextAnalyticsProvider
    {
        private const int MaxRecordsPerRequest = 25;

        private static readonly TimeSpan RequestTimeout = TimeSpan.FromMinutes(3);

        private static readonly TimeSpan PollingInterval = TimeSpan.FromSeconds(15);

        private readonly TextAnalyticsClient TextAnalyticsClient;

        private readonly string Locale;

        private readonly ILogger Log;

        public TextAnalyticsProvider(string locale, string subscriptionKey, string region, ILogger log)
        {
            TextAnalyticsClient = new TextAnalyticsClient(new Uri($"https://{region}.api.cognitive.microsoft.com"), new AzureKeyCredential(subscriptionKey));
            Locale = locale;
            Log = log;
        }

        public async Task<IEnumerable<string>> AddUtteranceLevelEntitiesAsync(
            SpeechTranscript speechTranscript,
            SentimentAnalysisSetting sentimentAnalysisSetting)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            var errors = new List<string>();

            if (sentimentAnalysisSetting != SentimentAnalysisSetting.UtteranceLevel)
            {
                return errors;
            }

            var documents = speechTranscript.RecognizedPhrases.Where(r => r.NBest.FirstOrDefault() != null).Select(r => new TextDocumentInput($"{r.Channel}_{r.Offset}", r.NBest.First().Display) { Language = Locale });

            var actions = new TextAnalyticsActions
            {
                DisplayName = "IngestionClient",
                AnalyzeSentimentActions = new List<AnalyzeSentimentAction>() { new AnalyzeSentimentAction() }
            };

            var (sentimentResults, piiResults, requestErrors) = await this.GetDocumentResultsAsync(documents, actions).ConfigureAwait(false);
            errors.AddRange(requestErrors);

            foreach (var recognizedPhrase in speechTranscript.RecognizedPhrases)
            {
                var index = $"{recognizedPhrase.Channel}_{recognizedPhrase.Offset}";
                var firstNBest = recognizedPhrase.NBest.FirstOrDefault();

                var sentimentResult = sentimentResults.Where(s => s.Id == index).FirstOrDefault();
                if (sentimentResult != null && firstNBest != null)
                {
                    firstNBest.Sentiment = new Sentiment()
                    {
                        Negative = sentimentResult.DocumentSentiment.ConfidenceScores.Negative,
                        Positive = sentimentResult.DocumentSentiment.ConfidenceScores.Positive,
                        Neutral = sentimentResult.DocumentSentiment.ConfidenceScores.Neutral,
                    };
                }
            }

            return errors;
        }

        public async Task<IEnumerable<string>> AddAudioLevelEntitiesAsync(
            SpeechTranscript speechTranscript,
            SentimentAnalysisSetting sentimentAnalysisSetting,
            PiiRedactionSetting piiRedactionSetting)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            var errors = new List<string>();

            if (sentimentAnalysisSetting != SentimentAnalysisSetting.AudioLevel && piiRedactionSetting != PiiRedactionSetting.UtteranceAndAudioLevel)
            {
                return errors;
            }

            // Remove other nBests if pii is redacted
            if (piiRedactionSetting != PiiRedactionSetting.None)
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

            var (sentimentResults, piiResults, requestErrors) = await this.GetDocumentResultsAsync(documents, actions).ConfigureAwait(false);
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

                if (piiRedactionSetting == PiiRedactionSetting.UtteranceAndAudioLevel)
                {
                    var piiResult = piiResults.Where(document => document.Id.Equals($"{channel}", StringComparison.OrdinalIgnoreCase)).SingleOrDefault();

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

            return errors;
        }

        private async Task<(
            IEnumerable<AnalyzeSentimentResult> sentimentResults,
            IEnumerable<RecognizePiiEntitiesResult> piiResults,
            IEnumerable<string> errors)>
            GetDocumentResultsAsync(
            IEnumerable<TextDocumentInput> documents,
            TextAnalyticsActions actions)
        {
            var errors = new List<string>();
            var sentimentResults = new List<AnalyzeSentimentResult>();
            var piiResults = new List<RecognizePiiEntitiesResult>();

            var chunkedDocuments = new List<List<TextDocumentInput>>();
            var totalDocuments = documents.Count();

            for (int i = 0; i < totalDocuments; i += MaxRecordsPerRequest)
            {
                var chunk = documents.Skip(i).Take(Math.Min(MaxRecordsPerRequest, totalDocuments - i)).ToList();
                chunkedDocuments.Add(chunk);
            }

            Log.LogInformation($"Sending text analytics requests for {chunkedDocuments.Count} chunks in total.");

            var tasks = new List<Task<(
                IEnumerable<AnalyzeSentimentResult> sentimentResults,
                IEnumerable<RecognizePiiEntitiesResult> piiResults,
                IEnumerable<string> errors)>>();

            var counter = 0;
            foreach (var documentChunk in chunkedDocuments)
            {
                var index = counter;
                tasks.Add(GetChunkedResults(index, documentChunk, actions));
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

        private async Task<(
            IEnumerable<AnalyzeSentimentResult> sentimentResults,
            IEnumerable<RecognizePiiEntitiesResult> piiResults,
            IEnumerable<string> errors)>
            GetChunkedResults(int chunkId, List<TextDocumentInput> documentChunk, TextAnalyticsActions actions)
        {
            var errors = new List<string>();
            var sentimentResults = new List<AnalyzeSentimentResult>();
            var piiResults = new List<RecognizePiiEntitiesResult>();

            try
            {
                Log.LogInformation($"Sending text analytics request for document chunk with id {chunkId}.");
                using var cts = new CancellationTokenSource();
                cts.CancelAfter(RequestTimeout);

                var operation = await TextAnalyticsClient.StartAnalyzeActionsAsync(documentChunk, actions, cancellationToken: cts.Token).ConfigureAwait(false);
                await operation.WaitForCompletionAsync(PollingInterval, cts.Token).ConfigureAwait(false);
                Log.LogInformation($"Received text analytics response for document chunk with id {chunkId}.");

                await foreach (var documentsInPage in operation.Value)
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
            catch (OperationCanceledException)
            {
                throw new TimeoutException($"The operation has timed out after {RequestTimeout.TotalSeconds} seconds.");
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

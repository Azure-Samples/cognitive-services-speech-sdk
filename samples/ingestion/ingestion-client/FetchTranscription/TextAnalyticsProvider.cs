// <copyright file="TextAnalyticsProvider.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Azure;
    using Azure.AI.TextAnalytics;
    using Connector;
    using Connector.Enums;
    using Microsoft.Extensions.Logging;

    public class TextAnalyticsProvider
    {
        private const int MaxRecordsPerRequest = 25;

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
            SentimentAnalysisSetting sentimentAnalysisSetting,
            PiiRedactionSetting piiRedactionSetting)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            var errors = new List<string>();

            if (piiRedactionSetting != PiiRedactionSetting.UtteranceLevel && sentimentAnalysisSetting != SentimentAnalysisSetting.UtteranceLevel)
            {
                return errors;
            }

            // Remove other nBests if pii is redacted
            if (piiRedactionSetting != PiiRedactionSetting.None)
            {
                foreach (var phrase in speechTranscript.RecognizedPhrases)
                {
                    if (phrase.NBest == null || !phrase.NBest.Any())
                    {
                        continue;
                    }

                    var firstNBest = phrase.NBest.First();
                    phrase.NBest = new[] { firstNBest };
                }
            }

            var documents = speechTranscript.RecognizedPhrases.Where(r => r.NBest.FirstOrDefault() != null).Select(r => new TextDocumentInput($"{r.Channel}_{r.Offset}", r.NBest.First().Display) { Language = Locale });

            var actions = new TextAnalyticsActions
            {
                DisplayName = "IngestionClient"
            };

            if (sentimentAnalysisSetting == SentimentAnalysisSetting.UtteranceLevel)
            {
                actions.AnalyzeSentimentActions = new List<AnalyzeSentimentAction>() { new AnalyzeSentimentAction() };
            }

            if (piiRedactionSetting == PiiRedactionSetting.UtteranceLevel)
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

            var fullTranscriptionPerChannelDict = new Dictionary<int, StringBuilder>();
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

                var piiResult = piiResults.Where(s => s.Id == index).FirstOrDefault();
                if (firstNBest != null)
                {
                    firstNBest.ITN = string.Empty;
                    firstNBest.MaskedITN = string.Empty;
                    firstNBest.Lexical = string.Empty;

                    // Remove word level timestamps if they exist
                    firstNBest.Words = null;

                    if (piiResult != null)
                    {
                        firstNBest.Display = piiResult.Entities.RedactedText;
                    }

                    if (fullTranscriptionPerChannelDict.ContainsKey(recognizedPhrase.Channel))
                    {
                        fullTranscriptionPerChannelDict[recognizedPhrase.Channel].Append(" " + firstNBest.Display);
                    }
                    else
                    {
                        fullTranscriptionPerChannelDict.Add(recognizedPhrase.Channel, new StringBuilder(firstNBest.Display));
                    }
                }
            }

            if (piiRedactionSetting == PiiRedactionSetting.UtteranceLevel)
            {
                foreach (var combinedRecognizedPhrase in speechTranscript.CombinedRecognizedPhrases)
                {
                    var channel = combinedRecognizedPhrase.Channel;

                    // Remove full transcription for channel:
                    combinedRecognizedPhrase.MaskedITN = string.Empty;
                    combinedRecognizedPhrase.ITN = string.Empty;
                    combinedRecognizedPhrase.Lexical = string.Empty;

                    if (fullTranscriptionPerChannelDict.ContainsKey(channel))
                    {
                        combinedRecognizedPhrase.Display = fullTranscriptionPerChannelDict[channel].ToString();
                    }
                }
            }

            return errors;
        }

        public async Task<IEnumerable<string>> AddAudioLevelEntitiesAsync(
            SpeechTranscript speechTranscript,
            SentimentAnalysisSetting sentimentAnalysisSetting)
        {
            speechTranscript = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            var documents = speechTranscript.CombinedRecognizedPhrases.Where(r => !string.IsNullOrEmpty(r.Display)).Select(r => new TextDocumentInput($"{r.Channel}", r.Display) { Language = Locale });

            var errors = new List<string>();

            if (sentimentAnalysisSetting != SentimentAnalysisSetting.AudioLevel)
            {
                return errors;
            }

            var actions = new TextAnalyticsActions
            {
                DisplayName = "IngestionClient",
                AnalyzeSentimentActions = new List<AnalyzeSentimentAction>() { new AnalyzeSentimentAction() }
            };

            var (sentimentResults, _, requestErrors) = await this.GetDocumentResultsAsync(documents, actions).ConfigureAwait(false);
            errors.AddRange(requestErrors);

            foreach (var combinedRecognizedPhrase in speechTranscript.CombinedRecognizedPhrases)
            {
                var sentimentResult = sentimentResults.Where(document => document.Id.Equals($"{combinedRecognizedPhrase.Channel}", StringComparison.OrdinalIgnoreCase)).SingleOrDefault();

                if (sentimentResult == null)
                {
                    continue;
                }

                combinedRecognizedPhrase.Sentiment = new Sentiment()
                {
                    Negative = sentimentResult.DocumentSentiment.ConfidenceScores.Negative,
                    Positive = sentimentResult.DocumentSentiment.ConfidenceScores.Positive,
                    Neutral = sentimentResult.DocumentSentiment.ConfidenceScores.Neutral,
                };
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

            var counter = 0;
            var chunks = chunkedDocuments.Count;

            foreach (var documentChunk in chunkedDocuments)
            {
                try
                {
                    Log.LogInformation($"Sending text analytics request for document chunk {counter}/{chunks}.");
                    var operation = await TextAnalyticsClient.StartAnalyzeActionsAsync(documentChunk, actions).ConfigureAwait(false);
                    await operation.WaitForCompletionAsync().ConfigureAwait(false);
                    Log.LogInformation($"Received text analytics response for document chunk {counter}/{chunks}.");

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

                // do not catch throttling errors, rather throw and retry
                catch (RequestFailedException e) when (e.Status != 429)
                {
                    errors.Add($"Text analytics request failed with error: {e.Message}");
                }

                counter++;
            }

            return (sentimentResults, piiResults, errors);
        }
    }
}

// <copyright file="TextAnalytics.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net.Http;
    using System.Text;
    using System.Threading.Tasks;
    using System.Web;
    using Connector;
    using Connector.Enums;
    using Connector.Serializable;
    using Connector.Serializable.TextAnalytics;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public class TextAnalytics
    {
        // Request limits for v3 API: https://docs.microsoft.com/azure/cognitive-services/text-analytics/concepts/data-limits
        private const int SentimentRequestLimit = 10;

        private const int TextAnalyticsRequestCharacterLimit = 5120;

        private const int PiiRedactionRequestLimit = 5;

        private const string Caller = "Microsoft Speech to Text";

        private static readonly HttpClient HttpClient = new HttpClient();

        private readonly Uri SentimentAnalysisUri;

        private readonly Uri PiiRedactionUri;

        private readonly string Locale;

        private readonly string SubscriptionKey;

        private readonly ILogger Log;

        public TextAnalytics(string locale, string subscriptionKey, string region, ILogger log)
        {
            Locale = locale;
            SentimentAnalysisUri = new Uri($"https://{region}.api.cognitive.microsoft.com/text/analytics/v3.0/sentiment?showStats=False");
            PiiRedactionUri = new Uri($"https://{region}.api.cognitive.microsoft.com/text/analytics/v3.1/entities/recognition/pii");
            SubscriptionKey = subscriptionKey;
            Log = log;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Don't fail full transcription job because of text analytics error - return transcript without text analytics.")]
        public async Task<IEnumerable<string>> AddSentimentToTranscriptAsync(SpeechTranscript speechTranscript, SentimentAnalysisSetting sentimentSetting)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var sentimentErrors = new List<string>();

            try
            {
                var textAnalyticsChunks = new List<TextAnalyticsRequestsChunk>();

                if (sentimentSetting == SentimentAnalysisSetting.UtteranceLevel)
                {
                    textAnalyticsChunks = CreateUtteranceLevelRequests(speechTranscript, SentimentRequestLimit);
                }
                else if (sentimentSetting == SentimentAnalysisSetting.AudioLevel)
                {
                    textAnalyticsChunks = CreateAudioLevelRequests(speechTranscript, SentimentRequestLimit);
                }

                var responses = new List<HttpResponseMessage>();
                foreach (var chunk in textAnalyticsChunks)
                {
                    var chunkString = JsonConvert.SerializeObject(chunk);
                    var response = await MakeRequestAsync(SentimentAnalysisUri, chunkString).ConfigureAwait(false);
                    responses.Add(response);
                }

                Log.LogInformation($"Total responses: {responses.Count}");
                sentimentErrors = await AddSentimentToSpeechTranscriptAsync(responses, speechTranscript, sentimentSetting).ConfigureAwait(false);

                return sentimentErrors;
            }
            catch (Exception e)
            {
                var sentimentError = $"Sentiment Analysis failed with exception: {e.Message}";
                Log.LogError(sentimentError);
                sentimentErrors.Add(sentimentError);
                return sentimentErrors;
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Don't fail full transcription job because of text analytics error - return transcript without text analytics.")]
        public async Task<IEnumerable<string>> RedactPiiAsync(SpeechTranscript speechTranscript, PiiRedactionSetting piiRedactionSetting)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var piiRedactionErrors = new List<string>();

            // Remove all nBests except the first one to make sure that no pii is included in other nBests:
            foreach (var phrase in speechTranscript.RecognizedPhrases)
            {
                if (phrase.NBest == null || !phrase.NBest.Any())
                {
                    continue;
                }

                var firstNBest = phrase.NBest.FirstOrDefault();
                phrase.NBest = new[] { firstNBest };
            }

            try
            {
                var textAnalyticsChunks = new List<TextAnalyticsRequestsChunk>();
                if (piiRedactionSetting == PiiRedactionSetting.UtteranceLevel)
                {
                    textAnalyticsChunks = CreateUtteranceLevelRequests(speechTranscript, PiiRedactionRequestLimit);
                }

                var responses = new List<HttpResponseMessage>();
                foreach (var chunk in textAnalyticsChunks)
                {
                    var uriBuilder = new UriBuilder(PiiRedactionUri);
                    var query = HttpUtility.ParseQueryString(uriBuilder.Query);

                    if (!string.IsNullOrEmpty(FetchTranscriptionEnvironmentVariables.PiiCategories))
                    {
                        query["piiCategories"] = FetchTranscriptionEnvironmentVariables.PiiCategories;
                    }

                    if (!string.IsNullOrEmpty(FetchTranscriptionEnvironmentVariables.PiiMinimumPrecision))
                    {
                        query["minimumPrecision"] = FetchTranscriptionEnvironmentVariables.PiiMinimumPrecision;
                    }

                    uriBuilder.Query = query.ToString();

                    var chunkString = JsonConvert.SerializeObject(chunk);
                    var response = await MakeRequestAsync(uriBuilder.Uri, chunkString).ConfigureAwait(false);
                    responses.Add(response);
                }

                Log.LogInformation($"Total responses: {responses.Count}");
                piiRedactionErrors = await RedactPiiInSpeechTranscriptAsync(responses, speechTranscript, piiRedactionSetting).ConfigureAwait(false);

                return piiRedactionErrors;
            }
            catch (Exception e)
            {
                var piiRedactionError = $"PII Redaction failed with exception: {e.Message}";
                Log.LogError(piiRedactionError);
                piiRedactionErrors.Add(piiRedactionError);
                return piiRedactionErrors;
            }
        }

        private List<TextAnalyticsRequestsChunk> CreateUtteranceLevelRequests(SpeechTranscript speechTranscript, int documentRequestLimit)
        {
            var textAnalyticChunks = new List<TextAnalyticsRequestsChunk>();

            if (!speechTranscript.RecognizedPhrases.Any())
            {
                return textAnalyticChunks;
            }

            var textAnalyticsDocumentList = new List<TextAnalyticsRequest>();
            foreach (var recognizedPhrase in speechTranscript.RecognizedPhrases)
            {
                var text = recognizedPhrase.NBest.FirstOrDefault().Display;

                if (!string.IsNullOrEmpty(text))
                {
                    var id = $"{recognizedPhrase.Channel}_{recognizedPhrase.Offset}";
                    text = text.Substring(0, Math.Min(text.Length, TextAnalyticsRequestCharacterLimit));
                    var textAnalyticsDocument = new TextAnalyticsRequest(Locale, id, text);
                    textAnalyticsDocumentList.Add(textAnalyticsDocument);
                }
            }

            for (int i = 0; i < textAnalyticsDocumentList.Count; i += documentRequestLimit)
            {
                textAnalyticChunks.Add(new TextAnalyticsRequestsChunk(textAnalyticsDocumentList.GetRange(i, Math.Min(documentRequestLimit, textAnalyticsDocumentList.Count - i))));
            }

            Log.LogInformation($"Received {textAnalyticChunks.Count} text analytics chunks from {textAnalyticsDocumentList.Count} documents.");
            return textAnalyticChunks;
        }

        private List<TextAnalyticsRequestsChunk> CreateAudioLevelRequests(SpeechTranscript speechTranscript, int documentRequestLimit)
        {
            var textAnalyticChunks = new List<TextAnalyticsRequestsChunk>();
            if (!speechTranscript.RecognizedPhrases.Any() || !speechTranscript.CombinedRecognizedPhrases.Any())
            {
                return textAnalyticChunks;
            }

            var textAnalyticsDocumentList = new List<TextAnalyticsRequest>();
            foreach (var combinedRecognizedPhrase in speechTranscript.CombinedRecognizedPhrases)
            {
                var channel = combinedRecognizedPhrase.Channel;
                var requestCount = 0;

                var displayForm = combinedRecognizedPhrase.Display;

                for (int i = 0; i < displayForm.Length; i += TextAnalyticsRequestCharacterLimit)
                {
                    var displayChunk = displayForm.Substring(i, Math.Min(TextAnalyticsRequestCharacterLimit, displayForm.Length - i));
                    var textAnalyticsDocument = new TextAnalyticsRequest(Locale, $"{channel}_{requestCount}", displayChunk);
                    textAnalyticsDocumentList.Add(textAnalyticsDocument);
                    requestCount += 1;
                }
            }

            for (int i = 0; i < textAnalyticsDocumentList.Count; i += documentRequestLimit)
            {
                textAnalyticChunks.Add(new TextAnalyticsRequestsChunk(textAnalyticsDocumentList.GetRange(i, Math.Min(documentRequestLimit, textAnalyticsDocumentList.Count - i))));
            }

            Log.LogInformation($"Received {textAnalyticChunks.Count} text analytics chunks from {textAnalyticsDocumentList.Count} documents.");
            return textAnalyticChunks;
        }

        private async Task<List<string>> RedactPiiInSpeechTranscriptAsync(List<HttpResponseMessage> responses, SpeechTranscript speechTranscript, PiiRedactionSetting piiRedactionSetting)
        {
            var piiRedactionErrors = new List<string>();

            if (!speechTranscript.RecognizedPhrases.Any())
            {
                return piiRedactionErrors;
            }

            var displayFull = new List<string>();
            var fullTranscriptionPerChannelDict = new Dictionary<int, StringBuilder>();

            foreach (var message in responses)
            {
                message.EnsureSuccessStatusCode();

                var responseBody = await message.Content.ReadAsStringAsync().ConfigureAwait(false);
                var textAnalyticsResponse = JsonConvert.DeserializeObject<TextAnalyticsResponse>(responseBody);

                foreach (var error in textAnalyticsResponse.Errors)
                {
                    var errorMessage = $"PII redaction failed with error: {error.Error.InnerError.Message}";
                    Log.LogError(errorMessage);
                    piiRedactionErrors.Add(errorMessage);
                }

                foreach (var document in textAnalyticsResponse.Documents)
                {
                    if (piiRedactionSetting == PiiRedactionSetting.UtteranceLevel)
                    {
                        var phrase = speechTranscript.RecognizedPhrases.Where(e => $"{e.Channel}_{e.Offset}".Equals(document.Id, StringComparison.Ordinal)).FirstOrDefault();

                        // Remove all text but the display form
                        if (phrase.NBest == null || !phrase.NBest.Any())
                        {
                            continue;
                        }

                        var nBest = phrase.NBest.FirstOrDefault();
                        nBest.ITN = string.Empty;
                        nBest.MaskedITN = string.Empty;
                        nBest.Lexical = string.Empty;

                        // Remove word level timestamps if they exist
                        nBest.Words = null;

                        nBest.Display = document.RedactedText;
                        phrase.NBest = new[] { nBest };

                        if (fullTranscriptionPerChannelDict.ContainsKey(phrase.Channel))
                        {
                            fullTranscriptionPerChannelDict[phrase.Channel].Append(" " + nBest.Display);
                        }
                        else
                        {
                            fullTranscriptionPerChannelDict.Add(phrase.Channel, new StringBuilder(nBest.Display));
                        }
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
                    combinedRecognizedPhrase.Display = string.Empty;

                    if (fullTranscriptionPerChannelDict.ContainsKey(channel))
                    {
                        combinedRecognizedPhrase.Display = fullTranscriptionPerChannelDict[channel].ToString();
                    }
                }
            }

            return piiRedactionErrors;
        }

        private async Task<List<string>> AddSentimentToSpeechTranscriptAsync(List<HttpResponseMessage> responses, SpeechTranscript speechTranscript, SentimentAnalysisSetting sentimentSetting)
        {
            var sentimentErrors = new List<string>();

            if (!speechTranscript.RecognizedPhrases.Any())
            {
                return sentimentErrors;
            }

            var textAnalyticsDocuments = new List<TextAnalyticsDocument>();

            foreach (var message in responses)
            {
                message.EnsureSuccessStatusCode();

                var responseBody = await message.Content.ReadAsStringAsync().ConfigureAwait(false);
                var textAnalyticsResponse = JsonConvert.DeserializeObject<TextAnalyticsResponse>(responseBody);

                foreach (var error in textAnalyticsResponse.Errors)
                {
                    var errorMessage = $"Sentiment extraction failed with error: {error.Error.InnerError.Message}";
                    Log.LogError(errorMessage);
                    sentimentErrors.Add(errorMessage);
                }

                textAnalyticsDocuments.AddRange(textAnalyticsResponse.Documents);
            }

            if (sentimentSetting == SentimentAnalysisSetting.UtteranceLevel)
            {
                foreach (var document in textAnalyticsDocuments)
                {
                    // Matching sentiment and transcription JSON by using the timestamp
                    var target = speechTranscript.RecognizedPhrases.Where(e => $"{e.Channel}_{e.Offset}".Equals(document.Id, StringComparison.Ordinal)).FirstOrDefault();

                    var nBest = target.NBest.FirstOrDefault();
                    if (nBest != null)
                    {
                        if (nBest.Sentiment == null)
                        {
                            nBest.Sentiment = new Sentiment();
                        }

                        nBest.Sentiment.Negative = document.ConfidenceScores.Negative;
                        nBest.Sentiment.Positive = document.ConfidenceScores.Positive;
                        nBest.Sentiment.Neutral = document.ConfidenceScores.Neutral;
                    }
                }
            }
            else if (sentimentSetting == SentimentAnalysisSetting.AudioLevel)
            {
                foreach (var combinedRecognizedPhrase in speechTranscript.CombinedRecognizedPhrases)
                {
                    var documents = textAnalyticsDocuments.Where(document => document.Id.StartsWith($"{combinedRecognizedPhrase.Channel}_", StringComparison.OrdinalIgnoreCase));

                    if (!documents.Any())
                    {
                        continue;
                    }

                    if (combinedRecognizedPhrase.Sentiment == null)
                    {
                        combinedRecognizedPhrase.Sentiment = new Sentiment();
                    }

                    combinedRecognizedPhrase.Sentiment.Negative = documents.Average(d => d.ConfidenceScores.Negative);
                    combinedRecognizedPhrase.Sentiment.Positive = documents.Average(d => d.ConfidenceScores.Positive);
                    combinedRecognizedPhrase.Sentiment.Neutral = documents.Average(d => d.ConfidenceScores.Neutral);
                }
            }

            return sentimentErrors;
        }

        private async Task<HttpResponseMessage> MakeRequestAsync(Uri requestUri, string chunkString)
        {
            using var requestMessage = new HttpRequestMessage(HttpMethod.Post, requestUri);
            requestMessage.Headers.Add("Ocp-Apim-Subscription-Key", SubscriptionKey);
            requestMessage.Headers.Add("x-ms-sender", Caller);

            requestMessage.Content = new StringContent(chunkString, Encoding.UTF8, "application/json");

            return await HttpClient.SendAsync(requestMessage).ConfigureAwait(false);
        }
    }
}

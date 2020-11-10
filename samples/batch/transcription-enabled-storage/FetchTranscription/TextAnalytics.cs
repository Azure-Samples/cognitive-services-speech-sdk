// <copyright file="TextAnalytics.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Net.Http;
    using System.Net.Http.Headers;
    using System.Reflection.Metadata.Ecma335;
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
        // Request limits for v3 API: https://docs.microsoft.com/en-us/azure/cognitive-services/text-analytics/concepts/data-limits
        private const int SentimentRequestLimit = 10;

        private const int TextAnalyticsRequestCharacterLimit = 5120;

        private const int EntityRecognitionRequestLimit = 5;

        private const string Caller = "Microsoft Speech to Text";

        private const string SentimentSuffix = "/sentiment?showStats=False";

        private const string EntityRecognitionSuffix = "/entities/recognition/general";

        private Uri TextAnalyticsUri;

        private string Locale;

        private string SubscriptionKey;

        private ILogger Log;

        public TextAnalytics(string locale, string subscriptionKey, string region, ILogger log)
        {
            Locale = locale;
            TextAnalyticsUri = new Uri($"https://{region}.api.cognitive.microsoft.com/text/analytics/v3.0");
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
                    var response = await MakeRequestAsync(chunkString, SentimentSuffix).ConfigureAwait(false);
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
        public async Task<IEnumerable<string>> RedactEntitiesAsync(SpeechTranscript speechTranscript, EntityRedactionSetting entityRedactionSetting)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var entityRedactionErrors = new List<string>();

            try
            {
                var textAnalyticsChunks = new List<TextAnalyticsRequestsChunk>();
                if (entityRedactionSetting == EntityRedactionSetting.UtteranceLevel)
                {
                    textAnalyticsChunks = CreateUtteranceLevelRequests(speechTranscript, EntityRecognitionRequestLimit);
                }

                var responses = new List<HttpResponseMessage>();
                foreach (var chunk in textAnalyticsChunks)
                {
                    var chunkString = JsonConvert.SerializeObject(chunk);
                    var response = await MakeRequestAsync(chunkString, EntityRecognitionSuffix).ConfigureAwait(false);
                    responses.Add(response);
                }

                Log.LogInformation($"Total responses: {responses.Count}");
                entityRedactionErrors = await RedactEntitiesInSpeechTranscriptAsync(responses, speechTranscript, entityRedactionSetting).ConfigureAwait(false);

                return entityRedactionErrors;
            }
            catch (Exception e)
            {
                var entityRedactionError = $"Entity Redaction failed with exception: {e.Message}";
                Log.LogError(entityRedactionError);
                entityRedactionErrors.Add(entityRedactionError);
                return entityRedactionErrors;
            }
        }

        private static bool IsMaskableEntityType(TextAnalyticsEntity entity)
        {
            switch (entity.Category)
            {
                case EntityCategory.Person:
                case EntityCategory.Organization:
                case EntityCategory.PhoneNumber:
                case EntityCategory.Email:
                case EntityCategory.URL:
                case EntityCategory.IPAddress:
                case EntityCategory.Quantity:
                    return true;

                default: return false;
            }
        }

        private static string RedactEntitiesInText(string text, IEnumerable<TextAnalyticsEntity> entities)
        {
            var maskableEntities = entities.Where(e => IsMaskableEntityType(e)).ToList();
            var cleanedEntities = RemoveOverlappingEntities(maskableEntities);

            // Order descending to make insertions that do not impact the offset of other entities
            cleanedEntities = cleanedEntities.OrderByDescending(o => o.Offset).ToList();

            foreach (var entity in cleanedEntities)
            {
                text = RedactEntityInText(text, entity);
            }

            return text;
        }

        private static string RedactEntityInText(string text, TextAnalyticsEntity entity)
        {
            var preMask = text.Substring(0, entity.Offset);
            var postMask = text.Substring(entity.Offset + entity.Length, text.Length - (entity.Offset + entity.Length));

            if (entity.Category == EntityCategory.Quantity && entity.SubCategory.Equals("Number", StringComparison.OrdinalIgnoreCase))
            {
                text = preMask + new string('#', entity.Length) + postMask;
            }
            else
            {
                if (!string.IsNullOrEmpty(entity.SubCategory))
                {
                    text = $"{preMask}#{entity.Category}-{entity.SubCategory}#{postMask}";
                }
                else
                {
                    text = $"{preMask}#{entity.Category}#{postMask}";
                }
            }

            return text;
        }

        private static List<TextAnalyticsEntity> RemoveOverlappingEntities(List<TextAnalyticsEntity> textAnalyticsEntities)
        {
            if (textAnalyticsEntities.Count <= 1)
            {
                return textAnalyticsEntities;
            }

            textAnalyticsEntities.OrderBy(o => o.Offset);
            List<TextAnalyticsEntity> resultEntities = textAnalyticsEntities;
            bool foundOverlap;

            do
            {
                foundOverlap = false;
                var remainingEntities = new List<TextAnalyticsEntity>();
                for (int i = 0; i < resultEntities.Count; i++)
                {
                    var current = resultEntities.ElementAt(i);
                    if (i != resultEntities.Count - 1)
                    {
                        var next = resultEntities.ElementAt(i + 1);

                        // if current entity overlaps with the next entity, ignore the next entity.
                        if (current.Offset + current.Length >= next.Offset)
                        {
                            foundOverlap = true;
                            i++;
                        }
                    }

                    remainingEntities.Add(current);
                }

                resultEntities = remainingEntities;
            }
            while (foundOverlap);

            return resultEntities;
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
                var id = $"{recognizedPhrase.Channel}_{recognizedPhrase.Offset}";
                var text = recognizedPhrase.NBest.FirstOrDefault().Display;
                text = text.Substring(0, Math.Min(text.Length, TextAnalyticsRequestCharacterLimit));
                var textAnalyticsDocument = new TextAnalyticsRequest(Locale, id, text);
                textAnalyticsDocumentList.Add(textAnalyticsDocument);
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
            if (!speechTranscript.RecognizedPhrases.Any())
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

        private async Task<List<string>> RedactEntitiesInSpeechTranscriptAsync(List<HttpResponseMessage> responses, SpeechTranscript speechTranscript, EntityRedactionSetting entityRedactionSetting)
        {
            var entityRedactionErrors = new List<string>();

            if (!speechTranscript.RecognizedPhrases.Any())
            {
                return entityRedactionErrors;
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
                    var errorMessage = $"Entity redaction failed with error: {error.Error.InnerError.Message}";
                    Log.LogError(errorMessage);
                    entityRedactionErrors.Add(errorMessage);
                }

                foreach (var document in textAnalyticsResponse.Documents)
                {
                    if (entityRedactionSetting == EntityRedactionSetting.UtteranceLevel)
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

                        nBest.Display = RedactEntitiesInText(nBest.Display, document.Entities);
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

            if (entityRedactionSetting == EntityRedactionSetting.UtteranceLevel)
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

            return entityRedactionErrors;
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

        private async Task<HttpResponseMessage> MakeRequestAsync(string chunkString, string uriSuffix)
        {
            using var client = new HttpClient();
            var queryString = HttpUtility.ParseQueryString(string.Empty);

            // Request headers
            client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", SubscriptionKey);
            client.DefaultRequestHeaders.Add("client-request-id", SubscriptionKey);
            client.DefaultRequestHeaders.Add("x-ms-sender", Caller);

            var uri = new Uri(TextAnalyticsUri.AbsoluteUri + uriSuffix);

            HttpResponseMessage response;

            // Request body
            byte[] byteData = Encoding.UTF8.GetBytes(chunkString);

            using (var content = new ByteArrayContent(byteData))
            {
                content.Headers.ContentType = new MediaTypeHeaderValue("application/json");
                response = await client.PostAsync(uri, content).ConfigureAwait(false);
                return response;
            }
        }
    }
}

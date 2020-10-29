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
    using System.Net.Http.Headers;
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

        public async Task<IEnumerable<string>> AddSentimentToTranscriptAsync(SpeechTranscript speechTranscript)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var textAnalyticsChunks = CreateRequestChunks(speechTranscript, SentimentRequestLimit);

            var responses = new List<HttpResponseMessage>();
            foreach (var chunk in textAnalyticsChunks)
            {
                var chunkString = JsonConvert.SerializeObject(chunk);
                var response = await MakeRequestAsync(chunkString, SentimentSuffix).ConfigureAwait(false);
                responses.Add(response);
            }

            Log.LogInformation($"Total responses: {responses.Count}");
            var sentimentErrors = await AddSentimentToSpeechTranscriptAsync(responses, speechTranscript).ConfigureAwait(false);
            return sentimentErrors;
        }

        public async Task<IEnumerable<string>> RedactEntitiesAsync(SpeechTranscript speechTranscript)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var textAnalyticsChunks = CreateRequestChunks(speechTranscript, EntityRecognitionRequestLimit);
            var responses = new List<HttpResponseMessage>();
            foreach (var chunk in textAnalyticsChunks)
            {
                var chunkString = JsonConvert.SerializeObject(chunk);
                var response = await MakeRequestAsync(chunkString, EntityRecognitionSuffix).ConfigureAwait(false);
                responses.Add(response);
            }

            Log.LogInformation($"Total responses: {responses.Count}");
            var entityRedactionErrors = await RedactEntitiesInSpeechTranscriptAsync(responses, speechTranscript).ConfigureAwait(false);

            return entityRedactionErrors;
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

        private static RecognizedPhrase RedactEntityInRecognizedPhrase(RecognizedPhrase recognizedPhrase, TextAnalyticsEntity entity)
        {
            if (!IsMaskableEntityType(entity) || !recognizedPhrase.NBest.Any())
            {
                return recognizedPhrase;
            }

            var nBest = recognizedPhrase.NBest.FirstOrDefault();

            // only keep first nBest:
            recognizedPhrase.NBest = new[] { nBest };

            var displayForm = nBest.Display;

            var preMask = displayForm.Substring(0, entity.Offset);
            var postMask = displayForm.Substring(entity.Offset + entity.Length, displayForm.Length - (entity.Offset + entity.Length));

            if (entity.Category == EntityCategory.Quantity && entity.SubCategory.Equals("Number", StringComparison.OrdinalIgnoreCase))
            {
                displayForm = preMask + new string('#', entity.Length) + postMask;
            }
            else
            {
                if (!string.IsNullOrEmpty(entity.SubCategory))
                {
                    displayForm = $"{preMask}#{entity.Category}-{entity.SubCategory}#{postMask}";
                }
                else
                {
                    displayForm = $"{preMask}#{entity.Category}#{postMask}";
                }
            }

            nBest.Display = displayForm;
            return recognizedPhrase;
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

        private List<TextAnalyticsRequestsChunk> CreateRequestChunks(SpeechTranscript speechTranscript, int documentRequestLimit)
        {
            var textAnalyticsDocumentList = new List<TextAnalyticsRequest>();
            var textAnalyticChunks = new List<TextAnalyticsRequestsChunk>();

            if (!speechTranscript.RecognizedPhrases.Any())
            {
                return textAnalyticChunks;
            }

            foreach (var recognizedPhrase in speechTranscript.RecognizedPhrases)
            {
                var id = $"{recognizedPhrase.Channel}_{recognizedPhrase.Offset}";
                var textAnalyticsDocument = new TextAnalyticsRequest(Locale, id, recognizedPhrase.NBest.FirstOrDefault().Display);
                textAnalyticsDocumentList.Add(textAnalyticsDocument);
            }

            for (int i = 0; i < textAnalyticsDocumentList.Count; i += documentRequestLimit)
            {
                textAnalyticChunks.Add(new TextAnalyticsRequestsChunk(textAnalyticsDocumentList.GetRange(i, Math.Min(documentRequestLimit, textAnalyticsDocumentList.Count - i))));
            }

            Log.LogInformation($"Received {textAnalyticChunks.Count} text analytics chunks from {textAnalyticsDocumentList.Count} documents.");
            return textAnalyticChunks;
        }

        private async Task<IEnumerable<string>> RedactEntitiesInSpeechTranscriptAsync(List<HttpResponseMessage> responses, SpeechTranscript speechTranscript)
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

                // Matching entities and transcription JSON by using the timestamp
                foreach (var document in textAnalyticsResponse.Documents)
                {
                    var newSegment = speechTranscript.RecognizedPhrases.Where(e => $"{e.Channel}_{e.Offset}".Equals(document.Id, StringComparison.Ordinal)).FirstOrDefault();

                    // Remove all text but the display form
                    if (newSegment.NBest == null || !newSegment.NBest.Any())
                    {
                        continue;
                    }

                    var nBest = newSegment.NBest.FirstOrDefault();

                    nBest.ITN = string.Empty;
                    nBest.MaskedITN = string.Empty;
                    nBest.Lexical = string.Empty;

                    // Remove word level timestamps if they exist
                    nBest.Words = null;

                    var maskableEntities = document.Entities.Where(e => IsMaskableEntityType(e)).ToList();
                    var entities = RemoveOverlappingEntities(maskableEntities);

                    // Order descending to make insertions that do not impact the offset of other entities
                    entities = entities.OrderByDescending(o => o.Offset).ToList();

                    foreach (var entity in entities)
                    {
                        RedactEntityInRecognizedPhrase(newSegment, entity);
                    }

                    // Create full transcription per channel
                    if (fullTranscriptionPerChannelDict.ContainsKey(newSegment.Channel))
                    {
                        fullTranscriptionPerChannelDict[newSegment.Channel].Append(" " + nBest.Display);
                    }
                    else
                    {
                        fullTranscriptionPerChannelDict.Add(newSegment.Channel, new StringBuilder(nBest.Display));
                    }
                }
            }

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

            return entityRedactionErrors;
        }

        private async Task<IEnumerable<string>> AddSentimentToSpeechTranscriptAsync(List<HttpResponseMessage> responses, SpeechTranscript speechTranscript)
        {
            var sentimentErrors = new List<string>();

            if (!speechTranscript.RecognizedPhrases.Any())
            {
                return sentimentErrors;
            }

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

                // Matching sentiment and transcription JSON by using the timestamp
                foreach (var document in textAnalyticsResponse.Documents)
                {
                    var targetSegment = speechTranscript.RecognizedPhrases.Where(e => $"{e.Channel}_{e.Offset}".Equals(document.Id, StringComparison.Ordinal)).FirstOrDefault();

                    var nBest = targetSegment.NBest.FirstOrDefault();
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

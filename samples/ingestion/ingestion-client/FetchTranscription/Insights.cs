// <copyright file="Insights.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net.Http;
    using System.Net.Http.Headers;
    using System.Text;
    using System.Threading.Tasks;
    using Connector;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    public class Insights
    {
        private static readonly TimeSpan InsightsTimeout = TimeSpan.FromMinutes(5);

        private static readonly HttpClient HttpClient = new () { Timeout = InsightsTimeout };

        private readonly Uri PiiRedactionUri;

        private readonly Uri CallReasonUri;

        private readonly string SubscriptionKey;

        private readonly string[] PiiCategories;

        private readonly ILogger Log;

        public Insights(string subscriptionKey, string region, string[] piiCategories, ILogger log)
        {
            PiiRedactionUri = new Uri($"https://{region}.stt.speech.microsoft.com/api/v1.0/insights/pii/transcript/redact");
            CallReasonUri = new Uri($"https://{region}.stt.speech.microsoft.com/api/v1.0/insights/callreason/transcript/predict");
            SubscriptionKey = subscriptionKey;
            PiiCategories = piiCategories;
            Log = log;
        }

        public string ConvertSpeechTranscriptToInsightsFormat(SpeechTranscript speechTranscript)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var isDiarizationEnabled = speechTranscript.RecognizedPhrases.Any(r => r.Speaker == 2);

            var requestObject = new
            {
                Transcript = new
                {
                    Segments = speechTranscript.RecognizedPhrases.Select(
                        phrase =>
                        new
                        {
                            Offset = phrase.OffsetInTicks,
                            Duration = phrase.DurationInTicks,
                            Confidence = phrase.NBest.FirstOrDefault()?.Confidence,
                            Display = phrase.NBest.FirstOrDefault()?.Display,
                            Itn = phrase.NBest.FirstOrDefault()?.ITN,
                            MaskedItn = phrase.NBest.FirstOrDefault()?.MaskedITN,
                            Lexical = phrase.NBest.FirstOrDefault()?.Lexical,
                            Channel = Math.Max(phrase.Channel, phrase.Speaker),
                            Words = phrase.NBest.FirstOrDefault()?.Words?.Select(
                                word =>
                                new
                                {
                                    Word = word.Word,
                                    Offset = word.OffsetInTicks,
                                    Duration = word.DurationInTicks
                                })
                        })
                }
            };

            var requestJObject = JObject.FromObject(requestObject);

            if (isDiarizationEnabled)
            {
                var segmentsObj = requestJObject["Transcript"] as JObject;

                segmentsObj.Property("Segments")
                    .AddAfterSelf(
                    new JProperty(
                        "ChannelRoles",
                        new JObject(
                            new JProperty("1", "Agent"),
                            new JProperty("2", "Customer"))));
            }

            if (this.PiiCategories.Any())
            {
                requestJObject.Add("Categories", JToken.FromObject(this.PiiCategories));
            }

            return JsonConvert.SerializeObject(requestJObject, Formatting.Indented);
        }

        public async Task<IEnumerable<string>> AddRedactionToTranscriptAsync(SpeechTranscript speechTranscript)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var redactionErrors = new List<string>();

            try
            {
                var chunkString = this.ConvertSpeechTranscriptToInsightsFormat(speechTranscript);
                var byteData = Encoding.UTF8.GetBytes(chunkString);

                using var content = new ByteArrayContent(byteData);
                content.Headers.ContentType = new MediaTypeHeaderValue("application/json");
                content.Headers.Add("Ocp-Apim-Subscription-Key", SubscriptionKey);

                Log.LogInformation("Starting redaction request.");
                var response = await HttpClient.PostAsync(PiiRedactionUri, content).ConfigureAwait(false);

                await response.EnsureSuccessStatusCodeAsync().ConfigureAwait(false);

                var respo = await response.Content.ReadAsStringAsync().ConfigureAwait(false);

                speechTranscript.AddRedactionResponse(JObject.Parse(respo));
                Log.LogInformation("Added redaction response to transcript.");
            }
            catch (Exception exception)
            {
                // only throw if there is a chance that we'd get the response in a retry, otherwise just log it as error
                if (exception is HttpStatusCodeException statusCodeException &&
                    statusCodeException.HttpStatusCode.HasValue &&
                    statusCodeException.HttpStatusCode.Value.IsRetryableStatus())
                {
                    throw;
                }

                var entityRedactionError = $"Redaction request failed with exception: {exception.Message}";
                Log.LogError(entityRedactionError);
                redactionErrors.Add(entityRedactionError);
            }

            return redactionErrors.AsEnumerable<string>();
        }

        public async Task<IEnumerable<string>> AddCallReasonToTranscriptAsync(SpeechTranscript speechTranscript)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            var callReasonErrors = new List<string>();

            try
            {
                var chunkString = this.ConvertSpeechTranscriptToInsightsFormat(speechTranscript);
                var byteData = Encoding.UTF8.GetBytes(chunkString);

                using var content = new ByteArrayContent(byteData);
                content.Headers.ContentType = new MediaTypeHeaderValue("application/json");
                content.Headers.Add("Ocp-Apim-Subscription-Key", SubscriptionKey);

                Log.LogInformation("Starting call reason request.");
                var response = await HttpClient.PostAsync(CallReasonUri, content).ConfigureAwait(false);

                response.EnsureSuccessStatusCode();

                var respo = await response.Content.ReadAsStringAsync().ConfigureAwait(false);

                speechTranscript.AddCallReasonResponse(JObject.Parse(respo));
                Log.LogInformation("Added call reason response to transcript.");
            }
            catch (Exception exception)
            {
                // only throw if there is a chance that we'd get the response in a retry, otherwise just log it as error
                if (exception is HttpStatusCodeException statusCodeException &&
                    statusCodeException.HttpStatusCode.HasValue &&
                    statusCodeException.HttpStatusCode.Value.IsRetryableStatus())
                {
                    throw;
                }

                var entityRedactionError = $"Call reason request failed with exception: {exception.Message}";
                Log.LogError(entityRedactionError);
                callReasonErrors.Add(entityRedactionError);
            }

            return callReasonErrors.AsEnumerable<string>();
        }
    }
}

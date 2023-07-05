// <copyright file="BatchClient.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.Net.Http;
    using System.Net.Sockets;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Connector.Serializable.TranscriptionFiles;
    using Newtonsoft.Json;
    using Polly;
    using Polly.Retry;

    public static class BatchClient
    {
        private const string TranscriptionsBasePath = "speechtotext/v3.0/Transcriptions/";

        private const int MaxNumberOfRetries = 10;

        private static readonly TimeSpan PostTimeout = TimeSpan.FromMinutes(1);

        private static readonly TimeSpan DefaultTimeout = TimeSpan.FromMinutes(2);

        private static readonly TimeSpan GetFilesTimeout = TimeSpan.FromMinutes(5);

        private static readonly HttpClient HttpClient = new HttpClient() { Timeout = Timeout.InfiniteTimeSpan };

        private static readonly AsyncRetryPolicy RetryPolicy =
            Policy
                .Handle<Exception>(e => e is HttpStatusCodeException || e is HttpRequestException)
                .WaitAndRetryAsync(MaxNumberOfRetries, retryAttempt => TimeSpan.FromSeconds(5));

        public static Task<TranscriptionReportFile> GetTranscriptionReportFileFromSasAsync(string sasUri)
        {
            return GetAsync<TranscriptionReportFile>(sasUri, null, DefaultTimeout);
        }

        public static Task<SpeechTranscript> GetSpeechTranscriptFromSasAsync(string sasUri)
        {
            return GetAsync<SpeechTranscript>(sasUri, null, DefaultTimeout);
        }

        public static Task<Transcription> GetTranscriptionAsync(string transcriptionLocation, string subscriptionKey)
        {
            return GetAsync<Transcription>(transcriptionLocation, subscriptionKey, DefaultTimeout);
        }

        public static async Task<TranscriptionFiles> GetTranscriptionFilesAsync(string transcriptionLocation, string subscriptionKey)
        {
            var path = $"{transcriptionLocation}/files";
            var combinedTranscriptionFiles = new List<TranscriptionFile>();

            do
            {
                var transcriptionFiles = await GetAsync<TranscriptionFiles>(path, subscriptionKey, GetFilesTimeout).ConfigureAwait(false);
                combinedTranscriptionFiles.AddRange(transcriptionFiles.Values);
                path = transcriptionFiles.NextLink;
            }
            while (!string.IsNullOrEmpty(path));

            return new TranscriptionFiles(combinedTranscriptionFiles, null);
        }

        public static Task DeleteTranscriptionAsync(string transcriptionLocation, string subscriptionKey)
        {
            return DeleteAsync(transcriptionLocation, subscriptionKey, DefaultTimeout);
        }

        public static async Task<Uri> PostTranscriptionAsync(TranscriptionDefinition transcriptionDefinition, string hostName, string subscriptionKey)
        {
            var path = $"{hostName}{TranscriptionsBasePath}";
            var payloadString = JsonConvert.SerializeObject(transcriptionDefinition);

            return await PostAsync(path, subscriptionKey, payloadString, PostTimeout).ConfigureAwait(false);
        }

        private static async Task<Uri> PostAsync(string path, string subscriptionKey, string payloadString, TimeSpan timeout)
        {
            var responseMessage = await SendHttpRequestMessage(HttpMethod.Post, path, subscriptionKey, payloadString, timeout).ConfigureAwait(false);
            return responseMessage.Headers.Location;
        }

        private static async Task DeleteAsync(string path, string subscriptionKey, TimeSpan timeout)
        {
            await SendHttpRequestMessage(HttpMethod.Delete, path, subscriptionKey, payload: null, timeout: timeout).ConfigureAwait(false);
        }

        private static async Task<TResponse> GetAsync<TResponse>(string path, string subscriptionKey, TimeSpan timeout)
        {
            var responseMessage = await SendHttpRequestMessage(HttpMethod.Get, path, subscriptionKey, payload: null, timeout: timeout).ConfigureAwait(false);

            var contentString = await responseMessage.Content.ReadAsStringAsync().ConfigureAwait(false);
            return JsonConvert.DeserializeObject<TResponse>(contentString);
        }

        private static async Task<HttpResponseMessage> SendHttpRequestMessage(HttpMethod httpMethod, string path, string subscriptionKey, string payload, TimeSpan timeout)
        {
            try
            {
                using var cts = new CancellationTokenSource();
                cts.CancelAfter(timeout);

                var responseMessage = await RetryPolicy.ExecuteAsync(
                    async (token) =>
                    {
                        using var httpRequestMessage = new HttpRequestMessage(httpMethod, path);
                        if (!string.IsNullOrEmpty(subscriptionKey))
                        {
                            httpRequestMessage.Headers.Add("Ocp-Apim-Subscription-Key", subscriptionKey);
                        }

                        if (!string.IsNullOrEmpty(payload))
                        {
                            httpRequestMessage.Content = new StringContent(payload, Encoding.UTF8, "application/json");
                        }

                        var responseMessage = await HttpClient.SendAsync(httpRequestMessage, token).ConfigureAwait(false);

                        await responseMessage.EnsureSuccessStatusCodeAsync().ConfigureAwait(false);
                        return responseMessage;
                    },
                    cts.Token).ConfigureAwait(false);

                return responseMessage;
            }
            catch (HttpRequestException httpRequestException)
            {
                throw new TransientFailureException($"HttpRequestException: {httpRequestException.Message}.", httpRequestException);
            }
            catch (OperationCanceledException operationCanceledException)
            {
                throw new TransientFailureException($"Operation was canceled after {timeout.TotalSeconds} seconds.", operationCanceledException);
            }
        }
    }
}

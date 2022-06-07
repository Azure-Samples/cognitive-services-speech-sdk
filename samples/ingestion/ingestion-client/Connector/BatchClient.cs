// <copyright file="BatchClient.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Connector.Serializable.TranscriptionFiles;
    using Newtonsoft.Json;

    public static class BatchClient
    {
        private const string TranscriptionsBasePath = "speechtotext/v3.0/Transcriptions/";

        private static readonly TimeSpan PostTimeout = TimeSpan.FromMinutes(1);

        private static readonly TimeSpan DefaultTimeout = TimeSpan.FromMinutes(2);

        private static readonly TimeSpan GetFilesTimeout = TimeSpan.FromMinutes(5);

        private static readonly HttpClient HttpClient = new () { Timeout = Timeout.InfiniteTimeSpan };

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
            using var requestMessage = new HttpRequestMessage(HttpMethod.Post, path);
            requestMessage.Headers.Add("Ocp-Apim-Subscription-Key", subscriptionKey);
            requestMessage.Content = new StringContent(payloadString, Encoding.UTF8, "application/json");

            var responseMessage = await SendHttpRequestMessage(requestMessage, timeout).ConfigureAwait(false);

            return responseMessage.Headers.Location;
        }

        private static async Task DeleteAsync(string path, string subscriptionKey, TimeSpan timeout)
        {
            using var requestMessage = new HttpRequestMessage(HttpMethod.Delete, path);
            if (!string.IsNullOrEmpty(subscriptionKey))
            {
                requestMessage.Headers.Add("Ocp-Apim-Subscription-Key", subscriptionKey);
            }

            await SendHttpRequestMessage(requestMessage, timeout).ConfigureAwait(false);
        }

        private static async Task<TResponse> GetAsync<TResponse>(string path, string subscriptionKey, TimeSpan timeout)
        {
            using var requestMessage = new HttpRequestMessage(HttpMethod.Get, path);
            if (!string.IsNullOrEmpty(subscriptionKey))
            {
                requestMessage.Headers.Add("Ocp-Apim-Subscription-Key", subscriptionKey);
            }

            var responseMessage = await SendHttpRequestMessage(requestMessage, timeout).ConfigureAwait(false);

            var contentString = await responseMessage.Content.ReadAsStringAsync().ConfigureAwait(false);
            return JsonConvert.DeserializeObject<TResponse>(contentString);
        }

        private static async Task<HttpResponseMessage> SendHttpRequestMessage(HttpRequestMessage httpRequestMessage, TimeSpan timeout)
        {
            try
            {
                using var cts = new CancellationTokenSource();
                cts.CancelAfter(timeout);
                var responseMessage = await HttpClient.SendAsync(httpRequestMessage, cts.Token).ConfigureAwait(false);

                await responseMessage.EnsureSuccessStatusCodeAsync().ConfigureAwait(false);

                return responseMessage;
            }
            catch (HttpRequestException requestException)
                when (requestException.InnerException != null &&
                requestException.InnerException is SocketException socketException &&
                socketException.SocketErrorCode == SocketError.TimedOut)
            {
                throw new TimeoutException($"Timeout in httpRequestException with socketException: {requestException}.");
            }
            catch (OperationCanceledException)
            {
                throw new TimeoutException($"The operation has timed out after {timeout.TotalSeconds} seconds.");
            }
        }
    }
}

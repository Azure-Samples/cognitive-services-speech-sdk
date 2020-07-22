// <copyright file="BatchClient.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Text;
    using System.Threading.Tasks;
    using Connector.Serializable.TranscriptionFiles;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public class BatchClient
    {
        private const string SpeechToTextBasePath = "speechtotext/v3.0/";

        private static ILogger Log;

        private readonly string SubscriptionKey;

        private readonly string HostName;

        public BatchClient(string subscriptionKey, string hostName, ILogger log)
        {
            SubscriptionKey = subscriptionKey;
            HostName = hostName;
            Log = log;
        }

        public static bool IsThrottledOrTimeoutStatusCode(HttpStatusCode statusCode)
        {
            if (statusCode == HttpStatusCode.TooManyRequests ||
                statusCode == HttpStatusCode.GatewayTimeout ||
                statusCode == HttpStatusCode.RequestTimeout ||
                statusCode == HttpStatusCode.BadGateway ||
                statusCode == HttpStatusCode.NotFound)
            {
                return true;
            }

            return false;
        }

        public Task<IEnumerable<Transcription>> GetTranscriptionsAsync()
        {
            var path = $"{SpeechToTextBasePath}Transcriptions";
            return this.GetAsync<IEnumerable<Transcription>>(path);
        }

        public Task<Transcription> GetTranscriptionAsync(Guid id)
        {
            var path = $"{SpeechToTextBasePath}Transcriptions/{id}";
            return this.GetAsync<Transcription>(path);
        }

        public Task<TranscriptionFiles> GetTranscriptionFilesAsync(Guid id)
        {
            var path = $"{SpeechToTextBasePath}Transcriptions/{id}/files";
            return this.GetAsync<TranscriptionFiles>(path);
        }

        public Task<Uri> PostTranscriptionAsync(string name, string description, string locale, Dictionary<string, string> properties, Uri recordingsUrl, IEnumerable<Guid> modelIds)
        {
            var models = modelIds.Select(m => ModelIdentity.Create(m)).ToList();
            var path = $"{SpeechToTextBasePath}Transcriptions/";

            var transcriptionDefinition = TranscriptionDefinition.Create(name, description, locale, recordingsUrl, properties, models);
            return this.PostAsJsonAsync(path, transcriptionDefinition);
        }

        public Task DeleteTranscriptionWithRetryAsync(Guid id)
        {
            var path = $"{SpeechToTextBasePath}Transcriptions/{id}";
            return this.DeleteAsync(path);
        }

        private static Uri GetLocationFromPostResponseAsync(WebHeaderCollection headers)
        {
            return new Uri(headers["Location"]);
        }

        private static async Task<HttpWebResponse> GetHttpWebResponseAsync(HttpWebRequest request)
        {
            var response = (HttpWebResponse)(await request.GetResponseAsync().ConfigureAwait(false));

            if (IsThrottledOrTimeoutStatusCode(response.StatusCode))
            {
                throw new TimeoutException();
            }

            if (response.StatusCode != HttpStatusCode.Accepted && response.StatusCode != HttpStatusCode.Created)
            {
                Log.LogInformation($"Failure: Status Code {response.StatusCode}, {response.StatusDescription}");
                throw new NotImplementedException();
            }

            return response;
        }

        private HttpClient CreateHttpClient()
        {
            var client = new HttpClient();
            client.Timeout = TimeSpan.FromMinutes(25);
            client.BaseAddress = new Uri(HostName);
            client.DefaultRequestHeaders.TransferEncodingChunked = null;
            client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", SubscriptionKey);
            return client;
        }

        private async Task<Uri> PostAsJsonAsync(string path, TranscriptionDefinition payload)
        {
            var request = BuildWebRequest(path, payload);
            var webResponse = await GetHttpWebResponseAsync(request).ConfigureAwait(false);
            Log.LogInformation("StatusCode: " + webResponse.StatusCode);
            return GetLocationFromPostResponseAsync(webResponse.Headers);
        }

        private HttpWebRequest BuildWebRequest(string path, TranscriptionDefinition payload)
        {
            var request = (HttpWebRequest)WebRequest.Create(new Uri(HostName + path));

            request.ContentType = "application/json; charset=UTF-8";
            request.Accept = "application/json";
            request.Method = "POST";
            request.Headers.Add("Ocp-Apim-Subscription-Key", SubscriptionKey);

            var payloadString = JsonConvert.SerializeObject(payload);
            Log.LogInformation($"Request body: {payloadString}");
            var data = Encoding.ASCII.GetBytes(payloadString);
            request.ContentLength = data.Length;

            using (var stream = request.GetRequestStream())
            {
                stream.Write(data, 0, data.Length);
            }

            return request;
        }

        private async Task DeleteAsync(string path)
        {
            Log.LogInformation($"Creating DELETE request for {HostName + path}");

            using (var httpClient = CreateHttpClient())
            {
                var response = await httpClient.DeleteAsync(new Uri(HostName + path)).ConfigureAwait(false);

                if (IsThrottledOrTimeoutStatusCode(response.StatusCode))
                {
                    throw new TimeoutException();
                }

                if (!response.IsSuccessStatusCode)
                {
                    Log.LogInformation("Failure: Status Code: " + response.StatusCode);
                    Log.LogInformation("Failure: Response Header: " + response.Content.Headers);
                    throw new NotImplementedException();
                }
            }
        }

        private async Task<TResponse> GetAsync<TResponse>(string path)
        {
            Log.LogInformation($"Creating GET request for {HostName + path}");

            using (var httpClient = CreateHttpClient())
            {
                var response = await httpClient.GetAsync(new Uri(HostName + path)).ConfigureAwait(false);

                if (IsThrottledOrTimeoutStatusCode(response.StatusCode))
                {
                    throw new TimeoutException();
                }

                var contentType = response.Content.Headers.ContentType;
                if (response.IsSuccessStatusCode && string.Equals(contentType.MediaType, "application/json", StringComparison.OrdinalIgnoreCase))
                {
                    var result = await response.Content.ReadAsJsonAsync<TResponse>().ConfigureAwait(false);
                    return result;
                }

                Log.LogInformation("Failure: Status Code: " + response.StatusCode);
                Log.LogInformation("Failure: Response Header: " + response.Content.Headers);
                throw new NotImplementedException();
            }
        }
    }
}

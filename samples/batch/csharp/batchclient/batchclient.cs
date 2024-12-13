//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Net;
    using System.Net.Http;
    using System.Net.Http.Formatting;
    using System.Threading.Tasks;
    using Newtonsoft.Json;
    using Polly;
    using Polly.Retry;

    public partial class BatchClient : IDisposable
    {
        private const int MaxNumberOfRetries = 5;

        private readonly HttpClient client;
        private readonly string speechToTextBasePath;
        private readonly string apiVersion;

        private static AsyncRetryPolicy<HttpResponseMessage> transientFailureRetryingPolicy = Policy
            .Handle<HttpRequestException>()
            .Or<TaskCanceledException>()
            .OrResult<HttpResponseMessage>(x => !x.IsSuccessStatusCode && (int)x.StatusCode == 429)
            .WaitAndRetryAsync(MaxNumberOfRetries, retryAttempt => TimeSpan.FromSeconds(Math.Pow(2, retryAttempt)), (result, timeSpan, retryCount, context) =>
            {
                Console.WriteLine($"Request failed with {result.Exception?.ToString() ?? result.Result?.StatusCode.ToString()}. Waiting {timeSpan} before next retry. Retry attempt {retryCount}");
            });

        private BatchClient(HttpClient client, string apiVersion)
        {
            this.client = client;
            this.speechToTextBasePath = "speechtotext/";
            this.apiVersion = apiVersion;
        }

        public static BatchClient CreateApiClient(string key, string hostName, string apiVersion)
        {
            var client = new HttpClient();
            client.Timeout = TimeSpan.FromMinutes(25);
            client.BaseAddress = new UriBuilder(Uri.UriSchemeHttps, hostName).Uri;

            client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", key);

            return new BatchClient(client, apiVersion);
        }

        private async Task<TResponse> PostAsJsonAsync<TPayload, TResponse>(string path, TPayload payload)
        {
            string json = JsonConvert.SerializeObject(payload, SpeechJsonContractResolver.WriterSettings);
            StringContent content = new StringContent(json);
            content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;

            var response = await transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.PostAsync(path, content))
                .ConfigureAwait(false);

            if (response.IsSuccessStatusCode)
            {
                return await response.Content.ReadAsAsync<TResponse>(
                    new[]
                    {
                        new JsonMediaTypeFormatter
                        {
                            SerializerSettings = SpeechJsonContractResolver.ReaderSettings
                        }
                    }).ConfigureAwait(false);
            }

            throw await CreateExceptionAsync(response).ConfigureAwait(false);
        }

        private async Task<TResponse> GetAsync<TResponse>(string path)
        {
            var response = await transientFailureRetryingPolicy
                .ExecuteAsync(async () => await this.client.GetAsync(path).ConfigureAwait(false))
                .ConfigureAwait(false);

            if (response.IsSuccessStatusCode)
            {
                var result = await response.Content.ReadAsAsync<TResponse>().ConfigureAwait(false);

                return result;
            }

            throw await CreateExceptionAsync(response);
        }

        private static async Task<FailedHttpClientRequestException> CreateExceptionAsync(HttpResponseMessage response)
        {
            switch (response.StatusCode)
            {
                case HttpStatusCode.Forbidden:
                    return new FailedHttpClientRequestException(response.StatusCode, "No permission to access this resource.");
                case HttpStatusCode.Unauthorized:
                    return new FailedHttpClientRequestException(response.StatusCode, "Not authorized to see the resource.");
                case HttpStatusCode.NotFound:
                    return new FailedHttpClientRequestException(response.StatusCode, "The resource could not be found.");
                case HttpStatusCode.UnsupportedMediaType:
                    return new FailedHttpClientRequestException(response.StatusCode, "The file type isn't supported.");
                case HttpStatusCode.BadRequest:
                    {
                        var content = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                        var shape = new { Message = string.Empty };
                        var result = JsonConvert.DeserializeAnonymousType(content, shape);
                        if (result != null && !string.IsNullOrEmpty(result.Message))
                        {
                            return new FailedHttpClientRequestException(response.StatusCode, result.Message);
                        }

                        return new FailedHttpClientRequestException(response.StatusCode, response.ReasonPhrase);
                    }

                default:
                    return new FailedHttpClientRequestException(response.StatusCode, response.ReasonPhrase);
            }
        }

        public void Dispose()
        {
            this.client?.Dispose();
        }
    }
}

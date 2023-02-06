//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace CallCenter
{
    public static class RestHelper
    {
        public static async Task<(HttpResponseMessage response, string content)> SendGet(string uri, string key, HttpStatusCode[] expectedStatusCodes)
        {
            using (var client = new HttpClient())
            using (var request = new HttpRequestMessage())
            {
                request.Method = HttpMethod.Get;
                request.RequestUri = new Uri(uri);
                request.Headers.Add("Ocp-Apim-Subscription-Key", key);

                var response_1 = await client.SendAsync(request);
                if (response_1.Content == null)
                {
                    throw new Exception($"The response from {uri} contains no content.");
                }
                var response_2 = await response_1.Content.ReadAsStringAsync();
                if (!expectedStatusCodes.Contains(response_1.StatusCode))
                {
                    throw new Exception($"The response from {uri} has an unexpected status code: {response_1.StatusCode}");
                }
                
                return (response_1, response_2);
            }
        }

        public static async Task<(HttpResponseMessage response, string content)> SendPost(string uri, string requestBody, string key, HttpStatusCode[] expectedStatusCodes)
        {
            using (var client = new HttpClient())
            using (var request = new HttpRequestMessage())
            {
                request.Method = HttpMethod.Post;
                request.RequestUri = new Uri(uri);
                request.Headers.Add("Ocp-Apim-Subscription-Key", key);

                if (!String.IsNullOrEmpty(requestBody))
                {
                    request.Content = new StringContent(requestBody, Encoding.UTF8, "application/json");
                }
                var response_1 = await client.SendAsync(request);
                if (response_1.Content == null)
                {
                    throw new Exception($"The response from {uri} contains no content.");
                }
                var response_2 = await response_1.Content.ReadAsStringAsync();
                if (!expectedStatusCodes.Contains(response_1.StatusCode))
                {
                    throw new Exception($"The response from {uri} has an unexpected status code: {response_1.StatusCode}. Response:{Environment.NewLine}{response_2}");
                }
                
                return (response_1, response_2);
            }
        }

        public static async Task SendDelete(string uri, string key)
        {
            using (var client = new HttpClient())
            using (var request = new HttpRequestMessage())
            {
                request.Method = HttpMethod.Delete;
                request.RequestUri = new Uri(uri);
                request.Headers.Add("Ocp-Apim-Subscription-Key", key);

                var response = await client.SendAsync(request);
                if (HttpStatusCode.NoContent != response.StatusCode)
                {
                    throw new Exception($"HTTP DELETE request returned unexpected status code: {response.StatusCode}. Full response:{Environment.NewLine}{response.ToString()}");
                }
                return;
            }
        }
    }
}
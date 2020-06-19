//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    /// <summary>
    /// Azure subscription helper methods
    /// </summary>
    public class AzureSubscription
    {
        private static HttpClient _client = new HttpClient();

        /// <summary>
        /// Generates an Azure Authorization token from the subscription key and region
        /// </summary>
        /// <param name="subscriptionKey">The Azure subscription key</param>
        /// <param name="region">The Azure region (e.g. westus)</param>
        /// <returns>Asynchronous task that returns the authorization token</returns>


        /// <summary>
        /// Generates an Azure Authorization token from the subscription key and region
        /// </summary>
        /// <param name="subscriptionKey">The Azure subscription key</param>
        /// <param name="region">The Azure region (e.g. westus)</param>
        /// <param name="token">(Optional) The cancellation token to use</param>
        /// <returns>Asynchronous task that returns the authorization token</returns>
        public static Task<string> GenerateAuthorizationTokenAsync(string subscriptionKey, string region, CancellationToken token = default(CancellationToken))
            => GenerateAuthorizationTokenAsync(subscriptionKey, region, TimeSpan.FromMinutes(10), token);

        /// <summary>
        /// Generates an Azure Authorization token from the subscription key and region
        /// </summary>
        /// <param name="subscriptionKey">The Azure subscription key</param>
        /// <param name="region">The Azure region (e.g. westus)</param>
        /// <param name="authTokenValidity">How long the token is valid for</param>
        /// <param name="token">(Optional) The cancellation token to use</param>
        /// <returns>Asynchronous task that returns the authorization token</returns>
        public static async Task<string> GenerateAuthorizationTokenAsync(string subscriptionKey, string region, TimeSpan authTokenValidity, CancellationToken token = default(CancellationToken))
        {
            if (string.IsNullOrWhiteSpace(subscriptionKey))
            {
                throw new ArgumentException(nameof(subscriptionKey) + " cannot be null, empty or consist only of white space");
            }
            else if (string.IsNullOrWhiteSpace(region))
            {
                throw new ArgumentException(nameof(region) + " cannot be null, empty or consist only of white space");
            }
            else if (authTokenValidity <= TimeSpan.Zero)
            {
                throw new ArgumentOutOfRangeException(nameof(authTokenValidity), "The authorization token validity must be greater than 0");
            }

            string urlString = string.Format(
                CultureInfo.InvariantCulture,
                "https://{0}.api.cognitive.microsoft.com/sts/v1.0/issueToken?expiredTime={1}",
                Uri.EscapeDataString(region.Trim()),
                (int)Math.Round(authTokenValidity.TotalSeconds));

            using (var request = new HttpRequestMessage(HttpMethod.Post, urlString))
            {
                request.Headers.Add("Ocp-Apim-Subscription-Key", subscriptionKey.Trim());

                using (var response = await _client.SendAsync(request, token))
                {
                    response.EnsureSuccessStatusCode();

                    return await response.Content.ReadAsStringAsync();
                }
            }
        }
    }
}

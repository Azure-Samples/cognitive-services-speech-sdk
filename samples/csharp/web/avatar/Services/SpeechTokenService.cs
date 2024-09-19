//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Avatar.Models;
using Azure.Core;
using Azure.Identity;
using Microsoft.Extensions.Options;

namespace Avatar.Services
{
    public class SpeechTokenService
    {
        private readonly HttpClient _httpClient;

        private readonly ClientSettings _clientSettings;

        public SpeechTokenService(HttpClient httpClient, IOptions<ClientSettings> clientSettings)
        {
            _httpClient = httpClient;
            _clientSettings = clientSettings.Value;
        }

        public async Task RefreshSpeechTokenAsync(CancellationToken stoppingToken)
        {
            while (!stoppingToken.IsCancellationRequested)
            {
                if (!string.IsNullOrEmpty(_clientSettings.SpeechPrivateEndpoint))
                {
                    var credential = new DefaultAzureCredential(new DefaultAzureCredentialOptions
                    {
                        ManagedIdentityClientId = _clientSettings.UserAssignedManagedIdentityClientId
                    });

                    var token = await credential.GetTokenAsync(new TokenRequestContext(new[] { "https://cognitiveservices.azure.com/.default" }));
                    GlobalVariables.SpeechToken = $"aad#{_clientSettings.SpeechResourceUrl}#{token.Token}";
                    Console.WriteLine("Token refreshed using managed identity.");
                }
                else
                {
                    var url = $"https://{_clientSettings.SpeechRegion}.api.cognitive.microsoft.com/sts/v1.0/issueToken";
                    var request = new HttpRequestMessage(HttpMethod.Post, url);
                    request.Headers.Add("Ocp-Apim-Subscription-Key", _clientSettings.SpeechKey);

                    var response = await _httpClient.SendAsync(request);
                    response.EnsureSuccessStatusCode();

                    GlobalVariables.SpeechToken = await response.Content.ReadAsStringAsync();
                    Console.WriteLine("Token refreshed using API key.");
                }

                Console.WriteLine($"Token generated: {GlobalVariables.SpeechToken}");
                await Task.Delay(TimeSpan.FromMinutes(9), stoppingToken); // Refresh every 9 minutes
            }
        }
    }
}

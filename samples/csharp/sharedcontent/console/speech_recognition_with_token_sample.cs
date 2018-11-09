//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace MicrosoftSpeechSDKSamples
{
    // For more information about Authorization Token please refer to:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-apis#authentication
    public class SpeechRecognitionWithTokenSample
    {
        // Specify your subscription key and service region (e.g., "westus").
        private const string subscriptionKey = "YourSubscriptionKey";
        private const string region = "YourServiceRegion";

        private static SpeechConfig config;
        private static string authorizationToken;
        
        // Authorization token expires every 10 minutes. Renew it every 9 minutes.
        private static TimeSpan RefreshTokenDuration = TimeSpan.FromMinutes(9);

        // Continuous Recognition with Microphone using Authorization Token
        public static async Task ContinuousRecognitionWithAuthorizationTokenAsync()
        {
            // Gets a fresh authorization token from 
            // specified subscription key and service region (e.g., "westus").
            authorizationToken = await GetToken(subscriptionKey, region);

            // Creates an instance of a speech config with 
            // acquired authorization token and service region (e.g., "westus").
            // The default language is "en-us".
            config = SpeechConfig.FromAuthorizationToken(authorizationToken, region);

            // Define the cancellation token in order to stop the periodic renewal 
            // of authorization token after completing recognition.
            CancellationTokenSource source = new CancellationTokenSource();

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = new SpeechRecognizer(config))
            {

                // Run task for token renewal in the background.
                var tokenRenewTask = StartTokenRenewTask(source.Token, recognizer); 

                // Subscribe to events.
                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason}");

                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                Console.WriteLine("Press any key to stop");
                Console.ReadKey();

                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                // Cancel cancellationToken to stop the token renewal task.
                source.Cancel();
            }
        }

        // Renews authorization token periodically until cancellationToken is cancelled.
        public static Task StartTokenRenewTask(CancellationToken cancellationToken, SpeechRecognizer recognizer)
        {
            return Task.Run(async () =>
            {
                while (!cancellationToken.IsCancellationRequested)
                {
                    await Task.Delay(RefreshTokenDuration, cancellationToken);

                    if (!cancellationToken.IsCancellationRequested)
                    {
                        recognizer.AuthorizationToken = await GetToken(subscriptionKey, region);
                    }
                }
            });
        }

        // Gets an authorization token by sending a POST request to the token service.
        public static async Task<string> GetToken(string subscriptionKey, string region)
        {
            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", subscriptionKey);
                UriBuilder uriBuilder = new UriBuilder("https://" + region + ".api.cognitive.microsoft.com/sts/v1.0/issueToken");

                using (var result = await client.PostAsync(uriBuilder.Uri.AbsoluteUri, null))
                {
                    Console.WriteLine("Token Uri: {0}", uriBuilder.Uri.AbsoluteUri);
                    if (result.IsSuccessStatusCode)
                    {
                        return await result.Content.ReadAsStringAsync();
                    }
                    else
                    {
                        throw new HttpRequestException($"Cannot get token from {uriBuilder.ToString()}. Error: {result.StatusCode}");
                    }
                }
            }
        }
    }
}

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

        private static SpeechFactory factory;
        private static string authorizationToken;
        
        // Authorization token expires every 10 minutes. Renew it every 9 minutes.
        private static TimeSpan RefreshTokenDuration = TimeSpan.FromMinutes(9);

        // Continuous Recognition with Microphone using Authorization Token
        public static async Task ContinuousRecognitionWithAuthorizationTokenAsync()
        {
            // Gets a fresh authorization token from 
            // specified subscription key and service region (e.g., "westus").
            authorizationToken = await GetToken(subscriptionKey, region);

            // Creates an instance of a speech factory with 
            // acquired authorization token and service region (e.g., "westus").
            factory = SpeechFactory.FromAuthorizationToken(authorizationToken, region);

            // Define the cancellation token in order to stop the periodic renewal 
            // of authorization token after completing recognition.
            CancellationTokenSource source = new CancellationTokenSource();

            // Run task for token renewal in the background.
            var tokenRenewTask = StartTokenRenewTask(source.Token); 

            // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Subscribe to events.
                recognizer.FinalResultReceived += (s, e) => {
                    var result = e.Result;
                    if (result.RecognitionStatus == RecognitionStatus.Recognized)
                    {
                        Console.WriteLine($"\n    Final result: Status: {result.RecognitionStatus.ToString()}, Text: {result.Text}.");
                    }
                    else
                    {
                        Console.WriteLine($"Recognition status: {result.RecognitionStatus.ToString()}");
                        if (result.RecognitionStatus == RecognitionStatus.Canceled)
                        {
                            Console.WriteLine($"There was an error, reason: {result.RecognitionFailureReason}");
                        }
                        else
                        {
                            Console.WriteLine("No speech could be recognized.\n");
                        }
                    }
                };

                recognizer.RecognitionErrorRaised += (s, e) => {
                    Console.WriteLine($"\n    An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}");
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
        public static Task StartTokenRenewTask(CancellationToken cancellationToken)
        {
            return Task.Run(async () =>
            {
                while (!cancellationToken.IsCancellationRequested)
                {
                    await Task.Delay(RefreshTokenDuration, cancellationToken);

                    if (!cancellationToken.IsCancellationRequested)
                    {
                        factory.AuthorizationToken = await GetToken(subscriptionKey, region);
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

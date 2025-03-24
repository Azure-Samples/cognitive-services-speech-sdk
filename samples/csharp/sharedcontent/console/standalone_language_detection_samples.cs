//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Threading.Tasks;
using Azure.Core;
using Azure.Identity;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class StandaloneLanguageDetectionSamples
    {
        /// <summary>
        /// The automatic detect source language configuration
        /// </summary>
        private static AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(new string[] { "en-US", "zh-CN" });

        // Language detection from microphone.
        public static async Task LanguageDetectionWithMicrophoneAsync()
        {
            // <languageDetectionWithMicrophone>
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a source language recognizer using microphone as audio input.
            using (var recognizer = new SourceLanguageRecognizer(config, autoDetectSourceLanguageConfig))
            {
                // Starts recognizing.
                Console.WriteLine("Say something in English or Mandarin Chinese...");

                // Starts language detection, and returns after a single utterance is recognized.
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                // shot detection like command or query.
                // For long-running multi-utterance detection, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    var lidResult = AutoDetectSourceLanguageResult.FromResult(result);
                    Console.WriteLine($"DETECTED: Language={lidResult.Language}");
                }
                else if (result.Reason == ResultReason.NoMatch)
                {
                    Console.WriteLine($"NOMATCH: The spoken language could not be detected.");
                }
                else if (result.Reason == ResultReason.Canceled)
                {
                    var cancellation = CancellationDetails.FromResult(result);
                    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                    if (cancellation.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                }
            }
            // </languageDetectionWithMicrophone>
        }

        // Language Detection with file async
        public static async Task LanguageDetectionWithFileAsync()
        {
            // <languageDetectionInAccuracyWithFile>
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"LanguageDetection_enUS.wav"))
            {
                using (var recognizer = new SourceLanguageRecognizer(config, autoDetectSourceLanguageConfig, audioInput))
                {
                    // Starts language detection, and returns after a single utterance is recognized.
                    // The task returns the recognition text as result.
                    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                    // shot detection like command or query.
                    // For long-running multi-utterance detection, use StartContinuousRecognitionAsync() instead.
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                    // Checks result.
                    if (result.Reason == ResultReason.RecognizedSpeech)
                    {
                        var lidResult = AutoDetectSourceLanguageResult.FromResult(result);
                        Console.WriteLine($"DETECTED: Language={lidResult.Language}");
                    }
                    else if (result.Reason == ResultReason.NoMatch)
                    {

                        Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                    }
                    else if (result.Reason == ResultReason.Canceled)
                    {
                        var cancellation = CancellationDetails.FromResult(result);
                        Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                        if (cancellation.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }
                    }
                }
            }
            // </languageDetectionInAccuracyWithFile>
        }

        // Continuous language detection
        public static async Task ContinuousLanguageDetectionWithFileAsync()
        {
            // <languageDetectionContinuousWithFile>
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
            // Please refer to the documentation of Language ID for more information.
            // http://aka.ms/speech/lid?pivots=programming-language-csharp
            config.SetProperty(PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous");

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Creates a speech recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"en-us_zh-cn.wav"))
            {
                using (var recognizer = new SourceLanguageRecognizer(config, autoDetectSourceLanguageConfig, audioInput))
                {
                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            var lidResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                            Console.WriteLine($"DETECTED: Language={lidResult.Language}");
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

                        stopRecognition.TrySetResult(0);
                    };

                    recognizer.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine("\n    Session started event.");
                    };

                    recognizer.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine("\n    Session stopped event.");
                        Console.WriteLine("\nStop recognition.");
                        stopRecognition.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopRecognition.Task });

                    // Stops recognition.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            // </languageDetectionContinuousWithFile>
        }

        // Language detection from microphone authenticated via AAD token credential.
        public static async Task SingleDetectionWithAADTokenCredentailAsync()
        {
            // Create a token credential using DefaultAzureCredential.
            // This credential supports multiple authentication methods, including Managed Identity, environment variables, and Azure CLI login.
            // For more types of token credentials, refer to: 
            // https://learn.microsoft.com/dotnet/api/azure.identity.defaultazurecredential?view=azure-dotnet
            TokenCredential credential = new DefaultAzureCredential(new DefaultAzureCredentialOptions()
            {
                ManagedIdentityClientId = "{Your App Id}", // Specify the Managed Identity Client ID if using a user-assigned managed identity.
            });

            // Define the v2 endpoint for Azure Speech Service.
            // This is required when using a private endpoint with a custom domain.
            // For details on setting up a custom domain with private links, see: 
            // https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name
            var v2Endpoint = string.Format("wss://{custom domain}/stt/speech/universal/v2");

            // Create a SpeechConfig instance using the v2 endpoint and the token credential for authentication.
            var config = SpeechConfig.FromEndpoint(new Uri(v2Endpoint), credential);

            // Creates a source language recognizer using microphone as audio input.
            using (var recognizer = new SourceLanguageRecognizer(config, autoDetectSourceLanguageConfig))
            {
                // Starts recognizing.
                Console.WriteLine("Say something in English or Mandarin Chinese...");

                // Starts language detection, and returns after a single utterance is recognized.
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                // shot detection like command or query.
                // For long-running multi-utterance detection, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    var lidResult = AutoDetectSourceLanguageResult.FromResult(result);
                    Console.WriteLine($"DETECTED: Language={lidResult.Language}");
                }
                else if (result.Reason == ResultReason.NoMatch)
                {
                    Console.WriteLine($"NOMATCH: The spoken language could not be detected.");
                }
                else if (result.Reason == ResultReason.Canceled)
                {
                    var cancellation = CancellationDetails.FromResult(result);
                    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                    if (cancellation.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                }
            }
        }

        // Continuous language detection authenticated via AAD token credential
        public static async Task ContinuousLanguageDetectionWithAADTokenCredentialAsync()
        {
            // Create a token credential using DefaultAzureCredential.
            // This credential supports multiple authentication methods, including Managed Identity, environment variables, and Azure CLI login.
            // For more types of token credentials, refer to: 
            // https://learn.microsoft.com/dotnet/api/azure.identity.defaultazurecredential?view=azure-dotnet
            TokenCredential credential = new DefaultAzureCredential(new DefaultAzureCredentialOptions()
            {
                ManagedIdentityClientId = "{Your App Id}", // Specify the Managed Identity Client ID if using a user-assigned managed identity.
            });

            // Define the v2 endpoint for Azure Speech Service.
            // This is required when using a private endpoint with a custom domain.
            // For details on setting up a custom domain with private links, see: 
            // https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name
            var v2Endpoint = string.Format("wss://{custom domain}/stt/speech/universal/v2");

            // Create a SpeechConfig instance using the v2 endpoint and the token credential for authentication.
            var config = SpeechConfig.FromEndpoint(new Uri(v2Endpoint), credential);

            // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
            // Please refer to the documentation of Language ID for more information.
            // http://aka.ms/speech/lid?pivots=programming-language-csharp
            config.SetProperty(PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous");

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Creates a speech recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"en-us_zh-cn.wav"))
            {
                using (var recognizer = new SourceLanguageRecognizer(config, autoDetectSourceLanguageConfig, audioInput))
                {
                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            var lidResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                            Console.WriteLine($"DETECTED: Language={lidResult.Language}");
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

                        stopRecognition.TrySetResult(0);
                    };

                    recognizer.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine("\n    Session started event.");
                    };

                    recognizer.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine("\n    Session stopped event.");
                        Console.WriteLine("\nStop recognition.");
                        stopRecognition.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopRecognition.Task });

                    // Stops recognition.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
        }
    }
}

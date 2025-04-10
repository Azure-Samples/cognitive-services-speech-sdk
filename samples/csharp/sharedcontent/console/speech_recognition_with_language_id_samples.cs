//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionWithLanguageIdSamples
    {
        /// <summary>
        /// The automatic detect source language configuration
        /// </summary>
        private static AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(new string[] { "en-US" , "de-DE" });

        // Speech recognition from microphone.
        public static async Task RecognitionWithMicrophoneAsync()
        {
            // <recognitionWithMicrophone>
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = new SpeechRecognizer(config, autoDetectSourceLanguageConfig))
            {
                // Starts recognizing.
                Console.WriteLine("Say something in English or German...");

                // Starts speech recognition, and returns after a single utterance is recognized. The end of a
                // single utterance is determined by listening for silence at the end or until a maximum of about 30
                // seconds of audio is processed.  The task returns the recognition text and the detected language as result.
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                // shot recognition like command or query.
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    var lidResult = AutoDetectSourceLanguageResult.FromResult(result);
                    Console.WriteLine($"RECOGNIZED: Text={result.Text} with language={lidResult.Language}");
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
            // </recognitionWithMicrophone>
        }

        /// <summary>
        /// Speech recognition with auto detection for source language with universal v2 endpoint
        /// We only support multi-lingual continuous recognition in universal v2 endpoint
        /// </summary>
        public static async Task MultiLingualRecognitionWithUniversalV2Endpoint()
        {
            // Creates an instance of a speech config with specified subscription key and service region.
            // Note: For multi-lingual speech recognition with language id, it only works with speech v2 endpoint,
            // you must use SpeechConfig.FromEndpoint() in order to use the speech v2 endpoint.

            /// Replace YourServiceRegion with your region, for example "westus",
            var v2EndpointInString = String.Format("wss://{0}.stt.speech.microsoft.com/speech/universal/v2", "YourServiceRegion");
            var v2EndpointUrl = new Uri(v2EndpointInString);

            // Creates an instance of a speech config with specified subscription key.
            // Replace YourSubscriptionKey with your subscription key
            var config = SpeechConfig.FromEndpoint(v2EndpointUrl, "YourSubscriptionKey");

            // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
            // Please refer to the documentation of Language ID for more information.
            // https://aka.ms/speech/lid?pivots=programming-language-csharp
            config.SetProperty(PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous");

            // Define the set of input spoken languages to detect
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(new string[] { "en-US", "zh-CN" });

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Creates a speech recognizer using the auto detect source language config, and the file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"en-us_zh-cn.wav"))
            {
                using (var recognizer = new SpeechRecognizer(config, autoDetectSourceLanguageConfig, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizingSpeech)
                        {
                            Console.WriteLine($"RECOGNIZING: Text={e.Result.Text}");
                            // Retrieve the detected language
                            var autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                            Console.WriteLine($"DETECTED: Language={autoDetectSourceLanguageResult.Language}");
                        }
                    };

                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            // Retrieve the detected language
                            var autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                            Console.WriteLine($"DETECTED: Language={autoDetectSourceLanguageResult.Language}");
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

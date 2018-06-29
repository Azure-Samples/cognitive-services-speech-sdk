//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionSamples
    {
        // Speech recognition from microphone.
        public static async Task RecognitionWithMicrophoneAsync()
        {
            // <recognitionWithMicrophone>
            // Creates an instance of a speech factory with specified
            // subscription key and service region. Replace with your own subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Starts recognizing.
                Console.WriteLine("Say something...");

                // Starts recognition. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
                {
                    Console.WriteLine($"There was an error. Status:{result.RecognitionStatus.ToString()}, Reason:{result.RecognitionFailureReason}");
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}");
                }
            }
            // </recognitionWithMicrophone>
        }

        // Speech recognition in the specified spoken language.
        public static async Task RecognitionWithLanguageAsync()
        {
            // <recognitionWithLanguage>
            // Creates an instance of a speech factory with specified
            // subscription key and service region. Replace with your own subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer for the specified language, using microphone as audio input.
            var lang = "de-de";
            using (var recognizer = factory.CreateSpeechRecognizer(lang))
            {
                // Starts recognizing.
                Console.WriteLine($"Say something in {lang} ...");

                // Starts recognition. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
                {
                    Console.WriteLine($"There was an error. Status:{result.RecognitionStatus.ToString()}, Reason:{result.RecognitionFailureReason}");
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}");
                }
            }
            // </recognitionWithLanguage>
        }

        // Speech recognition from file.
        public static async Task RecognitionWithFileAsync()
        {
            // <recognitionFromFile>
            // Creates an instance of a speech factory with specified
            // subscription key and service region. Replace with your own subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(@"YourAudioFile.wav"))
            {
                // Starts recognition. It returns when the first utterance is recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
                {
                    Console.WriteLine($"There was an error. Status:{result.RecognitionStatus.ToString()}, Reason:{result.RecognitionFailureReason}");
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}");
                }
            }
            // </recognitionFromFile>
        }

        // <recognitionCustomized>
        // Speech recognition using a customized model.
        public static async Task RecognitionUsingCustomizedModelAsync()
        {
            // Creates an instance of a speech factory with specified
            // subscription key and service region. Replace with your own subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Replace with the CRIS deployment id of your customized model.
                recognizer.DeploymentId = "YourDeploymentId";

                Console.WriteLine("Say something...");

                // Starts recognition. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks results.
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
                {
                    Console.WriteLine($"There was an error. Status:{result.RecognitionStatus.ToString()}, Reason:{result.RecognitionFailureReason}");
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}");
                }
            }
        }
        // </recognitionCustomized>

        // <recognitionContinuous>
        // Speech recognition with events
        public static async Task ContinuousRecognitionAsync()
        {
            // Creates an instance of a speech factory with specified
            // subscription key and service region. Replace with your own subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Subscribes to events.
                recognizer.IntermediateResultReceived += (s, e) => {
                    Console.WriteLine($"\n    Partial result: {e.Result.Text}.");
                };

                recognizer.FinalResultReceived += (s, e) => {
                    if (e.Result.RecognitionStatus == RecognitionStatus.Recognized)
                    {
                        Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, Text: {e.Result.Text}.");
                    }
                    else
                    {
                        Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, FailureReason: {e.Result.RecognitionFailureReason}.");
                    }
                };

                recognizer.RecognitionErrorRaised += (s, e) => {
                    Console.WriteLine($"\n    An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}");
                };

                recognizer.OnSessionEvent += (s, e) => {
                    Console.WriteLine($"\n    Session event. Event: {e.EventType.ToString()}.");
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                Console.WriteLine("Press any key to stop");
                Console.ReadKey();

                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
        }
        // </recognitionContinuous>

        // <recognitionAudioStream>
        // The TaskCompletionSource to stop recognition.
        private static TaskCompletionSource<int> stopRecognitionTaskCompletionSource;

        // Speech recognition with audio stream
        public static async Task RecognitionWithAudioStreamAsync()
        {
            stopRecognitionTaskCompletionSource = new TaskCompletionSource<int>();

            // Creates an instance of a speech factory with specified
            // subscription key and service region. Replace with your own subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Create an audio stream from a wav file.
            // Replace with your own audio file name.
            var stream = Helper.OpenWaveFile(@"YourAudioFile.wav");

            // Creates a speech recognizer using audio stream input.
            using (var recognizer = factory.CreateSpeechRecognizerWithStream(stream))
            {
                // Subscribes to events.
                recognizer.IntermediateResultReceived += (s, e) => {
                    Console.WriteLine($"\n    Partial result: {e.Result.Text}.");
                };

                recognizer.FinalResultReceived += (s, e) => {
                    if (e.Result.RecognitionStatus == RecognitionStatus.Recognized)
                    {
                        Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, Text: {e.Result.Text}.");
                    }
                    else
                    {
                        Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, FailureReason: {e.Result.RecognitionFailureReason}.");
                    }
                };

                recognizer.RecognitionErrorRaised += (s, e) => {
                    Console.WriteLine($"\n    An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}");
                    stopRecognitionTaskCompletionSource.TrySetResult(0);
                };

                recognizer.OnSessionEvent += (s, e) =>
                {
                    Console.WriteLine($"\nSession event. Event: {e.EventType.ToString()}.");
                    // Stops translation when session stop is detected.
                    if (e.EventType == SessionEventType.SessionStoppedEvent)
                    {
                        Console.WriteLine($"\nStop translation.");
                        stopRecognitionTaskCompletionSource.TrySetResult(0);
                    }
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                // Waits for completion.
                await stopRecognitionTaskCompletionSource.Task.ConfigureAwait(false);

                // Stops recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
        }
        // </recognitionAudioStream>
    }
}

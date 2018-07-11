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
            // Creates an instance of a speech factory with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Starts recognizing.
                Console.WriteLine("Say something...");

                // Performs recognition.
                // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
                // only for single shot recognition like command or query. For long-running recognition, use
                // StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
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
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}, Offset: {result.OffsetInTicks}, Duration: {result.Duration}.");
                }
            }
            // </recognitionWithMicrophone>
        }

        // Speech recognition in the specified spoken language and uses detailed output format.
        public static async Task RecognitionWithLanguageAndDetailedOutputAsync()
        {
            // <recognitionWithLanguageAndDetailedOutputFormat>
            // Creates an instance of a speech factory with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer for the specified language, using microphone as audio input.
            // Replace the language with your language in BCP-47 format, e.g. en-US.
            var lang = "de-DE";
            // Requests detailed output format.
            using (var recognizer = factory.CreateSpeechRecognizer(lang, OutputFormat.Detailed))
            {
                // Starts recognizing.
                Console.WriteLine($"Say something in {lang} ...");

                // Performs recognition.
                // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
                // only for single shot recognition like command or query. For long-running recognition, use
                // StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
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
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}, Offset: {result.OffsetInTicks}, Duration: {result.Duration}.");
                    Console.WriteLine("Detailed results:");
                    var detailedResults = result.Best();
                    foreach (var item in detailedResults)
                    {
                        Console.WriteLine($"Confidence: {item.Confidence}, Text: {item.Text}, LexicalForm: {item.LexicalForm}, NormalizedForm: {item.NormalizedForm}, MaskedNormalizedForm: {item.MaskedNormalizedForm}");
                    }
                }
            }
            // </recognitionWithLanguageAndDetailedOutputFormat>
        }

        // Speech recognition using a customized model.
        public static async Task RecognitionUsingCustomizedModelAsync()
        {
            // <recognitionCustomized>
            // Creates an instance of a speech factory with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Replace with the CRIS deployment id of your customized model.
                recognizer.DeploymentId = "YourDeploymentId";

                Console.WriteLine("Say something...");

                // Performs recognition.
                // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
                // only for single shot recognition like command or query. For long-running recognition, use
                // StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks results.
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
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
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}, Offset: {result.OffsetInTicks}, Duration: {result.Duration}.");
                }
            }
            // </recognitionCustomized>
        }

        // Continuous speech recognition.
        public static async Task ContinuousRecognitionWithFileAsync()
        {
            // <recognitionContinuousWithFile>
            // Creates an instance of a speech factory with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            var stopRecognition = new TaskCompletionSource<int>();

            // Creates a speech recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(@"whatstheweatherlike.wav"))
            {
                // Subscribes to events.
                recognizer.IntermediateResultReceived += (s, e) => {
                    Console.WriteLine($"\n    Partial result: {e.Result.Text}.");
                };

                recognizer.FinalResultReceived += (s, e) => {
                    var result = e.Result;
                    Console.WriteLine($"Recognition status: {result.RecognitionStatus.ToString()}");
                    switch (result.RecognitionStatus)
                    {
                        case RecognitionStatus.Recognized:
                            Console.WriteLine($"\n    Final result: Text: {result.Text}, Offset: {result.OffsetInTicks}, Duration: {result.Duration}.");
                            break;
                        case RecognitionStatus.InitialSilenceTimeout:
                            Console.WriteLine("The start of the audio stream contains only silence, and the service timed out waiting for speech.\n");
                            break;
                        case RecognitionStatus.InitialBabbleTimeout:
                            Console.WriteLine("The start of the audio stream contains only noise, and the service timed out waiting for speech.\n");
                            break;
                        case RecognitionStatus.NoMatch:
                            Console.WriteLine("The speech was detected in the audio stream, but no words from the target language were matched. Possible reasons could be wrong setting of the target language or wrong format of audio stream.\n");
                            break;
                        case RecognitionStatus.Canceled:
                            Console.WriteLine($"There was an error, reason: {result.RecognitionFailureReason}");
                            break;
                    }
                };

                recognizer.RecognitionErrorRaised += (s, e) => {
                    Console.WriteLine($"\n    An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}");
                    stopRecognition.TrySetResult(0);
                };

                recognizer.OnSessionEvent += (s, e) => {
                    Console.WriteLine($"\n    Session event. Event: {e.EventType.ToString()}.");
                    // Stops translation when session stop is detected.
                    if (e.EventType == SessionEventType.SessionStoppedEvent)
                    {
                        Console.WriteLine($"\nStop recognition.");
                        stopRecognition.TrySetResult(0);
                    }
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                // Waits for completion.
                // Use Task.WaitAny to keep the task rooted.
                Task.WaitAny(new[] { stopRecognition.Task });

                // Stops recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
            // </recognitionContinuousWithFile>
        }

        // Speech recognition with audio stream
        public static async Task RecognitionWithAudioStreamAsync()
        {
            // <recognitionAudioStream>
            // Creates an instance of a speech factory with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            var stopRecognition = new TaskCompletionSource<int>();

            // Create an audio stream from a wav file.
            // Replace with your own audio file name.
            using (var stream = Helper.OpenWaveFile(@"whatstheweatherlike.wav"))
            {
                // Creates a speech recognizer using audio stream input.
                using (var recognizer = factory.CreateSpeechRecognizerWithStream(stream))
                {
                    // Subscribes to events.
                    recognizer.IntermediateResultReceived += (s, e) =>
                    {
                        Console.WriteLine($"\n    Partial result: {e.Result.Text}.");
                    };

                    recognizer.FinalResultReceived += (s, e) =>
                    {
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

                    recognizer.RecognitionErrorRaised += (s, e) =>
                    {
                        Console.WriteLine($"\n    An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}");
                        stopRecognition.TrySetResult(0);
                    };

                    recognizer.OnSessionEvent += (s, e) =>
                    {
                        Console.WriteLine($"\nSession event. Event: {e.EventType.ToString()}.");
                        // Stops translation when session stop is detected.
                        if (e.EventType == SessionEventType.SessionStoppedEvent)
                        {
                            Console.WriteLine($"\nStop recognition.");
                            stopRecognition.TrySetResult(0);
                        }
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
            // </recognitionAudioStream>
        }
    }
}

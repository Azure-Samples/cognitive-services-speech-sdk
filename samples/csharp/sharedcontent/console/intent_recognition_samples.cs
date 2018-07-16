//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Intent;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class IntentRecognitionSamples
    {
        // Intent recognition using microphone.
        public static async Task RecognitionWithMicrophoneAsync()
        {
            // <intentRecognitionWithMicrophone>
            // Creates an instance of a speech factory with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Service Speech SDK, the intent recognition services
            // requires a specific subscription key from https://www.luis.ai/.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

            // Creates an intent recognizer using microphone as audio input.The default language is "en-us".
            using (var recognizer = factory.CreateIntentRecognizer())
            {
                // Creates a Language Understanding model using the app id, and adds specific intents from your model
                var model = LanguageUnderstandingModel.FromAppId("YourLanguageUnderstandingAppId");
                recognizer.AddIntent("id1", model, "YourLanguageUnderstandingIntentName1");
                recognizer.AddIntent("id2", model, "YourLanguageUnderstandingIntentName2");
                recognizer.AddIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");

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
                    Console.WriteLine($"We recognized: {result.Text}.");
                    Console.WriteLine($"\n    Intent Id: {result.IntentId}.");
                    Console.WriteLine($"\n    Language Understanding JSON: {result.Properties.Get<string>(ResultPropertyKind.LanguageUnderstandingJson)}.");
                }
            }
            // </intentRecognitionWithMicrophone>
        }

        // Continuous intent recognition using file input.
        public static async Task ContinuousRecognitionWithFileAsync()
        {
            // <intentContinuousRecognitionWithFile>
            // Creates an instance of a speech factory with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Service Speech SDK, the intent recognition services
            // requires a specific subscription key from https://www.luis.ai/.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

            // Creates an intent recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var recognizer = factory.CreateIntentRecognizerWithFileInput("whatstheweatherlike.wav"))
            {
                // The TaskCompletionSource to stop recognition.
                var stopRecognition = new TaskCompletionSource<int>();

                // Creates a Language Understanding model using the app id, and adds specific intents from your model
                var model = LanguageUnderstandingModel.FromAppId("YourLanguageUnderstandingAppId");
                recognizer.AddIntent("id1", model, "YourLanguageUnderstandingIntentName1");
                recognizer.AddIntent("id2", model, "YourLanguageUnderstandingIntentName2");
                recognizer.AddIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");

                // Subscribes to events.
                recognizer.IntermediateResultReceived += (s, e) => {
                    Console.WriteLine($"\n    Partial result: {e.Result.Text}.");
                };

                recognizer.FinalResultReceived += (s, e) => {
                    if (e.Result.RecognitionStatus == RecognitionStatus.Recognized)
                    {
                        Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, Text: {e.Result.Text}.");
                        Console.WriteLine($"\n    Intent Id: {e.Result.IntentId}.");
                        Console.WriteLine($"\n    Language Understanding JSON: {e.Result.Properties.Get<string>(ResultPropertyKind.LanguageUnderstandingJson)}.");
                    }
                    else
                    {
                        Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, FailureReason: {e.Result.RecognitionFailureReason}.");
                    }
                };

                recognizer.RecognitionErrorRaised += (s, e) => {
                    Console.WriteLine($"\n    An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}");
                    stopRecognition.TrySetResult(0);
                };

                recognizer.OnSessionEvent += (s, e) => {
                    Console.WriteLine($"\n    Session event. Event: {e.EventType.ToString()}.");
                    // Stops recognition when session stop is detected.
                    if (e.EventType == SessionEventType.SessionStoppedEvent)
                    {
                        Console.WriteLine($"\nStop recognition.");
                        stopRecognition.TrySetResult(0);
                    }
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                // Waits for completion.
                // Use Task.WaitAny to keep the task rooted.
                Task.WaitAny(new[] { stopRecognition.Task });

                // Stops recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
            // </intentContinuousRecognitionWithFile>
        }

        // Intent recognition in the specified language, using microphone.
        public static async Task RecognitionWithMicrophoneUsingLanguageAsync()
        {
            // <intentRecognitionWithLanguage>
            // Creates an instance of a speech factory with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Service Speech SDK, the intent recognition services
            // requires a specific subscription key from https://www.luis.ai/.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

            // Creates an intent recognizer in the specified language using microphone as audio input.
            var lang = "de-de";

            using (var recognizer = factory.CreateIntentRecognizer(lang))
            {
                // Creates a Language Understanding model using the app id, and adds specific intents from your model
                var model = LanguageUnderstandingModel.FromAppId("YourLanguageUnderstandingAppId");
                recognizer.AddIntent("id1", model, "YourLanguageUnderstandingIntentName1");
                recognizer.AddIntent("id2", model, "YourLanguageUnderstandingIntentName2");
                recognizer.AddIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");

                // Starts recognizing.
                Console.WriteLine("Say something in " + lang + "...");

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
                    Console.WriteLine($"We recognized: {result.Text}.");
                    Console.WriteLine($"\n    Intent Id: {result.IntentId}.");
                    Console.WriteLine($"\n    Language Understanding JSON: {result.Properties.Get<string>(ResultPropertyKind.LanguageUnderstandingJson)}.");
                }
            }
            // </intentRecognitionWithLanguage>
        }
    }
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Recognition;

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionSamples
    {
        // Speech recognition from microphone.
        public static async Task RecognitionWithMicrophoneAsync()
        {
            // <recognitionWithMicrophone>
            // Gets recognizer factory.
            var factory = RecognizerFactory.Instance;

            // Sets subscription key.
            // Replace the key with your own key.
            factory.SubscriptionKey = "YourSubscriptionKey";

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Starts recognizing.
                Console.WriteLine("Say something...");

                // Starts recognition. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason != RecognitionStatus.Success)
                {
                    Console.WriteLine($"There was an error, reason {result.Reason}-{result.RecognizedText}");
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.RecognizedText}");
                }
            }
            // </recognitionWithMicrophone>
        }

        // Speech recognition from file.
        public static async Task RecognitionWithFileAsync()
        {
            // <recognitionFromFile>
            // Gets recognizer factory.
            var factory = RecognizerFactory.Instance;

            // Sets subscription key.
            // Replace the key with your own key.
            factory.SubscriptionKey = "YourSubscriptionKey";

            // Creates a speech recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(@"YourAudioFileName"))
            {
                // Starts recognition. It returns when the first utterance is recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason != RecognitionStatus.Success)
                {
                    Console.WriteLine($"There was an error, reason {result.Reason}-{result.RecognizedText}");
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.RecognizedText}");
                }
            }
            // </recognitionFromFile>
        }

        // <recognitionCustomized>
        // Speech recognition using a customized model.
        public static async Task RecognitionUsingCustomizedModelAsync()
        {
            // Creates a recognizer factory.
            var factory = RecognizerFactory.Instance;

            // Sets subscription key.
            // Replace with your own subscription key from https://www.cris.ai.
            factory.SubscriptionKey = "YourSubscriptionKey";

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Replace with the CRIS deployment id of your customized model.
                recognizer.DeploymentId = "YourDeploymentId";

                Console.WriteLine("Say something...");

                // Starts recognition. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks results.
                if (result.Reason != RecognitionStatus.Success)
                {
                    Console.WriteLine($"There was an error, reason {result.Reason}-{result.RecognizedText}");
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.RecognizedText}");
                }
            }
        }
        // </recognitionCustomized>

        // <recognitionContinuous>
        // Speech recognition with events
        public static async Task ContinuousRecognitionAsync()
        {
            var factory = RecognizerFactory.Instance;

            // Replaces the key with your own key.
            factory.SubscriptionKey = "YourSubscriptionKey";

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Subscribes to events.
                recognizer.IntermediateResultReceived += (s, e) =>
                        { Console.WriteLine($"\n    Partial result: {e.Result.RecognizedText}."); };
                recognizer.FinalResultReceived += (s, e) =>
                        { Console.WriteLine($"\n    Final result: Status: {e.Result.Reason}, Text: {e.Result.RecognizedText}."); };
                recognizer.RecognitionErrorRaised += (s, e) =>
                        { Console.WriteLine($"\n    Sorry, error occurred. Reason: {e.Status.ToString()}"); };
                recognizer.OnSessionEvent += (s, e) =>
                        { Console.WriteLine($"\n    Session event. Event: {e.EventType.ToString()}."); };

                // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                Console.WriteLine("Press any key to stop");
                Console.ReadKey();

                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
        }
        // </recognitionContinuous>
    }
}

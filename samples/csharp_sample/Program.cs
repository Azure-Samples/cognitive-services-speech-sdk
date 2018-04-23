//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Recognition;

namespace MicrosoftSpeechSDKSamples
{
    class Program
    {
        // Speech recognition from microphone.
        public static async Task RecognitionWithMicrophoneAsync()
        {
            // Creates recognizer factory.
            var factory = RecognizerFactory.Instance;

            // Sets subscription key.
            // Replaces the key with your own key.
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
        }

        // Speech recognition from file.
        public static async Task RecognitionWithFileAsync()
        {
            // Creates recognizer factory.
            var factory = RecognizerFactory.Instance;

            // Sets subscription key.
            // Replaces the key with your own key.
            factory.SubscriptionKey = "YourSubscriptionKey";

            // Creates a speech recognizer using file as audio input.
            // Replaces with your own audio file name.
            using (var recognizer = factory.CreateSpeechRecognizer(@"YourOwnAudioFile"))
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
        }

        // Speech recognition using a customized model.
        public static async Task RecognitionUsingCustomizedModelAsync()
        {
            // Creates a recognizer factory.
            var factory = RecognizerFactory.Instance;

            // Sets subscription key.
            // Replaces the key with your own key.
            factory.SubscriptionKey = "YourSubscriptionKey";

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = factory.CreateSpeechRecognizer())
            {
                // Replaces with the deployment id of your customized model.
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

        static void Main(string[] args)
        {

            Console.WriteLine("1. Recognition with microphone input.");
            Console.WriteLine("2. Recognition with file input.");
            Console.WriteLine("3. Using Customized model for recognition.");
            Console.WriteLine("4. Continuous Recognition using events.");
            Console.Write("Your choice: (0: Stop.) ");

            ConsoleKeyInfo x;
            do
            {
                x = Console.ReadKey();
                switch (x.Key)
                {
                    case ConsoleKey.D1:
                        RecognitionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                        RecognitionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                        RecognitionUsingCustomizedModelAsync().Wait();
                        break;
                    case ConsoleKey.D4:
                        ContinuousRecognitionAsync().Wait();
                        break;
                    case ConsoleKey.D0:
                        Console.WriteLine("Exiting...");
                        break;
                    default:
                        Console.WriteLine("Invalid input.");
                        break;
                }
                Console.WriteLine("Recognition done. Your Choice:");
            } while (x.Key != ConsoleKey.D0);
        }
    }
}

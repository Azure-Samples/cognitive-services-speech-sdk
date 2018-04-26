//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Recognition;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class TranslationSamples
    {
        // Translation from microphone.
        // <TranslationWithMicrophoneAsync>
        public static async Task TranslationWithMicrophoneAsync()
        {
            // Creates an instance of a speech factory with specified
            // subscription key.
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "");

            // Sets source and target languages
            string fromLanguage = "en-US";
            List<string> toLanguages = new List<string>() { "de-DE", "fr-FR" };

            // Creates a translation recognizer using microphone as audio input.
            using (var recognizer = factory.CreateTranslationRecognizer(fromLanguage, toLanguages))
            {
                Console.WriteLine("Say something...");

                // Starts translation. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason != RecognitionStatus.Success)
                {
                    Console.WriteLine($"There was an error in speech recognition, reason {result.Reason}-{result.RecognizedText}");
                }
                else if (result.TranslationStatus != RecognitionStatus.Success)
                {
                    Console.WriteLine($"There was an error in translation, status: {result.TranslationStatus}");
                }
                else
                {
                    Console.WriteLine($"We recognized in {fromLanguage}: {result.RecognizedText}");
                    foreach (var element in result.Translations)
                    {
                        Console.WriteLine($"    Translated into {element.Key}: <{element.Value}>");
                    }
                }
            }
        }
        // </TranslationWithMicrophoneAsync>

        // Translation using file input.
        // <TranslationWithFileAsync>
        public static async Task TranslationWithFileAsync()
        {
            // Creates an instance of a speech factory with specified
            // subscription key.
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "");

            // Sets source and target languages
            string fromLanguage = "en-US";
            List<string> toLanguages = new List<string>() { "de-DE", "fr-FR" };

            // Creates a translation recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var recognizer = factory.CreateTranslationRecognizerWithFileInput(@"YourAudioFileName", fromLanguage, toLanguages))
            {
                // Starts translation. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason != RecognitionStatus.Success)
                {
                    Console.WriteLine($"There was an error in speech recognition, reason {result.Reason}-{result.RecognizedText}");
                }
                else if (result.TranslationStatus != RecognitionStatus.Success)
                {
                    Console.WriteLine($"There was an error in translation, status: {result.TranslationStatus}");
                }
                else
                {
                    Console.WriteLine($"We recognized in {fromLanguage}: {result.RecognizedText}");
                    foreach (var element in result.Translations)
                    {
                        Console.WriteLine($"    Translated into {element.Key}: <{element.Value}>");
                    }
                }
            }
        }
        // </TranslationWithFileAsync>

        // Translation continuous recognition with events
        // <TranslationContinuousRecognitionAsync>
        public static async Task TranslationContinuousRecognitionAsync()
        {
            // Creates an instance of a speech factory with specified
            // subscription key.
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "");

            // Sets source and target languages
            string fromLanguage = "en-US";
            List<string> toLanguages = new List<string>() { "de-DE", "fr-FR" };

            // Creates a translation recognizer using microphone as audio input.
            using (var recognizer = factory.CreateTranslationRecognizer(fromLanguage, toLanguages))
            {
                // Subscribes to events.
                recognizer.IntermediateResultReceived += (s, e) => {
                    Console.WriteLine($"\nPartial result: recognized in {fromLanguage}: {e.Result.RecognizedText}.");
                    foreach (var element in e.Result.Translations)
                    {
                        Console.WriteLine($"    Translated into {element.Key}: <{element.Value}>");
                    }
                };

                recognizer.FinalResultReceived += (s, e) => {
                    Console.WriteLine($"\nFinal result: Status: {e.Result.TranslationStatus}, recognized text in {fromLanguage}: {e.Result.RecognizedText}.");
                    foreach (var element in e.Result.Translations)
                    {
                        Console.WriteLine($"    Translated into {element.Key}: <{element.Value}>");
                    }
                };

                recognizer.SynthesisResultReceived += (s, e) =>
                {
                    Console.WriteLine($"Synthesis result received. Size of audio data: {e.Result.Audio.Length}");
                };

                recognizer.RecognitionErrorRaised += (s, e) => {
                    Console.WriteLine($"\nSorry, error occurred. Reason: {e.Status.ToString()}");
                };

                recognizer.OnSessionEvent += (s, e) => {
                    Console.WriteLine($"\nSession event. Event: {e.EventType.ToString()}.");
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                Console.WriteLine("Press any key to stop");
                Console.ReadKey();

                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
        }
        // </TranslationContinuousRecognitionAsync>
    }
}

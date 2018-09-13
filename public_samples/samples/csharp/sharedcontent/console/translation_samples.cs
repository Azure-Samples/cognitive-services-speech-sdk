//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;
#if NET461
using System.Media;
#endif
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class TranslationSamples
    {
        // Translation from microphone.
        public static async Task TranslationWithMicrophoneAsync()
        {
            // <TranslationWithMicrophoneAsync>
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Voice name of synthesis output.
            const string GermanVoice = "de-DE-Hedda";

            // Creates an instance of a speech translator config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechTranslatorConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");
            config.SpeechRecognitionLanguage = fromLanguage;
            config.VoiceName = GermanVoice;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");

            // Creates a translation recognizer using microphone as audio input.
            using (var recognizer = new TranslationRecognizer(config))
            {
                // Subscribes to events.
                recognizer.IntermediateResultReceived += (s, e) => {
                    Console.WriteLine($"\nPartial result: recognized in {fromLanguage}: {e.Result.Text}.");
                    if (e.Result.TranslationStatus == TranslationStatus.Success)
                    {
                        foreach (var element in e.Result.Translations)
                        {
                            Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
                        }
                    }
                    else
                    {
                        Console.WriteLine($"    Translation failed. Status: {e.Result.TranslationStatus.ToString()}, FailureReason: {e.Result.FailureReason}");
                    }
                };

                recognizer.FinalResultReceived += (s, e) => {
                    var result = e.Result;
                    if (result.RecognitionStatus != RecognitionStatus.Recognized)
                    {
                        Console.WriteLine($"Recognition status: {result.RecognitionStatus.ToString()}");
                        if (result.RecognitionStatus == RecognitionStatus.Canceled)
                        {
                            Console.WriteLine($"There was an error, reason: {result.RecognitionFailureReason}");
                        }
                        return;
                    }
                    else
                    {
                        Console.WriteLine($"\nFinal result: Status: {result.RecognitionStatus.ToString()}, recognized text in {fromLanguage}: {result.Text}.");
                        if (result.TranslationStatus == TranslationStatus.Success)
                        {
                            foreach (var element in result.Translations)
                            {
                                Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
                            }
                        }
                        else
                        {
                            Console.WriteLine($"    Translation failed. Status: {result.TranslationStatus.ToString()}, FailureReason: {result.FailureReason}");
                        }
                    }
                };

                recognizer.SynthesisResultReceived += (s, e) =>
                {
                    if (e.Result.Status == SynthesisStatus.Success)
                    {
                        Console.WriteLine($"Synthesis result received. Size of audio data: {e.Result.Audio.Length}");
                        #if NET461
                        using (var m = new MemoryStream(e.Result.Audio))
                        {
                            SoundPlayer simpleSound = new SoundPlayer(m);
                            simpleSound.PlaySync();
                        }
                        #endif
                    }
                    else if (e.Result.Status == SynthesisStatus.SynthesisEnd)
                    {
                        Console.WriteLine($"Synthesis result: end of synthesis result.");
                    }
                    else
                    {
                        Console.WriteLine($"Synthesis error. Status: {e.Result.Status.ToString()}, Failure reason: {e.Result.FailureReason}");
                    }
                };

                recognizer.RecognitionErrorRaised += (s, e) => {
                    Console.WriteLine($"\nAn error occurred. Status: {e.Status.ToString()}");
                };

                recognizer.OnSessionEvent += (s, e) => {
                    Console.WriteLine($"\nSession event. Event: {e.EventType.ToString()}.");
                };

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                Console.WriteLine("Say something...");
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                do
                {
                    Console.WriteLine("Press Enter to stop");
                } while (Console.ReadKey().Key != ConsoleKey.Enter);

                // Stops continuous recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
            // </TranslationWithMicrophoneAsync>
        }

        // Translation using file input.
        public static async Task TranslationWithFileAsync()
        {
            // <TranslationWithFileAsync>
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Creates an instance of a speech translator config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechTranslatorConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");
            config.SpeechRecognitionLanguage = fromLanguage;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            var stopTranslation = new TaskCompletionSource<int>();

            // Creates a translation recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput(@"whatstheweatherlike.wav"))
            {
                using (var recognizer = new TranslationRecognizer(config, audioInput))
                {
                    // Subscribes to events.
                    recognizer.IntermediateResultReceived += (s, e) => {
                        Console.WriteLine($"\nPartial result: recognized in {fromLanguage}: {e.Result.Text}.");
                        if (e.Result.TranslationStatus == TranslationStatus.Success)
                        {
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
                            }
                        }
                        else
                        {
                            Console.WriteLine($"    Translation failed. Status: {e.Result.TranslationStatus.ToString()}, FailureReason: {e.Result.FailureReason}");
                        }
                    };

                    recognizer.FinalResultReceived += (s, e) => {
                        var result = e.Result;
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
                            return;
                        }
                        else
                        {
                            Console.WriteLine($"\nFinal result: Status: {result.RecognitionStatus.ToString()}, recognized text in {fromLanguage}: {result.Text}.");
                            if (result.TranslationStatus == TranslationStatus.Success)
                            {
                                foreach (var element in result.Translations)
                                {
                                    Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
                                }
                            }
                            else
                            {
                                Console.WriteLine($"    Translation failed. Status: {result.TranslationStatus.ToString()}, FailureReason: {result.FailureReason}");
                            }
                        }
                    };

                    recognizer.RecognitionErrorRaised += (s, e) => {
                        Console.WriteLine($"\nAn error occurred. Status: {e.Status.ToString()}");
                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.OnSpeechDetectedEvent += (s, e) => {
                        Console.WriteLine($"\nSpeech detected event. Event: {e.EventType.ToString()}.");
                    };

                    recognizer.OnSessionEvent += (s, e) => {
                        Console.WriteLine($"\nSession event. Event: {e.EventType.ToString()}.");
                        // Stops translation when session stop is detected.
                        if (e.EventType == SessionEventType.SessionStoppedEvent)
                        {
                            Console.WriteLine($"\nStop translation.");
                            stopTranslation.TrySetResult(0);
                        }
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            // </TranslationWithFileAsync>
        }

        // Translation using audio stream.
        public static async Task TranslationWithAudioStreamAsync()
        {
            // <TranslationWithAudioStreamAsync>
            // Translation source language.
            // Replace with a language of your choice.
            string fromLanguage = "en-US";

            // Creates an instance of a speech translator config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechTranslatorConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");
            config.SpeechRecognitionLanguage = fromLanguage;

            // Translation target language(s).
            // Replace with language(s) of your choice.
            config.AddTargetLanguage("de");
            config.AddTargetLanguage("fr");

            var stopTranslation = new TaskCompletionSource<int>();

            // Create an audio stream from a wav file.
            // Replace with your own audio file name.
            using (var audioInput = Helper.OpenWavFile(@"whatstheweatherlike.wav"))
            {
                // Creates a translation recognizer using audio stream as input.
                using (var recognizer = new TranslationRecognizer(config, audioInput))
                {
                    // Subscribes to events.
                    recognizer.IntermediateResultReceived += (s, e) =>
                    {
                        Console.WriteLine($"\nPartial result: recognized in {fromLanguage}: {e.Result.Text}.");
                        if (e.Result.TranslationStatus == TranslationStatus.Success)
                        {
                            foreach (var element in e.Result.Translations)
                            {
                                Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
                            }
                        }
                        else
                        {
                            Console.WriteLine($"    Translation failed. Status: {e.Result.TranslationStatus.ToString()}, FailureReason: {e.Result.FailureReason}");
                        }
                    };

                    recognizer.FinalResultReceived += (s, e) =>
                    {
                        var result = e.Result;
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
                            return;
                        }
                        else
                        {
                            Console.WriteLine($"\nFinal result: Status: {result.RecognitionStatus.ToString()}, recognized text in {fromLanguage}: {result.Text}.");
                            if (result.TranslationStatus == TranslationStatus.Success)
                            {
                                foreach (var element in result.Translations)
                                {
                                    Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
                                }
                            }
                            else
                            {
                                Console.WriteLine($"    Translation failed. Status: {result.TranslationStatus.ToString()}, FailureReason: {result.FailureReason}");
                            }
                        }
                    };

                    recognizer.RecognitionErrorRaised += (s, e) =>
                    {
                        Console.WriteLine($"\nAn error occurred. Status: {e.Status.ToString()}");
                        stopTranslation.TrySetResult(0);
                    };

                    recognizer.OnSpeechDetectedEvent += (s, e) =>
                    {
                        Console.WriteLine($"\nSpeech detected event. Event: {e.EventType.ToString()}.");
                    };

                    recognizer.OnSessionEvent += (s, e) =>
                    {
                        Console.WriteLine($"\nSession event. Event: {e.EventType.ToString()}.");
                        // Stops translation when session stop is detected.
                        if (e.EventType == SessionEventType.SessionStoppedEvent)
                        {
                            Console.WriteLine($"\nStop translation.");
                            stopTranslation.TrySetResult(0);
                        }
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    Console.WriteLine("Start translation...");
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopTranslation.Task });

                    // Stops translation.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            // </TranslationWithAudioStreamAsync>
        }
    }
}

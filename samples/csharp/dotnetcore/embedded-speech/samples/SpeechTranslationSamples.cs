//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;


namespace MicrosoftSpeechSDKSamples
{
    public class SpeechTranslationSamples
    {
        // List available embedded speech translation models.
        public static void ListEmbeddedModels()
        {
            // Creates an instance of an embedded speech config.
            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            if (speechConfig == null)
            {
                return;
            }

            // Gets a list of models.
            var models = speechConfig.GetSpeechTranslationModels();

            if (models.Count > 0)
            {
                Console.WriteLine($"Models found [{models.Count}]:");
                foreach (var model in models)
                {
                    Console.WriteLine();
                    Console.WriteLine(model.Name);
                    Console.Write($" Source language(s) [{model.SourceLanguages.Length}]: ");
                    foreach (var sourceLang in model.SourceLanguages)
                    {
                        Console.Write($"{sourceLang} ");
                    }
                    Console.WriteLine();
                    Console.Write($" Target language(s) [{model.TargetLanguages.Length}]: ");
                    foreach (var targetLang in model.TargetLanguages)
                    {
                        Console.Write($"{targetLang} ");
                    }
                    Console.WriteLine();
                    Console.WriteLine($" Path: {model.Path}");
                }
            }
            else
            {
                Console.Error.WriteLine("No models found. Either the path is not valid or the format of model(s) is unknown.");
            }
        }


        private static async Task TranslateSpeechAsync(TranslationRecognizer recognizer)
        {
            // Subscribes to events.
            recognizer.Recognizing += (s, e) =>
            {
                // Intermediate result (hypothesis).
                // Note that embedded "many-to-1" translation models support only one
                // target language (the model native output language). For example, a
                // "Many-to-English" model generates only output in English.
                // At the moment embedded translation cannot provide transcription of
                // the source language.
                if (e.Result.Reason == ResultReason.TranslatingSpeech)
                {
                    // Source (input) language identification is enabled when TranslationRecognizer
                    // is created with an AutoDetectSourceLanguageConfig argument.
                    // In case the model does not support this functionality or the language cannot
                    // be identified, the result Language is "Unknown".
                    var sourceLangResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                    var sourceLang = sourceLangResult.Language;

                    foreach (var translation in e.Result.Translations)
                    {
                        var targetLang = translation.Key;
                        var outputText = translation.Value;
                        Console.WriteLine($"Translating [{sourceLang} -> {targetLang}]: {outputText}");
                    }
                }
            };

            recognizer.Recognized += (s, e) =>
            {
                // Final result. May differ from the last intermediate result.
                if (e.Result.Reason == ResultReason.TranslatedSpeech)
                {
                    var sourceLangResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                    var sourceLang = sourceLangResult.Language;

                    foreach (var translation in e.Result.Translations)
                    {
                        var targetLang = translation.Key;
                        var outputText = translation.Value;
                        Console.WriteLine($"TRANSLATED [{sourceLang} -> {targetLang}]: {outputText}");
                    }
                }
                else if (e.Result.Reason == ResultReason.NoMatch)
                {
                    // NoMatch occurs when no speech was recognized.
                    var reason = NoMatchDetails.FromResult(e.Result).Reason;
                    Console.WriteLine($"NO MATCH: Reason={reason}");
                }
            };

            recognizer.Canceled += (s, e) =>
            {
                Console.WriteLine($"CANCELED: Reason={e.Reason}");

                if (e.Reason == CancellationReason.Error)
                {
                    // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
                    Console.Error.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                    Console.Error.WriteLine($"CANCELED: ErrorDetails=\"{e.ErrorDetails}\"");
                }
            };

            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("Session started.");
            };

            recognizer.SessionStopped += (s, e) =>
            {
                Console.WriteLine("Session stopped.");
            };

            // The following lines run continuous recognition that listens for speech
            // in input audio and generates results until stopped. To run recognition
            // only once, replace this code block with
            //
            // var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            //
            // and optionally check this returned result for the outcome instead of
            // doing it in event handlers.

            // Starts continuous recognition.
            await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

            Console.WriteLine("Say something (press Enter to stop)...");
            Console.ReadKey();

            // Stops recognition.
            await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
        }


        // Translates speech using embedded speech config and the system default microphone device.
        public static void EmbeddedTranslationFromMicrophone()
        {
            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            var sourceLangConfig = AutoDetectSourceLanguageConfig.FromOpenRange(); // optional, for input language identification
            using var audioConfig = AudioConfig.FromDefaultMicrophoneInput();

            using var recognizer = new TranslationRecognizer(speechConfig, sourceLangConfig, audioConfig);
            TranslateSpeechAsync(recognizer).Wait();
        }
    }
}

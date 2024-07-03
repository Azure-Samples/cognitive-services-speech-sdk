//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;


namespace MicrosoftSpeechSDKSamples
{
    public class IntentRecognitionSamples
    {
        // Creates a pattern matching model with some example intents.
        // For more examples on intent patterns, see
        // https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/csharp/sharedcontent/console/intent_recognition_samples.cs
        private static PatternMatchingModel CreatePatternMatchingModel()
        {
            // Creates a pattern matching model. The id is necessary to identify this
            // model from others if there are several models in use at the same time.
            var model = new PatternMatchingModel("SomeUniqueIdForThisModel");

            // Make the keyword optional (surround with [ and ] in the pattern string)
            // in case the pattern matching model is used without keyword recognition.
            string patternKeywordOptional = "[" + Settings.GetKeywordPhrase() + "][{any}]";

            // Specify some intents to add. Example inputs:
            // - "Turn on the radio."
            // - "Switch off the lights in the bedroom."

            // Notes about patterns:
            // - surround entity names with { and }
            // - surround alternatives with ( and ) and separate with |
            // - surround optional parts with [ and ]
            // - default entity {any} matches any text
            string patternTurnOn = "(turn|switch) on";
            string patternTurnOff = "(turn|switch) off";
            string patternTarget = "[{any}] {targetName}";      // using {any} to match any text before {targetName}
            string patternLocation = "[{any}] {locationName}";  // using {any} to match any text before {locationName}

            // Define acceptable values for the entities. By default an entity can have any value.
            // If these are not defined, patterns with {any} before the entity name will not work.
            var targetNames = new List<string> { "lamp", "lights", "radio", "TV" };
            model.Entities.Add(PatternMatchingEntity.CreateListEntity(
                "targetName",
                EntityMatchMode.Strict,
                targetNames
                ));

            var locationNames = new List<string>{ "living room", "bedroom", "kitchen" };
            model.Entities.Add(PatternMatchingEntity.CreateListEntity(
                "locationName",
                EntityMatchMode.Strict,
                locationNames
                ));

            // Add intents (id, phrase(s)) to the model.
            var intents = new List<KeyValuePair<string, List<string>>>
            {
                new KeyValuePair<string, List<string>>(
                    "HomeAutomation.TurnOn",
                    new List<string>
                    {
                        patternKeywordOptional + " " + patternTurnOn + " " + patternTarget,
                        patternKeywordOptional + " " + patternTurnOn + " " + patternTarget + " " + patternLocation
                    }),
                new KeyValuePair<string, List<string>>(
                    "HomeAutomation.TurnOff",
                    new List<string>
                    {
                        patternKeywordOptional + " " + patternTurnOff + " " + patternTarget,
                        patternKeywordOptional + " " + patternTurnOff + " " + patternTarget + " " + patternLocation
                    })
            };

            Console.WriteLine("Added intents");
            foreach (var intent in intents)
            {
                model.Intents.Add(new PatternMatchingIntent(intent.Key, intent.Value));
                Console.WriteLine($"  id=\"{intent.Key}\"");
                foreach (var phrase in intent.Value)
                {
                    Console.WriteLine($"    phrase=\"{phrase}\"");
                }
            }
            Console.WriteLine("where");
            Console.Write("  targetName can be one of ");
            foreach (var targetName in targetNames)
            {
                Console.Write($"\"{targetName}\" ");
            }
            Console.WriteLine();
            Console.Write("  locationName can be one of ");
            foreach (var locationName in locationNames)
            {
                Console.Write($"\"{locationName}\" ");
            }
            Console.WriteLine("\n");

            return model;
        }


        private static async Task RecognizeIntentAsync(bool useKeyword)
        {
            // Creates an instance of an embedded speech config.
            var speechConfig = Settings.CreateEmbeddedSpeechConfig();

            // Creates an intent recognizer using microphone as audio input.
            using var audioConfig = AudioConfig.FromDefaultMicrophoneInput();
            using var recognizer = new IntentRecognizer(speechConfig, audioConfig);

            // Creates a pattern matching model.
            var patternMatchingModel = CreatePatternMatchingModel();
            // Adds the model to a new language model collection.
            var modelCollection = new LanguageUnderstandingModelCollection();
            modelCollection.Add(patternMatchingModel);
            // Applies the language model collection to the recognizer.
            // This replaces all previously applied models.
            recognizer.ApplyLanguageModels(modelCollection);

            // Subscribes to events.
            recognizer.Recognizing += (s, e) =>
            {
                // Intermediate result (hypothesis).
                if (e.Result.Reason == ResultReason.RecognizingSpeech)
                {
                    Console.WriteLine($"Recognizing:{e.Result.Text}");
                }
                else if (e.Result.Reason == ResultReason.RecognizingKeyword)
                {
                    // ignored
                }
            };

            recognizer.Recognized += (s, e) =>
            {
                if (e.Result.Reason == ResultReason.RecognizedKeyword)
                {
                    // Keyword detected, speech recognition will start.
                    Console.WriteLine($"KEYWORD: Text={e.Result.Text}");
                }
                else if (e.Result.Reason == ResultReason.RecognizedIntent)
                {
                    // Final result (intent recognized). May differ from the last intermediate result.
                    Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                    Console.WriteLine($"  Intent Id: {e.Result.IntentId}");

                    if (e.Result.IntentId.Contains("HomeAutomation"))
                    {
                        var entities = e.Result.Entities;
                        if (entities.ContainsKey("targetName"))
                        {
                            Console.WriteLine($"     Target: {entities["targetName"]}");
                            if (entities.ContainsKey("locationName"))
                            {
                                Console.WriteLine($"   Location: {entities["locationName"]}");
                            }
                        }
                    }
                }
                else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                {
                    // Final result (no intent recognized). May differ from the last intermediate result.
                    Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                    Console.WriteLine("  (intent not recognized)");
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

            if (useKeyword)
            {
                // Creates an instance of a keyword recognition model.
                var keywordModel = KeywordRecognitionModel.FromFile(Settings.GetKeywordModelFileName());

                // Starts the following routine:
                // 1. Listen for a keyword in input audio. There is no timeout.
                //    Speech that does not start with the keyword is ignored.
                // 2. If the keyword is spotted, start normal speech recognition.
                // 3. After a recognition result (that always includes at least
                //    the keyword), go back to step 1.
                // Steps 1-3 repeat until StopKeywordRecognitionAsync() is called.
                await recognizer.StartKeywordRecognitionAsync(keywordModel).ConfigureAwait(false);

                Console.WriteLine($"Say something starting with \"{Settings.GetKeywordPhrase()}\" (press Enter to stop)...");
                Console.ReadKey();

                // Stops recognition.
                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
            else
            {
                // Starts continuous recognition.
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                Console.WriteLine("Say something (press Enter to stop)...");
                Console.ReadKey();

                // Stops recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
        }


        // Recognizes intents using embedded speech config and the system default microphone device.
        public static void EmbeddedRecognitionFromMicrophone()
        {
            var useKeyword = false;
            RecognizeIntentAsync(useKeyword).Wait();
        }


        // Recognizes intents using embedded speech config and the system default microphone device.
        // Recognition is triggered with a keyword.
        public static void EmbeddedRecognitionWithKeywordFromMicrophone()
        {
            var useKeyword = true;
            RecognizeIntentAsync(useKeyword).Wait();
        }
    }
}

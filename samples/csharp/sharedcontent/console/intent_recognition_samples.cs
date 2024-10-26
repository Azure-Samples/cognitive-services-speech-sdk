//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class IntentRecognitionSamples
    {
        /// <summary>
        /// Intent recognition using microphone. 
        /// </summary>
        public static async Task RecognitionWithMicrophoneAsync()
        {
            // <intentRecognitionWithMicrophone>
            // Creates an instance of a speech config with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Services Speech SDK, the Language Understanding service
            // requires a specific subscription key from https://www.luis.ai/.
            // The Language Understanding service calls the required key 'endpoint key'.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

            // Creates an intent recognizer using microphone as audio input.
            using (var recognizer = new IntentRecognizer(config))
            {
                // Creates a Language Understanding model using the app id, and adds specific intents from your model
                var model = LanguageUnderstandingModel.FromAppId("YourLanguageUnderstandingAppId");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

                // Starts recognizing.
                Console.WriteLine("Say something...");

                // Starts intent recognition, and returns after a single utterance is recognized. The end of a
                // single utterance is determined by listening for silence at the end or until a maximum of about 30
                // seconds of audio is processed.  The task returns the recognition text as result. 
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                // shot recognition like command or query. 
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason == ResultReason.RecognizedIntent)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine($"    Intent Id: {result.IntentId}.");
                    Console.WriteLine($"    Language Understanding JSON: {result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult)}.");
                }
                else if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine($"    Intent not recognized.");
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
            // </intentRecognitionWithMicrophone>
        }

        /// <summary>
        /// Continuous intent recognition using file input. 
        /// </summary>
        public static async Task ContinuousRecognitionWithFileAsync()
        {
            // <intentContinuousRecognitionWithFile>
            // Creates an instance of a speech config with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Services Speech SDK, the Language Understanding service
            // requires a specific subscription key from https://www.luis.ai/.
            // The Language Understanding service calls the required key 'endpoint key'.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

            // Creates an intent recognizer using file as audio input.
            // Replace with your own audio file name.
            using (var audioInput = AudioConfig.FromWavFileInput("YourAudioFile.wav"))
            {
                using (var recognizer = new IntentRecognizer(config, audioInput))
                {
                    // The TaskCompletionSource to stop recognition.
                    var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

                    // Creates a Language Understanding model using the app id, and adds specific intents from your model
                    var model = LanguageUnderstandingModel.FromAppId("YourLanguageUnderstandingAppId");
                    recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
                    recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
                    recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

                    // Subscribes to events.
                    recognizer.Recognizing += (s, e) =>
                    {
                        Console.WriteLine($"RECOGNIZING: Text={e.Result.Text}");
                    };

                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizedIntent)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Intent Id: {e.Result.IntentId}.");
                            Console.WriteLine($"    Language Understanding JSON: {e.Result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult)}.");
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            Console.WriteLine($"    Intent not recognized.");
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
            // </intentContinuousRecognitionWithFile>
        }

        /// <summary>
        /// Intent recognition in the German, using microphone.
        /// </summary>
        public static async Task RecognitionWithMicrophoneUsingLanguageAsync()
        {
            // Creates an instance of a speech config with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Services Speech SDK, the Language Understanding service
            // requires a specific subscription key from https://www.luis.ai/.
            // The Language Understanding service calls the required key 'endpoint key'.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");
            var language = "de-de";
            config.SpeechRecognitionLanguage = language;

            // Creates an intent recognizer in the specified language using microphone as audio input.
            using (var recognizer = new IntentRecognizer(config))
            {
                // Creates a Language Understanding model using the app id, and adds specific intents from your model
                var model = LanguageUnderstandingModel.FromAppId("YourLanguageUnderstandingAppId");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

                // Starts recognizing.
                Console.WriteLine("Say something in " + language + "...");

                // Starts speech recognition, and returns after a single utterance is recognized. The end of a
                // single utterance is determined by listening for silence at the end or until a maximum of about 30
                // seconds of audio is processed.  The task returns the recognition text as result. 
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                // shot recognition like command or query. 
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason == ResultReason.RecognizedIntent)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine($"    Intent Id: {result.IntentId}.");
                    Console.WriteLine($"    Language Understanding JSON: {result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult)}.");
                }
                else if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine($"    Intent not recognized.");
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
        }

        /// <summary>
        /// Use pattern matching for intent recognition from your default microphone input
        /// </summary>
        public static async Task IntentPatternMatchingWithMicrophoneAsync()
        {
            // Creates an instance of a speech config with specified subscription key and service region. Note that in
            // contrast to the other samples this DOES NOT require a LUIS application.
            // The default recognition language is "en-us".
            var config = SpeechConfig.FromSubscription(
                "YourSubscriptionKey",
                "YourServiceRegion");

            // Creates an intent recognizer using microphone as audio input.
            using (var recognizer = new IntentRecognizer(config))
            {

                // Creates a Pattern Matching model and adds specific intents from your model. The
                // Id is used to identify this model from others in the collection.
                var model = new PatternMatchingModel("YourPatternMatchingModelId");

                // Creates a string with a pattern that uses groups of optional words. Optional phrases in square brackets can
                // select one phrase from several choices by separating them inside the brackets with a pipe '|'. Here,
                // "[Go | Take me]" will match either "Go", "Take me", or "". Note the space after the keyword.
                var patternWithOptionalWords = "[Go | Take me] to [floor|level] {floorName}";

                // Creates a string with a pattern that uses an optional entity and group that could be used to tie commands
                // together. Optional patterns in square brackets can also include a reference to an entity. "[{parkingLevel}]"
                // includes a match against the named entity as an optional component in this pattern.
                var patternWithOptionalEntity = "Go to parking [{parkingLevel}]";

                // You can also have multiple entities of the same name in a single pattern by adding appending a unique identifier
                // to distinguish between the instances. For example:
                var patternWithTwoOfTheSameEntity = "Go to floor {floorName:1} [and then go to floor {floorName:2}]";
                // NOTE: Both floorName:1 and floorName:2 are tied to the same list of entries. The identifier can be a string
                //       and is separated from the entity name by a ':'

                // Adds some intents to look for specific patterns.
                model.Intents.Add(new PatternMatchingIntent(
                    "ChangeFloors", patternWithOptionalWords, patternWithOptionalEntity, patternWithTwoOfTheSameEntity));
                model.Intents.Add(new PatternMatchingIntent(
                    "DoorControl", "{action} the doors", "{action} doors", "{action} the door", "{action} door"));

                // Creates the "floorName" entity and set it to type list.
                // Adds acceptable values. NOTE the default entity type is Any and so we do not need
                // to declare the "action" entity.
                model.Entities.Add(PatternMatchingEntity.CreateListEntity(
                    "floorName",
                    EntityMatchMode.Strict,
                    "ground floor", "lobby", "1st", "first", "one", "1", "2nd", "second", "two", "2"));

                // Creates the "parkingLevel" entity as a pre-built integer
                model.Entities.Add(PatternMatchingEntity.CreateIntegerEntity("parkingLevel"));

                // Add the model to a new language model collection
                var modelCollection = new LanguageUnderstandingModelCollection();
                modelCollection.Add(model);

                // Apply the language model collection to the recognizer.
                recognizer.ApplyLanguageModels(modelCollection);

                Console.WriteLine("Say something...");

                // Starts intent recognition, and returns after a single utterance is recognized. The end of a single
                // utterance is determined by listening for silence at the end, or until a maximum of about 30 seconds of audio
                // is processed. The task returns the recognition text as result. 
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single shot
                // recognition like command or query.
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync();

                // Checks result.
                if (result.Reason == ResultReason.RecognizedIntent)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine($"{"Intent Id=",13} {result.IntentId}.");

                    var entities = result.Entities;
                    switch (result.IntentId)
                    {
                        case "ChangeFloors":
                            if (entities.TryGetValue("floorName", out string floorName))
                            {
                                Console.WriteLine($"{"FloorName=",17} {floorName}");
                            }

                            if (entities.TryGetValue("floorName:1", out floorName))
                            {
                                Console.WriteLine($"{"FloorName:1=",17} {floorName}");
                            }

                            if (entities.TryGetValue("floorName:2", out floorName))
                            {
                                Console.WriteLine($"{"FloorName:2=",17} {floorName}");
                            }

                            if (entities.TryGetValue("parkingLevel", out string parkingLevel))
                            {
                                Console.WriteLine($"{"ParkingLevel=",17} {parkingLevel}");
                            }
                            break;
                        case "DoorControl":
                            if (entities.TryGetValue("action", out string action))
                            {
                                Console.WriteLine($"{"Action=",17} {action}");
                            }
                            break;
                        default:
                            Console.WriteLine($"Unknown intent ID: {result.IntentId}");
                            break;
                    }
                }
                else if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine($"{"Intent not recognized.",17}");
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
        }

        /// <summary>
        /// Keyword-triggered intent recognition using microphone. This is useful for when you don't have a push-to-talk feature
        /// and want to activate your device with voice only. A keyword model is used for local recognition and activation.
        /// NOTE: It is possible to still call recognize once during a keyword spotting session if you want to have both
        /// push-to-talk and keyword activation.
        /// Example interaction: "Computer turn on the lights".
        /// </summary>
        public static async Task IntentPatternMatchingWithMicrophoneAndKeywordSpottingAsync()
        {
            // Creates an instance of a speech config with specified subscription key and service region. Note that in
            // contrast to the other samples this DOES NOT require a LUIS application.
            // The default recognition language is "en-us".
            var config = SpeechConfig.FromSubscription(
                "YourSubscriptionKey",
                "YourServiceRegion");

            // Creates an instance of a keyword recognition model. Update this to
            // point to the location of your keyword recognition model.
            var keywordModel = KeywordRecognitionModel.FromFile(@"YourKeywordRecognitionModelFile.table");

            // The phrase your keyword recognition model triggers on.
            var keyword = "YourKeyword";

            // Creates an intent recognizer using microphone as audio input.
            using (var recognizer = new IntentRecognizer(config))
            {

                // Create a string containing the keyword with the optional pattern tags on it. This can be useful if you
                // are using push to talk and keyword activation.
                var keywordOptionalPattern = "[" + keyword + "]";

                // Creates a Pattern Matching model and adds specific intents from your model. The Id is used to identify
                // this model from others in the collection.
                var patternMatchingModel = new PatternMatchingModel("YourPatternMatchingModelId");

                // Creates the "floorName" entity and set it to type list.
                // Adds acceptable values. NOTE the default entity type is Any and so we do not need
                // to declare the "action" entity.
                patternMatchingModel.Entities.Add(PatternMatchingEntity.CreateListEntity(
                    "floorName",
                    EntityMatchMode.Strict,
                    "ground floor", "lobby", "1st", "first", "one", "1", "2nd", "second", "two", "2"));

                // Creates the "parkingLevel" entity as a pre-built integer
                patternMatchingModel.Entities.Add(PatternMatchingEntity.CreateIntegerEntity("parkingLevel"));

                // Creates a string with a pattern that uses groups of optional words. Optional phrases in square brackets can
                // select one phrase from several choices by separating them inside the brackets with a pipe '|'. Here,
                // "[Go | Take me]" will match either "Go", "Take me", or "". Note the space after the keyword.
                var patternWithOptionalWords = keywordOptionalPattern + " " + "[Go | Take me] to [floor|level] {floorName}";

                // Creates a string with a pattern that uses an optional entity and group that could be used to tie commands
                // together. Optional patterns in square brackets can also include a reference to an entity. "[{parkingLevel}]"
                // includes a match against the named entity as an optional component in this pattern.
                var patternWithOptionalEntity = keywordOptionalPattern + " " + "Go to parking [{parkingLevel}]";

                // You can also have multiple entities of the same name in a single pattern by adding appending a unique identifier
                // to distinguish between the instances. For example:
                var patternWithTwoOfTheSameEntity = keywordOptionalPattern + " "
                    + "Go to floor {floorName:1} [and then go to floor {floorName:2}]";
                // NOTE: Both floorName:1 and floorName:2 are tied to the same list of entries. The identifier can be a string
                //       and is separated from the entity name by a ':'

                // Adds some intents to look for specific patterns.
                patternMatchingModel.Intents.Add(new PatternMatchingIntent(
                    "ChangeFloors", patternWithOptionalWords, patternWithOptionalEntity, patternWithTwoOfTheSameEntity));
                patternMatchingModel.Intents.Add(new PatternMatchingIntent("DoorControl",
                    keywordOptionalPattern + " " + "{action} the doors",
                    keywordOptionalPattern + " " + "{action} doors",
                    keywordOptionalPattern + " " + "{action} the door",
                    keywordOptionalPattern + " " + "{action} door"));

                // Add the model to a new language model collection
                var modelCollection = new LanguageUnderstandingModelCollection();
                modelCollection.Add(patternMatchingModel);

                // Apply the language model collection to the recognizer.
                recognizer.ApplyLanguageModels(modelCollection);

                var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

                // Subscribes to events.
                recognizer.Recognizing += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                    {
                        Console.WriteLine($"RECOGNIZING KEYWORD: Text={e.Result.Text}");
                    }
                    else if (e.Result.Reason == ResultReason.RecognizingSpeech)
                    {
                        Console.WriteLine($"RECOGNIZING: Text={e.Result.Text}");
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                // Checks result.
                var result = e.Result;
                    if (result.Reason == ResultReason.RecognizedKeyword)
                    {
                        Console.WriteLine($"RECOGNIZED KEYWORD: Text={e.Result.Text}");
                    }
                    else if (result.Reason == ResultReason.RecognizedIntent)
                    {
                        Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                        Console.WriteLine($"{"Intent Id=",13} {result.IntentId}.");

                        var entities = result.Entities;
                        switch (result.IntentId)
                        {
                            case "ChangeFloors":
                                if (entities.TryGetValue("floorName", out string floorName))
                                {
                                    Console.WriteLine($"{"FloorName=",17} {floorName}");
                                }

                                if (entities.TryGetValue("floorName:1", out floorName))
                                {
                                    Console.WriteLine($"{"FloorName:1=",17} {floorName}");
                                }

                                if (entities.TryGetValue("floorName:2", out floorName))
                                {
                                    Console.WriteLine($"{"FloorName:2=",17} {floorName}");
                                }

                                if (entities.TryGetValue("parkingLevel", out string parkingLevel))
                                {
                                    Console.WriteLine($"{"ParkingLevel=",17} {parkingLevel}");
                                }
                                break;
                            case "DoorControl":
                                if (entities.TryGetValue("action", out string action))
                                {
                                    Console.WriteLine($"{"Action=",17} {action}");
                                }
                                break;
                            default:
                                Console.WriteLine($"Unknown intent ID: {result.IntentId}");
                                break;
                        }
                    }
                    else if (result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                        Console.WriteLine($"{"Intent not recognized.",17}");
                    }
                    else if (result.Reason == ResultReason.NoMatch)
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
                    Console.WriteLine($"{"Session started event.",17}");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"{"Session stopped event.",17}");
                    Console.WriteLine($"{"Stop recognition.",17}");
                    stopRecognition.TrySetResult(0);
                };

                // Starts recognizing.
                Console.WriteLine($"Say something starting with the keyword '{keyword}' followed by whatever you want...");

                // Starts continuous recognition using the keyword model. Use
                // StopKeywordRecognitionAsync() to stop recognition.
                await recognizer.StartKeywordRecognitionAsync(keywordModel).ConfigureAwait(false);

                // Waits for a single successful keyword-triggered speech recognition (or error).
                await stopRecognition.Task;

                // Stops recognition.
                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }
    }
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <skeleton_1>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Intent;

namespace helloworld
{
    class Program
    {
        public static async Task RecognizeIntentAsync()
        {
            // </skeleton_1>
            // Creates an instance of a speech config with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Services Speech SDK, the Language Understanding service
            // requires a specific subscription key from https://www.luis.ai/.
            // The Language Understanding service calls the required key 'endpoint key'.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            // The default language is "en-us".
            // <create_speech_configuration>
            var config = SpeechConfig.FromSubscription(
                "YourLanguageUnderstandingSubscriptionKey",
                "YourLanguageUnderstandingServiceRegion");
            // </create_speech_configuration>

            // <create_intent_recognizer_1>
            // Creates an intent recognizer using microphone as audio input.
            using (var recognizer = new IntentRecognizer(config))
            {
                // </create_intent_recognizer_1>

                // <add_intents>
                // Creates a Language Understanding model using the app id, and adds specific intents from your model
                var model = LanguageUnderstandingModel.FromAppId("YourLanguageUnderstandingAppId");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
                recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");
                // </add_intents>

                // To add all of the possible intents from a LUIS model to the recognizer, uncomment the line below:
                // recognizer.AddAllIntents(model);

                // <recognize_intent>
                // Starts recognizing.
                Console.WriteLine("Say something...");

                // Starts intent recognition, and returns after a single utterance is recognized. The end of a
                // single utterance is determined by listening for silence at the end or until a maximum of about 30
                // seconds of audio is processed.  The task returns the recognition text as result. 
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                // shot recognition like command or query. 
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync();
                // </recognize_intent>

                // <print_results>
                // Checks result.
                switch (result.Reason)
                {
                    case ResultReason.RecognizedIntent:
                        Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                        Console.WriteLine($"    Intent Id: {result.IntentId}.");
                        var json = result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
                        Console.WriteLine($"    Language Understanding JSON: {json}.");
                        break;
                    case ResultReason.RecognizedSpeech:
                        Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                        Console.WriteLine($"    Intent not recognized.");
                        break;
                    case ResultReason.NoMatch:
                        Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        break;
                    case ResultReason.Canceled:
                        var cancellation = CancellationDetails.FromResult(result);
                        Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                        if (cancellation.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }
                        break;
                }
                // </print_results>
            // <create_intent_recognizer_2>
            }
            // </create_intent_recognizer_2>
        // <skeleton_2>
        }

        static async Task Main()
        {
            await RecognizeIntentAsync();
            Console.WriteLine("Please press <Return> to continue.");
            Console.ReadLine();
        }
    }
}
// </skeleton_2>

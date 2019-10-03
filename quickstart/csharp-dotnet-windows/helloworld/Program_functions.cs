//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <Generic_Header>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace helloworld
{
    class Program
    {
// </Generic_Header>

// <Main>
        static void Main()
        {
            var config = CreateSpeechConfig("YourSubscriptionKey", "YourServiceRegion");
            using(var recognizer = CreateSpeechRecognizer(config))
            {
                var result = RecnogizeSpeech(recognizer);
                DisplayResult(result);
            }
            Console.WriteLine("Please press <Return> to continue.");
            Console.ReadLine();
        }
// </Main>

// <Config_Setup>
        public static SpeechConfig CreateSpeechConfig(string key, string region)
        {
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");
            return config;
        }
// </Config_Setup>

// <Create_Recognizer>
        public static SpeechRecognizer CreateSpeechRecognizer(SpeechConfig config)
        {
            return new SpeechRecognizer(config);
        }
// </Create_Recognizer>

// <Recognize_Speech>
        public static async SpeechRecognitionResult RecognizeSpeech(SpeechRecognizer recognizer)
        {
            var result = await recognizer.RecognizeOnceAsync();
            return result;
        }
// </Recognize_Speech>

// <Results>
        public static DisplayResult(SpeechRecognitionResult result)
        {
            if (result.Reason == ResultReason.RecognizedSpeech)
            {
                Console.WriteLine($"We recognized: {result.Text}");
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
// </Results>
    }
}
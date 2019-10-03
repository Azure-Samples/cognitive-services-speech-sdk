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
        static void Main()
        {
            QuickstartAsync().Wait();
            Console.WriteLine("Please press <Return> to continue.");
            Console.ReadLine();
        }

        public static async Task QuickstartAsync()
        {
// </Generic_Header>
// <Config_Setup>
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            using (var recognizer = new SpeechRecognizer(config))
            {
// </Config_Setup>
// <Recognize_Once>
                var result = await recognizer.RecognizeOnceAsync();
// </Recognize_Once>
// <Results>
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
// </Results>
            }
        }
    }
}

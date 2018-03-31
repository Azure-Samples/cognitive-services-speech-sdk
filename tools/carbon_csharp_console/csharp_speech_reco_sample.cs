//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Threading.Tasks;
using Carbon;
using Carbon.Recognition;
using Carbon.Recognition.Speech;

namespace CarbonSamples
{
    public class SpeechRecognitionSamples
    {
        private static void MyIntermediateResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format("Speech recogniton: intermediate result: {0} ", e.ToString()));
        }

        private static void MyFinalResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format("Speech recogniton:  final result: {0} ", e.ToString()));
        }

        private static void MyErrorEventHandler(object sender, RecognitionErrorEventArgs e)
        {
            Console.WriteLine(String.Format("Speech recogniton: error occured. SessionId: {0}, Reason: {1}", e.SessionId, e.Status));
        }

        private static void MySessionEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format("Speech recogniton: Session event: {0}.", e.ToString()));
        }

        public static async Task SpeechRecognitionAsync(string[] args)
        {
            var keySpeech = args[0];

            var factory = new RecognizerFactory();
            factory.SubscriptionKey = keySpeech;

            SpeechRecognizer reco;
            if (string.Compare(args[1], "mic", true) == 0)
            {
                reco = factory.CreateSpeechRecognizer();
            }
            else
            {
                reco = factory.CreateSpeechRecognizer(args[1]);
            }

            // Subscribes to events.
            reco.OnIntermediateResult += MyIntermediateResultEventHandler;
            reco.OnFinalResult += MyFinalResultEventHandler;
            reco.OnRecognitionError += MyErrorEventHandler;
            reco.OnSessionEvent += MySessionEventHandler;

            // Starts recognition.
            var result = await reco.RecognizeAsync();

            Console.WriteLine("Speech Recognition: Recognition result: " + result);

            // Unsubscribe to events.
            reco.OnIntermediateResult -= MyIntermediateResultEventHandler;
            reco.OnFinalResult -= MyFinalResultEventHandler;
            reco.OnRecognitionError -= MyErrorEventHandler;
            reco.OnSessionEvent -= MySessionEventHandler;
        }
    }

}

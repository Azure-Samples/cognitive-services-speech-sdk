//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Recognition;
using Microsoft.CognitiveServices.Speech.Recognition.Intent;

namespace MicrosoftSpeechSDKSamples
{
    public class IntentRecognitionSamples
    {
        private static void MyIntermediateResultEventHandler(object sender, IntentRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format("Intent recognition: intermediate rsult: {0} ", e.ToString()));
        }

        private static void MyFinalResultEventHandler(object sender, IntentRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format("Intent recognition: final result: {0} ", e.ToString()));
        }

        private static void MyErrorEventHandler(object sender, RecognitionErrorEventArgs e)
        {
            Console.WriteLine(String.Format("Intent recognition: error occurred. SessionId: {0}, Reason: {1}", e.SessionId, e.Status));
        }

        private static void MySessionEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format("Intent recognition: Session event: {0}.", e.ToString()));
        }

        public static async Task IntentRecognitionBaseModelAsync(RecognizerFactory factory, string fileName)
        {
            Console.WriteLine("Intent Recognition using base speech model.");
            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = factory.CreateIntentRecognizer())
                {
                    await DoIntentRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                using (var reco = factory.CreateIntentRecognizerWithFileInput(fileName))
                {
                    await DoIntentRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
        }

        public static async Task IntentRecognitionByEndpointAsync(RecognizerFactory factory, string endpoint, string fileName)
        {
            Console.WriteLine(String.Format("Intent Recognition using endpoint:{0}.", endpoint));

            factory.EndpointURL = new Uri(endpoint);

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = factory.CreateIntentRecognizer())
                {
                    await DoIntentRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                using (var reco = factory.CreateIntentRecognizerWithFileInput(fileName))
                {
                    await DoIntentRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
        }

        public static async Task DoIntentRecognitionAsync(IntentRecognizer reco)
        {
            // Subscribes to events.
            reco.IntermediateResultReceived += MyIntermediateResultEventHandler;
            reco.FinalResultReceived += MyFinalResultEventHandler;
            reco.RecognitionErrorRaised += MyErrorEventHandler;
            reco.OnSessionEvent += MySessionEventHandler;

            // Todo: Add LUIS intent.
            reco.AddIntent("WeatherIntent", "weather");

            // Starts recognition.
            var result = await reco.RecognizeAsync().ConfigureAwait(false);

            Console.WriteLine("Intent Recognition: recognition result: " + result);

            // Unsubscribe to events.
            reco.IntermediateResultReceived -= MyIntermediateResultEventHandler;
            reco.FinalResultReceived -= MyFinalResultEventHandler;
            reco.RecognitionErrorRaised -= MyErrorEventHandler;
            reco.OnSessionEvent -= MySessionEventHandler;
        }

    }

}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Globalization;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;

namespace MicrosoftSpeechSDKSamples
{
    public class IntentRecognitionSamples
    {
        private static void MyIntermediateResultEventHandler(object sender, IntentRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent recognition: intermediate rsult: {0} ", e.ToString()));
        }

        private static void MyFinalResultEventHandler(object sender, IntentRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent recognition: final result: {0} ", e.ToString()));
        }

        private static void MyErrorEventHandler(object sender, RecognitionErrorEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent recognition: error occurred. SessionId: {0}, Reason: {1}", e.SessionId, e.Status));
        }

        private static void MySessionEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent recognition: Session event: {0}.", e.ToString()));
        }

        public static async Task IntentRecognitionBaseModelAsync(string keySpeech, string fileName)
        {
            Console.WriteLine("Intent Recognition using base speech model.");

            var factory = SpeechFactory.FromSubscription(keySpeech, "europewest");

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = factory.CreateIntentRecognizer())
                {
                    await DoIntentRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                var audioInput = AudioConfig.FromWavFileInput(fileName);
                using (var reco = factory.CreateIntentRecognizerFromConfig(audioInput))
                {
                    await DoIntentRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
        }

        //public static async Task IntentRecognitionCustomizedModelAsync(string keySpeech, string modelId, string fileName)
        //{
        //    var factory = SpeechFactory.Instance;

        //    Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent Recognition using customized speech model:{0}.", modelId));

        //    factory.SubscriptionKey = keySpeech;
        //    factory.ModelId = modelId;

        //    await DoIntentRecognitionAsync(factory, fileName).ConfigureAwait(false);
        //}

        public static async Task IntentRecognitionByEndpointAsync(string subKey, string endpoint, string fileName)
        {
            var factory = SpeechFactory.FromEndPoint(new Uri(endpoint), subKey);

            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent Recognition using endpoint:{0}.", endpoint));

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = factory.CreateIntentRecognizer())
                {
                    await DoIntentRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                var audioInput = AudioConfig.FromWavFileInput(fileName);
                using (var reco = factory.CreateIntentRecognizerFromConfig(audioInput))
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

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

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionSamples
    {
        private static void MyIntermediateResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture,"Speech recognition: intermediate result: {0} ", e.ToString()));
        }

        private static void MyFinalResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture,"Speech recognition: final result: {0} ", e.ToString()));
        }

        private static void MyErrorEventHandler(object sender, RecognitionErrorEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture,"Speech recognition: error occurred. SessionId: {0}, Reason: {1}", e.SessionId, e.Status));
        }

        private static void MySessionEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture,"Speech recognition: Session event: {0}.", e.ToString()));
        }

        public static async Task SpeechRecognitionBaseModelAsync(string keySpeech, string fileName)
        {
            var factory = SpeechFactory.FromSubscription(keySpeech, "");

            Console.WriteLine("Speech Recognition using base model.");
            if (string.IsNullOrEmpty(fileName) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = factory.CreateSpeechRecognizer())
                {
                    await DoSpeechRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                using (var reco = factory.CreateSpeechRecognizerWithFileInput(fileName))
                {
                    await DoSpeechRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
        }

        //public static async Task SpeechRecognitionCustomizedModelAsync(string keySpeech, string modelId, string fileName)
        //{
        //    var factory = SpeechFactory.Instance;

        //    Console.WriteLine(String.Format("Speech Recognition using customized model:{0}.", modelId));

        //    factory.SubscriptionKey = keySpeech;

        //    await DoSpeechRecognitionAsync(factory, fileName).ConfigureAwait(false);
        //}

        public static async Task SpeechRecognitionByEndpointAsync(string subscriptionKey, string endpoint, string fileName)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture,"Speech Recognition using endopoint:{0}.", endpoint));
            SpeechFactory factory = SpeechFactory.FromEndPoint(new Uri(endpoint), subscriptionKey);

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = factory.CreateSpeechRecognizer())
                {
                    await DoSpeechRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                using (var reco = factory.CreateSpeechRecognizerWithFileInput(fileName))
                {
                    await DoSpeechRecognitionAsync(reco).ConfigureAwait(false);
                }
            }
        }

        public static async Task DoSpeechRecognitionAsync(SpeechRecognizer reco)
        {
            // Subscribes to events.
            reco.IntermediateResultReceived += MyIntermediateResultEventHandler;
            reco.FinalResultReceived += MyFinalResultEventHandler;
            reco.RecognitionErrorRaised += MyErrorEventHandler;
            reco.OnSessionEvent += MySessionEventHandler;

            // Starts recognition.
            var result = await reco.RecognizeAsync().ConfigureAwait(false);

            Console.WriteLine("Speech Recognition: Recognition result: " + result);

            // Unsubscribe to events.
            reco.IntermediateResultReceived -= MyIntermediateResultEventHandler;
            reco.FinalResultReceived -= MyFinalResultEventHandler;
            reco.RecognitionErrorRaised -= MyErrorEventHandler;
            reco.OnSessionEvent -= MySessionEventHandler;
        }
    }
}

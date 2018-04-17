//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Recognition;
using Microsoft.CognitiveServices.Speech.Recognition.Translation;

namespace CarbonSamples
{
    public class TranslationSamples
    {
        private static void MyIntermediateResultEventHandler(object sender, TranslationTextResultEventArgs e)
        {
            Console.WriteLine(String.Format("Translation: intermediate result: {0} ", e.ToString()));
        }

        private static void MyFinalResultEventHandler(object sender, TranslationTextResultEventArgs e)
        {
            Console.WriteLine(String.Format("Translation:  final result: session id: {0} text: {1}", e.SessionId, e.Result.RecognizedText));
        }

        private static void MyErrorEventHandler(object sender, RecognitionErrorEventArgs e)
        {
            Console.WriteLine(String.Format("Translation: error occured. SessionId: {0}, Reason: {1}", e.SessionId, e.Status));
        }

        private static void MySessionEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format("Translation: Session event: {0}.", e.ToString()));
        }

        public static async Task TranslationBaseModelAsync(string keyTranslation, string fileName)
        {
            var factory = new RecognizerFactory();

            Console.WriteLine("Translation using base model.");

            factory.SubscriptionKey = keyTranslation;

            await DoTranslationAsync(factory, fileName).ConfigureAwait(false); ;
        }

        //public static async Task TranslationCustomizedModelAsync(string keyTranslation string modelId, string fileName)
        //{
        //    var factory = new RecognizerFactory();

        //    Console.WriteLine(String.Format("Translation using customized model:{0}.", modelId));

        //    factory.SubscriptionKey = keyTranslation;

        //    await DoTranslationAsync(factory, fileName).ConfigureAwait(false);
        //}

        public static async Task TranslationByEndpointAsync(string keyTranslation, string endpoint, string fileName)
        {
            var factory = new RecognizerFactory();

            Console.WriteLine(String.Format("Translation using endopoint:{0}.", endpoint));

            factory.SubscriptionKey = keyTranslation;
            factory.Endpoint = endpoint;

            await DoTranslationAsync(factory, fileName).ConfigureAwait(false);
        }

        public static async Task DoTranslationAsync(RecognizerFactory factory, string fileName)
        {
            TranslationRecognizer reco;
            if (fileName == null)
            {
                reco = factory.CreateTranslationRecognizer("en-us", new List<string>() { "de-de" });
            }
            else
            {
                reco = factory.CreateTranslationRecognizer(fileName, "en-us", new List<string>() { "de-de" });
            }

            // Subscribes to events.
            reco.IntermediateResultReceived += MyIntermediateResultEventHandler;
            reco.FinalResultReceived += MyFinalResultEventHandler;
            reco.RecognitionErrorRaised += MyErrorEventHandler;
            reco.OnSessionEvent += MySessionEventHandler;

            // Starts recognition.
            var result = await reco.RecognizeAsync().ConfigureAwait(false);

            Console.WriteLine("Translation Recognition: Recognition result: " + result);

            // Unsubscribe to events.
            reco.IntermediateResultReceived -= MyIntermediateResultEventHandler;
            reco.FinalResultReceived -= MyFinalResultEventHandler;
            reco.RecognitionErrorRaised -= MyErrorEventHandler;
            reco.OnSessionEvent -= MySessionEventHandler;
        }
    }
}

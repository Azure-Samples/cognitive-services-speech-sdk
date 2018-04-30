//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Globalization;
using System.Collections.Generic;
using System.IO;
using System.Media;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Translation;

namespace MicrosoftSpeechSDKSamples
{
    public class TranslationSamples
    {
        private static void MyIntermediateResultEventHandler(object sender, TranslationTextResultEventArgs e)
        {
            Console.WriteLine($"Translation: intermediate result: {e.ToString()}.");
        }

        private static void MyFinalResultEventHandler(object sender, TranslationTextResultEventArgs e)
        {
            Console.WriteLine($"Translation: final result: {e.ToString()}.");
        }

        private static void MySynthesisEventHandler(object sender, TranslationSynthesisResultEventArgs e)
        {
            Console.WriteLine($"Translation: synthesis result: {e.ToString()}.");
            using (var m = new MemoryStream(e.Result.Audio))
            {
                SoundPlayer simpleSound = new SoundPlayer(m);
                simpleSound.PlaySync();
            }
        }

        private static void MyErrorEventHandler(object sender, RecognitionErrorEventArgs e)
        {
            Console.WriteLine(String.Format("Translation: error occurred. SessionId: {0}, Reason: {1}", e.SessionId, e.Status));
        }

        private static void MySpeechEndDetectedHandler(object sender, RecognitionEventArgs e)
        {
            Console.WriteLine(String.Format("Translation: Session event: {0}.", e.ToString()));
            if (e.EventType == RecognitionEventType.SpeechEndDetectedEvent)
            {
                translationEndTaskCompletionSource.TrySetResult(0);
            }
        }

        public static async Task TranslationBaseModelAsync(string keyTranslation, string fileName)
        {
            var factory = SpeechFactory.FromSubscription(keyTranslation, "");
            Console.WriteLine("Translation using base model.");


            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                Console.WriteLine($"Translation into languages: {To2Langs[0]}, and {To2Langs[1]}:");
                using (var reco = factory.CreateTranslationRecognizer(FromLang, To2Langs))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }

                Console.WriteLine($"Translation into {ToChinese} with voice {ChineseVoice}");
                using (var reco = factory.CreateTranslationRecognizer(FromLang, ToChinese, ChineseVoice))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                Console.WriteLine($"Translation into languages: {To2Langs[0]}, and {To2Langs[1]}:");
                using (var reco = factory.CreateTranslationRecognizerWithFileInput(fileName, FromLang, To2Langs))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }

                Console.WriteLine($"Translation into {ToChinese} with voice {ChineseVoice}");
                using (var reco = factory.CreateTranslationRecognizerWithFileInput(fileName, FromLang, ToChinese, ChineseVoice))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }

                Console.WriteLine($"Translation into {ToGerman} with voice {GermanVoice}");
                using (var reco = factory.CreateTranslationRecognizerWithFileInput(fileName, FromLang, ToGerman, GermanVoice))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }
            }
        }

        public static async Task TranslationByEndpointAsync(string subKey, string endpoint, string fileName)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture,"Translation using endopoint:{0}.", endpoint));

            SpeechFactory factory = SpeechFactory.FromEndPoint(new Uri(endpoint), subKey);

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                // The language setting does not have any effect if the endpoint is specified.
                using (var reco = factory.CreateTranslationRecognizer(FromLang, To2Langs))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                // The language setting does not have any effect if the endpoint is specified.
                using (var reco = factory.CreateTranslationRecognizerWithFileInput(fileName, FromLang, To2Langs))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }
            }
        }

        public static async Task DoTranslationAsync(TranslationRecognizer reco)
        {
            // Subscribes to events.
            reco.IntermediateResultReceived += MyIntermediateResultEventHandler;
            reco.FinalResultReceived += MyFinalResultEventHandler;
            reco.SynthesisResultReceived += MySynthesisEventHandler;
            reco.RecognitionErrorRaised += MyErrorEventHandler;
            reco.OnSpeechDetectectedEvent += MySpeechEndDetectedHandler;
            reco.Parameters.Set(ParameterNames.SpeechModelId, "d4501bd5-a593-45bf-82a6-36ffc59d80a5");

            translationEndTaskCompletionSource = new TaskCompletionSource<int>();

            // Starts translation.
            await reco.StartContinuousRecognitionAsync().ConfigureAwait(false);

            // Waits for completion.
            await translationEndTaskCompletionSource.Task.ConfigureAwait(false);

            // Stops translation.
            await reco.StopContinuousRecognitionAsync().ConfigureAwait(false);
        }

        private static TaskCompletionSource<int> translationEndTaskCompletionSource;

        private static string FromLang = "en-us";
        private static List<string> To2Langs = new List<string>(){ "de-DE", "zh-CN" };
        private static List<string> ToGerman = new List<string>() { "de-DE" };
        private static string GermanVoice = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";
        private static List<string> ToChinese = new List<string>() { "zh-CN" };
        private static string ChineseVoice = "Microsoft Server Speech Text to Speech Voice (zh-CN, Yaoyao, Apollo)";
    }
}

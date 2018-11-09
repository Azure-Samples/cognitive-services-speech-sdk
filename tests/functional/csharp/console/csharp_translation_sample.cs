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
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;

namespace MicrosoftSpeechSDKSamples
{
    public class TranslationSamples
    {
        private static void MyRecognizingEventHandler(object sender, TranslationRecognitionEventArgs e)
        {
            Console.WriteLine($"Translation: intermediate result: {e.ToString()}.");
        }

        private static void MyRecognizedEventHandler(object sender, TranslationRecognitionEventArgs e)
        {
            Console.WriteLine($"Translation: final result: {e.ToString()}.");
        }

        private static void MySynthesizingEventHandler(object sender, TranslationSynthesisEventArgs e)
        {
            Console.WriteLine($"Translation: synthesis result: {e.ToString()}.");
            if (e.Result.Reason != ResultReason.SynthesizingAudioCompleted)
            {
                using (var m = new MemoryStream(e.Result.GetAudio()))
                {
                    SoundPlayer simpleSound = new SoundPlayer(m);
                    simpleSound.PlaySync();
                }
            }
        }

        private static void MyCanceledEventHandler(object sender, TranslationRecognitionCanceledEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Translation: canceled. SessionId: {0}, Reason: {1}", e.SessionId, e.Reason));
        }

        private static void MySessionStoppedHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Translation: Session stopped event: {0}.", e.ToString()));
            translationEndTaskCompletionSource.TrySetResult(0);
        }

        public static async Task TranslationBaseModelAsync(string keyTranslation, string region, string fileName, bool useStream)
        {
            var config = SpeechTranslationConfig.FromSubscription(keyTranslation, region);
            config.SpeechRecognitionLanguage = FromLang;
            To2Langs.ForEach(l => config.AddTargetLanguage(l));

            var chineseConfig = SpeechTranslationConfig.FromSubscription(keyTranslation, region);
            chineseConfig.SpeechRecognitionLanguage = FromLang;
            chineseConfig.VoiceName = ChineseVoice;
            chineseConfig.AddTargetLanguage(ChineseLocale);

            var germanConfig = SpeechTranslationConfig.FromSubscription(keyTranslation, region);
            germanConfig.SpeechRecognitionLanguage = FromLang;
            germanConfig.VoiceName = GermanVoice;
            germanConfig.AddTargetLanguage(GermanLocale);

            Console.WriteLine("Translation using base model.");
            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                Console.WriteLine($"Translation into languages: {To2Langs[0]}, and {To2Langs[1]}:");
                using (var reco = new TranslationRecognizer(config))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }

                Console.WriteLine($"Translation into {ChineseLocale} with voice {ChineseVoice}");
                using (var reco = new TranslationRecognizer(chineseConfig))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                Console.WriteLine($"Translation into languages: {To2Langs[0]}, and {To2Langs[1]}:");
                if (useStream)
                {
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco).ConfigureAwait(false);
                    }
                }
                else
                {
                    var audioInput = AudioConfig.FromWavFileInput(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco).ConfigureAwait(false);
                    }

                    Console.WriteLine($"Translation into {ChineseLocale} with voice {ChineseVoice}");
                    using (var reco = new TranslationRecognizer(chineseConfig, audioInput))
                    {
                        await DoTranslationAsync(reco).ConfigureAwait(false);
                    }

                    Console.WriteLine($"Translation into {GermanLocale} with voice {GermanVoice}");
                    using (var reco = new TranslationRecognizer(germanConfig, audioInput))
                    {
                        await DoTranslationAsync(reco).ConfigureAwait(false);
                    }
                }
            }
        }

        public static async Task TranslationByEndpointAsync(string subKey, string endpoint, string fileName, bool useStream)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Translation using endpoint:{0}.", endpoint));

            SpeechTranslationConfig config = SpeechTranslationConfig.FromEndpoint(new Uri(endpoint), subKey);
            config.SpeechRecognitionLanguage = FromLang;

            To2Langs.ForEach(l => config.AddTargetLanguage(l));

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                // The language setting does not have any effect if the endpoint is specified.
                using (var reco = new TranslationRecognizer(config))
                {
                    await DoTranslationAsync(reco).ConfigureAwait(false);
                }
            }
            else
            {
                if (useStream)
                {
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco).ConfigureAwait(false);
                    }
                }
                else
                {
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco).ConfigureAwait(false);
                    }
                }
            }
        }

        public static async Task DoTranslationAsync(TranslationRecognizer reco)
        {
            // Subscribes to events.
            reco.Recognizing += MyRecognizingEventHandler;
            reco.Recognized += MyRecognizedEventHandler;
            reco.Synthesizing += MySynthesizingEventHandler;
            reco.Canceled += MyCanceledEventHandler;
            reco.SessionStopped += MySessionStoppedHandler;

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
        private static List<string> To2Langs = new List<string>() { "de-DE", "zh-CN" };
        private static string GermanLocale = "de-DE";
        private static string GermanVoice = "Microsoft Server Speech Text to Speech Voice (de-DE, HeddaRUS)";
        private static string ChineseLocale = "zh-CN";
        private static string ChineseVoice = "Microsoft Server Speech Text to Speech Voice (zh-CN, Yaoyao, Apollo)";
    }
}

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
            if (e.Result.Reason == ResultReason.TranslatedSpeech)
            {
                Console.WriteLine($"Translated: {e.ToString()}");
            }
            else if (e.Result.Reason == ResultReason.RecognizedSpeech)
            {
                Console.WriteLine($"ONLY Speech recognized : {e.ToString()}");
            }
            else if (e.Result.Reason == ResultReason.NoMatch)
            {
                Console.WriteLine($"NOMATCH: Speech could not be recognized. Reason={NoMatchDetails.FromResult(e.Result).Reason}, Offset={e.Result.OffsetInTicks}, Duration={e.Result.Duration}");
            }
            else
            {
                Console.WriteLine($"Unexpected result. {e.ToString()}");
            }
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
            Console.WriteLine($"CANCELED: Reason={e.Reason}");

            if (e.Reason == CancellationReason.Error)
            {
                Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
            }
        }
        private static void MyConnectedEventHandler(object sender, ConnectionEventArgs e)
        {
            Console.WriteLine($"Connected event: {e.ToString()}.");
        }
        private static void MyDisconnectedEventHandler(object sender, ConnectionEventArgs e)
        {
            Console.WriteLine($"Disconnected event: {e.ToString()}.");
        }

        private static void MySessionStoppedEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine($"Session stopped event: {e.ToString()}.");
        }

        public static async Task TranslationBaseModelAsync(string keyTranslation, string region, string fileName, bool useStream, bool useContinuousRecognition, string deviceName = null)
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
                AudioConfig audioConfig;
                if (string.IsNullOrEmpty(deviceName))
                {
                    audioConfig = AudioConfig.FromDefaultMicrophoneInput();
                }
                else
                {
                    audioConfig = AudioConfig.FromMicrophoneInput(deviceName);
                }
                Console.WriteLine($"Translation into languages: {To2Langs[0]}, and {To2Langs[1]}:");
                using (var reco = new TranslationRecognizer(config, audioConfig))
                {
                    await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                }

                Console.WriteLine($"Translation into {ChineseLocale} with voice {ChineseVoice}");
                using (var reco = new TranslationRecognizer(chineseConfig))
                {
                    await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
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
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }
                }
                else
                {
                    var audioInput = AudioConfig.FromWavFileInput(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }

                    Console.WriteLine($"Translation into {ChineseLocale} with voice {ChineseVoice}");
                    using (var reco = new TranslationRecognizer(chineseConfig, audioInput))
                    {
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }

                    Console.WriteLine($"Translation into {GermanLocale} with voice {GermanVoice}");
                    using (var reco = new TranslationRecognizer(germanConfig, audioInput))
                    {
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }
                }
            }
        }

        public static async Task TranslationByEndpointAsync(string subKey, string endpoint, string fileName, bool useStream, bool useContinuousRecognition, string deviceName = null)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Translation using endpoint:{0}.", endpoint));

            SpeechTranslationConfig config = SpeechTranslationConfig.FromEndpoint(new Uri(endpoint), subKey);
            config.SpeechRecognitionLanguage = FromLang;

            To2Langs.ForEach(l => config.AddTargetLanguage(l));

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                AudioConfig audioConfig;
                if (string.IsNullOrEmpty(deviceName))
                {
                    audioConfig = AudioConfig.FromDefaultMicrophoneInput();
                }
                else
                {
                    audioConfig = AudioConfig.FromMicrophoneInput(deviceName);
                }
                // The language setting does not have any effect if the endpoint is specified.
                using (var reco = new TranslationRecognizer(config, audioConfig))
                {
                    await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                }
            }
            else
            {
                if (useStream)
                {
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }
                }
                else
                {
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }
                }
            }
        }

        public static async Task TranslationCustomizedModelAsync(string subKey, string modelId, string region, string fileName, bool useStream, bool useContinuousRecognition, string deviceName = null)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Translation using customized model: {0}.", modelId));

            var config = SpeechTranslationConfig.FromSubscription(subKey, region);
            config.SpeechRecognitionLanguage = FromLang;

            To2Langs.ForEach(l => config.AddTargetLanguage(l));
            config.EndpointId = modelId;

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                AudioConfig audioConfig;
                if (string.IsNullOrEmpty(deviceName))
                {
                    audioConfig = AudioConfig.FromDefaultMicrophoneInput();
                }
                else
                {
                    audioConfig = AudioConfig.FromMicrophoneInput(deviceName);
                }
                using (var reco = new TranslationRecognizer(config, audioConfig))
                {
                    await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                }
            }
            else
            {
                if (useStream)
                {
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }
                }
                else
                {
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new TranslationRecognizer(config, audioInput))
                    {
                        await DoTranslationAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                    }
                }
            }
        }

        private static async Task DoTranslationAsync(TranslationRecognizer reco, bool useContinuousRecognition)
        {
            if (useContinuousRecognition)
            {
                await ContinuousTranslationAsync(reco);
            }
            else
            {
                await SingleShotTranslationAsync(reco);
            }
        }

        private static async Task SingleShotTranslationAsync(TranslationRecognizer reco)
        {
            Console.WriteLine("Single-shot translation.");

            var connection = Connection.FromRecognizer(reco);
            // Subscribes to events.
            connection.Connected += MyConnectedEventHandler;
            connection.Disconnected += MyDisconnectedEventHandler;
            reco.Recognizing += MyRecognizingEventHandler;
            reco.Recognized += MyRecognizedEventHandler;
            reco.Canceled += MyCanceledEventHandler;
            reco.SessionStopped += MySessionStoppedEventHandler;

            // Starts recognition.
            var result = await reco.RecognizeOnceAsync().ConfigureAwait(false);

            Console.WriteLine("Translation result: " + result);

            // Unsubscribe to events.
            connection.Connected -= MyConnectedEventHandler;
            connection.Disconnected -= MyDisconnectedEventHandler;
            reco.Recognizing -= MyRecognizingEventHandler;
            reco.Recognized -= MyRecognizedEventHandler;
            reco.Canceled -= MyCanceledEventHandler;
            reco.SessionStopped -= MySessionStoppedEventHandler;
        }

        private static async Task ContinuousTranslationAsync(TranslationRecognizer reco)
        {
            var connection = Connection.FromRecognizer(reco);
            // Subscribes to events.
            connection.Connected += MyConnectedEventHandler;
            connection.Disconnected += MyDisconnectedEventHandler;
            reco.Recognizing += MyRecognizingEventHandler;
            reco.Recognized += MyRecognizedEventHandler;
            reco.Synthesizing += MySynthesizingEventHandler;
            reco.Canceled += MyCanceledEventHandler;
            reco.SessionStopped += (s, e) =>
            {
                MySessionStoppedEventHandler(s, e);
                translationEndTaskCompletionSource.TrySetResult(0);
            };

            translationEndTaskCompletionSource = new TaskCompletionSource<int>();

            // Starts translation.
            await reco.StartContinuousRecognitionAsync().ConfigureAwait(false);

            // Waits for completion.
            await translationEndTaskCompletionSource.Task.ConfigureAwait(false);

            // Stops translation.
            await reco.StopContinuousRecognitionAsync().ConfigureAwait(false);

            // Unsubscribe to events.
            connection.Connected -= MyConnectedEventHandler;
            connection.Disconnected -= MyDisconnectedEventHandler;
            reco.Recognizing -= MyRecognizingEventHandler;
            reco.Recognized -= MyRecognizedEventHandler;
            reco.Canceled -= MyCanceledEventHandler;
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

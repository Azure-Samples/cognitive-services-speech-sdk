//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;

namespace MicrosoftSpeechSDKSamples
{
    public class MemoryLeakTests
    {
        // List of speech-to-Text codes are copied from https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/language-support [7 dec 2018]
        public static readonly string[] Speech2TextAll =
        {
            "ar-EG", "ca-ES", "da-DK", "de-DE", "en-AU", "en-CA", "en-GB", "en-IN", "en-NZ", "en-US", "es-ES", "es-MX", "fi-FI", "fr-CA", "fr-FR",
            "hi-IN", "it-IT", "ja-JP", "ko-KR", "nb-NO", "nl-NL", "pl-PL", "pt-BR", "pt-PT", "ru-RU", "sv-SE", "zh-CN", "zh-HK", "zh-TW", "th-TH"
        };

        // List of text languages are copied from https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/language-support [7 dec 2018]
        public static readonly string[] TextLangAll =
        {
            "af", "ar", "bn", "bs", "bg", "yue", "ca", "zh-Hans", "zh-Hant", "hr", "cs", "da", "nl", "en", "et", "fj", "fil", "fi", "fr", "de", "el",
            "ht", "he", "hi", "mww", "hu", "id", "it", "ja", "sw", "tlh", "ko", "lv", "lt", "mg", "ms", "mt", "nb", "fa", "pl", "pt", "otq",
            "ro", "ru", "sm", "sr-Cyrl", "sr-Latn", "sk", "sl", "es", "sv", "ty", "ta", "th", "to", "tr", "uk", "ur", "vi", "cy", "yua"
        };

        private struct MemoryKPI
        {
            public long WorkingSetBytes;
            public long PrivateMemoryBytes;
            public long AllHeapsBytes;
            public long GCTotalMemoryBytes;
        }

        private static MemoryKPI baselineMemoryKPI;
        private static MemoryKPI previousMemoryKPI;
        private static MemoryKPI currentMemoryKPI;
        private static Process myProc;
        private static PerformanceCounter perfCounterForBytesInAllHeaps;

        private static List<string> outputMsgs = new List<string>(500);

        public static void UpdateMemoryUsuage(string header)
        {
            previousMemoryKPI = currentMemoryKPI;
            GC.Collect(GC.MaxGeneration);
            GC.WaitForPendingFinalizers();
            myProc.Refresh();
            currentMemoryKPI.GCTotalMemoryBytes = GC.GetTotalMemory(true);
            currentMemoryKPI.AllHeapsBytes = (long)perfCounterForBytesInAllHeaps.NextValue();
            currentMemoryKPI.PrivateMemoryBytes = myProc.PrivateMemorySize64;
            currentMemoryKPI.WorkingSetBytes = myProc.WorkingSet64;

            var msg = String.Format("{0}:\n    GCTotalMemory:{1}->{2}, diff:{3}(KB)\n    BytesInAllHeap:{4}->{5}, diff:{6}(KB)\n    PrivateMemroy:{7}->{8}, diff:{9}(KB)\n    WorkingSet:{10}->{11}, diff:{12}(KB)\n",
                         header,
                         previousMemoryKPI.GCTotalMemoryBytes, currentMemoryKPI.GCTotalMemoryBytes, (currentMemoryKPI.GCTotalMemoryBytes - previousMemoryKPI.GCTotalMemoryBytes) / 1000.0,
                         previousMemoryKPI.AllHeapsBytes, currentMemoryKPI.AllHeapsBytes, (currentMemoryKPI.AllHeapsBytes - previousMemoryKPI.AllHeapsBytes) / 1000.0,
                         previousMemoryKPI.PrivateMemoryBytes, currentMemoryKPI.PrivateMemoryBytes, (currentMemoryKPI.PrivateMemoryBytes - previousMemoryKPI.PrivateMemoryBytes) / 1000.0,
                         previousMemoryKPI.WorkingSetBytes, currentMemoryKPI.WorkingSetBytes, (currentMemoryKPI.WorkingSetBytes - previousMemoryKPI.WorkingSetBytes) / 1000.0);

            Console.Write("\n");
            Console.WriteLine(msg);
            outputMsgs.Add(msg);
        }

        public static TranslationRecognizer CreateTranslationRecognizer(string subkey, string region, string audioFile, string fromLanguage, List<string> toLanguages, string voice = null)
        {
            var audioInput = AudioConfig.FromWavFileInput(audioFile);
            var config = SpeechTranslationConfig.FromSubscription(subkey, region);
            config.SpeechRecognitionLanguage = fromLanguage;
            toLanguages.ForEach(l => config.AddTargetLanguage(l));
            if (!string.IsNullOrEmpty(voice))
            {
                config.VoiceName = voice;
            }
            return new TranslationRecognizer(config, audioInput);
        }

        public static async Task<TranslationRecognitionResult> TranslationMemoryLeakTestHelper(string subkey, string region, string audioFile, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = CreateTranslationRecognizer(subkey, region, audioFile, fromLanguage, toLanguages, voice: null))
            {
                var tcs = new TaskCompletionSource<int>();
                var recognizedEventArgs = new List<TranslationRecognitionEventArgs>();
                var sessionStartedEventArgs = new List<SessionEventArgs>();
                var recognizedResults = new List<RecognitionResult>();
                var connectedEventArgs = new List<ConnectionEventArgs>();
                recognizer.SessionStarted += (s, e) =>
                {
                    sessionStartedEventArgs.Add(e);
                };
                recognizer.Recognized += (s, e) =>
                {
                    recognizedEventArgs.Add(e);
                    recognizedResults.Add(e.Result);
                };
                recognizer.SessionStopped += (s, e) =>
                {
                    tcs.TrySetResult(0);
                };
                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.SessionId);
                    tcs.TrySetResult(0);
                };
                var connection = Connection.FromRecognizer(recognizer);
                int connectedEventCount = 0;
                int disconnectedEventCount = 0;
                connection.Connected += (s, e) =>
                {
                    connectedEventCount++;
                    connectedEventArgs.Add(e);
                };
                connection.Disconnected += (s, e) =>
                {
                    disconnectedEventCount++;
                };

                TranslationRecognitionResult result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromMinutes(6)));
                GC.KeepAlive(connection);
                return result;
            }
        }

        public static async Task TranslationMemoryLeakTest(string subscriptionKey, string region, string fileName)
        {
            int runCount = 0;

            foreach (var locale in Speech2TextAll)
            {
                foreach (var lang in TextLangAll)
                {
                    Console.Write(".");
                    runCount++;
                    if (runCount % 100 == 0)
                    {
                        UpdateMemoryUsuage($"Translation Test Count:{runCount}");
                    }

                    var result = await TranslationMemoryLeakTestHelper(subscriptionKey, region, fileName, locale, new List<string> { lang });
                }
            }
            UpdateMemoryUsuage($"Translation Test ends (Total tests: {runCount})");
        }

        public static SpeechRecognizer CreateSpeechRecognizer(string subkey, string region, string audioFile, string recoLanguage, bool useStream)
        {
            AudioConfig audioInput;
            if (useStream)
            {
                audioInput = Util.OpenWavFile(audioFile);
            }
            else
            {
                audioInput = AudioConfig.FromWavFileInput(audioFile);
            }
            var config = SpeechConfig.FromSubscription(subkey, region);
            config.SpeechRecognitionLanguage = recoLanguage;
            return new SpeechRecognizer(config, audioInput);
        }

        public static async Task<SpeechRecognitionResult> RunSpeechRecoMemoryLeakTest(SpeechRecognizer recognizer)
        {
            var tcs = new TaskCompletionSource<int>();
            var recognizedEventArgs = new List<SpeechRecognitionEventArgs>();
            var sessionStartedEventArgs = new List<SessionEventArgs>();
            var recognizedResults = new List<RecognitionResult>();
            var connectedEventArgs = new List<ConnectionEventArgs>();
            recognizer.SessionStarted += (s, e) =>
            {
                sessionStartedEventArgs.Add(e);
            };
            recognizer.Recognized += (s, e) =>
            {
                recognizedEventArgs.Add(e);
                recognizedResults.Add(e.Result);
            };
            recognizer.SessionStopped += (s, e) =>
            {
                tcs.TrySetResult(0);
            };
            recognizer.Canceled += (s, e) =>
            {
                Console.WriteLine("Canceled: " + e.SessionId);
                tcs.TrySetResult(0);
            };
            var connection = Connection.FromRecognizer(recognizer);
            int connectedEventCount = 0;
            int disconnectedEventCount = 0;
            connection.Connected += (s, e) =>
            {
                connectedEventCount++;
                connectedEventArgs.Add(e);
            };
            connection.Disconnected += (s, e) =>
            {
                disconnectedEventCount++;
            };

            SpeechRecognitionResult result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromMinutes(6)));
            GC.KeepAlive(connection);
            return result;
        }

        public static async Task<SpeechRecognitionResult> SpeechRecoDisposeMemoryLeakTestHelper(string subkey, string region, string audioFile, string recoLanguage, bool useStream)
        {
            using (var recognizer = CreateSpeechRecognizer(subkey, region, audioFile, recoLanguage, useStream))
            {
                var result = await RunSpeechRecoMemoryLeakTest(recognizer);
                return result;
            }
        }

        public static async Task<SpeechRecognitionResult> SpeechRecoFinalizerMemoryLeakTestHelper(string subkey, string region, string audioFile, string recoLanguage, bool useStream)
        {
            var recognizer = CreateSpeechRecognizer(subkey, region, audioFile, recoLanguage, useStream);
            var result = await RunSpeechRecoMemoryLeakTest(recognizer);
            return result;
        }

        public static async Task SpeechRecoMemoryLeakTest(string subscriptionKey, string region, string fileName, bool useDispose, bool useStream)
        {
            int runCount = 0;

            for (int i = 0; i < 50; i++)
            {
                foreach (var locale in Speech2TextAll)
                {
                    Console.Write(".");
                    runCount++;
                    if (runCount % 100 == 0)
                    {
                        UpdateMemoryUsuage($"SpeechReco: (useDispose={useDispose}, useStream={useStream}) Test Count:{runCount}");
                    }

                    RecognitionResult result;
                    if (useDispose)
                    {
                        result = await SpeechRecoDisposeMemoryLeakTestHelper(subscriptionKey, region, fileName, locale, useStream);
                    }
                    else
                    {
                        result = await SpeechRecoFinalizerMemoryLeakTestHelper(subscriptionKey, region, fileName, locale, useStream);
                    }
                }
            }
            UpdateMemoryUsuage($"SpeechReco: (useDispose={useDispose}, useStream={useStream}) Test ends (Total tests:{runCount})");
        }

        public static async Task MemoryLeakTest(string subscriptionKey, string region, string fileName, string testKind)
        {
            bool testSpeech = true;
            bool testTranslation = true;
            bool testFileInput = true;
            bool testStreamInput = true;
            string myInstanceName = null;

            outputMsgs.Clear();
            myProc = Process.GetCurrentProcess();
            var perfCat = new PerformanceCounterCategory(".NET CLR Memory");
            foreach (var instanceName in perfCat.GetInstanceNames())
            {
                try
                {
                    using (var pid = new PerformanceCounter(perfCat.CategoryName, "Process ID", instanceName))
                    {
                        if ((int)pid.NextValue() == myProc.Id)
                        {
                            myInstanceName = instanceName;
                            break;
                        }
                    }

                }
                catch (InvalidOperationException)
                {
                    Console.WriteLine("Failed to get instance name.");
                    return;
                }
            }

            perfCounterForBytesInAllHeaps = new PerformanceCounter(".NET CLR Memory", "# bytes in all heaps", myInstanceName, true);

            if (!string.IsNullOrEmpty(testKind))
            {
                if (testKind == "speech")
                {
                    testTranslation = false;
                }
                else if (testKind == "translation")
                {
                    testSpeech = false;
                }
                else if (testKind == "speech_file")
                {
                    testTranslation = false;
                    testStreamInput = false;
                }
                else if (testKind == "speech_stream")
                {
                    testTranslation = false;
                    testFileInput = false;
                }
                else
                {
                    Console.WriteLine("Allowed testKind: speech, translation, speech_file, speech_stream. Found invalid testkind: " + testKind);
                    return;
                }
            }

            // Run some tests to stablize memory usage.
            for (int i = 0; i < 5; i++)
            {
                if (testSpeech)
                {
                    if (testFileInput)
                    {
                        await SpeechRecoDisposeMemoryLeakTestHelper(subscriptionKey, region, fileName, "en-us", false);
                        await SpeechRecoFinalizerMemoryLeakTestHelper(subscriptionKey, region, fileName, "en-us", false);
                    }
                    if (testStreamInput)
                    {
                        await SpeechRecoDisposeMemoryLeakTestHelper(subscriptionKey, region, fileName, "en-us", true);
                        await SpeechRecoFinalizerMemoryLeakTestHelper(subscriptionKey, region, fileName, "en-us", true);
                    }
                }
                if (testTranslation)
                {
                    await TranslationMemoryLeakTestHelper(subscriptionKey, region, fileName, "en-us", new List<string> { "DE" });
                }
            }

            Console.WriteLine("Press any key to start.");
            Console.ReadKey();

            // Start memory measurement.
            UpdateMemoryUsuage(String.Format("Start memory tests: Testing {0} {1} {2} {3} {4}", testSpeech ? "Speech" : "", testTranslation ? "Translation" : "",
                testSpeech ? "using " : "", testSpeech && testFileInput ? "File" : "", testSpeech && testStreamInput ? "Stream" : ""));
            baselineMemoryKPI = currentMemoryKPI;

            if (testSpeech)
            {
                if (testFileInput)
                {
                    await SpeechRecoMemoryLeakTest(subscriptionKey, region, fileName, true /*dispose*/, false);
                    await SpeechRecoMemoryLeakTest(subscriptionKey, region, fileName, false /*no dispose*/, false);
                }
                if (testStreamInput)
                {
                    await SpeechRecoMemoryLeakTest(subscriptionKey, region, fileName, true /*dispose*/, true);
                    await SpeechRecoMemoryLeakTest(subscriptionKey, region, fileName, false /*no dispose*/, true);
                }
            }
            if (testTranslation)
            {
                await TranslationMemoryLeakTest(subscriptionKey, region, fileName);
            }

            UpdateMemoryUsuage($"Memory tests ends.");

            var msg = String.Format("{0}\n:    GCTotalMemory:{1}->{2}, diff:{3}(KB)\n    BytesInAllHeap:{4}->{5}, diff:{6}(KB)\n    PrivateMemroy:{7}->{8}, diff:{9}(KB)\n    WorkingSet:{10}->{11}, diff:{12}(KB)\n",
                         "Final memory usage compared to baseline",
                         baselineMemoryKPI.GCTotalMemoryBytes, currentMemoryKPI.GCTotalMemoryBytes, (currentMemoryKPI.GCTotalMemoryBytes - baselineMemoryKPI.GCTotalMemoryBytes) / 1000.0,
                         baselineMemoryKPI.AllHeapsBytes, currentMemoryKPI.AllHeapsBytes, (currentMemoryKPI.AllHeapsBytes - baselineMemoryKPI.AllHeapsBytes) / 1000.0,
                         baselineMemoryKPI.PrivateMemoryBytes, currentMemoryKPI.PrivateMemoryBytes, (currentMemoryKPI.PrivateMemoryBytes - baselineMemoryKPI.PrivateMemoryBytes) / 1000.0,
                         baselineMemoryKPI.WorkingSetBytes, currentMemoryKPI.WorkingSetBytes, (currentMemoryKPI.WorkingSetBytes - baselineMemoryKPI.WorkingSetBytes) / 1000.0);
            outputMsgs.Add(msg);
            Console.WriteLine(msg);

            var filename = $"memory-usage-{myProc.Id}-{DateTime.Now.ToString("MM-dd-hh-mm-ss")}.txt";
            System.IO.File.WriteAllLines(filename, outputMsgs);

            Console.WriteLine($"Write result ({outputMsgs.Count} entries) to {filename}");
        }
    }
}

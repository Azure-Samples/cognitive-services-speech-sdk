//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech;

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static AssertHelpers;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class SpeechRecognitionUnidecTests : RecognitionTestBase
    {
        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestMethod, TestCategory(TestCategory.OfflineUnidec)]
        public async Task OfflineUnidecRecognizeFromFileSingleShot()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.WeatherMultiTurns.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.offlineConfig, audioInput)))
            {
                var numUtterances = TestData.English.WeatherMultiTurns.Utterances.Length;

                for (var i = 0; i < numUtterances; i++)
                {
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                    Assert.IsTrue(TestData.English.WeatherMultiTurns.Utterances[i].IndexOf(result.Text, StringComparison.OrdinalIgnoreCase) >= 0,
                        $"Utterance {i}: Expected '{TestData.English.WeatherMultiTurns.Utterances[i]}', actual '{result.Text}'");
                }
            }
        }

        [TestMethod, TestCategory(TestCategory.OfflineUnidec)]
        public async Task OfflineUnidecRecognizeFromFileContinuous()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.WeatherMultiTurns.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.offlineConfig, audioInput)))
            {
                var numUtterances = TestData.English.WeatherMultiTurns.Utterances.Length;

                for (var i = 0; i < numUtterances; i++)
                {
                    string resultText = string.Empty;
                    var tcs = new TaskCompletionSource<int>();

                    if (i == 0)
                    {
                        recognizer.Recognized += (s, e) =>
                        {
                            if (e.Result.Reason == ResultReason.RecognizedSpeech)
                            {
                                resultText = e.Result.Text;
                            }
                            tcs.TrySetResult(0);
                        };
                        recognizer.Canceled += (s, e) =>
                        {
                            tcs.TrySetResult(0);
                        };
                        recognizer.SessionStopped += (s, e) =>
                        {
                            tcs.TrySetResult(0);
                        };
                    }

                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                    await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromSeconds(5)));
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                    Assert.IsTrue(TestData.English.WeatherMultiTurns.Utterances[i].IndexOf(resultText, StringComparison.OrdinalIgnoreCase) >= 0,
                        $"Utterance {i}: Expected '{TestData.English.WeatherMultiTurns.Utterances[i]}', actual '{resultText}'");
                }
            }
        }
        [TestMethod, TestCategory(TestCategory.OfflineUnidec)]
        public async Task OfflineUnidecKwsComputerFoundFromStream()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.offlineConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(TimeSpan.FromSeconds(15)));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod, TestCategory(TestCategory.OfflineUnidec)]
        public async Task OfflineUnidecKwsComputerFoundFromWavFile()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromWavFileInput(str);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.offlineConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(TimeSpan.FromSeconds(15)));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }
    }
}

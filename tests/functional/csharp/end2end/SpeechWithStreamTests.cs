//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;
using MicrosoftSpeechSDKSamples;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using System.Text.RegularExpressions;
    using Microsoft.CognitiveServices.Speech.Audio;
    using System.Threading;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public sealed class SpeechWithStreamTests : RecognitionTestBase
    {
        private SpeechWithStreamHelper speechHelper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestInitialize]
        public void TestInitalize()
        {
            speechHelper = new SpeechWithStreamHelper();
        }

        [TestMethod]
        public async Task SimpleRecognitionBatman()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(this.defaultConfig, TestData.English.Batman.AudioFile);
            AssertMatching(TestData.English.Batman.Utterances[0], result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedMP3()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, TestData.English.WeatherMP3.AudioFile, AudioStreamContainerFormat.MP3);
            AssertMatching(TestData.English.WeatherMP3.Utterance, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedOPUS()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, TestData.English.WeatherOPUS.AudioFile, AudioStreamContainerFormat.OGG_OPUS);
            AssertMatching(TestData.English.WeatherOPUS.Utterance, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedFLAC()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, TestData.English.WeatherFLAC.AudioFile, AudioStreamContainerFormat.FLAC);
            AssertMatching(TestData.English.WeatherFLAC.Utterance, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedALAW()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, TestData.English.WeatherALAW.AudioFile, AudioStreamContainerFormat.ALAW);
            AssertMatching(TestData.English.WeatherALAW.Utterance, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedMULAW()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, TestData.English.WeatherMULAW.AudioFile, AudioStreamContainerFormat.MULAW);
            AssertMatching(TestData.English.WeatherMULAW.Utterance, result.Text);
        }

        [TestMethod]
        public async Task DetailedRecognitionBatman()
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(this.defaultConfig, TestData.English.Batman.AudioFile);
            var detailedRecognitionText = result.Best().ToArray()[0].Text;
            var detailedRecognitionNormalizedForm = result.Best().ToArray()[0].NormalizedForm;
            var detailedRecognitionLexicalForm = result.Best().ToArray()[0].NormalizedForm;

            AssertMatching(TestData.English.Batman.Utterances[0], detailedRecognitionText);
            AssertMatching(TestData.English.Batman.Utterances[0], detailedRecognitionNormalizedForm);
            AssertMatching(TestData.English.Batman.Utterances[0], detailedRecognitionLexicalForm);
        }

        [TestMethod]
        public async Task SimpleRecognitionBatmanContinuous()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(this.defaultConfig, TestData.English.Batman.AudioFile);
            Assert.AreEqual(TestData.English.Batman.Utterances.Length, result.Count, "Unexpected number of utterances.");

            string[] resultUtterances = result.Select(r => r.Result.Text).ToArray();
            AssertFuzzyMatching(resultUtterances, TestData.English.Batman.Utterances, 5);
        }

        [TestMethod]
        public async Task DetailedRecognitionBatmanContinuous()
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(this.defaultConfig, TestData.English.Batman.AudioFile);
            Assert.AreNotEqual(result.Count, 0, "Bad count of recognized utterances");

            string[] bestResultUtterances = result.Select(r => r.Result.Best().ToArray()[0].Text).ToArray();
            string[] normalizedFormResultUtterances = result.Select(r => r.Result.Best().ToArray()[0].NormalizedForm).ToArray();
            string[] lexicalFormResultUtterances = result.Select(r => r.Result.Best().ToArray()[0].LexicalForm).ToArray();
            lexicalFormResultUtterances = lexicalFormResultUtterances.Select(s => Regex.Replace(s, " '", "", RegexOptions.Compiled)).ToArray();

            AssertFuzzyMatching(bestResultUtterances, TestData.English.Batman.Utterances, 3);
            AssertFuzzyMatching(normalizedFormResultUtterances, TestData.English.Batman.Utterances, 3);
            AssertFuzzyMatching(lexicalFormResultUtterances, TestData.English.Batman.Utterances, 3);

            Assert.AreEqual(TestData.English.Batman.Utterances.Length, result.Count, "Unexpected number of utterances");
            var actualRecognitionTextResults = result.Select(t => t.Result.Text).ToArray();
            for (var i = 0; i < result.Count; i++)
            {
                // allow 5 % of word error rate, or at least 2 errors (for short utterances)
                AssertStringWordEditPercentage(Normalize(TestData.English.Batman.Utterances[i]), Normalize(actualRecognitionTextResults[i]), 5, 2);
            }
        }

        //[Ignore]
        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        [TestMethod]
        public async Task InteractiveCheckFileOffsets(bool usingPreConnection)
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            var audioInput = Util.OpenWavFile(TestData.English.Batman.AudioFile);
            var results = new List<SpeechRecognitionResult>();
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(false);
                }
                while (true)
                {
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    if (result.Reason == ResultReason.Canceled)
                    {
                        break;
                    }
                    Console.WriteLine(result.Text);
                    Console.WriteLine($"Result OffsetInTicks {result.OffsetInTicks.ToString()}");
                    Console.WriteLine($"Result Duration {result.Duration.Ticks.ToString()}\n");
                    results.Add(result);
                }

                var texts = results.Select(r => r.Text).Where(t => !string.IsNullOrEmpty(t)).ToArray();
                AssertFuzzyMatching(TestData.English.Batman.UtterancesInteractive, texts, 5);
            }
        }

        [TestMethod]
        public async Task ContinuousCheckFileOffsets()
        {
            const int Times = 2;
            var audioInput = Util.OpenWavFile(TestData.English.Batman.AudioFile, Times);
            var results = new List<SpeechRecognitionResult>();
            var taskSource = new TaskCompletionSource<bool>();
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = new SpeechRecognizer(this.defaultConfig, audioInput))
            { 
                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Result recognized {e.ToString()}");
                    Console.WriteLine($"Result OffsetInTicks {e.Result.OffsetInTicks.ToString()}");
                    Console.WriteLine($"Result Duration {e.Result.Duration.Ticks.ToString()}\n");

                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        results.Add(e.Result);

                    }
                };

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine($"Received session started event {e.ToString()}");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Received session stopped event {e.ToString()}");
                    Console.WriteLine("Exiting due to session stop event");
                    taskSource.SetResult(true);
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine($"Received cancel event {e.ToString()}, exiting");
                };

                await recognizer.StartContinuousRecognitionAsync();
                // Make sure the task is rooted.
                await Task.WhenAny(taskSource.Task, Task.Delay(TimeSpan.FromMinutes(6)));
                await recognizer.StopContinuousRecognitionAsync();

                string[] resultUtterances = results.Select(r => r.Text).ToArray();
                AssertFuzzyMatching(resultUtterances, TestData.English.Batman.Utterances.Concat(TestData.English.Batman.Utterances).ToArray(), 2);

                // Checking durations.
                var offsets = results
                    .Where(r => !string.IsNullOrEmpty(r.Text))
                    .Select(r => new Tuple<long, long>(r.OffsetInTicks, r.Duration.Ticks))
                    .ToList();

                // Just make sure we received some offsets
                for (int i = 0; i < offsets.Count; i++)
                {
                    Assert.IsTrue(offsets[i].Item1 > 0, $"Offsets is 0, index {i}");
                    Assert.IsTrue(offsets[i].Item2 > 0, $"Duration is 0, index {i}");

                    // make sure the next offset increases at least by the current duration
                    if (i < offsets.Count-1) {
                        Assert.IsTrue(offsets[i+1].Item1 >= offsets[i].Item1 + offsets[i].Item2, 
                            $"Duration not increasing sufficient: index {i}, offset: {offsets[i].Item1}, duration: {offsets[i].Item2}, next offset: {offsets[i+1].Item1}");
                    }
                }
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ContinousRecognitionLongRunning()
        {
            const int Times = 10;
            var audioInput = Util.OpenWavFile(TestData.English.Batman.AudioFile, Times);
            var results = new List<SpeechRecognitionResult>();
            var taskSource = new TaskCompletionSource<bool>();
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = new SpeechRecognizer(this.defaultConfig, audioInput))
            {
                recognizer.Recognized += (s, e) =>
                {
                    // Console.WriteLine($"Result OffsetInTicks: {e.Result.OffsetInTicks.ToString()}; Duration: {e.Result.Duration.Ticks.ToString()}; Recognized {e.ToString()}");
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        results.Add(e.Result);
                    }
                };

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine($"Received session started event {e.ToString()}");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Received session stopped event {e.ToString()}");
                    Console.WriteLine("Exiting due to session stop event");
                    taskSource.SetResult(true);
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine($"Received cancel event {e.ToString()}, exiting");
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(taskSource.Task, Task.Delay(TimeSpan.FromMinutes(20)));
                Assert.AreEqual(TaskStatus.RanToCompletion, taskSource.Task.Status, "The timeout happened. SessionStopped event was not received in 20 minutes.");
                await recognizer.StopContinuousRecognitionAsync();

                // Checking text results.
                var texts = results.Select(r => r.Text).Where(t => !string.IsNullOrEmpty(t)).ToList();
                var expected = string.Join(" ", TestData.English.Batman.Utterances);
                for (int i = 1; i < Times; i++)
                {
                    expected += " ";
                    expected += string.Join(" ", TestData.English.Batman.Utterances);
                }
                expected = Normalize(expected);

                var actual = string.Join(" ", texts.ToArray());
                actual = Normalize(actual);
                // don't do a hard string comparison, we allow a small number of word edits (word insert/delete/move)
                AssertStringWordEditCount(expected, actual, 20);
            }
        }
    }
}

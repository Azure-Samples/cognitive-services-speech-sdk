//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using MicrosoftSpeechSDKSamples;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static Config;
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
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedMP3()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_MP3].FilePath.GetRootRelativePath(), AudioStreamContainerFormat.MP3);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_MP3].Utterances[Language.EN][0].Text, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedOPUS()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_OPUS].FilePath.GetRootRelativePath(), AudioStreamContainerFormat.OGG_OPUS);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_OPUS].Utterances[Language.EN][0].Text, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedFLAC()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_FLAC].FilePath.GetRootRelativePath(), AudioStreamContainerFormat.FLAC);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_FLAC].Utterances[Language.EN][0].Text, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedALAW()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_A_LAW].FilePath.GetRootRelativePath(), AudioStreamContainerFormat.ALAW);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_A_LAW].Utterances[Language.EN][0].Text, result.Text);
        }

        [TestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        public async Task SimpleRecognitionCompressedMULAW()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_MU_LAW].FilePath.GetRootRelativePath(), AudioStreamContainerFormat.MULAW);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_MU_LAW].Utterances[Language.EN][0].Text, result.Text);
        }

        [TestMethod]
        public async Task DetailedRecognitionBatman()
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var detailedRecognitionText = result.Best().ToArray()[0].Text;
            var detailedRecognitionNormalizedForm = result.Best().ToArray()[0].NormalizedForm;
            var detailedRecognitionLexicalForm = result.Best().ToArray()[0].NormalizedForm;

            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, detailedRecognitionText);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, detailedRecognitionNormalizedForm);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, detailedRecognitionLexicalForm);
        }

        [TestMethod]
        public async Task SimpleRecognitionBatmanContinuous()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            Assert.AreEqual(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Length, result.Count, "Unexpected number of nonempty utterances.");

            string[] resultUtterances = result.Select(r => r.Result.Text).ToArray();
            AssertFuzzyMatching(resultUtterances, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray(), 5);
        }

        [TestMethod]
        public async Task DetailedRecognitionBatmanContinuous()
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(this.defaultConfig, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            Assert.AreNotEqual(result.Count, 0, "Bad count of recognized utterances");

            string[] bestResultUtterances = result.Select(r => r.Result.Best().ToArray()[0].Text).ToArray();
            string[] normalizedFormResultUtterances = result.Select(r => r.Result.Best().ToArray()[0].NormalizedForm).ToArray();
            string[] lexicalFormResultUtterances = result.Select(r => r.Result.Best().ToArray()[0].LexicalForm).ToArray();
            lexicalFormResultUtterances = lexicalFormResultUtterances.Select(s => Regex.Replace(s, " '", "", RegexOptions.Compiled)).ToArray();

            AssertFuzzyMatching(bestResultUtterances, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray(), 3);
            AssertFuzzyMatching(normalizedFormResultUtterances, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray(), 3);
            AssertFuzzyMatching(lexicalFormResultUtterances, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray(), 3);

            Assert.AreEqual(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Length, result.Count, "Unexpected number of nonempty utterances");
            var actualRecognitionTextResults = result.Select(t => t.Result.Text).ToArray();
            for (var i = 0; i < result.Count; i++)
            {
                // allow 5 % of word error rate, or at least 2 errors (for short utterances)
                AssertStringWordEditPercentage(Normalize(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN][i].Text), Normalize(actualRecognitionTextResults[i]), 5, 2);
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
            var audioInput = Util.OpenWavFile(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var results = new List<SpeechRecognitionResult>();
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
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
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray(), texts, 5);
                }
            }
        }

        [TestMethod]
        public async Task ContinuousCheckFileOffsets()
        {
            const int Times = 2;
            var audioInput = Util.OpenWavFile(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Times);
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
                AssertFuzzyMatching(resultUtterances, AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray().Concat(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray()).ToArray(), 2);

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
            var audioInput = Util.OpenWavFile(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Times);
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
                var expected = string.Join(" ", AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray());
                for (int i = 1; i < Times; i++)
                {
                    expected += " ";
                    expected += string.Join(" ", AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Select(x => x.Text).ToArray());
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

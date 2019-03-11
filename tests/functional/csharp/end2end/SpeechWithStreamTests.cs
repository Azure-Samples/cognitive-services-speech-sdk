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
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(this.config, TestData.English.Batman.AudioFile);
            AssertMatching(TestData.English.Batman.Utterances[0], result.Text);
        }

        [TestMethod]
        public async Task DetailedRecognitionBatman()
        {
            this.config.SpeechRecognitionLanguage = Language.EN;
            this.config.OutputFormat = OutputFormat.Detailed;
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(this.config, TestData.English.Batman.AudioFile);
            var detailedRecognitionText = string.Join(" ", result.Best().Select(r => r.Text).ToList());
            var detailedRecognitionNormalizedForm = string.Join(" ", result.Best().Select(r => r.NormalizedForm).ToList());
            var detailedRecognitionLexicalForm = string.Join(" ", result.Best().Select(r => r.NormalizedForm).ToList());

            AssertMatching(TestData.English.Batman.Utterances[0], detailedRecognitionText);
            AssertMatching(TestData.English.Batman.Utterances[0], detailedRecognitionNormalizedForm);
            AssertMatching(TestData.English.Batman.Utterances[0], detailedRecognitionLexicalForm);
        }

        [TestMethod]
        public async Task SimpleRecognitionBatmanContinuous()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(this.config, TestData.English.Batman.AudioFile);
            Assert.AreEqual(TestData.English.Batman.Utterances.Length, result.Count, "Unexpected number of utterances.");
            var actualRecognitionTextResults = result.Select(t => t.Result.Text).ToArray();
            for (var i = 0; i < result.Count; i++)
            {
                AssertMatching(TestData.English.Batman.Utterances[i], actualRecognitionTextResults[i]);
            }
        }

        [TestMethod]
        public async Task DetailedRecognitionBatmanContinuous()
        {
            this.config.SpeechRecognitionLanguage = Language.EN;
            this.config.OutputFormat = OutputFormat.Detailed;
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(this.config, TestData.English.Batman.AudioFile);
            Assert.AreNotEqual(result.Count, 0);

            var firstUtteranceText = string.Join(" ", result[0].Result.Best().Select(r => r.Text).ToList());
            var firstUtteranceNormalizedForm = string.Join(" ", result[0].Result.Best().Select(r => r.NormalizedForm).ToList());
            var firstUtteranceLexicalForm = string.Join(" ", result[0].Result.Best().Select(r => r.NormalizedForm).ToList());

            AssertMatching(TestData.English.Batman.Utterances[0], firstUtteranceText);
            AssertMatching(TestData.English.Batman.Utterances[0], firstUtteranceNormalizedForm);
            AssertMatching(TestData.English.Batman.Utterances[0], firstUtteranceLexicalForm);

            var lastUtteranceText = string.Join(" ", result.Last().Result.Best().Select(r => r.Text).ToList());
            var lastUtteranceNormalizedForm = string.Join(" ", result.Last().Result.Best().Select(r => r.NormalizedForm).ToList());
            var lastUtteranceLexicalForm = string.Join(" ", result.Last().Result.Best().Select(r => r.NormalizedForm).ToList());

            AssertMatching(TestData.English.Batman.Utterances.Last(), lastUtteranceText);
            AssertMatching(TestData.English.Batman.Utterances.Last(), lastUtteranceNormalizedForm);
            AssertMatching(TestData.English.Batman.Utterances.Last(), lastUtteranceLexicalForm);

            Assert.AreEqual(TestData.English.Batman.Utterances.Length, result.Count, "Unexpected number of utterances");
            var actualRecognitionTextResults = result.Select(t => t.Result.Text).ToArray();
            for (var i = 0; i < result.Count; i++)
            {
                AssertMatching(TestData.English.Batman.Utterances[i], actualRecognitionTextResults[i]);
            }
        }

        [TestMethod]
        public async Task DetailedRecognitionBatmanContinuousBest()
        {
            this.config.SpeechRecognitionLanguage = Language.EN;
            this.config.OutputFormat = OutputFormat.Detailed;
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(this.config, TestData.English.Batman.AudioFile);
            Assert.AreNotEqual(result.Count, 0, "Received no result");

            var firstUtteranceText = string.Join(" ", result[0].Result.Best().Select(r => r.Text).ToList());
            Assert.IsFalse(string.IsNullOrEmpty(firstUtteranceText), $"Utterance is unexpectedly empty {firstUtteranceText}");
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        [TestMethod]
        public async Task InteractiveCheckFileOffsets(bool usingPreConnection)
        {
            this.config.SpeechRecognitionLanguage = Language.EN;
            var audioInput = Util.OpenWavFile(TestData.English.Batman.AudioFile);
            var results = new List<SpeechRecognitionResult>();
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
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

                var texts = results.Select(r => r.Text).Where(t => !string.IsNullOrEmpty(t)).ToList();
                var expected = string.Join(" ", TestData.English.Batman.UtterancesInteractiveMode);
                var actual = string.Join(" ", texts.ToArray());
                AssertMatching(expected, actual);
            }
        }

        [TestMethod]
        public async Task ContinuousCheckFileOffsets()
        {
            const int Times = 2;
            var audioInput = Util.OpenWavFile(TestData.English.Batman.AudioFile, Times);
            var results = new List<SpeechRecognitionResult>();
            var taskSource = new TaskCompletionSource<bool>();
            this.config.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = new SpeechRecognizer(this.config, audioInput))
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

                // Checking text results.
                var texts = results.Select(r => r.Text).Where(t => !string.IsNullOrEmpty(t)).ToList();
                var expected = string.Join(" ", TestData.English.Batman.Utterances);
                expected += " ";
                expected += string.Join(" ", TestData.English.Batman.Utterances2);
                expected = Normalize(expected);

                var actual = string.Join(" ", texts.ToArray());
                actual = Normalize(actual);
                // dont do a hard string comparison, we allow a small percentage of word edits (word insert/delete/move)
                AssertStringWordEditPercentage(expected, actual, 2);

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
            this.config.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = new SpeechRecognizer(this.config, audioInput))
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
                    expected += string.Join(" ", TestData.English.Batman.Utterances2);
                }
                expected = Normalize(expected);

                var actual = string.Join(" ", texts.ToArray());
                actual = Normalize(actual);
                // dont do a hard string comparison, we allow a small percentage of word edits (word insert/delete/move)
                AssertStringWordEditCount(expected, actual, 10);
            }
        }
    }
}

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

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        [TestMethod]
        [Ignore("needs fixing on the service side, fails very often, returning only 15 utterances, chaning offsets, or mistakes in transcription")]
        public async Task ContinuousCheckFileOffsets(bool usingPreConnection)
        {
            const int Times = 2;
            var audioInput = Util.OpenWavFile(TestData.English.Batman.AudioFile, Times);
            var results = new List<SpeechRecognitionResult>();
            var taskSource = new TaskCompletionSource<bool>();
            this.config.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = new SpeechRecognizer(this.config, audioInput))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
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
                expected = expected.Replace("super powered extra ", ""); // removing a substring which sometimes leads to trouble

                Assert.AreEqual(TestData.English.Batman.Utterances.Length * Times, results.Count);
               
                var actual = string.Join(" ", texts.ToArray());
                actual = Normalize(actual);
                actual = actual.Replace("super powered extra ", ""); // removing a substring which sometimes leads to trouble
                actual = actual.Replace("super cross trails ", "");  // this is the recognition which sometimes is created
                AssertMatching(expected, actual);

                // Checking durations.
                var offsets = results
                    .Where(r => !string.IsNullOrEmpty(r.Text))
                    .Select(r => new Tuple<long, long>(r.OffsetInTicks, r.Duration.Ticks))
                    .ToList();
                var expectedOffsets = new List<Tuple<long, long>>
                {
                    new Tuple<long, long>(5400000, 200100000),
                    new Tuple<long, long>(213400000, 6200000),
                    new Tuple<long, long>(229200000, 200100000),
                    new Tuple<long, long>(440500000, 200100000),
                    new Tuple<long, long>(648500000, 42000000),
                    new Tuple<long, long>(708000000, 68700000),
                    new Tuple<long, long>(788700000, 200100000),
                    new Tuple<long, long>(996700000, 113100000),
                    new Tuple<long, long>(1151900000, 200100000),
                    new Tuple<long, long>(1359900000, 6300000),
                    new Tuple<long, long>(1375800000, 200100000),
                    new Tuple<long, long>(1587100000, 200100000),
                    new Tuple<long, long>(1795100000, 42000000),
                    new Tuple<long, long>(1854600000, 68700000),
                    new Tuple<long, long>(1935300000, 200100000),
                    new Tuple<long, long>(2143300000, 113100000)
                };
                Assert.AreEqual(expectedOffsets.Count, offsets.Count, "Number of offsets should match");
                var zipped = expectedOffsets
                    .Zip(offsets, (f, s) => new { FirstOffset = f.Item1, SecondOffset = s.Item1, FirstDuration = f.Item2, SecondDuration = s.Item2 })
                    .ToList();

                // Currently sometimes decoder gives different duration for the same file sent twice sequentially
                // This needs further investigation.
                for (int i = 0; i < zipped.Count; i++)
                {
                    long delta =  zipped[i].SecondOffset / 20;                 // a delta of 5 percent is allowed
                    Assert.AreEqual(zipped[i].FirstOffset, zipped[i].SecondOffset, delta, $"Offsets should be within a delta, index {i}");
                    delta =  zipped[i].SecondDuration / 20;                    // a delta of 5 percent is allowed
                    Assert.AreEqual(zipped[i].FirstDuration, zipped[i].SecondDuration, delta, $"Durations should be within a delta, index {i}");
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
                await Task.WhenAny(taskSource.Task, Task.Delay(TimeSpan.FromMinutes(20)));
                Assert.AreEqual(TaskStatus.RanToCompletion, taskSource.Task.Status, "The timeout happened. SessionStopped event was not received in 20 minutes.");
                await recognizer.StopContinuousRecognitionAsync();

                // Checking text results.
                var texts = results.Select(r => r.Text).Where(t => !string.IsNullOrEmpty(t)).ToList();
                var expected = string.Join(" ", TestData.English.Batman.Utterances);
                expected += " ";
                expected += string.Join(" ", TestData.English.Batman.Utterances2);

                var actual = string.Join(" ", texts.ToArray());
                Assert.AreEqual(TestData.English.Batman.Utterances.Length * Times, results.Count);
                AssertMatching(expected, actual);
            }
        }
    }
}

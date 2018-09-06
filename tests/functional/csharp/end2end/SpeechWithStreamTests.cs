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
            speechHelper = new SpeechWithStreamHelper(factory);
        }

        [TestMethod]
        public async Task SimpleRecognitionBatman()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(TestData.English.Batman.AudioFile);
            AssertMatching(TestData.English.Batman.Utterances[0], result.Text);
        }

        [TestMethod]
        public async Task DetailedRecognitionBatman()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(TestData.English.Batman.AudioFile, Language.EN, OutputFormat.Detailed);
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
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(TestData.English.Batman.AudioFile);
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
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(TestData.English.Batman.AudioFile, Language.EN, OutputFormat.Detailed);
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
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(TestData.English.Batman.AudioFile, Language.EN, OutputFormat.Detailed);
            Assert.AreNotEqual(result.Count, 0, "Received no result");

            var firstUtteranceText = string.Join(" ", result[0].Result.Best().Select(r => r.Text).ToList());
            Assert.IsFalse(string.IsNullOrEmpty(firstUtteranceText), $"Utterance is unexpectedly empty {firstUtteranceText}");
        }

        [TestMethod]
        [Ignore]
        public async Task InteractiveCheckFileOffsets()
        {
            var factory = this.factory;
            var stream = Util.OpenWaveFile(TestData.English.Batman.AudioFile, 1);
            var results = new List<SpeechRecognitionResult>();
            using (var recognizer = this.factory.CreateSpeechRecognizerWithStream(stream, Language.EN))
            {
                while (true)
                {
                    var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                    if (result.RecognitionStatus == RecognitionStatus.Canceled)
                    {
                        break;
                    }
                    results.Add(result);
                }

                var texts = results.Select(r => r.Text).Where(t => !string.IsNullOrEmpty(t)).ToList();
                var expected = string.Join(" ", TestData.English.Batman.Utterances);
                var actual = string.Join(" ", texts.ToArray());
                AssertMatching(expected, actual);
            }
        }

        [TestMethod]
        public async Task ContinuousCheckFileOffsets()
        {
            const int Times = 2;
            var stream = Util.OpenWaveFile(TestData.English.Batman.AudioFile, Times);
            var results = new List<SpeechRecognitionResult>();
            var taskSource = new TaskCompletionSource<bool>();
            using (var recognizer = this.factory.CreateSpeechRecognizerWithStream(stream, Language.EN))
            {
                recognizer.FinalResultReceived += (s, e) =>
                {
                    Console.WriteLine($"Result recognized {e.ToString()}");
                    if (e.Result.RecognitionStatus == RecognitionStatus.Canceled)
                    {
                        Console.WriteLine($"Received cancelled result {e.Result.ToString()}, exiting");
                        taskSource.SetResult(false);
                    }
                    else
                    {
                        results.Add(e.Result);
                    }
                };

                recognizer.OnSessionEvent += (s, e) =>
                {
                    Console.WriteLine($"Received session event {e.ToString()}");
                    if (e.EventType == SessionEventType.SessionStoppedEvent)
                    {
                        Console.WriteLine("Exiting due to session stop event");
                        taskSource.SetResult(true);
                    }
                };

                recognizer.RecognitionErrorRaised += (s, e) =>
                {
                    Console.WriteLine($"Received error event {e.ToString()}, exiting");
                    taskSource.SetResult(false);
                };

                await recognizer.StartContinuousRecognitionAsync();
                // Make sure the task is rooted.
                await Task.WhenAny(taskSource.Task, Task.Delay(TimeSpan.FromMinutes(6)));
                await recognizer.StopContinuousRecognitionAsync();

                // Checking text results.
                var texts = results.Select(r => r.Text).Where(t => !string.IsNullOrEmpty(t)).ToList();
                var expected = string.Join(" ", TestData.English.Batman.Utterances);
                expected = string.Join(" ", Enumerable.Repeat(expected, Times).ToArray());
                var actual = string.Join(" ", texts.ToArray());
                Assert.AreEqual(TestData.English.Batman.Utterances.Length * Times, results.Count);
                AssertMatching(expected, actual);

                // Checking durations.
                var offsets = results
                    .Where(r => !string.IsNullOrEmpty(r.Text))
                    .Select(r => new Tuple<long, long>(r.OffsetInTicks, r.Duration.Ticks))
                    .ToList();
                var expectedOffsets = new List<Tuple<long, long>>
                {
                    new Tuple<long, long>(5300000, 98600000),
                    new Tuple<long, long>(108900000, 71600000),
                    new Tuple<long, long>(183600000, 81400000),
                    new Tuple<long, long>(265100000, 91300000),
                    new Tuple<long, long>(356500000, 79000000),
                    new Tuple<long, long>(439300000, 110300000),
                    new Tuple<long, long>(553400000, 96700000),
                    new Tuple<long, long>(650200000, 44600000),
                    new Tuple<long, long>(707000000, 70400000),
                    new Tuple<long, long>(784600000, 90600000),
                    new Tuple<long, long>(875300000, 85400000),
                    new Tuple<long, long>(964600000, 122200000),
                    new Tuple<long, long>(1086900000, 23600000),

                    new Tuple<long, long>(1147600000, 102800000),
                    new Tuple<long, long>(1255500000, 71600000),
                    new Tuple<long, long>(1330200000, 81400000),
                    new Tuple<long, long>(1411700000, 86200000),
                    new Tuple<long, long>(1498000000, 83300000),
                    new Tuple<long, long>(1585900000, 110400000),
                    new Tuple<long, long>(1700100000, 96600000),
                    new Tuple<long, long>(1796800000, 44600000),
                    new Tuple<long, long>(1853600000, 70400000),
                    new Tuple<long, long>(1931200000, 90500000),
                    new Tuple<long, long>(2021800000, 85400000),
                    new Tuple<long, long>(2111200000, 122200000),
                    new Tuple<long, long>(2233500000, 23600000),
                };
                Assert.AreEqual(expectedOffsets.Count, offsets.Count, "Number of offsets should match");
                var zipped = expectedOffsets
                    .Zip(offsets, (f, s) => new { FirstOffset = f.Item1, SecondOffset = s.Item1, FirstDuration = f.Item2, SecondDuration = s.Item2 })
                    .ToList();

                // Currently sometimes decoder gives different duration for the same file sent twice sequentially
                // This needs further investigation.
                for (int i = 0; i < zipped.Count; i++)
                {
                    Assert.AreEqual(zipped[i].FirstOffset, zipped[i].SecondOffset, $"Offsets should be equal, index {i}");
                    Assert.AreEqual(zipped[i].FirstDuration, zipped[i].SecondDuration, $"Durations should be equal, index {i}");
                }
            }
        }
    }
}

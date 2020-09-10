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
using System.Diagnostics;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public sealed class SpeechWithStreamTests : RecognitionTestBase
    {
        private SpeechWithStreamHelper speechHelper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            BaseClassInit(context);
        }

        [ClassCleanup]
        new public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void TestInitalize()
        {
            speechHelper = new SpeechWithStreamHelper();
        }

        [DataTestMethod, TestCategory(TestCategory.CompressedStreamTest)]
        [DataRow(AudioStreamContainerFormat.MP3)]
        [DataRow(AudioStreamContainerFormat.OGG_OPUS)]
        [DataRow(AudioStreamContainerFormat.FLAC)]
        [DataRow(AudioStreamContainerFormat.ALAW)]
        [DataRow(AudioStreamContainerFormat.MULAW)]
        public async Task SimpleRecognitionWithCompressedFormat(AudioStreamContainerFormat format)
        {
            var filePathKey =
                format == AudioStreamContainerFormat.MP3 ? AudioUtteranceKeys.SINGLE_UTTERANCE_MP3
                : format == AudioStreamContainerFormat.OGG_OPUS ? AudioUtteranceKeys.SINGLE_UTTERANCE_OPUS
                : format == AudioStreamContainerFormat.FLAC ? AudioUtteranceKeys.SINGLE_UTTERANCE_FLAC
                : format == AudioStreamContainerFormat.ALAW ? AudioUtteranceKeys.SINGLE_UTTERANCE_A_LAW
                : format == AudioStreamContainerFormat.MULAW ? AudioUtteranceKeys.SINGLE_UTTERANCE_MU_LAW
                : null;
            SPXTEST_ISNOTNULL(filePathKey, $"Unsupported container format mapping for $'{format}'");
            var filePath = AudioUtterancesMap[filePathKey].FilePath.GetRootRelativePath();

            var result = await this.speechHelper.GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(
                this.defaultConfig,
                filePath,
                format);
            AssertMatching(AudioUtterancesMap[filePathKey].Utterances[Language.EN][0].Text, result.Text);
        }

        [DataTestMethod]
        [DataRow(false, false, true, DisplayName = "Simple one-shot with preconnect")]
        [DataRow(false, true, false, DisplayName = "Simple continuous")]
        [DataRow(true, false, false, DisplayName = "Detailed one-shot")]
        [DataRow(true, true, true, DisplayName = "Detailed continuous with preconnect")]
        public async Task Batman(bool useDetailedFormat, bool useContinuousReco, bool usePreconnect)
        {
            var allowedEditPercentage = 7;
            var utterance = AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH];

            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;

            if (useDetailedFormat)
            {
                this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            }

            // When doing continuous recognition, we'll get a collection of final result events as
            // we process the whole file. For single shot, we'll get only the first result. Either
            // way, we'll represent this as a list of results for comparisons.
            var results = useContinuousReco ?
                (await this.speechHelper.GetFinalRecoEventsForContinuousAsync(
                    this.defaultConfig,
                    utterance.FilePath.GetRootRelativePath(),
                    usePreconnect))
                    .Select(resultEvent => resultEvent.Result)
                : new List<SpeechRecognitionResult>() {
                    await this.speechHelper.GetFinalRecoAsync(
                    this.defaultConfig,
                    utterance.FilePath.GetRootRelativePath(),
                    usePreconnect)
                  };

            var referenceResults = useContinuousReco ?
                utterance.Utterances[Language.EN]
                : new Utterance[] { utterance.Utterances[Language.EN][0] };

            SPXTEST_ARE_EQUAL(
                results.Count(),
                referenceResults.Length,
                $"Expected {referenceResults.Length} result(s); actual was {results.Count()}");

            // The display form (Result.Text) should match what's expected -- with a little fuzziness for resiliency
            AssertFuzzyMatching(
                results.Select(result => result.Text).ToArray(),
                referenceResults.Select(referenceResult => referenceResult.Text).ToArray(),
                tolerance: allowedEditPercentage);

            if (useDetailedFormat)
            {
                // For detailed results, we'll further sanity check the normalized and lexical
                // forms. This particular 'Batman' utterance should have very ltitle difference
                // between its normalized and lexical form -- they should be identical except
                // for an extra space before a possessive single quotation mark
                // (e.g. "Batman 's).
                results.Select(result => result.Best().First())
                    .ToList()
                    .ForEach(oneBest =>
                    {
                        SPXTEST_ISTRUE(!string.IsNullOrEmpty(oneBest.NormalizedForm), "A detailed result had an empty normalized form.");
                        SPXTEST_ISTRUE(!string.IsNullOrEmpty(oneBest.LexicalForm), "A detailed result had an empty lexical form.");

                        var lexicalForm = oneBest.LexicalForm.Replace(" '", "'");
                        var normalizedForm = oneBest.NormalizedForm.Replace(" '", "'");
                        SPXTEST_ISTRUE(lexicalForm == normalizedForm,
                            $@"The sanitized lexical form of a result did not match its normalized form.\n  
                            Normalized: {oneBest.NormalizedForm}\n  
                            Lexical   : {oneBest.LexicalForm}");
                    });

            }

            // Check the durations and offsets
            var minimumExpectedNextOffset = 0L;

            results.ToList().ForEach(result =>
            {
                SPXTEST_ISTRUE(
                    result.OffsetInTicks > 0,
                    $"Result unexpectedly reports a value of 0 for its offset.\n  Text: {result.Text}");
                SPXTEST_ISTRUE(
                    result.Duration > TimeSpan.Zero,
                    $"Result unexpectedly reports a value of 0 for its duration.\n  Text: {result.Text}");
                SPXTEST_ISTRUE(
                    result.OffsetInTicks > minimumExpectedNextOffset,
                    $@"Expected an offset of at least {minimumExpectedNextOffset} after previous results;
                    actual was {result.OffsetInTicks}.");
                minimumExpectedNextOffset = result.OffsetInTicks + result.Duration.Ticks;
            });
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ReallyLongBatman()
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
                SPXTEST_ARE_EQUAL(TaskStatus.RanToCompletion, taskSource.Task.Status, "The timeout happened. SessionStopped event was not received in 20 minutes.");
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
                AssertStringWordEditCount(expected, actual, (Times * 10));
            }
        }
    }
}

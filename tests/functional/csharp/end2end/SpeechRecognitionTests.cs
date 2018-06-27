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

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    [TestClass]
    public class SpeechRecognitionTests : RecognitionTestBase
    {
        private static string deploymentId;
        private SpeechRecognitionTestsHelper speechRecognitionTestsHelper;
        private const int testTimeout = 90000;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
            deploymentId = Config.GetSettingByKey<String>(context, "DeploymentId");
        }

        [TestInitialize]
        public void Initialize()
        {
            speechRecognitionTestsHelper = new SpeechRecognitionTestsHelper();
        }

        [Ignore("TODO https://msasg.visualstudio.com/Skyman/_workitems/edit/1269097")]
        [TestMethod, Timeout(testTimeout)]
        public async Task ValidBaselineRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsTrue(result.Duration.Ticks > 0, result.RecognitionStatus.ToString());
                Assert.AreEqual(0, result.Offset.Ticks);
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(result.Text, TestData.English.Weather.Utterance));
            }
        }

        [TestMethod, Timeout(testTimeout)] //TODO: Remove Test after Bug 1269097 is fixed
        public async Task ContinuousValidBaselineRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                Assert.IsTrue(await speechRecognitionTestsHelper.IsValidSimpleRecognizer(recognizer, TestData.English.Weather.Utterance));
            }
        }

        [Ignore("TODO https://msasg.visualstudio.com/Skyman/_workitems/edit/1269097")]
        [TestMethod, Timeout(testTimeout)]
        public async Task ValidCustomRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                recognizer.DeploymentId = deploymentId;
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(result.Text, TestData.English.Weather.Utterance));
            }
        }

        [TestMethod, Timeout(testTimeout)] //TODO: Remove Test after Bug 1269097 is fixed
        public async Task ContinuousValidCustomRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                recognizer.DeploymentId = deploymentId;
                Assert.IsTrue(await speechRecognitionTestsHelper.IsValidSimpleRecognizer(recognizer, TestData.English.Weather.Utterance));
            }
        }

        [Ignore] // TODO ENABLE AFTER FIXING BROKEN SERVICE
        [TestMethod, Timeout(testTimeout)]
        public async Task InvalidKeyHandledProperly()
        {
            var factory = SpeechFactory.FromSubscription("invalidKey", region);
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.RecognitionFailureReason));
                Assert.IsTrue(result.RecognitionFailureReason.Contains("401"));
                Assert.AreEqual(RecognitionStatus.Canceled, result.RecognitionStatus);
            }
        }

        [TestMethod, Timeout(testTimeout)]
        public async Task InvalidRegionHandledProperly()
        {
            var factory = SpeechFactory.FromSubscription(subscriptionKey, "invalidRegion");
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.RecognitionFailureReason));
                Assert.IsTrue(result.RecognitionFailureReason.Contains("Connection failed"));
                Assert.AreEqual(RecognitionStatus.Canceled, result.RecognitionStatus);
            }
        }

        [TestMethod, Timeout(testTimeout)]
        public void InvalidInputFileHandledProperly()
        {
            Assert.ThrowsException<ApplicationException>(() => factory.CreateSpeechRecognizerWithFileInput("invalidFile.wav"));
        }

        [TestMethod, Timeout(testTimeout)]
        public async Task InvalidDeploymentIdHandledProperly()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                recognizer.DeploymentId = "invalidDeploymentId";
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.RecognitionFailureReason));
                Assert.IsTrue(result.RecognitionFailureReason.Contains("400"));
                Assert.AreEqual(RecognitionStatus.Canceled, result.RecognitionStatus);
            }
        }

        [Ignore("TODO https://msasg.visualstudio.com/Skyman/_workitems/edit/1269097")]
        [TestMethod, Timeout(testTimeout)]
        public async Task GermanRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.Text), result.RecognitionStatus.ToString());
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(result.Text, TestData.German.FirstOne.Utterance));
            }
        }

        [TestMethod, Timeout(testTimeout)] //TODO: Remove Test after Bug 1269097 is fixed
        public async Task ContinuousGermanRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE))
            {
                Assert.IsTrue(await speechRecognitionTestsHelper.IsValidSimpleRecognizer(recognizer, TestData.German.FirstOne.Utterance));
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning), Timeout(testTimeout)]
        [Ignore("TODO https://msasg.visualstudio.com/Skyman/_workitems/edit/1304778")]
        public async Task ContinuousRecognitionOnLongFileInput()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Batman.AudioFile))
            {
                List<string> recognizedText = new List<string>();
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.FinalResultReceived += (s, e) =>
                {
                    if (e.Result.Text.Length > 0)
                    {
                        recognizedText.Add(e.Result.Text);
                    }
                };

                await speechRecognitionTestsHelper.CompleteContinuousRecognition(recognizer);

                Assert.IsTrue(speechRecognitionTestsHelper.FinalResultEventCount > 0, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.ErrorEventCount, AssertOutput.WrongErrorCount);
                Assert.IsTrue(speechRecognitionTestsHelper.SpeechStartedEventCount > 0, AssertOutput.WrongSpeechStartedCount);
                Assert.IsTrue(speechRecognitionTestsHelper.SpeechEndedEventCount > 0, AssertOutput.WrongSpeechEndedCount);
                Assert.IsTrue(recognizedText.Count > 0);
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(recognizedText[0], TestData.English.Batman.Utterances[0]));
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(recognizedText.Last(), TestData.English.Batman.Utterances.Last()));
            }
        }

        [TestMethod, Timeout(testTimeout)]
        public async Task SubscribeToManyEventHandlers()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                const int numLoops = 7;
                for (int i = 0; i < numLoops; i++)
                {
                    speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                }

                await speechRecognitionTestsHelper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(numLoops, speechRecognitionTestsHelper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(numLoops, speechRecognitionTestsHelper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(numLoops, speechRecognitionTestsHelper.SpeechEndedEventCount, AssertOutput.WrongSpeechEndedCount);
                Assert.AreEqual(numLoops, speechRecognitionTestsHelper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod, Timeout(testTimeout)]
        public async Task UnsubscribeFromEventHandlers()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                speechRecognitionTestsHelper.UnsubscribeFromCounterEventHandlers(recognizer);

                await speechRecognitionTestsHelper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(0, speechRecognitionTestsHelper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.SpeechEndedEventCount, AssertOutput.WrongSpeechEndedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod, Timeout(testTimeout)]
        public async Task ResubscribeToEventHandlers()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                const int numSubscriptions = 3;
                const int numUnsubscriptions = 2;
                const int diff = numSubscriptions - numUnsubscriptions + numSubscriptions;

                for (int i = 0; i < numSubscriptions; i++)
                {
                    speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                }

                for (int i = 0; i < numUnsubscriptions; i++)
                {
                    speechRecognitionTestsHelper.UnsubscribeFromCounterEventHandlers(recognizer);
                }

                for (int i = 0; i < numSubscriptions; i++)
                {
                    speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                }

                await speechRecognitionTestsHelper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(diff, speechRecognitionTestsHelper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(diff, speechRecognitionTestsHelper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(diff, speechRecognitionTestsHelper.SpeechEndedEventCount, AssertOutput.WrongSpeechEndedCount);
                Assert.AreEqual(diff, speechRecognitionTestsHelper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod, Timeout(testTimeout)]
        public async Task ChangeSubscriptionDuringRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.OnSessionEvent += (s, e) =>
                {
                    if (e.EventType == SessionEventType.SessionStartedEvent)
                    {
                        speechRecognitionTestsHelper.UnsubscribeFromCounterEventHandlers(recognizer);
                    }
                };

                await speechRecognitionTestsHelper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(1, speechRecognitionTestsHelper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.SpeechEndedEventCount, AssertOutput.WrongSpeechEndedCount);
                Assert.AreEqual(0, speechRecognitionTestsHelper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }
    }
}

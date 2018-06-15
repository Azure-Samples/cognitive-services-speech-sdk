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
        private TaskCompletionSource<int> taskCompletionSource;
        private TimeSpan timeout = TimeSpan.FromSeconds(90);

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

        [TestMethod]
        public async Task ValidBaselineRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

                Assert.AreEqual(speechRecognitionTestsHelper.FinalResultEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechStartedEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechEndedEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.SessionStartedEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.ErrorEventCount, 0);
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(result.Text, TestData.English.Weather.Utterance));
            }
        }

        [TestMethod]
        public async Task ValidCustomRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                recognizer.DeploymentId = deploymentId;
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.AreEqual(speechRecognitionTestsHelper.FinalResultEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechStartedEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechEndedEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.SessionStartedEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.ErrorEventCount, 0);
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(result.Text, TestData.English.Weather.Utterance));
            }
        }

        [Ignore] // TODO ENABLE AFTER FIXING BROKEN SERVICE
        [TestMethod]
        public async Task InvalidKeyHandledProperly()
        {
            var factory = SpeechFactory.FromSubscription("invalidKey", region);
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.RecognitionFailureReason));
                Assert.IsTrue(result.RecognitionFailureReason.Contains("401"));
                Assert.AreEqual(result.RecognitionStatus, RecognitionStatus.Canceled);
            }
        }

        [TestMethod]
        public async Task InvalidRegionHandledProperly()
        {
            var factory = SpeechFactory.FromSubscription(subscriptionKey, "invalidRegion");
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.RecognitionFailureReason));
                Assert.IsTrue(result.RecognitionFailureReason.Contains("Connection failed"));
                Assert.AreEqual(result.RecognitionStatus, RecognitionStatus.Canceled);
            }
        }

        [TestMethod]
        public void InvalidInputFileHandledProperly()
        {
            Assert.ThrowsException<ApplicationException>(() => factory.CreateSpeechRecognizerWithFileInput("invalidFile.wav"));
        }

        [TestMethod]
        public async Task InvalidDeploymentIdHandledProperly()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                recognizer.DeploymentId = "invalidDeploymentId";
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.RecognitionFailureReason));
                Assert.IsTrue(result.RecognitionFailureReason.Contains("400"));
                Assert.AreEqual(result.RecognitionStatus, RecognitionStatus.Canceled);
            }
        }

        [TestMethod]
        [Ignore]
        public async Task GermanRecognition()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.Text));
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(result.Text, TestData.German.FirstOne.Utterance));
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ContinuousRecognitionOnLongFileInput()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Batman.AudioFile))
            {
                taskCompletionSource = new TaskCompletionSource<int>();
                List<string> recognizedText = new List<string>();
                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.FinalResultReceived += (s, e) =>
                {
                    if (e.Result.Text.Length > 0)
                    {
                        recognizedText.Add(e.Result.Text);
                    }
                };
                recognizer.OnSessionEvent += (s, e) =>
                {
                    if (e.EventType == SessionEventType.SessionStoppedEvent)
                    {
                        taskCompletionSource.TrySetResult(0);
                    }
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                Assert.IsTrue(speechRecognitionTestsHelper.FinalResultEventCount > 0);
                Assert.AreEqual(speechRecognitionTestsHelper.ErrorEventCount, 0);
                Assert.IsTrue(speechRecognitionTestsHelper.SpeechStartedEventCount > 0);
                Assert.IsTrue(speechRecognitionTestsHelper.SpeechEndedEventCount > 0);
                Assert.AreEqual(speechRecognitionTestsHelper.SessionStartedEventCount, 1);
                Assert.IsTrue(recognizedText.Count > 0);
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(recognizedText[0], TestData.English.Batman.Utterances[0]));
                Assert.IsTrue(speechRecognitionTestsHelper.AreResultsMatching(recognizedText.Last(), TestData.English.Batman.Utterances.Last()));
            }
        }

        [TestMethod]
        [Ignore]
        public async Task SubscribeToManyEventHandlers()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {
                const int numLoops = 7;
                for (int i = 0; i < numLoops; i++)
                {
                    speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                }

                await recognizer.RecognizeAsync().ConfigureAwait(false);

                Assert.AreEqual(speechRecognitionTestsHelper.FinalResultEventCount, numLoops);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechStartedEventCount, numLoops);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechEndedEventCount, numLoops);
                Assert.AreEqual(speechRecognitionTestsHelper.SessionStartedEventCount, numLoops);
                Assert.AreEqual(speechRecognitionTestsHelper.ErrorEventCount, 0);
            }
        }

        [TestMethod]
        [Ignore]
        public async Task UnsubscribeFromEventHandlers()
        {
            using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile))
            {

                speechRecognitionTestsHelper.SubscribeToCounterEventHandlers(recognizer);
                speechRecognitionTestsHelper.UnsubscribeFromCounterEventHandlers(recognizer);

                await recognizer.RecognizeAsync().ConfigureAwait(false);

                Assert.AreEqual(speechRecognitionTestsHelper.FinalResultEventCount, 0);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechStartedEventCount, 0);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechEndedEventCount, 0);
                Assert.AreEqual(speechRecognitionTestsHelper.SessionStartedEventCount, 0);
                Assert.AreEqual(speechRecognitionTestsHelper.ErrorEventCount, 0);
            }
        }

        [TestMethod]
        [Ignore]
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

                await recognizer.RecognizeAsync().ConfigureAwait(false);

                Assert.AreEqual(speechRecognitionTestsHelper.FinalResultEventCount, diff);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechStartedEventCount, diff);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechEndedEventCount, diff);
                Assert.AreEqual(speechRecognitionTestsHelper.SessionStartedEventCount, diff);
                Assert.AreEqual(speechRecognitionTestsHelper.ErrorEventCount, 0);
            }
        }

        [TestMethod]
        [Ignore]
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

                await recognizer.RecognizeAsync().ConfigureAwait(false);

                Assert.AreEqual(speechRecognitionTestsHelper.SessionStartedEventCount, 1);
                Assert.AreEqual(speechRecognitionTestsHelper.FinalResultEventCount, 0);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechStartedEventCount, 0);
                Assert.AreEqual(speechRecognitionTestsHelper.SpeechEndedEventCount, 0);
                Assert.AreEqual(speechRecognitionTestsHelper.ErrorEventCount, 0);
            }
        }
    }
}

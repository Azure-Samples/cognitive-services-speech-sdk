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
    using static AssertHelpers;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class SpeechRecognitionTests : RecognitionTestBase
    {
        private static string deploymentId;
        private SpeechRecognitionTestsHelper helper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
            deploymentId = Config.GetSettingByKey<String>(context, "DeploymentId");
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
        }

        [TestMethod]
        public async Task ValidBaselineRecognition()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                helper.SubscribeToCounterEventHandlers(recognizer);
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsTrue(result.Duration.Ticks > 0, result.RecognitionStatus.ToString(), "Duration == 0");
                Assert.AreEqual(0, result.OffsetInTicks, "Offset not zero");
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
            }
        }

        [TestMethod] //TODO: Remove Test after Bug 1269097 is fixed
        public async Task ContinuousValidBaselineRecognition()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                AssertMatching(
                    TestData.English.Weather.Utterance,
                    await helper.GetFirstRecognizerResult(recognizer));
            }
        }

        [TestMethod]
        public async Task ValidCustomRecognition()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                recognizer.DeploymentId = deploymentId;
                helper.SubscribeToCounterEventHandlers(recognizer);
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
            }
        }

        [TestMethod] //TODO: Remove Test after Bug 1269097 is fixed
        public async Task ContinuousValidCustomRecognition()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                recognizer.DeploymentId = deploymentId;
                AssertMatching(TestData.English.Weather.Utterance,
                    await helper.GetFirstRecognizerResult(recognizer));
            }
        }

        [TestMethod]
        public async Task InvalidKeyHandledProperly()
        {
            var factory = SpeechFactory.FromSubscription("invalidKey", region);
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                AssertStringContains(result.RecognitionFailureReason, "WebSocket Upgrade failed with an authentication error (401)");
                Assert.AreEqual(RecognitionStatus.Canceled, result.RecognitionStatus);
            }
        }

        [TestMethod]
        public async Task InvalidRegionHandledProperly()
        {
            var factory = SpeechFactory.FromSubscription(subscriptionKey, "invalidRegion");
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                AssertStringContains(result.RecognitionFailureReason, "Connection failed");
                Assert.AreEqual(RecognitionStatus.Canceled, result.RecognitionStatus);
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
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                recognizer.DeploymentId = "invalidDeploymentId";
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                AssertStringContains(result.RecognitionFailureReason, "WebSocket Upgrade failed with a bad request (400)");
                Assert.AreEqual(RecognitionStatus.Canceled, result.RecognitionStatus);
            }
        }

        [TestMethod]
        public async Task InvalidLanguageHandledProperly()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile, "InvalidLang")))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                AssertStringContains(result.RecognitionFailureReason, "WebSocket Upgrade failed with a bad request (400)");
                Assert.AreEqual(RecognitionStatus.Canceled, result.RecognitionStatus);
            }
        }

        [TestMethod]
        public async Task GermanRecognition()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE)))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.Text), result.RecognitionStatus.ToString());
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
            }
        }

        [TestMethod] //TODO: Remove Test after Bug 1269097 is fixed
        public async Task ContinuousGermanRecognition()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE)))
            {
                var result = await helper.GetFirstRecognizerResult(recognizer);
                AssertMatching(TestData.German.FirstOne.Utterance, result);
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ContinuousRecognitionOnLongFileInput()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Batman.AudioFile)))
            {
                List<string> recognizedText = new List<string>();
                helper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.FinalResultReceived += (s, e) =>
                {
                    if (e.Result.Text.Length > 0)
                    {
                        recognizedText.Add(e.Result.Text);
                    }
                };

                await helper.CompleteContinuousRecognition(recognizer);

                Assert.IsTrue(helper.FinalResultEventCount > 0, $"Invalid number of final result events {helper.FinalResultEventCount}");
                Assert.AreEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
                Assert.AreEqual(1, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.IsTrue(recognizedText.Count > 0, $"Invalid number of text messages {recognizedText.Count}");

                AssertMatching(TestData.English.Batman.Utterances[0], recognizedText[0]);
                AssertMatching(TestData.English.Batman.Utterances.Last(), recognizedText.Last());
            }
        }

        [TestMethod]
        public async Task SubscribeToManyEventHandlers()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                const int numLoops = 7;
                for (int i = 0; i < numLoops; i++)
                {
                    helper.SubscribeToCounterEventHandlers(recognizer);
                }
                await helper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(numLoops, helper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(numLoops, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(numLoops, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod]
        public async Task UnsubscribeFromEventHandlers()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                helper.SubscribeToCounterEventHandlers(recognizer);
                helper.UnsubscribeFromCounterEventHandlers(recognizer);

                await helper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(0, helper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(0, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(0, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod]
        public async Task ResubscribeToEventHandlers()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                const int numSubscriptions = 3;
                const int numUnsubscriptions = 2;
                const int diff = numSubscriptions - numUnsubscriptions + numSubscriptions;

                for (int i = 0; i < numSubscriptions; i++)
                {
                    helper.SubscribeToCounterEventHandlers(recognizer);
                }

                for (int i = 0; i < numUnsubscriptions; i++)
                {
                    helper.UnsubscribeFromCounterEventHandlers(recognizer);
                }

                for (int i = 0; i < numSubscriptions; i++)
                {
                    helper.SubscribeToCounterEventHandlers(recognizer);
                }

                await helper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(diff, helper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(diff, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(diff, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod]
        public async Task ChangeSubscriptionDuringRecognition()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                helper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.OnSessionEvent += (s, e) =>
                {
                    if (e.EventType == SessionEventType.SessionStartedEvent)
                    {
                        helper.UnsubscribeFromCounterEventHandlers(recognizer);
                    }
                };

                await helper.CompleteContinuousRecognition(recognizer);
                Assert.AreEqual(1, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                Assert.AreEqual(0, helper.FinalResultEventCount, AssertOutput.WrongFinalResultCount);
                Assert.AreEqual(0, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.AreEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod]
        public void TestGetters()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile)))
            {
                Assert.IsTrue(string.IsNullOrEmpty(recognizer.Language));
                Assert.AreEqual(recognizer.Language, recognizer.Parameters.Get<string>(SpeechParameterNames.RecognitionLanguage));

                Assert.IsTrue(string.IsNullOrEmpty(recognizer.DeploymentId));
                recognizer.DeploymentId = deploymentId;
                Assert.AreEqual(deploymentId, recognizer.DeploymentId);
                Assert.AreEqual(deploymentId, recognizer.Parameters.Get<string>(SpeechParameterNames.DeploymentId));

                Assert.AreEqual(recognizer.OutputFormat, OutputFormat.Simple);
            }

            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE)))
            {
                Assert.AreEqual(Language.DE_DE, recognizer.Language);
                Assert.AreEqual(Language.DE_DE, recognizer.Parameters.Get<string>(SpeechParameterNames.RecognitionLanguage));

                Assert.AreEqual(recognizer.DeploymentId, recognizer.Parameters.Get<string>(SpeechParameterNames.DeploymentId));

                Assert.AreEqual(recognizer.OutputFormat, OutputFormat.Simple);
            }

            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE, OutputFormat.Simple)))
            {
                Assert.AreEqual(recognizer.OutputFormat, OutputFormat.Simple);
            }

            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.German.FirstOne.AudioFile, Language.DE_DE, OutputFormat.Detailed)))
            {
                Assert.AreEqual(recognizer.OutputFormat, OutputFormat.Detailed);
            }
        }

        [TestMethod]
        public async Task TestExceptionSwitchFromSingleShotToContinuous()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.StartContinuousRecognitionAsync());
                AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
            }
        }

        [TestMethod]
        public async Task TestExceptionSwitchFromContinuousToSingleShot()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile)))
            {
                recognizer.RecognitionErrorRaised += (s, e) => { Console.WriteLine($"Error received: {e.ToString()}"); };
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.RecognizeAsync());
                AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
            }
        }

        [TestMethod]
        public async Task TestSingleShotTwice()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Batman.AudioFile)))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsTrue(result.Duration.Ticks > 0, result.RecognitionStatus.ToString(), "First result duration should be greater than 0");
                var offset = result.OffsetInTicks;
                var expectedNextOffset = offset + result.Duration.Ticks;

                var result2 = await recognizer.RecognizeAsync().ConfigureAwait(false);
                var offset2 = result2.OffsetInTicks;

                Console.WriteLine($"Offset1 {offset}, offset1 + duration {expectedNextOffset}");
                Console.WriteLine($"Offset2 {offset2}, its duration {result2.Duration.Ticks}");
                Console.WriteLine($"Result1: {result.ToString()}");
                Console.WriteLine($"Result2: {result2.ToString()}");

                Assert.AreEqual(result.RecognitionStatus, RecognitionStatus.Recognized);
                AssertStringContains(result.Text, "detective skills");
                Assert.IsTrue(result.Duration.Ticks > 0, $"Result duration {result.Duration.Ticks} in {result.ToString()} should be greater than 0");

                Assert.AreEqual(result2.RecognitionStatus, RecognitionStatus.Recognized);
                Assert.IsTrue(offset2 >= expectedNextOffset, $"Offset of the second recognition {offset2} should be greater or equal than offset of the first plus duration {expectedNextOffset}.");
            }
        }

        [TestMethod]
        public async Task TestStartStopManyTimes()
        {
            const int NumberOfIterations = 100;
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Batman.AudioFile)))
            {
                string error = string.Empty;
                recognizer.RecognitionErrorRaised += (s, e) => { error = e.ToString(); };
                for (int i = 0; i < NumberOfIterations; ++i)
                {
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }

                if (!string.IsNullOrEmpty(error))
                {
                    Assert.Fail($"Error received: {error}");
                }
            }
        }


        [TestMethod]
        public async Task TestContinuousRecognitionTwice()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Batman.AudioFile)))
            {
                string error = string.Empty;
                recognizer.RecognitionErrorRaised += (s, e) => { error = e.ToString(); };
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                if (!string.IsNullOrEmpty(error))
                {
                    Assert.Fail($"Error received: {error}");
                }
            }
        }

        [TestMethod]
        public async Task TestInitialSilenceTimeout()
        {
            using (var recognizer = TrackSessionId(factory.CreateSpeechRecognizerWithFileInput(TestData.English.Silence.AudioFile)))
            {
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                Assert.IsTrue(result.RecognitionStatus == RecognitionStatus.InitialSilenceTimeout, result.RecognitionStatus.ToString());
                Assert.IsTrue(result.OffsetInTicks > 0, result.OffsetInTicks.ToString());
                Assert.IsTrue(String.IsNullOrEmpty(result.Text), result.Text);
            }
        }
    }
}

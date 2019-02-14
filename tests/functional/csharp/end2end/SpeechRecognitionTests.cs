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
        public void TestSetAndGetAuthToken()
        {
            var token = "x";
            var config = SpeechConfig.FromAuthorizationToken(token, "westus");
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var speechRecognizer = new SpeechRecognizer(config, audioInput))
            {
                Assert.AreEqual(token, speechRecognizer.AuthorizationToken);

                var newToken = "y";
                speechRecognizer.AuthorizationToken = newToken;
                Assert.AreEqual(token, config.AuthorizationToken);
                Assert.AreEqual(newToken, speechRecognizer.AuthorizationToken);
            }
        }

        [TestMethod]
        public async Task TestSetAuthorizationTokenOnSpeechRecognizer()
        {
            var invalidToken = "InvalidToken";
            var configWithToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);

            using (var speechRecognizer = new SpeechRecognizer(configWithToken, audioInput))
            {
                Assert.AreEqual(invalidToken, speechRecognizer.AuthorizationToken);

                var newToken = await Config.GetToken(subscriptionKey, region);
                speechRecognizer.AuthorizationToken = newToken;
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(newToken, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public void TestSetAndGetSubKey()
        {
            var config = SpeechConfig.FromSubscription("x", "westus");
            Assert.AreEqual("x", config.SubscriptionKey);
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ValidBaselineRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(false);
                }

                var result = await helper.CompleteRecognizeOnceAsync(recognizer, connection).ConfigureAwait(false);
                AssertConnectionCountMatching(helper.ConnectedEventCount, helper.DisconnectedEventCount);
                Assert.IsTrue(result.Duration.Ticks > 0, result.Reason.ToString(), "Duration == 0");
                Assert.AreEqual(300000, result.OffsetInTicks, "Offset not zero");
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousValidBaselineRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                AssertMatching(
                    TestData.English.Weather.Utterance,
                    await helper.GetFirstRecognizerResult(recognizer));
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ValidCustomRecognition(bool usingPreConnection)
        {
            this.config.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(false);
                }
                var result = await helper.CompleteRecognizeOnceAsync(recognizer, connection).ConfigureAwait(false);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                AssertConnectionCountMatching(helper.ConnectedEventCount, helper.DisconnectedEventCount);
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousValidCustomRecognition(bool usingPreConnection)
        {
            this.config.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                AssertMatching(TestData.English.Weather.Utterance,
                    await helper.GetFirstRecognizerResult(recognizer));
            }
        }

        [TestMethod]
        public async Task InvalidKeyHandledProperly()
        {
            var configWithInvalidKey = SpeechConfig.FromSubscription("invalidKey", region);
            await AssertConnectionError(configWithInvalidKey, CancellationErrorCode.AuthenticationFailure, "WebSocket Upgrade failed with an authentication error (401)");
        }

        [TestMethod]
        public async Task InvalidTokenHandledProperly()
        {
            var invalidToken = "InvalidToken";
            var configWithInvalidToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            await AssertConnectionError(configWithInvalidToken, CancellationErrorCode.AuthenticationFailure, "WebSocket Upgrade failed with an authentication error (401)");
        }

        [TestMethod]
        public async Task InvalidRegionHandledProperly()
        {
            var configWithInvalidRegion = SpeechConfig.FromSubscription(subscriptionKey, "invalidRegion");
            await AssertConnectionError(configWithInvalidRegion, CancellationErrorCode.ConnectionFailure, "Connection failed");
        }

        [TestMethod]
        public void InvalidInputFileHandledProperly()
        {
            var audioInput = AudioConfig.FromWavFileInput("invalidFile.wav");
            Assert.ThrowsException<ApplicationException>(() => new SpeechRecognizer(this.config, audioInput));
        }

        [TestMethod]
        public async Task InvalidDeploymentIdHandledProperly()
        {
            this.config.EndpointId = "invalidDeploymentId";
            await AssertConnectionError(this.config, CancellationErrorCode.BadRequest, "WebSocket Upgrade failed with a bad request (400)");
        }

        [TestMethod]
        public async Task InvalidLanguageHandledProperly()
        {
            this.config.SpeechRecognitionLanguage = "InvalidLang";
            await AssertConnectionError(this.config, CancellationErrorCode.BadRequest, "WebSocket Upgrade failed with a bad request (400)");
        }

        [TestMethod]
        public async Task InvalidConnectionForContinuousRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                connection.Open(false);
                var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.StartContinuousRecognitionAsync());
                AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
            }
        }

        [TestMethod]
        public async Task InvalidConnectionForSingleShotRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                connection.Open(true);
                var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.RecognizeOnceAsync());
                AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task InvalidOpenOrCloseDuringRecognition(bool testingOpen)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                var taskCompletionSource = new TaskCompletionSource<int>();
                bool expectedExceptionCaught = false;
                recognizer.SpeechStartDetected += (s, e) =>
                {
                    try
                    {
                        if (testingOpen)
                        {
                            connection.Open(true);
                        }
                        else
                        {
                            connection.Close();
                        }
                    }
                    catch (Exception ex)
                    {
                        if (ex.Message.Contains("Exception with an error code: 0x1f"))
                        {
                            expectedExceptionCaught = true;
                        }
                    }
                    taskCompletionSource.TrySetResult(0);
                };
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await Task.WhenAny(taskCompletionSource.Task, Task.Delay(TimeSpan.FromMinutes(1)));
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                Assert.IsTrue(expectedExceptionCaught, "No expected exception is caught in connection.Open().");
            }
        }

        [TestMethod]
        public async Task TestCloseConnection()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connection.Close();
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task GermanRecognition(bool usingPreConnection)
        {
            this.config.SpeechRecognitionLanguage = Language.DE_DE;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(false);
                }
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.IsFalse(string.IsNullOrEmpty(result.Text), result.Reason.ToString());
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousGermanRecognition(bool usingPreConnection)
        {
            this.config.SpeechRecognitionLanguage = Language.DE_DE;
            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                var result = await helper.GetFirstRecognizerResult(recognizer);
                AssertMatching(TestData.German.FirstOne.Utterance, result);
            }
        }

        [DataTestMethod, TestCategory(TestCategory.LongRunning)]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousRecognitionOnLongFileInput(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                bool expectFirstRecognizingResult = true;
                long hypothesisLatency = 0;
                long phraseLatency = 0;

                if (usingPreConnection)
                {
                    connection.Open(true);
                }

                List<string> recognizedText = new List<string>();
                helper.SubscribeToCounterEventHandlers(recognizer, connection);
                recognizer.Recognizing += (s, e) =>
                {
                    var latencyString = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_RecognitionLatencyMs);
                    if (expectFirstRecognizingResult)
                    {
                        hypothesisLatency = 0;
                        if (!string.IsNullOrEmpty(latencyString))
                        {
                            hypothesisLatency = Convert.ToInt64(latencyString);
                        }
                        Assert.IsTrue(hypothesisLatency > 0, $"Invalid hypothesis latency {latencyString}.");
                        expectFirstRecognizingResult = false;
                    }
                    else
                    {
                        Assert.IsTrue(string.IsNullOrEmpty(latencyString), $"Unexpected hypothesis latency {latencyString}.");
                    }
                };
                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Text.Length > 0)
                    {
                        recognizedText.Add(e.Result.Text);
                    }
                    var latencyString = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_RecognitionLatencyMs);
                    phraseLatency = 0;
                    if (!string.IsNullOrEmpty(latencyString))
                    {
                        phraseLatency = Convert.ToInt64(latencyString);
                    }
                    Assert.IsTrue(phraseLatency > 0, $"Invalid phrase latency {latencyString}.");
                    Assert.IsTrue(phraseLatency > hypothesisLatency, $"Phrase latency ({phraseLatency}) is smaller than hypothesis latency ({hypothesisLatency}).");
                    expectFirstRecognizingResult = true;
                    hypothesisLatency = 0;
                };

                await helper.CompleteContinuousRecognition(recognizer);

                AssertConnectionCountMatching(helper.ConnectedEventCount, helper.DisconnectedEventCount);
                Assert.IsTrue(helper.RecognizedEventCount > 0, $"Invalid number of final result events {helper.RecognizedEventCount}");
                AssertEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
                AssertEqual(1, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                Assert.IsTrue(recognizedText.Count > 0, $"Invalid number of text messages {recognizedText.Count}");

                AssertMatching(TestData.English.Batman.Utterances[0], recognizedText[0]);
                AssertMatching(TestData.English.Batman.Utterances.Last(), recognizedText.Last());

                // It is not required to close the conneciton explictly. But it is also used to keep the connection object alive to ensure that
                // connected and disconencted events can be received.
                connection.Close();
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task SubscribeToManyEventHandlers(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                const int numLoops = 7;
                for (int i = 0; i < numLoops; i++)
                {
                    helper.SubscribeToCounterEventHandlers(recognizer);
                }
                await helper.CompleteContinuousRecognition(recognizer);

                AssertEqual(numLoops, helper.RecognizedEventCount, AssertOutput.WrongFinalResultCount);
                AssertEqual(numLoops, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                AssertEqual(numLoops, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                AssertEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task UnsubscribeFromEventHandlers(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                helper.SubscribeToCounterEventHandlers(recognizer, connection);
                helper.UnsubscribeFromCounterEventHandlers(recognizer, connection);

                await helper.CompleteContinuousRecognition(recognizer);

                AssertEqual(0, helper.ConnectedEventCount, AssertOutput.WrongConnectedEventCount);
                AssertEqual(0, helper.DisconnectedEventCount, AssertOutput.WrongDisconnectedEventCount);
                AssertEqual(0, helper.RecognizedEventCount, AssertOutput.WrongFinalResultCount);
                AssertEqual(0, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                AssertEqual(0, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                AssertEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ResubscribeToEventHandlers(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
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

                AssertEqual(diff, helper.RecognizedEventCount, AssertOutput.WrongFinalResultCount);
                AssertEqual(diff, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                AssertEqual(diff, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                AssertEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ChangeSubscriptionDuringRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                helper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.SessionStarted += (s, e) =>
                {
                    helper.UnsubscribeFromCounterEventHandlers(recognizer);
                };

                await helper.CompleteContinuousRecognition(recognizer);
                AssertEqual(1, helper.SessionStartedEventCount, AssertOutput.WrongSessionStartedCount);
                AssertEqual(0, helper.RecognizedEventCount, AssertOutput.WrongFinalResultCount);
                AssertEqual(0, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                AssertEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
            }
        }

        [TestMethod]
        public void TestGetters()
        {
            this.config.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                Assert.IsTrue(string.IsNullOrEmpty(recognizer.SpeechRecognitionLanguage));
                Assert.AreEqual(recognizer.SpeechRecognitionLanguage, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                Assert.AreEqual(deploymentId, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EndpointId));
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.config.SpeechRecognitionLanguage = Language.DE_DE;
            this.config.EndpointId = string.Empty;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                Assert.AreEqual(Language.DE_DE, recognizer.SpeechRecognitionLanguage);
                Assert.AreEqual(Language.DE_DE, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.config.OutputFormat = OutputFormat.Simple;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.config.OutputFormat = OutputFormat.Detailed;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                Assert.AreEqual(OutputFormat.Detailed, recognizer.OutputFormat);
            }
        }

        [TestMethod]
        public async Task TestExceptionSwitchFromSingleShotToContinuous()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.StartContinuousRecognitionAsync());
                AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
            }
        }

        [TestMethod]
        public async Task TestExceptionSwitchFromContinuousToSingleShot()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Canceled += (s, e) => { Console.WriteLine($"Recognition Canceled: {e.ToString()}"); };
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.RecognizeOnceAsync());
                AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
            }
        }

        [TestMethod]
        public async Task TestSingleShotTwice()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.IsTrue(result.Duration.Ticks > 0, result.Reason.ToString(), "First result duration should be greater than 0");
                var offset = result.OffsetInTicks;
                var expectedNextOffset = offset + result.Duration.Ticks;

                var result2 = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var offset2 = result2.OffsetInTicks;

                Console.WriteLine($"Offset1 {offset}, offset1 + duration {expectedNextOffset}");
                Console.WriteLine($"Offset2 {offset2}, its duration {result2.Duration.Ticks}");
                Console.WriteLine($"Result1: {result.ToString()}");
                Console.WriteLine($"Result2: {result2.ToString()}");

                Assert.AreEqual(result.Reason, ResultReason.RecognizedSpeech);
                AssertStringContains(result.Text, "detective skills");
                Assert.IsTrue(result.Duration.Ticks > 0, $"Result duration {result.Duration.Ticks} in {result.ToString()} should be greater than 0");

                Assert.AreEqual(result2.Reason, ResultReason.RecognizedSpeech);
                Assert.IsTrue(offset2 >= expectedNextOffset, $"Offset of the second recognition {offset2} should be greater or equal than offset of the first plus duration {expectedNextOffset}.");
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task TestStartStopManyTimes(bool usingPreConnection)
        {
            const int NumberOfIterations = 100;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                string canceled = string.Empty;
                recognizer.Canceled += (s, e) => { canceled = e.ErrorDetails; };
                for (int i = 0; i < NumberOfIterations; ++i)
                {
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition Canceled: {canceled}");
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task TestContinuousRecognitionTwice(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(true);
                }
                string canceled = string.Empty;
                recognizer.Canceled += (s, e) => { canceled = e.ErrorDetails; };
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition Canceled: {canceled}");
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task TestInitialSilenceTimeout(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Silence.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
                if (usingPreConnection)
                {
                    connection.Open(false);
                }
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.NoMatch, result.Reason);
                Assert.IsTrue(result.OffsetInTicks > 0, result.OffsetInTicks.ToString());
                Assert.IsTrue(string.IsNullOrEmpty(result.Text), result.Text);

                var noMatch = NoMatchDetails.FromResult(result);
                Assert.AreEqual(NoMatchReason.InitialSilenceTimeout, noMatch.Reason);
            }
        }

        [TestMethod]
        public void TestPropertyCollectionWithoutRecognizer()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Silence.AudioFile);
            PropertyCollection properties;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                properties = recognizer.Properties;
            }
            Assert.AreEqual("", properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token));
        }

        [TestMethod]
        public async Task TestContinuous44KHz()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Margarita.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                List<string> recognizedText = new List<string>();
                helper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Text.Length > 0)
                    {
                        recognizedText.Add(e.Result.Text);
                    }
                };

                await helper.CompleteContinuousRecognition(recognizer);

                Assert.AreEqual(TestData.English.Margarita.Utterance.Length, recognizedText.Count);
                for (var i = 0; i < recognizedText.Count; i++)
                {
                    AssertMatching(TestData.English.Margarita.Utterance[i], recognizedText[i]);
                }
                
            }
        }

        [TestMethod]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task AsyncRecognitionAfterDisposingSpeechRecognizer()
        {
            this.config.SpeechRecognitionLanguage = Language.DE_DE;
            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput));
            recognizer.Dispose();
            await recognizer.StartContinuousRecognitionAsync();
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void DisposingSpeechRecognizerWhileAsyncRecognition()
        {
            this.config.SpeechRecognitionLanguage = Language.EN;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput));
            recognizer = helper.GetSpeechRecognizingAsyncNotAwaited(recognizer);
        }
    }
}

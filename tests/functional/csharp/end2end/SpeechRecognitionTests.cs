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
using Microsoft.CognitiveServices.Speech;

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
        private static string endpointInString;
        private static Uri endpointUrl;
        private static string officeEndpointString;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
            deploymentId = Config.DeploymentId;
            endpointInString = String.Format("wss://{0}.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1", region);
            endpointUrl = new Uri(endpointInString);
            officeEndpointString = "wss://officespeech.platform.bing.com/speech/recognition/dictation/office/v1";
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

        [TestMethod]
        public async Task DefaultLanguageAndOutputFormatRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                Assert.IsTrue(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should not be set here. RecoLanguage: " + recoLanguage);

                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);

                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("language=en-us"), "Incorrect default language (should be en-us) in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                var bestResults = result.Best().ToArray();
                Assert.AreEqual(0, bestResults.Length, "There should be no detailed result for default output format");
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ValidBaselineRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(false);
                    }

                    var result = await helper.CompleteRecognizeOnceAsync(recognizer, connection).ConfigureAwait(false);
                    AssertConnectionCountMatching(helper.ConnectedEventCount, helper.DisconnectedEventCount);
                    Assert.IsTrue(result.Duration.Ticks > 0, result.Reason.ToString(), "Duration == 0");
                    Assert.IsTrue(100000 < result.OffsetInTicks && result.OffsetInTicks < 700000, $"Offset value ${result.OffsetInTicks} seems incorrect");
                    AssertMatching(TestData.English.Weather.Utterance, result.Text);
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousValidBaselineRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(true);
                    }
                    AssertMatching(
                        TestData.English.Weather.Utterance,
                        await helper.GetFirstRecognizerResult(recognizer));
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ValidCustomRecognition(bool usingPreConnection)
        {
            this.defaultConfig.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                Assert.IsTrue(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(false);
                    }
                    var result = await helper.CompleteRecognizeOnceAsync(recognizer, connection).ConfigureAwait(false);
                    AssertMatching(TestData.English.Weather.Utterance, result.Text);
                    AssertConnectionCountMatching(helper.ConnectedEventCount, helper.DisconnectedEventCount);
                    var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                    Assert.IsFalse(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousValidCustomRecognition(bool usingPreConnection)
        {
            this.defaultConfig.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(true);
                    }
                    AssertMatching(TestData.English.Weather.Utterance,
                        await helper.GetFirstRecognizerResult(recognizer));
                }
            }
        }

        [TestMethod]
        public async Task InvalidKeyHandledProperly()
        {
            var configWithInvalidKey = SpeechConfig.FromSubscription("invalidKey", region);
            await AssertConnectionError(configWithInvalidKey, CancellationErrorCode.AuthenticationFailure, "WebSocket Upgrade failed with an authentication error (401)");
        }

        [TestMethod]
        public async Task InvalidAuthTokenHandledProperly()
        {
            var invalidToken = "InvalidToken";
            var configWithInvalidToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            await AssertConnectionError(configWithInvalidToken, CancellationErrorCode.AuthenticationFailure, "WebSocket Upgrade failed with an authentication error (401)");
        }

        [TestMethod]
        public async Task ExpiredAuthTokenHandledProperly()
        {
            var expiredToken = "eyJhbGciOiJodHRwOi8vd3d3LnczLm9yZy8yMDAxLzA0L3htbGRzaWctbW9yZSNobWFjLXNoYTI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ1cm46bXMuY29nbml0aXZlc2VydmljZXMiLCJleHAiOiIxNTU0MzE1Nzk5IiwicmVnaW9uIjoibm9ydGhldXJvcGUiLCJzdWJzY3JpcHRpb24taWQiOiIwNmZlNjU2MWVkZTM0NDdiYTg2NDY5Njc4YTIwNTNkYiIsInByb2R1Y3QtaWQiOiJTcGVlY2hTZXJ2aWNlcy5TMCIsImNvZ25pdGl2ZS1zZXJ2aWNlcy1lbmRwb2ludCI6Imh0dHBzOi8vYXBpLmNvZ25pdGl2ZS5taWNyb3NvZnQuY29tL2ludGVybmFsL3YxLjAvIiwiYXp1cmUtcmVzb3VyY2UtaWQiOiIvc3Vic2NyaXB0aW9ucy8zYTk2ZWY1Ni00MWE5LTQwYTAtYjBmMy1mYjEyNWMyYjg3OTgvcmVzb3VyY2VHcm91cHMvY3NzcGVlY2hzZGstY2FyYm9uL3Byb3ZpZGVycy9NaWNyb3NvZnQuQ29nbml0aXZlU2VydmljZXMvYWNjb3VudHMvc3BlZWNoc2Rrbm9ydGhldXJvcGUiLCJzY29wZSI6InNwZWVjaHNlcnZpY2VzIiwiYXVkIjoidXJuOm1zLnNwZWVjaHNlcnZpY2VzLm5vcnRoZXVyb3BlIn0.hVAWT2YHjknFI6qLhnjmjzoNgOgxKWguuFhJLlyDxLU";
            var configWithInvalidToken = SpeechConfig.FromAuthorizationToken(expiredToken, region);
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
            Assert.ThrowsException<ApplicationException>(() => new SpeechRecognizer(this.defaultConfig, audioInput));
        }

        [TestMethod]
        public async Task InvalidDeploymentIdHandledProperly()
        {
            this.defaultConfig.EndpointId = "invalidDeploymentId";
            await AssertConnectionError(this.defaultConfig, CancellationErrorCode.BadRequest, "WebSocket Upgrade failed with a bad request (400)");
        }

        [TestMethod]
        public async Task InvalidLanguageHandledProperly()
        {
            this.defaultConfig.SpeechRecognitionLanguage = "InvalidLang";
            await AssertConnectionError(this.defaultConfig, CancellationErrorCode.BadRequest, "WebSocket Upgrade failed with a bad request (400)");
        }

        [TestMethod]
        public async Task InvalidConnectionForContinuousRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    connection.Open(false);
                    var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.StartContinuousRecognitionAsync());
                    AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
                }
            }
        }

        [TestMethod]
        public async Task InvalidConnectionForSingleShotRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    connection.Open(true);
                    var ex = await Assert.ThrowsExceptionAsync<ApplicationException>(() => recognizer.RecognizeOnceAsync());
                    AssertStringContains(ex.Message, "Exception with an error code: 0x1e");
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task InvalidOpenOrCloseDuringRecognition(bool testingOpen)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
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
        }

        [TestMethod]
        public async Task TestCloseConnection()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    connection.Close();
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task GermanRecognition(bool usingPreConnection)
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.DE_DE;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(false);
                    }
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                    AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousGermanRecognition(bool usingPreConnection)
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.DE_DE;
            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(true);
                    }
                    var result = await helper.GetFirstRecognizerResult(recognizer);
                    AssertMatching(TestData.German.FirstOne.Utterance, result);
                }
            }
        }

        [DataTestMethod, TestCategory(TestCategory.LongRunning)]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousRecognitionOnLongFileInput(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(true);
                    }

                    List<string> recognizedText = new List<string>();
                    bool expectFirstRecognizingResult = true;
                    long firstHypothesisLatency = 0;
                    string hypothesisLatencyError = string.Empty;
                    string phraseLatencyError = string.Empty;

                    helper.SubscribeToCounterEventHandlers(recognizer, connection);
                    recognizer.Recognizing += (s, e) =>
                    {
                        var latencyString = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_RecognitionLatencyMs);
                        if (expectFirstRecognizingResult)
                        {
                            if (!string.IsNullOrEmpty(latencyString))
                            {
                                firstHypothesisLatency = Convert.ToInt64(latencyString);
                            }
                            if (firstHypothesisLatency <= 0)
                            {
                                hypothesisLatencyError += $"({e.Result.ResultId}, invalid latency {firstHypothesisLatency})";
                            }
                            expectFirstRecognizingResult = false;
                        }
                        else
                        {
                            if (!string.IsNullOrEmpty(latencyString))
                            {
                                hypothesisLatencyError += $"({e.Result.ResultId}, latency in non-first hypothesis: {latencyString})";
                            }
                        }
                    };
                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Text.Length > 0)
                        {
                            recognizedText.Add(e.Result.Text);
                        }
                        var latencyString = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_RecognitionLatencyMs);
                        long val = 0;
                        if (!string.IsNullOrEmpty(latencyString))
                        {
                            val = Convert.ToInt64(latencyString);
                        }
                        if (val <= 0)
                        {
                            phraseLatencyError += $"({e.Result.ResultId}, invalid latency {val})";
                        }
                        expectFirstRecognizingResult = true;
                        firstHypothesisLatency = 0;
                    };

                    await helper.CompleteContinuousRecognition(recognizer);

                    AssertConnectionCountMatching(helper.ConnectedEventCount, helper.DisconnectedEventCount);
                    Assert.IsTrue(helper.RecognizedEventCount > 0, $"Invalid number of final result events {helper.RecognizedEventCount}");
                    AssertEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
                    AssertEqual(1, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                    Assert.IsTrue(recognizedText.Count > 0, $"Invalid number of text messages {recognizedText.Count}");

                    AssertMatching(TestData.English.Batman.Utterances[0], recognizedText[0]);
                    AssertMatching(TestData.English.Batman.Utterances.Last(), recognizedText.Last());

                    Assert.IsTrue(string.IsNullOrEmpty(hypothesisLatencyError), $"hypothesisLatencyError: {hypothesisLatencyError}");
                    Assert.IsTrue(string.IsNullOrEmpty(phraseLatencyError), $"phraseLatencyError: {phraseLatencyError}");

                    GC.KeepAlive(connection);
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task SubscribeToManyEventHandlers(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
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
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task UnsubscribeFromEventHandlers(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
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
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ResubscribeToEventHandlers(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
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
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ChangeSubscriptionDuringRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
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
        }

        [TestMethod]
        public void TestGetters()
        {
            this.defaultConfig.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                Assert.IsTrue(string.IsNullOrEmpty(recognizer.SpeechRecognitionLanguage), $"No language should be set, is {recognizer.SpeechRecognitionLanguage}");
                Assert.AreEqual(recognizer.SpeechRecognitionLanguage, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                Assert.AreEqual(deploymentId, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EndpointId));
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.defaultConfig.SpeechRecognitionLanguage = Language.DE_DE;
            this.defaultConfig.EndpointId = string.Empty;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                Assert.AreEqual(Language.DE_DE, recognizer.SpeechRecognitionLanguage);
                Assert.AreEqual(Language.DE_DE, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.defaultConfig.OutputFormat = OutputFormat.Simple;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                Assert.AreEqual(OutputFormat.Detailed, recognizer.OutputFormat);
            }
        }

        [TestMethod]
        public async Task TestExceptionSwitchFromSingleShotToContinuous()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
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
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
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
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
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

                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertStringContains(result.Text, "detective skills");
                Assert.IsTrue(result.Duration.Ticks > 0, $"Result duration {result.Duration.Ticks} in {result.ToString()} should be greater than 0");

                Assert.AreEqual(ResultReason.RecognizedSpeech, result2.Reason);
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
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
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
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
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
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task TestInitialSilenceTimeout(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Silence.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(false);
                    }
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.AreEqual(ResultReason.NoMatch, result.Reason);
                    Assert.IsTrue(result.OffsetInTicks > 0 || result.Duration.Ticks > 0, $"Bad offset: {result.OffsetInTicks} or duration: {result.Duration}");
                    Assert.IsTrue(string.IsNullOrEmpty(result.Text), $"Bad result text: {result.Text}");

                    var noMatch = NoMatchDetails.FromResult(result);
                    Assert.AreEqual(NoMatchReason.InitialSilenceTimeout, noMatch.Reason);
                }
            }
        }

        [TestMethod]
        public void TestPropertyCollectionWithoutRecognizer()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Silence.AudioFile);
            PropertyCollection properties;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                properties = recognizer.Properties;
            }
            Assert.AreEqual("", properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token));
        }

        [TestMethod]
        public async Task TestContinuous44KHz()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Margarita.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
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

                AssertFuzzyMatching(TestData.English.Margarita.Utterance, recognizedText.ToArray(), 5);
            }
        }

        [TestMethod]
        public void CloseConnectionWithoutOpen()
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.DE_DE;
            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            var recognizer = new SpeechRecognizer(this.defaultConfig, audioInput);
            using (var connection = Connection.FromRecognizer(recognizer))
            {
                // Close the connection without opening it.
                connection.Close();
            }
        }

        [TestMethod]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task AsyncRecognitionAfterDisposingSpeechRecognizer()
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.DE_DE;
            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput));
            recognizer.Dispose();
            await recognizer.StartContinuousRecognitionAsync();
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void DisposingSpeechRecognizerWhileAsyncRecognition()
        {
            this.defaultConfig.SpeechRecognitionLanguage = Language.EN;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput));
            recognizer = helper.GetSpeechRecognizingAsyncNotAwaited(recognizer);
        }

        [TestMethod]
        public async Task FromEndpointGermanRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.SpeechRecognitionLanguage = Language.DE_DE;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
            }
        }

        [TestMethod]
        public async Task FromEndpointCustomRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                Assert.IsTrue(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsFalse(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);
            }
        }

        [TestMethod]
        public async Task FromEndpointDefaultLanguage()
        {
            var endpointWithDeploymentId = endpointInString + "?cid=" + deploymentId;
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithDeploymentId), subscriptionKey);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                Assert.IsTrue(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);

                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsFalse(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);

            }
        }

        [TestMethod]
        public async Task FromEndpointDetailedRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                AssertDetailedResult(result);
            }
        }

        [TestMethod]
        public async Task FromEndpointGermanRecognitionWithPropertyOverwrite()
        {
            var endpointWithLang = endpointInString + "?language=de-de";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithLang), subscriptionKey);
            // The property should not overwrite the query parameter in url.
            configFromEndpoint.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
            }
        }

        [TestMethod]
        public async Task FromEndpointCustomRecognitionWithPropertyOverwrite()
        {
            var endpointWithDeploymentId = endpointInString + "?cid=" + deploymentId;
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithDeploymentId), subscriptionKey);
            // The endpointId below is an invalid one.
            configFromEndpoint.EndpointId = "3a2ff182-b6ca-4888-a7d0-7a2de3141572";
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
            }
        }

        [TestMethod]
        public async Task FromEndpointDetailedRecognitionWithPropertyOverwrite()
        {
            var endpointWithOutputFormat = endpointInString + "?format=detailed";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithOutputFormat), subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Simple;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                AssertDetailedResult(result);
            }
        }

        [TestMethod]
        public async Task SetServicePropertySingleSetting()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyOverwrite()
        {
            this.defaultConfig.SpeechRecognitionLanguage = "en-US";
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
            }
        }

        [TestMethod]
        public async Task SetServiceProperty2Properties()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            this.defaultConfig.SetServiceProperty("format", "detailed", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
                AssertDetailedResult(result);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyFromEndpoint()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyFromEndpointWithParameter()
        {
            var endpointWithOutputFormat = endpointInString + "?format=detailed";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithOutputFormat), subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Simple;
            configFromEndpoint.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, result.Text);
                AssertDetailedResult(result);
            }
        }

        [TestMethod]
        public void SetServicePropertyInvalidParameters()
        {
            Assert.ThrowsException<ApplicationException>(() => this.defaultConfig.SetServiceProperty(null, "value", ServicePropertyChannel.UriQueryParameter));
            Assert.ThrowsException<ApplicationException>(() => this.defaultConfig.SetServiceProperty("", "value", ServicePropertyChannel.UriQueryParameter));
            Assert.ThrowsException<ApplicationException>(() => this.defaultConfig.SetServiceProperty("Name", null, ServicePropertyChannel.UriQueryParameter));
            Assert.ThrowsException<ApplicationException>(() => this.defaultConfig.SetServiceProperty("Name", "", ServicePropertyChannel.UriQueryParameter));
        }

        [TestMethod]
        public async Task DictationRecognition()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Punctuation.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                List<string> recognizedText = new List<string>();
                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Received result '{e.ToString()}'");
                    if (e.Result.Text.Length > 0)
                    {
                        recognizedText.Add(e.Result.Text);
                    }
                };

                await helper.CompleteContinuousRecognition(recognizer);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                Assert.AreEqual(1, recognizedText.Count, "The number of recognized texts is not 1, but " + recognizedText.Count);
                AssertMatching(TestData.English.Punctuation.Utterance, recognizedText[0]);
            }
        }

        [TestMethod]
        public async Task DictationRecognizeOnce()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Punctuation.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Punctuation.Utterance, result.Text);
            }
        }

        [TestMethod]
        public async Task DictationAllowSwitchRecognizeOnceAndContinuous()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Batman.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);

                var taskCompletionSource = new TaskCompletionSource<int>();
                recognizer.SessionStopped += (s, e) =>
                {
                    taskCompletionSource.TrySetResult(0);
                };
                string canceled = string.Empty;
                recognizer.Canceled += (s, e) =>
                {
                    canceled = e.ErrorDetails;
                    taskCompletionSource.TrySetResult(0);
                };
                recognizer.Recognized += (s, e) =>
                {
                    taskCompletionSource.TrySetResult(0);
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await Task.WhenAny(taskCompletionSource.Task, Task.Delay(TimeSpan.FromMinutes(3)));
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition Canceled: {canceled}");
                }
            }
        }

        [TestMethod]
        public async Task ProfanityMaskedRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Profanity.AudioFile);
            this.defaultConfig.SetProfanity(ProfanityOption.Masked);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                Assert.AreEqual(TestData.English.Profanity.MaskedUtterance, result.Text);
            }
        }

        [TestMethod]
        public async Task ProfanityRemovedRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Profanity.AudioFile);
            this.defaultConfig.SetProfanity(ProfanityOption.Removed);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                Assert.AreEqual(TestData.English.Profanity.RemovedUtterance, result.Text);
            }
        }

        [TestMethod]
        public async Task ProfanityRawRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Profanity.AudioFile);
            this.defaultConfig.SetProfanity(ProfanityOption.Raw);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(TestData.English.Profanity.RawUtterance, result.Text);
            }
        }

        [TestMethod]
        public async Task DictationCorrectionsSetParameter()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var config = SpeechConfig.FromEndpoint(new Uri(officeEndpointString), subscriptionKey);
            config.SetServiceProperty("format", "corrections", ServicePropertyChannel.UriQueryParameter);
            config.EnableDictation();

            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    recognizer.AuthorizationToken = "abc";

                    var phraseDetectionPayload = "{\"mode\": \"dictation\", \"grammarScenario\": \"Dictation_Office\",\"initialSilenceTimeout\": 2000,\"trailingSilenceTimeout\": 2000}";
                    connection.SetMessageParameter("speech.context", "phraseDetection", phraseDetectionPayload);

                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.IsTrue(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult).Contains("Corrections"));
                }
            }
        }

        [TestMethod]
        public async Task DictationSendMessage()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var config = SpeechConfig.FromEndpoint(new Uri(officeEndpointString), subscriptionKey);
            config.SetServiceProperty("format", "corrections", ServicePropertyChannel.UriQueryParameter);
            config.EnableDictation();

            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    recognizer.AuthorizationToken = "abc";

                    string payload = @"{
                ""Id"": ""Corrections"",
                ""Name"": ""Telemetry"",
                ""ClientSessionId"": ""DADAAAC4-019C-4D23-9301-7FD619BE68AB"",
                ""CorrectionEvents"": [
                    {
                        ""PhraseId"": ""AEDC2194-019C-4D23-9301-7FD619BE68A9"",
                        ""CorrectionId"": 0,
                        ""AlternateId"": 1,
                        ""TreatedInUX"": ""true"",
                        ""TriggerType"": ""click"",
                        ""EditType"": ""alternate""
                    },
                    {
                        ""PhraseId"": ""BEDC2194-019C-4D23-9301-7FD619BE68AA"",
                        ""CorrectionId"": 0,
                        ""AlternateId"": 2,
                        ""TriggerType"": ""hover"",
                        ""TreatedInUX"": ""false"",
                        ""EditType"": ""alternate""
                    }]}";
                    await connection.SendMessageAsync("event", payload);

                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.IsTrue(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult).Contains("Corrections"));
                }
            }
        }

        [TestMethod]
        public async Task DictationCorrectionsRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var config = SpeechConfig.FromEndpoint(new Uri(officeEndpointString), subscriptionKey);
            config.SetServiceProperty("format", "corrections", ServicePropertyChannel.UriQueryParameter);
            config.EnableDictation();

            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                recognizer.AuthorizationToken = "abc";
                // with a . in the end of left context, the first letter of the recognized text is capital case.
                recognizer.Properties.SetProperty("DictationInsertionPointLeft", "left context.");
                recognizer.Properties.SetProperty("DictationInsertionPointRight", "right context.");

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                if (result.Reason == ResultReason.Canceled)
                {
                    var cancellation = CancellationDetails.FromResult(result);
                    Console.WriteLine("First reco attempt failed (details=%s).", cancellation.ErrorDetails);
                }
                Assert.IsTrue(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult).Contains("Corrections"));
                Assert.IsTrue(result.Text.StartsWith("W"));
            }
        }

        [TestMethod]
        public async Task DictationCorrectionsContinuousRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var config = SpeechConfig.FromEndpoint(new Uri(officeEndpointString), subscriptionKey);
            config.SetServiceProperty("format", "corrections", ServicePropertyChannel.UriQueryParameter);
            config.EnableDictation();

            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                recognizer.AuthorizationToken = "abc";
                // without a . in the end of left context, the first letter of the recognized text is lower case.
                recognizer.Properties.SetProperty("DictationInsertionPointLeft", "left context \" with a quota");

                string json = string.Empty;
                string text = string.Empty;
                var taskCompletionSource = new TaskCompletionSource<int>();
                recognizer.SessionStopped += (s, e) =>
                {
                    taskCompletionSource.TrySetResult(0);
                };
                string canceled = string.Empty;
                recognizer.Canceled += (s, e) =>
                {
                    canceled = e.ErrorDetails;
                    Console.WriteLine("Recognition was canceled (details=%s).", canceled);
                    taskCompletionSource.TrySetResult(0);
                };
                recognizer.Recognized += (s, e) =>
                {
                    json = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                    text = e.Result.Text;
                    taskCompletionSource.TrySetResult(0);
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                await Task.WhenAny(taskCompletionSource.Task, Task.Delay(TimeSpan.FromMinutes(3)));
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                Assert.IsTrue(json.Contains("Corrections"));
                Assert.IsTrue(text.StartsWith("w"));
            }
        }

        [TestMethod]
        public async Task WordLevelTiming()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "true");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason, $"Expected status: RecognizedSpeech, actual status: {result.Reason}");
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                foreach (var detailedResult in result.Best())
                {
                    Assert.IsTrue(detailedResult.Text.Length > 0, $"Empty Text. Json result: {jsonResult}");
                    Assert.IsTrue(detailedResult.LexicalForm.Length > 0, $"Empty LexicalForm. Json result: {jsonResult}");
                    Assert.IsTrue(detailedResult.NormalizedForm.Length > 0, $"Empty NormallizedForm. Json result: {jsonResult}");
                    Assert.IsTrue(detailedResult.MaskedNormalizedForm.Length > 0, $"Empty MaskedNormalizedForm. Json result: {jsonResult}");
                    Assert.IsTrue(detailedResult.Words.Count() > 0, $"No words available. Json result: {jsonResult}");
                    foreach (var word in detailedResult.Words)
                    {
                        Assert.IsTrue(word.Offset > 0, $"The word {word.Word} has incorrect offset: {word.Offset}. Json result: {jsonResult}");
                        Assert.IsTrue(word.Duration > 0, $"The word {word.Word} has incorrect duration: {word.Duration}.Json result: {jsonResult}");
                    }
                }
            }
        }

        [TestMethod]
        public async Task WordLevelOffsetInMultiTurn()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.WeatherMultiTurns.AudioFile);
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "true");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, "3000");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                List<string> recognizedText = new List<string>();
                List<long> utteranceOffsets = new List<long>();
                List<long> utteranceDurations = new List<long>();
                List<string> jsonResult = new List<string>();
                List<DetailedSpeechRecognitionResult> detailedResults = new List<DetailedSpeechRecognitionResult>();
                string hypothesisLatencyError = string.Empty;
                string phraseLatencyError = string.Empty;

                helper.SubscribeToCounterEventHandlers(recognizer);
                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Text.Length > 0)
                    {
                        recognizedText.Add(e.Result.Text);
                        utteranceOffsets.Add(e.Result.OffsetInTicks);
                        utteranceDurations.Add(e.Result.Duration.Ticks);
                        jsonResult.Add(e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult));
                    }
                    if (e.Result.Best().Count() > 0)
                    {
                        detailedResults.Add(e.Result.Best().ElementAt(0));
                    }
                };

                await helper.CompleteContinuousRecognition(recognizer);

                AssertEqual(3, recognizedText.Count, $"Number of recognized text does not match: {recognizedText.Count}");
                AssertEqual(3, detailedResults.Count, $"Number of detailed results does not match: {detailedResults.Count}");
                AssertEqual(3, utteranceOffsets.Count, $"Number of utterance offsets does not match: {detailedResults.Count}");

                long utteranceOffset = 0;
                long utteranceDuration = 0;
                long wordOffset = 0;
                long wordDuration = 0;
                for (var i = 0; i < recognizedText.Count; i++)
                {
                    AssertMatching(TestData.English.WeatherMultiTurns.Utterances[i], recognizedText[i]);
                    Assert.IsTrue(utteranceOffsets[i] >= utteranceOffset + utteranceDuration, $"Utterance offset must be in ascending order. the previous offset {utteranceOffset}, the previous duration {utteranceDuration}, the current offset {utteranceOffsets[i]}.");
                    if (i >= 1)
                    {
                        Assert.IsTrue(utteranceOffsets[i] - utteranceOffset - utteranceDuration > 3 * 10000000, $"The silence between 2 utterances must be longer than 3s. The previous offset {utteranceOffset}, the previous duration {utteranceDuration}, the current offset {utteranceOffsets[i]}.");
                    }
                    utteranceOffset = utteranceOffsets[i];
                    utteranceDuration = utteranceDurations[i];

                    Assert.IsTrue(detailedResults[i].Text.Length > 0, $"Empty Text in detailed result {i}. Json result: {jsonResult[i]}");
                    Assert.IsTrue(detailedResults[i].LexicalForm.Length > 0, $"Empty LexicalForm in detailed result {i}. Json result: {jsonResult[i]}");
                    Assert.IsTrue(detailedResults[i].NormalizedForm.Length > 0, $"Empty NormallizedForm in detailed result {i}. Json result: {jsonResult[i]}");
                    Assert.IsTrue(detailedResults[i].MaskedNormalizedForm.Length > 0, $"Empty MaskedNormalizedForm in detailed result {i}. Json result: {jsonResult[i]}");
                    Assert.IsTrue(detailedResults[i].Words.Count() > 0, $"No words available in detailed result {i}. Json result: {jsonResult[i]}");
                    foreach (var word in detailedResults[i].Words)
                    {
                        Assert.IsTrue(word.Offset >= wordOffset + wordDuration, $"The word offset must be in ascending order. the previous offset {wordOffset}, the previous duration {wordDuration}, the current offset {word.Offset}. Json result: {jsonResult[i]}");
                        Assert.IsTrue(word.Duration > 0, $"The word {word.Word} has incorrect duration: {word.Duration}. Json result: {jsonResult[i]}");
                        wordOffset = word.Offset;
                        wordDuration = word.Duration;
                    }
                }
            }
        }

    }
}

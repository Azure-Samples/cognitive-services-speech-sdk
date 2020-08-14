//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using System.IO;
    using static AssertHelpers;
    using static Config;
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
            deploymentId = DefaultSettingsMap[DefaultSettingKeys.DEPLOYMENT_ID];
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            using (var speechRecognizer = new SpeechRecognizer(configWithToken, audioInput))
            {
                Assert.AreEqual(invalidToken, speechRecognizer.AuthorizationToken);

                var newToken = await GetToken(subscriptionKey, region);
                speechRecognizer.AuthorizationToken = newToken;
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(newToken, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                Assert.IsTrue(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should not be set here. RecoLanguage: " + recoLanguage);

                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);

                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("language=en-us"), "Incorrect default language (should be en-us) in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var bestResults = result.Best().ToArray();
                Assert.AreEqual(0, bestResults.Length, "There should be no detailed result for default output format");
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ValidBaselineRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                }
            }
        }

        [Ignore]
        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ValidBaselineRecognitionWithSovereignCloud(bool usingPreConnection)
        {
            var mooncakeSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Key;
            var mooncakeRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Region;

            var mooncakeConfig = SpeechConfig.FromSubscription(mooncakeSubscriptionKey, mooncakeRegion);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(mooncakeConfig, audioInput)))
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
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousValidBaselineRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(true);
                    }
                    AssertMatching(
                        AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text,
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(true);
                    }
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text,
                        await helper.GetFirstRecognizerResult(recognizer));
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousValidSkipAudioRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProperty("SPEECH-SkipAudioDurationHNS", "5000000");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                string plainExpectedText = Normalize(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text);
                string plainActualText = Normalize(await helper.GetFirstRecognizerResult(recognizer));

                plainExpectedText = plainExpectedText.Remove(0, 10);

                Assert.IsTrue(
                        plainExpectedText.Equals(plainActualText),
                            $"'{plainExpectedText}' (expected)\n is not equals \n'{plainActualText}' (actual)");
            }
        }

        [TestMethod]
        public async Task InvalidKeyHandledProperly()
        {
            await AssertConnectionError(
                SpeechConfig.FromSubscription("invalidKey", region),
                CancellationErrorCode.AuthenticationFailure,
                "WebSocket upgrade failed", "authentication error", "401");
        }

        [TestMethod]
        public async Task InvalidAuthTokenHandledProperly()
        {
            var invalidToken = "InvalidToken";
            await AssertConnectionError(
                SpeechConfig.FromAuthorizationToken(invalidToken, region),
                CancellationErrorCode.AuthenticationFailure,
                "WebSocket upgrade failed", "authentication error", "401");
        }

        [TestMethod]
        public async Task ExpiredAuthTokenHandledProperly()
        {
            var expiredToken = "eyJhbGciOiJodHRwOi8vd3d3LnczLm9yZy8yMDAxLzA0L3htbGRzaWctbW9yZSNobWFjLXNoYTI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ1cm46bXMuY29nbml0aXZlc2VydmljZXMiLCJleHAiOiIxNTU0MzE1Nzk5IiwicmVnaW9uIjoibm9ydGhldXJvcGUiLCJzdWJzY3JpcHRpb24taWQiOiIwNmZlNjU2MWVkZTM0NDdiYTg2NDY5Njc4YTIwNTNkYiIsInByb2R1Y3QtaWQiOiJTcGVlY2hTZXJ2aWNlcy5TMCIsImNvZ25pdGl2ZS1zZXJ2aWNlcy1lbmRwb2ludCI6Imh0dHBzOi8vYXBpLmNvZ25pdGl2ZS5taWNyb3NvZnQuY29tL2ludGVybmFsL3YxLjAvIiwiYXp1cmUtcmVzb3VyY2UtaWQiOiIvc3Vic2NyaXB0aW9ucy8zYTk2ZWY1Ni00MWE5LTQwYTAtYjBmMy1mYjEyNWMyYjg3OTgvcmVzb3VyY2VHcm91cHMvY3NzcGVlY2hzZGstY2FyYm9uL3Byb3ZpZGVycy9NaWNyb3NvZnQuQ29nbml0aXZlU2VydmljZXMvYWNjb3VudHMvc3BlZWNoc2Rrbm9ydGhldXJvcGUiLCJzY29wZSI6InNwZWVjaHNlcnZpY2VzIiwiYXVkIjoidXJuOm1zLnNwZWVjaHNlcnZpY2VzLm5vcnRoZXVyb3BlIn0.hVAWT2YHjknFI6qLhnjmjzoNgOgxKWguuFhJLlyDxLU";
            await AssertConnectionError(
                SpeechConfig.FromAuthorizationToken(expiredToken, region),
                CancellationErrorCode.AuthenticationFailure,
                "WebSocket upgrade failed", "authentication error", "401");
        }

        [TestMethod]
        public async Task InvalidSubscriptionKeyHandledProperly()
        {
            var configWithInvalidRegion = SpeechConfig.FromSubscription("invalid_subscription_key", RecognitionTestBase.region);
            // "websocket upgrade", "authentication", "subscription";
            // we use case insensitive comparison here since on Linux the error message is: "DNS connection failed (the remote host did not respond)"
            await AssertConnectionError(configWithInvalidRegion, CancellationErrorCode.AuthenticationFailure, "websocket upgrade", "authentication", "subscription");
        }

        [TestMethod]
        public async Task InvalidRegionHandledProperly()
        {
            var configWithInvalidRegion = SpeechConfig.FromSubscription(subscriptionKey, "invalidRegion");

            // we use case insensitive comparison here since on Linux the error message is: "DNS connection failed (the remote host did not respond)"
            await AssertConnectionError(configWithInvalidRegion, CancellationErrorCode.ConnectionFailure, "connection failed");
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
            await AssertConnectionError(this.defaultConfig, CancellationErrorCode.BadRequest, "WebSocket upgrade failed", "bad request", "400");
        }

        [TestMethod]
        public async Task InvalidLanguageHandledProperly()
        {
            this.defaultConfig.SpeechRecognitionLanguage = "InvalidLang";
            await AssertConnectionError(this.defaultConfig, CancellationErrorCode.BadRequest, "WebSocket upgrade failed", "bad request", "400");
        }

        [TestMethod]
        public async Task InvalidConnectionForContinuousRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.DE_DE, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(false);
                    }
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task GermanRecognitionWithSourceLangConfig(bool usingPreConnection)
        {
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, SourceLanguageConfig.FromLanguage(Language.DE_DE), AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(false);
                    }
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                }
            }
        }

        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousGermanRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    if (usingPreConnection)
                    {
                        connection.Open(true);
                    }
                    var result = await helper.GetFirstRecognizerResult(recognizer);
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result);
                }
            }
        }

        [DataTestMethod, TestCategory(TestCategory.LongRunning)]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ContinuousRecognitionOnLongFileInput(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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

                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, recognizedText[0]);
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Last().Text, recognizedText.Last());

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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                Assert.IsTrue(string.IsNullOrEmpty(recognizer.SpeechRecognitionLanguage), $"No language should be set, is {recognizer.SpeechRecognitionLanguage}");
                Assert.AreEqual(recognizer.SpeechRecognitionLanguage, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, SourceLanguageConfig.FromLanguage(Language.DE_DE, deploymentId), audioInput)))
            {
                Assert.AreEqual(Language.DE_DE, recognizer.SpeechRecognitionLanguage);
                Assert.AreEqual(recognizer.SpeechRecognitionLanguage, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                Assert.AreEqual(deploymentId, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EndpointId));
                Assert.AreEqual(OutputFormat.Simple, recognizer.OutputFormat);
            }

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput)))
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
                AssertStringContains(result.Text.ToLower(), "detective skills");
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SHORT_SILENCE].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SHORT_SILENCE].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.AUDIO_44_1KHZ].FilePath.GetRootRelativePath());
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

                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.AUDIO_44_1KHZ].Utterances[Language.EN].Select(x => x.Text).ToArray(), recognizedText.ToArray(), 5);
            }
        }

        [TestMethod]
        public void CloseConnectionWithoutOpen()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            var recognizer = new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput);
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput));
            recognizer.Dispose();
            await recognizer.StartContinuousRecognitionAsync();
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void DisposingSpeechRecognizerWhileAsyncRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.EN, audioInput));
            recognizer = helper.GetSpeechRecognizingAsyncNotAwaited(recognizer);
        }

        [TestMethod]
        public async Task FromEndpointGermanRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, Language.DE_DE, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task FromEndpointCustomRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                Assert.IsTrue(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsFalse(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);
            }
        }

        [TestMethod]
        public async Task FromEndpointDefaultLanguage()
        {
            var endpointWithDeploymentId = endpointInString + "?cid=" + deploymentId;
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithDeploymentId), subscriptionKey);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                Assert.IsTrue(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);

                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsFalse(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);

            }
        }

        [TestMethod]
        public async Task FromEndpointDetailedRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertDetailedResult(result);
            }
        }

        [TestMethod]
        public async Task FromEndpointGermanRecognitionWithPropertyOverwrite()
        {
            var endpointWithLang = endpointInString + "?language=de-de";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithLang), subscriptionKey);
            // The property should not overwrite the query parameter in url.
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, Language.EN, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task FromEndpointCustomRecognitionWithPropertyOverwrite()
        {
            var endpointWithDeploymentId = endpointInString + "?cid=" + deploymentId;
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithDeploymentId), subscriptionKey);
            // The endpointId below is an invalid one.
            configFromEndpoint.EndpointId = "3a2ff182-b6ca-4888-a7d0-7a2de3141572";
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task FromEndpointDetailedRecognitionWithPropertyOverwrite()
        {
            var endpointWithOutputFormat = endpointInString + "?format=detailed";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithOutputFormat), subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Simple;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertDetailedResult(result);
            }
        }

        [TestMethod]
        public async Task SetServicePropertySingleSetting()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyOverwrite()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.EN, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task SetServiceProperty2Properties()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            this.defaultConfig.SetServiceProperty("format", "detailed", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                AssertDetailedResult(result);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyFromEndpoint()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyFromEndpointWithParameter()
        {
            var endpointWithOutputFormat = endpointInString + "?format=detailed";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithOutputFormat), subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Simple;
            configFromEndpoint.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
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
        public async Task TestExceptionsDuringEventsRecognizeOnceAsync()
        {
            SpeechConfig config = SpeechConfig.FromSubscription(subscriptionKey, region);
            Assert.IsNotNull(config);
            using (SpeechRecognizer recognizer = new SpeechRecognizer(config, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath())))
            {
                Assert.IsNotNull(recognizer);
                using (Connection connection = Connection.FromRecognizer(recognizer))
                {
                    var tcs = new TaskCompletionSource<int>();

                    connection.Connected += (s, e) =>
                    {
                        Console.WriteLine("Now connected. Preparing throw exception");
                        throw new Exception("Connected");
                    };

                    connection.Disconnected += (s, e) =>
                    {
                        Console.WriteLine("Now disconnected. Preparing throw exception");
                        throw new Exception("Disconnected");
                    };
                    recognizer.Recognized += (s, e) =>
                    {
                        Console.WriteLine("Now recognized. Preparing throw exception");
                        throw new Exception("Recognized");
                    };

                    recognizer.Recognizing += (s, e) =>
                    {
                        Console.WriteLine("Now recognizing. Preparing throw exception");
                        throw new Exception("Recognizing");
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine("Now canceled. Preparing throw exception");
                        throw new Exception("Canceled");
                    };

                    recognizer.SpeechStartDetected += (s, e) =>
                    {
                        Console.WriteLine("Now speechstartdetected. Preparing throw exception");
                        throw new Exception("SpeechStartDetected");
                    };

                    recognizer.SpeechEndDetected += (s, e) =>
                    {
                        Console.WriteLine("Now speechenddetected. Preparing throw exception");
                        throw new Exception("SpeechEndDetected");
                    };

                    recognizer.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine("Now sessionstarted. Preparing throw exception");
                        throw new Exception("SessionStarted");
                    };

                    recognizer.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine("Now sessionstopped. Preparing throw exception");
                        tcs.TrySetResult(0);
                        throw new Exception("SessionStopped");
                    };


                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.IsNotNull(result);
                    Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                    await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromMinutes(1)));
                }
            }
        }

        [TestMethod]
        public async Task DictationRecognition()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].FilePath.GetRootRelativePath());
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
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].Utterances[Language.EN][0].Text, recognizedText[0]);
            }
        }

        [TestMethod]
        public async Task DictationRecognitionWithLocalSpeechDetection()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].FilePath.GetRootRelativePath());
            ManualResetEvent startEvent = new ManualResetEvent(false);
            ManualResetEvent endEvent = new ManualResetEvent(false);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Properties.SetProperty("UseLocalSpeechDetection", "true");
                List<string> recognizedText = new List<string>();
                recognizer.SpeechStartDetected += (sender, e) =>
                {
                    startEvent.Set();
                };

                recognizer.SpeechEndDetected += (sender, e) =>
                {
                    endEvent.Set();
                };

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
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].Utterances[Language.EN][0].Text, recognizedText[0]);
                bool start = startEvent.WaitOne(100);
                bool end = endEvent.WaitOne(100);
                Assert.AreEqual(true, start, "expected SpeechStartDetected but did not receive");
                Assert.AreEqual(true, end, "expected SpeechEndDetected but did not receive");
            }
        }

        [TestMethod]
        public async Task DictationRecognizeOnce()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task DictationAllowSwitchRecognizeOnceAndContinuous()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProfanity(ProfanityOption.Masked);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                Assert.IsFalse(String.IsNullOrEmpty(result.Text), "result.Text was null or empty.");
                AssertIfContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityRaw);
                AssertIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityMaskedPattern);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityMasked, result.Text);
            }
        }

        // TODO: THESE PROFANITY TESTS DON'T MAKE SENSE NEED TO RE-EXAMINE
        [TestMethod]
        public async Task ProfanityRemovedRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProfanity(ProfanityOption.Removed);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertIfContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].Text);
                WarnIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityRemoved);
            }
        }

        [TestMethod]
        public async Task ProfanityRawRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProfanity(ProfanityOption.Raw);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1].Utterances[Language.EN][0].Text, result.Text);
                WarnIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1].Utterances[Language.EN][0].ProfanityRaw);
            }
        }

        [TestMethod]
        public async Task DictationCorrectionsSetParameter()
        {
            // right now, Office endpoint takes any string as authorization token.
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var config = SpeechConfig.FromEndpoint(new Uri(officeEndpointString), subscriptionKey);
            config.SetServiceProperty("format", "corrections", ServicePropertyChannel.UriQueryParameter);
            config.SetServiceProperty("Authorization", "POPToken", ServicePropertyChannel.HttpHeader);
            config.EnableDictation();

            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                recognizer.Properties.SetProperty("HttpHeader#Authorization", "fromRecognizer");
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    var phraseDetectionPayload = "{\"mode\": \"dictation\", \"grammarScenario\": \"Dictation_Office\",\"initialSilenceTimeout\": 2000,\"trailingSilenceTimeout\": 2000}";
                    connection.SetMessageProperty("speech.context", "phraseDetection", phraseDetectionPayload);

                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Assert.IsTrue(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult).Contains("Corrections"));
                }
            }
        }

        [TestMethod]
        public async Task DictationSendMessage()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "true");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason, $"Expected status: RecognizedSpeech, actual status: {result.Reason}");
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                foreach (var detailedResult in result.Best())
                {
                    Assert.IsTrue(detailedResult.Text.Length > 0, $"Empty Text. Json result: {jsonResult}");
                    Assert.IsTrue(detailedResult.LexicalForm.Length > 0, $"Empty LexicalForm. Json result: {jsonResult}");
                    Assert.IsTrue(detailedResult.NormalizedForm.Length > 0, $"Empty NormalizedForm. Json result: {jsonResult}");
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_MULTIPLE_TURNS].FilePath.GetRootRelativePath());
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
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_MULTIPLE_TURNS].Utterances[Language.EN][i].Text, recognizedText[i]);
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

        [TestMethod]
        public async Task TestMessageReceivedEvents_SpeechRecognizer_RecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                var turnEndComplete = new TaskCompletionSource<int>();

                List<ConnectionMessage> messages = new List<ConnectionMessage>();
                connection.MessageReceived += (sender, e) => {
                    messages.Add(e.Message);
                    turnEndComplete.TrySetResult(0);
                };

                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                await Task.WhenAny(turnEndComplete.Task, Task.Delay(TimeSpan.FromMinutes(3)));

                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);

                int i = 0;
                var requestId = messages[0].Properties.GetProperty("X-RequestId");

                Assert.AreEqual(messages[i].Path, "turn.start");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                Assert.AreEqual(messages[i].Path, "speech.startDetected");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                while (messages[i].Path == "speech.hypothesis")
                {
                    Assert.AreEqual(messages[i].Path, "speech.hypothesis");
                    Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                    Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                    i += 1;
                }

                Assert.AreEqual(messages[i].Path, "speech.endDetected");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                Assert.AreEqual(messages[i].Path, "speech.phrase");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                Assert.AreEqual(messages[i].Path, "turn.end");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                Assert.AreEqual(i, messages.Count());
            }
        }

        [TestMethod]
        public async Task TestMessageReceivedEvents_SpeechRecognizer_ContinuousRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                var turnEndComplete = new TaskCompletionSource<int>();

                List<ConnectionMessage> messages = new List<ConnectionMessage>();
                connection.MessageReceived += (sender, e) => {
                    var turnEnd = e.Message.Path == "turn.end";
                    messages.Add(e.Message);
                    if (turnEnd) {
                        turnEndComplete.TrySetResult(0);
                    }
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                await Task.WhenAny(turnEndComplete.Task, Task.Delay(TimeSpan.FromMinutes(3)));
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                int i = 0;
                var requestId = messages[0].Properties.GetProperty("X-RequestId");

                Assert.AreEqual(messages[i].Path, "turn.start");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                Assert.AreEqual(messages[1].Path, "speech.startDetected");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                while (i < messages.Count() && messages[i].Path != "speech.endDetected")
                {
                    Assert.IsTrue(messages[i].Path == "speech.hypothesis" || messages[i].Path == "speech.phrase");
                    Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                    Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                    i += 1;
                }

                i -= 1;
                Assert.AreEqual(messages[i].Path, "speech.phrase");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                Assert.AreEqual(messages[i].Path, "speech.endDetected");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                if (messages[i].Path == "speech.phrase")
                {
                    Assert.IsTrue(messages[i].GetTextMessage().Contains("EndOfDictation"));
                    Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                    Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                    i += 1;
                }

                Assert.AreEqual(messages[i].Path, "turn.end");
                Assert.AreEqual(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                Assert.IsTrue(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                Assert.AreEqual(i, messages.Count());
            }
        }

        [TestMethod]
        public async Task TestSpeechConfigFromHost()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.hostConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        #region Dispose Timing Tests
        // Tests that start recognition in various modes and for different events allow the recognizer to fall out of scope and be disposed
        // mid recognition

        [TestMethod]
        public async Task CloseOnSessonStart()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.SessionStarted += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [TestMethod]
        public async Task CloseOnRecognizing()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [TestMethod]
        public async Task CloseOnRecognized()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognized += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [TestMethod]
        public async Task CloseOnSpeechStart()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.SpeechStartDetected += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [TestMethod]
        public async Task CloseOnConnected()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);

                connection.Connected += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }
        #endregion

        #region LID Tests
        [DataTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task LanguageIDRecognitionOnce(bool constructFromSourceLanguageConfig)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig;
            if (constructFromSourceLanguageConfig)
            {
                autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(
                    new SourceLanguageConfig[]
                    {
                        SourceLanguageConfig.FromLanguage(Language.CA_ES),
                        SourceLanguageConfig.FromLanguage(Language.DE_DE),
                    });
            }
            else
            {
                autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(new string[] { Language.DE_DE, Language.CA_ES });
            }
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, autoDetectSourceLanguageConfig, audioInput)))
            {
                List<string> recognizingText = new List<string>();
                List<string> errors = new List<string>();
                var lidInHypothesis = new HashSet<string>();
                recognizer.Recognizing += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizingSpeech)
                    {
                        recognizingText.Add(e.Result.Text);
                        var lidResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                        if (lidResult == null)
                        {
                            errors.Add("Hypothesis has null language id result");
                        }
                        else
                        {
                            lidInHypothesis.Add(lidResult.Language);
                        }
                    }
                    else
                    {
                        errors.Add("Unexpected result reason in hypothesis " + e.Result.Reason);
                    }
                };
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                var autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.FromResult(result);
                Assert.IsNotNull(autoDetectSourceLanguageResult);
                Assert.AreEqual(Language.DE_DE, autoDetectSourceLanguageResult.Language);
                Assert.AreEqual(0, errors.Count);
                Assert.IsTrue(recognizingText.Count > 0);
                Assert.AreEqual(1, lidInHypothesis.Count);
                Assert.AreEqual(Language.DE_DE, lidInHypothesis.First());

            }
        }

        [TestMethod]
        public async Task LanguageIDRecognitionContinous()
        {
            using (var recognizer = TrackSessionId(new SpeechRecognizer(
                                                   this.defaultConfig,
                                                   AutoDetectSourceLanguageConfig.FromLanguages(new string[] { Language.DE_DE, Language.CA_ES }),
                                                   AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                List<string> recognizedText = new List<string>();
                List<string> recognizingText = new List<string>();
                List<string> errors = new List<string>();
                var lidInHypothesis = new HashSet<string>();
                var lidInFinal = new HashSet<string>();
                recognizer.Recognizing += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizingSpeech)
                    {
                        recognizingText.Add(e.Result.Text);
                        var lidResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                        if (lidResult == null)
                        {
                            errors.Add("Hypothesis has null language id result");
                        }
                        else
                        {
                            lidInHypothesis.Add(lidResult.Language);
                        }
                    }
                    else
                    {
                        errors.Add("Unexpected result reason in hypothesis " + e.Result.Reason);
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        if (e.Result.Text.Length > 0)
                        {
                            recognizedText.Add(e.Result.Text);
                        }
                        var lidResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                        if (lidResult == null)
                        {
                            errors.Add("Final result has null language id result");
                        }
                        else
                        {
                            lidInFinal.Add(lidResult.Language);
                        }
                    }
                    else
                    {
                        errors.Add("Unexpected result reason in final result " + e.Result.Reason);
                    }
                };

                await helper.CompleteContinuousRecognition(recognizer);

                Assert.IsTrue(recognizingText.Count > 0);
                Assert.IsTrue(recognizedText.Count > 0);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, recognizedText[0]);
                Assert.AreEqual(0, errors.Count);
                Assert.AreEqual(1, lidInHypothesis.Count);
                Assert.AreEqual(Language.DE_DE, lidInHypothesis.First());
                Assert.AreEqual(1, lidInFinal.Count);
                Assert.AreEqual(Language.DE_DE, lidInFinal.First());
            }
        }
        #endregion

        [TestMethod]
        [Ignore]
        public async Task TestStopAfterErrorIsFast()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            var config = SpeechConfig.FromSubscription("BadKey", "westus2");

            ManualResetEventSlim canceled = new ManualResetEventSlim(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                recognizer.Canceled += (s, e) =>
                {
                    Assert.IsFalse(string.IsNullOrWhiteSpace(e.ErrorDetails));
                    Assert.Equals(e.Reason, CancellationReason.Error);
                    canceled.Set();
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                Assert.IsTrue(canceled.Wait(TimeSpan.FromMinutes(1)));

                var beforeStop = DateTime.UtcNow;
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                var afterStop = DateTime.UtcNow;

                Assert.IsTrue(TimeSpan.FromMilliseconds(500) > afterStop - beforeStop);
            }
        }

        private class ContinuousFilePullStream : PullAudioInputStreamCallback
        {
            FileStream fs;
            readonly object fsLock = new object();

            public ContinuousFilePullStream(string fileName)
            {
                Console.WriteLine("Trying to open " + fileName);
                fs = File.OpenRead(fileName);
            }

            public override int Read(byte[] dataBuffer, uint size)
            {
                lock (fsLock)
                {
                    if (fs == null)
                    {
                        return 0;
                    }

                    if (fs.Read(dataBuffer, 0, (int)size) < size)
                    {
                        // reset the file stream.
                        fs.Seek(0, SeekOrigin.Begin);
                    }
                }

                return dataBuffer.Length;
            }

            public override void Close()
            {
                lock (fsLock)
                {
                    fs.Dispose();
                    fs = null;
                }

                base.Close();
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task WordLevelTimingsWorkForLongSpeech()
        {
            var audioInput = AudioConfig.FromStreamInput(new ContinuousFilePullStream(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()));

            ManualResetEvent recognizedEnough = new ManualResetEvent(false);

            this.defaultConfig.RequestWordLevelTimestamps();

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Canceled += (s, e) =>
                {
                    Assert.IsFalse(string.IsNullOrWhiteSpace(e.ErrorDetails));
                    Assert.Equals(e.Reason, CancellationReason.Error);
                };

                recognizer.Recognized += (s, e) =>
                 {
                     var best = e.Result.Best();

                     if(e.Result.OffsetInTicks > int.MaxValue)
                     {
                         recognizedEnough.Set();
                     }
                 };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                Assert.IsTrue(recognizedEnough.WaitOne(TimeSpan.FromMinutes(4)));
            }
        }
    }
}

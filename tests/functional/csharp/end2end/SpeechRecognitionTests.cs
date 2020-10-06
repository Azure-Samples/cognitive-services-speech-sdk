//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using System.IO;
    using static SPXTEST;
    using static CatchUtils;
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
            LoggingTestBaseInit(context);
            BaseClassInit(context);

            deploymentId = DefaultSettingsMap[DefaultSettingKeys.DEPLOYMENT_ID];
            endpointInString = String.Format("wss://{0}.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1", region);
            endpointUrl = new Uri(endpointInString);
            officeEndpointString = "wss://officespeech.platform.bing.com/speech/recognition/dictation/office/v1";
        }

        [ClassCleanup]
        new public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
        }

        [RetryTestMethod]
        public void TestSetAndGetAuthToken()
        {
            var token = "x";
            var config = SpeechConfig.FromAuthorizationToken(token, "westus");
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var speechRecognizer = new SpeechRecognizer(config, audioInput))
            {
                SPXTEST_ARE_EQUAL(token, speechRecognizer.AuthorizationToken);

                var newToken = "y";
                speechRecognizer.AuthorizationToken = newToken;
                SPXTEST_ARE_EQUAL(token, config.AuthorizationToken);
                SPXTEST_ARE_EQUAL(newToken, speechRecognizer.AuthorizationToken);
            }
        }

        [RetryTestMethod]
        public async Task TestSetAuthorizationTokenOnSpeechRecognizer()
        {
            var invalidToken = "InvalidToken";
            var configWithToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            using (var speechRecognizer = new SpeechRecognizer(configWithToken, audioInput))
            {
                SPXTEST_ARE_EQUAL(invalidToken, speechRecognizer.AuthorizationToken);

                var newToken = await GetToken(subscriptionKey, region);
                speechRecognizer.AuthorizationToken = newToken;
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                SPXTEST_ARE_EQUAL(newToken, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [RetryTestMethod]
        public void TestSetAndGetSubKey()
        {
            var config = SpeechConfig.FromSubscription("x", "westus");
            SPXTEST_ARE_EQUAL("x", config.SubscriptionKey);
        }

        [RetryTestMethod]
        public async Task DefaultLanguageAndOutputFormatRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                SPXTEST_ISTRUE(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should not be set here. RecoLanguage: " + recoLanguage);

                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);

                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("language=en-us"), "Incorrect default language (should be en-us) in " + connectionUrl);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var bestResults = result.Best().ToArray();
                SPXTEST_ARE_EQUAL(0, bestResults.Length, "There should be no detailed result for default output format");
            }
        }

        [RetryTestMethod]
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
                    SPXTEST_ISTRUE(result.Duration.Ticks > 0, $"{result.Reason.ToString()} Duration == 0");
                    SPXTEST_ISTRUE(100000 < result.OffsetInTicks && result.OffsetInTicks < 700000, $"Offset value ${result.OffsetInTicks} seems incorrect");
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                }
            }
        }

        [Ignore]
        [RetryTestMethod]
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
                    SPXTEST_ISTRUE(result.Duration.Ticks > 0, $"{result.Reason.ToString()}, Duration == 0");
                    SPXTEST_ISTRUE(100000 < result.OffsetInTicks && result.OffsetInTicks < 700000, $"Offset value ${result.OffsetInTicks} seems incorrect");
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                }
            }
        }

        [RetryTestMethod]
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

        [RetryTestMethod]
        [DataRow(true)]
        [DataRow(false)]
        public async Task ValidCustomRecognition(bool usingPreConnection)
        {
            this.defaultConfig.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                SPXTEST_ISTRUE(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);
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
                    SPXTEST_ISFALSE(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);
                }
            }
        }

        [RetryTestMethod]
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

        [RetryTestMethod]
        [DataRow(true, DisplayName = "Using preconnect")]
        [DataRow(false, DisplayName = "Without preconnect")]
        public async Task ContinuousValidSkipAudioRecognition(bool usingPreConnection)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProperty("SPEECH-SkipAudioDurationHNS", "4250000");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                string plainExpectedText = Normalize(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text);
                string plainActualText = Normalize(await helper.GetFirstRecognizerResult(recognizer));

                plainExpectedText = plainExpectedText.Remove(0, 6);

                SPXTEST_ISTRUE(
                        plainExpectedText.Equals(plainActualText),
                            $"'{plainExpectedText}' (expected)\n is not equals \n'{plainActualText}' (actual)");
            }
        }

        [RetryTestMethod]
        public async Task InvalidKeyHandledProperly()
        {
            await AssertConnectionError(
                SpeechConfig.FromSubscription("invalidKey", region),
                CancellationErrorCode.AuthenticationFailure,
                "WebSocket upgrade failed", "authentication error", "401");
        }

        [RetryTestMethod]
        public async Task InvalidAuthTokenHandledProperly()
        {
            var invalidToken = "InvalidToken";
            await AssertConnectionError(
                SpeechConfig.FromAuthorizationToken(invalidToken, region),
                CancellationErrorCode.AuthenticationFailure,
                "WebSocket upgrade failed", "authentication error", "401");
        }

        [RetryTestMethod]
        public async Task ExpiredAuthTokenHandledProperly()
        {
            var expiredToken = "eyJhbGciOiJodHRwOi8vd3d3LnczLm9yZy8yMDAxLzA0L3htbGRzaWctbW9yZSNobWFjLXNoYTI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ1cm46bXMuY29nbml0aXZlc2VydmljZXMiLCJleHAiOiIxNTU0MzE1Nzk5IiwicmVnaW9uIjoibm9ydGhldXJvcGUiLCJzdWJzY3JpcHRpb24taWQiOiIwNmZlNjU2MWVkZTM0NDdiYTg2NDY5Njc4YTIwNTNkYiIsInByb2R1Y3QtaWQiOiJTcGVlY2hTZXJ2aWNlcy5TMCIsImNvZ25pdGl2ZS1zZXJ2aWNlcy1lbmRwb2ludCI6Imh0dHBzOi8vYXBpLmNvZ25pdGl2ZS5taWNyb3NvZnQuY29tL2ludGVybmFsL3YxLjAvIiwiYXp1cmUtcmVzb3VyY2UtaWQiOiIvc3Vic2NyaXB0aW9ucy8zYTk2ZWY1Ni00MWE5LTQwYTAtYjBmMy1mYjEyNWMyYjg3OTgvcmVzb3VyY2VHcm91cHMvY3NzcGVlY2hzZGstY2FyYm9uL3Byb3ZpZGVycy9NaWNyb3NvZnQuQ29nbml0aXZlU2VydmljZXMvYWNjb3VudHMvc3BlZWNoc2Rrbm9ydGhldXJvcGUiLCJzY29wZSI6InNwZWVjaHNlcnZpY2VzIiwiYXVkIjoidXJuOm1zLnNwZWVjaHNlcnZpY2VzLm5vcnRoZXVyb3BlIn0.hVAWT2YHjknFI6qLhnjmjzoNgOgxKWguuFhJLlyDxLU";
            await AssertConnectionError(
                SpeechConfig.FromAuthorizationToken(expiredToken, region),
                CancellationErrorCode.AuthenticationFailure,
                "WebSocket upgrade failed", "authentication error", "401");
        }

        [RetryTestMethod]
        public async Task InvalidSubscriptionKeyHandledProperly()
        {
            var configWithInvalidRegion = SpeechConfig.FromSubscription("invalid_subscription_key", RecognitionTestBase.region);
            // "websocket upgrade", "authentication", "subscription";
            // we use case insensitive comparison here since on Linux the error message is: "DNS connection failed (the remote host did not respond)"
            await AssertConnectionError(configWithInvalidRegion, CancellationErrorCode.AuthenticationFailure, "websocket upgrade", "authentication", "subscription");
        }

        [RetryTestMethod]
        public async Task InvalidRegionHandledProperly()
        {
            var configWithInvalidRegion = SpeechConfig.FromSubscription(subscriptionKey, "invalidRegion");

            // we use case insensitive comparison here since on Linux the error message is: "DNS connection failed (the remote host did not respond)"
            await AssertConnectionError(configWithInvalidRegion, CancellationErrorCode.ConnectionFailure, "connection failed");
        }

        [RetryTestMethod]
        public void InvalidInputFileHandledProperly()
        {
            var audioInput = AudioConfig.FromWavFileInput("invalidFile.wav");
            SPXTEST_THROWS<ApplicationException>(() => new SpeechRecognizer(this.defaultConfig, audioInput));
        }

        [RetryTestMethod]
        public async Task InvalidDeploymentIdHandledProperly()
        {
            this.defaultConfig.EndpointId = "invalidDeploymentId";
            await AssertConnectionError(this.defaultConfig, CancellationErrorCode.BadRequest, "WebSocket upgrade failed", "bad request", "400");
        }

        [RetryTestMethod]
        public async Task InvalidLanguageHandledProperly()
        {
            this.defaultConfig.SpeechRecognitionLanguage = "InvalidLang";
            await AssertConnectionError(this.defaultConfig, CancellationErrorCode.BadRequest, "WebSocket upgrade failed", "bad request", "400");
        }

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
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
                    SPXTEST_ISTRUE(expectedExceptionCaught, "No expected exception is caught in connection.Open().");
                }
            }
        }

        [RetryTestMethod]
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

        [RetryTestMethod]
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
                    SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                }
            }
        }

        [RetryTestMethod]
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
                    SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                }
            }
        }

        [RetryTestMethod]
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

        [RetryTestMethod, TestCategory(TestCategory.LongRunning)]
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
                    SPXTEST_ISTRUE(helper.RecognizedEventCount > 0, $"Invalid number of final result events {helper.RecognizedEventCount}");
                    AssertEqual(0, helper.ErrorEventCount, AssertOutput.WrongErrorCount);
                    AssertEqual(1, helper.SpeechStartedEventCount, AssertOutput.WrongSpeechStartedCount);
                    SPXTEST_ISTRUE(recognizedText.Count > 0, $"Invalid number of text messages {recognizedText.Count}");

                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, recognizedText[0]);
                    AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Last().Text, recognizedText.Last());

                    SPXTEST_ISTRUE(string.IsNullOrEmpty(hypothesisLatencyError), $"hypothesisLatencyError: {hypothesisLatencyError}");
                    SPXTEST_ISTRUE(string.IsNullOrEmpty(phraseLatencyError), $"phraseLatencyError: {phraseLatencyError}");

                    GC.KeepAlive(connection);
                }
            }
        }

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
        public void TestGetters()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                SPXTEST_ISTRUE(string.IsNullOrEmpty(recognizer.SpeechRecognitionLanguage), $"No language should be set, is {recognizer.SpeechRecognitionLanguage}");
                SPXTEST_ARE_EQUAL(recognizer.SpeechRecognitionLanguage, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                SPXTEST_ARE_EQUAL(OutputFormat.Simple, recognizer.OutputFormat);
            }

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, SourceLanguageConfig.FromLanguage(Language.DE_DE, deploymentId), audioInput)))
            {
                SPXTEST_ARE_EQUAL(Language.DE_DE, recognizer.SpeechRecognitionLanguage);
                SPXTEST_ARE_EQUAL(recognizer.SpeechRecognitionLanguage, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                SPXTEST_ARE_EQUAL(deploymentId, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EndpointId));
                SPXTEST_ARE_EQUAL(OutputFormat.Simple, recognizer.OutputFormat);
            }

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput)))
            {
                SPXTEST_ARE_EQUAL(Language.DE_DE, recognizer.SpeechRecognitionLanguage);
                SPXTEST_ARE_EQUAL(Language.DE_DE, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
                SPXTEST_ARE_EQUAL(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.defaultConfig.OutputFormat = OutputFormat.Simple;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                SPXTEST_ARE_EQUAL(OutputFormat.Simple, recognizer.OutputFormat);
            }

            this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                SPXTEST_ARE_EQUAL(OutputFormat.Detailed, recognizer.OutputFormat);
            }
        }

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
        public async Task TestSingleShotTwice()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ISTRUE(result.Duration.Ticks > 0, $"{result.Reason.ToString()} First result duration should be greater than 0");
                var offset = result.OffsetInTicks;
                var expectedNextOffset = offset + result.Duration.Ticks;

                var result2 = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var offset2 = result2.OffsetInTicks;

                Console.WriteLine($"Offset1 {offset}, offset1 + duration {expectedNextOffset}");
                Console.WriteLine($"Offset2 {offset2}, its duration {result2.Duration.Ticks}");
                Console.WriteLine($"Result1: {result.ToString()}");
                Console.WriteLine($"Result2: {result2.ToString()}");

                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertStringContains(result.Text.ToLower(), "detective skills");
                SPXTEST_ISTRUE(result.Duration.Ticks > 0, $"Result duration {result.Duration.Ticks} in {result.ToString()} should be greater than 0");

                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result2.Reason);
                SPXTEST_ISTRUE(offset2 >= expectedNextOffset, $"Offset of the second recognition {offset2} should be greater or equal than offset of the first plus duration {expectedNextOffset}.");
            }
        }

        [RetryTestMethod]
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
                    if (!string.IsNullOrEmpty(canceled))
                    {
                        break; // Fail test immediately to avoid invalid state exception
                    }
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                    Thread.Sleep(100); // Avoid hammering the service for test stability
                }
                var ok = string.IsNullOrEmpty(canceled) || canceled.ToLower().Contains("timeout");

                SPXTEST_REQUIRE(ok, $"Recognition Canceled w/ErrorDetails='{canceled}'");
            }
        }

        [RetryTestMethod]
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

                    SPXTEST_REQUIRE(string.IsNullOrEmpty(canceled), $"Recognition Canceled w/ErrorDetails='{canceled}'");
                }
            }
        }

        [RetryTestMethod]
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
                    SPXTEST_ARE_EQUAL(ResultReason.NoMatch, result.Reason);
                    SPXTEST_ISTRUE(result.OffsetInTicks > 0 || result.Duration.Ticks > 0, $"Bad offset: {result.OffsetInTicks} or duration: {result.Duration}");
                    SPXTEST_ISTRUE(string.IsNullOrEmpty(result.Text), $"Bad result text: {result.Text}");

                    var noMatch = NoMatchDetails.FromResult(result);
                    SPXTEST_ARE_EQUAL(NoMatchReason.InitialSilenceTimeout, noMatch.Reason);
                }
            }
        }

        [RetryTestMethod]
        public void TestPropertyCollectionWithoutRecognizer()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SHORT_SILENCE].FilePath.GetRootRelativePath());
            PropertyCollection properties;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                properties = recognizer.Properties;
            }
            SPXTEST_ARE_EQUAL("", properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token));
        }

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task AsyncRecognitionAfterDisposingSpeechRecognizer()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.DE_DE, audioInput));
            recognizer.Dispose();
            await recognizer.StartContinuousRecognitionAsync();
        }

        [RetryTestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void DisposingSpeechRecognizerWhileAsyncRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.EN, audioInput));
            recognizer = helper.GetSpeechRecognizingAsyncNotAwaited(recognizer);
        }

        [RetryTestMethod]
        public async Task FromEndpointGermanRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, Language.DE_DE, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
        public async Task FromEndpointCustomRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.EndpointId = deploymentId;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                SPXTEST_ISTRUE(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISFALSE(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);
            }
        }

        [RetryTestMethod]
        public async Task FromEndpointDefaultLanguage()
        {
            var endpointWithDeploymentId = endpointInString + "?cid=" + deploymentId;
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithDeploymentId), subscriptionKey);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                SPXTEST_ISTRUE(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should be empty. RecoLanguage: " + recoLanguage);

                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISFALSE(connectionUrl.Contains("language="), "ConnectionUrl should not contain language: " + connectionUrl);

            }
        }

        [RetryTestMethod]
        public async Task FromEndpointDetailedRecognition()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertDetailedResult(result);
            }
        }

        [RetryTestMethod]
        public async Task FromEndpointGermanRecognitionWithPropertyOverwrite()
        {
            var endpointWithLang = endpointInString + "?language=de-de";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithLang), subscriptionKey);
            // The property should not overwrite the query parameter in url.
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, Language.EN, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
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
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
        public async Task FromEndpointDetailedRecognitionWithPropertyOverwrite()
        {
            var endpointWithOutputFormat = endpointInString + "?format=detailed";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithOutputFormat), subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Simple;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertDetailedResult(result);
            }
        }

        [RetryTestMethod]
        public async Task SetServicePropertySingleSetting()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
        public async Task SetServicePropertyOverwrite()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, Language.EN, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
        public async Task SetServiceProperty2Properties()
        {
            this.defaultConfig.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            this.defaultConfig.SetServiceProperty("format", "detailed", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                AssertDetailedResult(result);
            }
        }

        [RetryTestMethod]
        public async Task SetServicePropertyFromEndpoint()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
        public async Task SetServicePropertyFromEndpointWithParameter()
        {
            var endpointWithOutputFormat = endpointInString + "?format=detailed";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithOutputFormat), subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Simple;
            configFromEndpoint.SetServiceProperty("language", "de-DE", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                AssertDetailedResult(result);
            }
        }

        [RetryTestMethod]
        public void SetServicePropertyInvalidParameters()
        {
            SPXTEST_THROWS<ApplicationException>(() => this.defaultConfig.SetServiceProperty(null, "value", ServicePropertyChannel.UriQueryParameter));
            SPXTEST_THROWS<ApplicationException>(() => this.defaultConfig.SetServiceProperty("", "value", ServicePropertyChannel.UriQueryParameter));
            SPXTEST_THROWS<ApplicationException>(() => this.defaultConfig.SetServiceProperty("Name", null, ServicePropertyChannel.UriQueryParameter));
            SPXTEST_THROWS<ApplicationException>(() => this.defaultConfig.SetServiceProperty("Name", "", ServicePropertyChannel.UriQueryParameter));
        }

        [RetryTestMethod]
        public async Task TestExceptionsDuringEventsRecognizeOnceAsync()
        {
            SpeechConfig config = SpeechConfig.FromSubscription(subscriptionKey, region);
            SPXTEST_ISNOTNULL(config);
            using (SpeechRecognizer recognizer = new SpeechRecognizer(config, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath())))
            {
                SPXTEST_ISNOTNULL(recognizer);
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
                    SPXTEST_ISNOTNULL(result);
                    SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                    AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                    await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromMinutes(1)));
                }
            }
        }

        [RetryTestMethod]
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
                SPXTEST_ISTRUE(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                SPXTEST_ARE_EQUAL(1, recognizedText.Count, "The number of recognized texts is not 1, but " + recognizedText.Count);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].Utterances[Language.EN][0].Text, recognizedText[0]);
            }
        }

        [RetryTestMethod]
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
                SPXTEST_ISTRUE(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                SPXTEST_ARE_EQUAL(1, recognizedText.Count, "The number of recognized texts is not 1, but " + recognizedText.Count);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].Utterances[Language.EN][0].Text, recognizedText[0]);
                bool start = startEvent.WaitOne(100);
                bool end = endEvent.WaitOne(100);
                SPXTEST_ARE_EQUAL(true, start, "expected SpeechStartDetected but did not receive");
                SPXTEST_ARE_EQUAL(true, end, "expected SpeechEndDetected but did not receive");
            }
        }

        [RetryTestMethod]
        public async Task DictationRecognizeOnce()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_PUNCTUATION].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
        public async Task DictationAllowSwitchRecognizeOnceAndContinuous()
        {
            this.defaultConfig.EnableDictation();
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);

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
                SPXTEST_ISTRUE(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                SPXTEST_REQUIRE(string.IsNullOrEmpty(canceled), $"Recognition Canceled w/ErrorDetails='{canceled}'");
            }
        }

        [RetryTestMethod]
        public async Task ProfanityMaskedRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProfanity(ProfanityOption.Masked);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                SPXTEST_ISFALSE(String.IsNullOrEmpty(result.Text), "result.Text was null or empty.");
                AssertIfContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityRaw);
                AssertIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityMaskedPattern);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityMasked, result.Text);
            }
        }

        // TODO: THESE PROFANITY TESTS DON'T MAKE SENSE NEED TO RE-EXAMINE
        [RetryTestMethod]
        public async Task ProfanityRemovedRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProfanity(ProfanityOption.Removed);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertIfContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].Text);
                WarnIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityRemoved);
            }
        }

        [RetryTestMethod]
        public async Task ProfanityRawRecognizeOnce()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProfanity(ProfanityOption.Raw);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1].Utterances[Language.EN][0].Text, result.Text);
                WarnIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1].Utterances[Language.EN][0].ProfanityRaw);
            }
        }

        [RetryTestMethod]
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
                    SPXTEST_ISTRUE(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult).Contains("Corrections"));
                }
            }
        }

        [RetryTestMethod]
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
                    SPXTEST_ISTRUE(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult).Contains("Corrections"));
                }
            }
        }

        [RetryTestMethod]
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
                SPXTEST_ISTRUE(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult).Contains("Corrections"));
                SPXTEST_ISTRUE(result.Text.StartsWith("W"));
            }
        }

        [RetryTestMethod]
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
                SPXTEST_ISTRUE(json.Contains("Corrections"));
                SPXTEST_ISTRUE(text.StartsWith("w"));
            }
        }

        [RetryTestMethod]
        public async Task WordLevelTiming()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "true");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason, $"Expected status: RecognizedSpeech, actual status: {result.Reason}");
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                foreach (var detailedResult in result.Best())
                {
                    SPXTEST_ISTRUE(detailedResult.Text.Length > 0, $"Empty Text. Json result: {jsonResult}");
                    SPXTEST_ISTRUE(detailedResult.LexicalForm.Length > 0, $"Empty LexicalForm. Json result: {jsonResult}");
                    SPXTEST_ISTRUE(detailedResult.NormalizedForm.Length > 0, $"Empty NormalizedForm. Json result: {jsonResult}");
                    SPXTEST_ISTRUE(detailedResult.MaskedNormalizedForm.Length > 0, $"Empty MaskedNormalizedForm. Json result: {jsonResult}");
                    SPXTEST_ISTRUE(detailedResult.Words.Count() > 0, $"No words available. Json result: {jsonResult}");
                    foreach (var word in detailedResult.Words)
                    {
                        SPXTEST_ISTRUE(word.Offset > 0, $"The word {word.Word} has incorrect offset: {word.Offset}. Json result: {jsonResult}");
                        SPXTEST_ISTRUE(word.Duration > 0, $"The word {word.Word} has incorrect duration: {word.Duration}.Json result: {jsonResult}");
                    }
                }
            }
        }

        [RetryTestMethod]
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
                    SPXTEST_ISTRUE(utteranceOffsets[i] >= utteranceOffset + utteranceDuration, $"Utterance offset must be in ascending order. the previous offset {utteranceOffset}, the previous duration {utteranceDuration}, the current offset {utteranceOffsets[i]}.");
                    if (i >= 1)
                    {
                        SPXTEST_ISTRUE(utteranceOffsets[i] - utteranceOffset - utteranceDuration > 3 * 10000000, $"The silence between 2 utterances must be longer than 3s. The previous offset {utteranceOffset}, the previous duration {utteranceDuration}, the current offset {utteranceOffsets[i]}.");
                    }
                    utteranceOffset = utteranceOffsets[i];
                    utteranceDuration = utteranceDurations[i];

                    SPXTEST_ISTRUE(detailedResults[i].Text.Length > 0, $"Empty Text in detailed result {i}. Json result: {jsonResult[i]}");
                    SPXTEST_ISTRUE(detailedResults[i].LexicalForm.Length > 0, $"Empty LexicalForm in detailed result {i}. Json result: {jsonResult[i]}");
                    SPXTEST_ISTRUE(detailedResults[i].NormalizedForm.Length > 0, $"Empty NormallizedForm in detailed result {i}. Json result: {jsonResult[i]}");
                    SPXTEST_ISTRUE(detailedResults[i].MaskedNormalizedForm.Length > 0, $"Empty MaskedNormalizedForm in detailed result {i}. Json result: {jsonResult[i]}");
                    SPXTEST_ISTRUE(detailedResults[i].Words.Count() > 0, $"No words available in detailed result {i}. Json result: {jsonResult[i]}");
                    foreach (var word in detailedResults[i].Words)
                    {
                        SPXTEST_ISTRUE(word.Offset >= wordOffset + wordDuration, $"The word offset must be in ascending order. the previous offset {wordOffset}, the previous duration {wordDuration}, the current offset {word.Offset}. Json result: {jsonResult[i]}");
                        SPXTEST_ISTRUE(word.Duration > 0, $"The word {word.Word} has incorrect duration: {word.Duration}. Json result: {jsonResult[i]}");
                        wordOffset = word.Offset;
                        wordDuration = word.Duration;
                    }
                }
            }
        }

        [RetryTestMethod]
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

                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);

                int i = 0;
                var requestId = messages[0].Properties.GetProperty("X-RequestId");

                SPXTEST_ARE_EQUAL(messages[i].Path, "turn.start");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                SPXTEST_ARE_EQUAL(messages[i].Path, "speech.startDetected");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                while (messages[i].Path == "speech.hypothesis")
                {
                    SPXTEST_ARE_EQUAL(messages[i].Path, "speech.hypothesis");
                    SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                    SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                    i += 1;
                }

                SPXTEST_ARE_EQUAL(messages[i].Path, "speech.endDetected");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                SPXTEST_ARE_EQUAL(messages[i].Path, "speech.phrase");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                SPXTEST_ARE_EQUAL(messages[i].Path, "turn.end");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                SPXTEST_ARE_EQUAL(i, messages.Count());
            }
        }

        [RetryTestMethod]
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

                SPXTEST_ARE_EQUAL(messages[i].Path, "turn.start");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                SPXTEST_ARE_EQUAL(messages[1].Path, "speech.startDetected");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                while (i < messages.Count() && messages[i].Path != "speech.endDetected")
                {
                    SPXTEST_ISTRUE(messages[i].Path == "speech.hypothesis" || messages[i].Path == "speech.phrase");
                    SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                    SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                    i += 1;
                }

                i -= 1;
                SPXTEST_ARE_EQUAL(messages[i].Path, "speech.phrase");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                SPXTEST_ARE_EQUAL(messages[i].Path, "speech.endDetected");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                if (messages[i].Path == "speech.phrase")
                {
                    SPXTEST_ISTRUE(messages[i].GetTextMessage().Contains("EndOfDictation"));
                    SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                    SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                    i += 1;
                }

                SPXTEST_ARE_EQUAL(messages[i].Path, "turn.end");
                SPXTEST_ARE_EQUAL(messages[i].Properties.GetProperty("X-RequestId"), requestId);
                SPXTEST_ISTRUE(messages[i].Properties.GetProperty("Content-Type").Contains("application/json"));
                i += 1;

                SPXTEST_ARE_EQUAL(i, messages.Count());
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechConfigFromHost()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.hostConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        #region Dispose Timing Tests
        // Tests that start recognition in various modes and for different events allow the recognizer to fall out of scope and be disposed
        // mid recognition

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
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

        [RetryTestMethod]
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
        [RetryTestMethod]
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
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                var autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.FromResult(result);
                SPXTEST_ISNOTNULL(autoDetectSourceLanguageResult);
                SPXTEST_ARE_EQUAL(Language.DE_DE.ToLower(), autoDetectSourceLanguageResult.Language.ToLower());
                SPXTEST_ARE_EQUAL(0, errors.Count);
                SPXTEST_ISTRUE(recognizingText.Count > 0);
                SPXTEST_ARE_EQUAL(1, lidInHypothesis.Count);
                SPXTEST_ARE_EQUAL(Language.DE_DE.ToLower(), lidInHypothesis.First().ToLower());

            }
        }

        [RetryTestMethod]
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

                SPXTEST_ISTRUE(recognizingText.Count > 0);
                SPXTEST_ISTRUE(recognizedText.Count > 0);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, recognizedText[0]);
                SPXTEST_ARE_EQUAL(0, errors.Count);
                SPXTEST_ARE_EQUAL(1, lidInHypothesis.Count);
                SPXTEST_ARE_EQUAL(Language.DE_DE.ToLower(), lidInHypothesis.First().ToLower());
                SPXTEST_ARE_EQUAL(1, lidInFinal.Count);
                SPXTEST_ARE_EQUAL(Language.DE_DE.ToLower(), lidInFinal.First().ToLower());
            }
        }
        #endregion

        [RetryTestMethod]
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
                    SPXTEST_ISFALSE(string.IsNullOrWhiteSpace(e.ErrorDetails));
                    SPXTEST_ARE_EQUAL(e.Reason, CancellationReason.Error);
                    canceled.Set();
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                SPXTEST_ISTRUE(canceled.Wait(TimeSpan.FromMinutes(1)));

                var beforeStop = DateTime.UtcNow;
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                var afterStop = DateTime.UtcNow;

                SPXTEST_ISTRUE(TimeSpan.FromMilliseconds(500) > afterStop - beforeStop);
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
                    SPXTEST_ISFALSE(string.IsNullOrWhiteSpace(e.ErrorDetails));
                    SPXTEST_ARE_EQUAL(e.Reason, CancellationReason.Error);
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

                SPXTEST_ISTRUE(recognizedEnough.WaitOne(TimeSpan.FromMinutes(4)));
            }
        }

        [RetryTestMethod]
        public async Task MultiTurnOffsetsIncreases()
        {
            var config = defaultConfig;
            config.SetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, "5000");
            
            ManualResetEvent doneEvent = new ManualResetEvent(false);
            ManualResetEvent endEvent = new ManualResetEvent(false);
            int recognized = 0;
            int endEvents = 0;
            UInt64 lastOffset = 0;
            UInt64 lastPhraseEnd = 0;
            UInt64 lastEndOffset = 0;
            UInt64 lastHypoOffset = 0;
            UInt64 lastStartOffset = 0;
            bool hypoPassed = true;
            bool recognizedPassed = true;
            bool endPassed = true;
            bool startPassed = true;
            var files = new string[] { AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(),
                                       AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(),
                                       AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(),
                                       AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(),
                                       AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath() };

            var fileStream = new RealTimeMultiFileStream(files);

            var audioInput = AudioConfig.FromStreamInput(fileStream);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, audioInput)))
            {
                var con = Connection.FromRecognizer(recognizer);
                con.MessageReceived += (s, m) =>
                {
                    Console.WriteLine($"{m.Message.Path} {m.Message.GetTextMessage()}");
               
                };

                recognizer.SpeechStartDetected += (s, e) =>
                {
                    Console.WriteLine($"Start {e.Offset}");

                    startPassed &= e.Offset > lastEndOffset;
                    
                    lastStartOffset = e.Offset;
                };

                recognizer.Recognizing += (s, h) =>
                {
                    Console.WriteLine($"Hypothesis Offset {h.Offset} {h.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult)}");

                    hypoPassed &= h.Offset >= lastHypoOffset;
                    hypoPassed &= h.Offset > lastEndOffset;
                    hypoPassed &= h.Offset >= lastStartOffset;
                    hypoPassed &= h.Offset == Convert.ToUInt64(h.Result.OffsetInTicks);
                    var json = h.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                    var jObj = (JObject)JsonConvert.DeserializeObject(json);
                   
                    var jsonOffset = jObj["Offset"].Value<UInt64>();
                    hypoPassed &= h.Offset == jsonOffset;

                    lastHypoOffset = h.Offset;

                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Reco Offset {e.Offset} Done {e.Result.Duration} {e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult)}");

                    recognizedPassed &= e.Offset > lastOffset;
                    recognizedPassed &= e.Offset >= lastStartOffset;
                    recognizedPassed &= e.Offset == Convert.ToUInt64(e.Result.OffsetInTicks);
                    var json = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                    var jObj = (JObject)JsonConvert.DeserializeObject(json);

                    var jsonOffset = jObj["Offset"].Value<UInt64>();
                    recognizedPassed &= e.Offset == jsonOffset;

                    lastOffset = e.Offset;
                    lastPhraseEnd = e.Offset + (Convert.ToUInt64(e.Result.Duration.TotalMilliseconds) * 10000);

                    if (files.Length == ++recognized)
                    {
                        doneEvent.Set();
                    }
                };

                recognizer.SpeechEndDetected += (s, e) =>
                {
                    Console.WriteLine($"EndOffset {e.Offset}");

                    endPassed &= e.Offset > lastEndOffset;
                    endPassed &= e.Offset >= lastPhraseEnd;
                    endPassed &= e.Offset > lastStartOffset;

                    lastEndOffset = e.Offset;
                    fileStream.NextFile();

                    if (files.Length == ++endEvents)
                    {
                        endEvent.Set();
                    }
                };

                await recognizer.StartContinuousRecognitionAsync();

                Assert.IsTrue(ManualResetEvent.WaitAll(new WaitHandle[] { doneEvent, endEvent }, TimeSpan.FromSeconds(600)), "Events were not received in time");
                Assert.IsTrue(hypoPassed, "Hypothesis passed.");
                Assert.IsTrue(recognizedPassed, "Recognized passed.");
                Assert.IsTrue(endPassed, "End passed.");
                Assert.IsTrue(startPassed, "Start passed");

                await recognizer.StopContinuousRecognitionAsync();
            }
        }
    }

    internal class ContinuousFilePullStream : PullAudioInputStreamCallback
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
}

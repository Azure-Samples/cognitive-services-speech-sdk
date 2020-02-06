//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class SpeechConfigTests : RecognitionTestBase
    {
        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestMethod]
        public void TestCreateRecognizerTypes()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var speechRecognizer = new SpeechRecognizer(this.defaultConfig, audioInput))
            {
                Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
            }

            var sourceLanguage = Language.EN;
            var translatorConfig = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            translatorConfig.SpeechRecognitionLanguage = sourceLanguage;

            var targetLanguages = new List<string>() { Language.DE, Language.ES };
            targetLanguages.ForEach(l => translatorConfig.AddTargetLanguage(l));

            using (var translationRecognizer = new TranslationRecognizer(translatorConfig, audioInput))
            {
                Assert.IsInstanceOfType(translationRecognizer, typeof(TranslationRecognizer));
            }

            using (var intentRecognizer = new IntentRecognizer(this.defaultConfig, audioInput))
            {
                Assert.IsInstanceOfType(intentRecognizer, typeof(IntentRecognizer));
            }
        }

        [TestMethod]
        public void TestGetters()
        {
            Assert.AreEqual(subscriptionKey, this.defaultConfig.SubscriptionKey);
            Assert.AreEqual(region, this.defaultConfig.Region);
        }

        [TestMethod]
        public async Task ConfigFromAuthorizationToken()
        {
            var token = await GetToken(subscriptionKey, region);
            var configWithToken = SpeechConfig.FromAuthorizationToken(token, region);

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithToken, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(token, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }

            using (var speechSynthesizer = new SpeechSynthesizer(configWithToken, null))
            {
                Assert.AreEqual(token, speechSynthesizer.AuthorizationToken);

                using (var result = await speechSynthesizer.SpeakTextAsync("{{{text1}}}"))
                {
                    SpeechSynthesisTests.CheckSynthesisResult(result);
                }
            }
        }

        [TestMethod]
        public async Task SetAuthorizationTokenOnConfig()
        {
            var invalidToken = "InvalidToken";
            var configWithToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            Assert.AreEqual(invalidToken, configWithToken.AuthorizationToken);

            var newToken = await GetToken(subscriptionKey, region);
            configWithToken.AuthorizationToken = newToken;
            Assert.AreEqual(newToken, configWithToken.AuthorizationToken);

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithToken, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();
                Assert.AreEqual(newToken, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task SetAuthorizationTokenOnRecognizer()
        {
            var invalidToken = "InvalidToken";
            var configWithToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            Assert.AreEqual(invalidToken, configWithToken.AuthorizationToken);

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithToken, audioInput)))
            {
                var newToken = await GetToken(subscriptionKey, region);
                speechRecognizer.AuthorizationToken = newToken;
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();
                Assert.AreEqual(newToken, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task SubscriptionKeyAndInvalidAuthToken()
        {
            var invalidToken = "InvalidToken";
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                speechRecognizer.AuthorizationToken = invalidToken;

                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(subscriptionKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(invalidToken, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }

            // Create synthesizer using subscription key.
            using (var speechSynthesizer = new SpeechSynthesizer(this.defaultConfig, null))
            {
                speechSynthesizer.AuthorizationToken = invalidToken;

                Assert.AreEqual(subscriptionKey, speechSynthesizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(invalidToken, speechSynthesizer.AuthorizationToken);

                using (var result = await speechSynthesizer.SpeakTextAsync("{{{text1}}}"))
                {
                    SpeechSynthesisTests.CheckSynthesisResult(result);
                }
            }
        }

        [TestMethod]
        public async Task SubscriptionKeyAndValidAuthToken()
        {
            var token = await GetToken(subscriptionKey, region);
            this.defaultConfig.AuthorizationToken = token;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(subscriptionKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(token, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }

            // Create synthesizer using subscription key.
            using (var speechSynthesizer = new SpeechSynthesizer(this.defaultConfig, null))
            {
                Assert.AreEqual(subscriptionKey, speechSynthesizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(token, speechSynthesizer.AuthorizationToken);

                using (var result = await speechSynthesizer.SpeakTextAsync("{{{text1}}}"))
                {
                    SpeechSynthesisTests.CheckSynthesisResult(result);
                }
            }
        }

        [TestMethod]
        public async Task SubscriptionKeyAndExpiredAuthToken()
        {
            var expiredToken = "eyJhbGciOiJodHRwOi8vd3d3LnczLm9yZy8yMDAxLzA0L3htbGRzaWctbW9yZSNobWFjLXNoYTI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ1cm46bXMuY29nbml0aXZlc2VydmljZXMiLCJleHAiOiIxNTU0MzE1Nzk5IiwicmVnaW9uIjoibm9ydGhldXJvcGUiLCJzdWJzY3JpcHRpb24taWQiOiIwNmZlNjU2MWVkZTM0NDdiYTg2NDY5Njc4YTIwNTNkYiIsInByb2R1Y3QtaWQiOiJTcGVlY2hTZXJ2aWNlcy5TMCIsImNvZ25pdGl2ZS1zZXJ2aWNlcy1lbmRwb2ludCI6Imh0dHBzOi8vYXBpLmNvZ25pdGl2ZS5taWNyb3NvZnQuY29tL2ludGVybmFsL3YxLjAvIiwiYXp1cmUtcmVzb3VyY2UtaWQiOiIvc3Vic2NyaXB0aW9ucy8zYTk2ZWY1Ni00MWE5LTQwYTAtYjBmMy1mYjEyNWMyYjg3OTgvcmVzb3VyY2VHcm91cHMvY3NzcGVlY2hzZGstY2FyYm9uL3Byb3ZpZGVycy9NaWNyb3NvZnQuQ29nbml0aXZlU2VydmljZXMvYWNjb3VudHMvc3BlZWNoc2Rrbm9ydGhldXJvcGUiLCJzY29wZSI6InNwZWVjaHNlcnZpY2VzIiwiYXVkIjoidXJuOm1zLnNwZWVjaHNlcnZpY2VzLm5vcnRoZXVyb3BlIn0.hVAWT2YHjknFI6qLhnjmjzoNgOgxKWguuFhJLlyDxLU";
            this.defaultConfig.AuthorizationToken = expiredToken;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(subscriptionKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(expiredToken, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }

            // Create synthesizer using subscription key.
            using (var speechSynthesizer = new SpeechSynthesizer(this.defaultConfig, null))
            {
                Assert.AreEqual(subscriptionKey, speechSynthesizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(expiredToken, speechSynthesizer.AuthorizationToken);

                using (var result = await speechSynthesizer.SpeakTextAsync("{{{text1}}}"))
                {
                    SpeechSynthesisTests.CheckSynthesisResult(result);
                }
            }
        }

        [TestMethod]
        public async Task InvalidSubscriptionKeyAndValidAuthToken()
        {
            var invalidKey = "InvalidKey";
            var token = await GetToken(subscriptionKey, region);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var configWithInvalidKey = SpeechConfig.FromSubscription(invalidKey, region);
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithInvalidKey, audioInput)))
            {
                speechRecognizer.AuthorizationToken = token;

                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(invalidKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(token, speechRecognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, await helper.GetFirstRecognizerResult(speechRecognizer));
            }

            using (var speechSynthesizer = new SpeechSynthesizer(configWithInvalidKey, null))
            {
                speechSynthesizer.AuthorizationToken = token;

                Assert.AreEqual(invalidKey, speechSynthesizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(token, speechSynthesizer.AuthorizationToken);

                using (var result = await speechSynthesizer.SpeakTextAsync("{{{text1}}}"))
                {
                    SpeechSynthesisTests.CheckSynthesisResult(result);
                }
            }
        }

        [TestMethod]
        public async Task InvalidSubscriptionKeyAndInvalidAuthToken()
        {
            var invalidKey = "InvalidKey";
            var invalidToken = "InvalidToken";
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var configWithInvalidKey = SpeechConfig.FromSubscription(invalidKey, region);
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithInvalidKey, audioInput)))
            {
                speechRecognizer.AuthorizationToken = invalidToken;

                Assert.AreEqual(invalidKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(invalidToken, speechRecognizer.AuthorizationToken);

                var result = await speechRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.Canceled, result.Reason);
                var cancellation = CancellationDetails.FromResult(result);
                Assert.AreEqual(CancellationReason.Error, cancellation.Reason);
                Assert.AreEqual(CancellationErrorCode.AuthenticationFailure, cancellation.ErrorCode);
                AssertHelpers.AssertStringContains(cancellation.ErrorDetails, "WebSocket Upgrade failed with an authentication error (401)");
                AssertHelpers.AssertStringContains(cancellation.ErrorDetails, "SessionId");
            }

            // Create synthesizer using subscription key.
            using (var speechSynthesizer = new SpeechSynthesizer(configWithInvalidKey, null))
            {
                speechSynthesizer.AuthorizationToken = invalidToken;

                Assert.AreEqual(invalidKey, speechSynthesizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(invalidToken, speechSynthesizer.AuthorizationToken);

                using (var result = await speechSynthesizer.SpeakTextAsync("{{{text1}}}"))
                {
                    Assert.AreEqual(ResultReason.Canceled, result.Reason);
                    Assert.AreEqual(result.AudioData.Length, 0);
                    var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                    Assert.AreEqual(CancellationReason.Error, cancellation.Reason);
                    Assert.AreEqual(CancellationErrorCode.AuthenticationFailure, cancellation.ErrorCode);
                    AssertHelpers.AssertStringContains(cancellation.ErrorDetails, "WebSocket Upgrade failed with an authentication error (401)");
                }
            }
        }

        [TestMethod]
        public void FromEndpointWithoutSubscriptionKeyAndAuthToken()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var speechConfig = SpeechConfig.FromEndpoint(new Uri("wss://westus.stt.speech.microsoft.com/speech/recognition/dictation/cognitiveservices/v1"));
            // Create recognizer using subscription key.
            using (var speechRecognizer = new SpeechRecognizer(speechConfig, audioInput))
            {
                // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
                Assert.IsTrue(string.IsNullOrEmpty(speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token)));
                Assert.IsTrue(string.IsNullOrEmpty(speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key)));
            }

            var translationConfig = SpeechTranslationConfig.FromEndpoint(new Uri("wss://westus.s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1"));
            translationConfig.SpeechRecognitionLanguage = Language.EN;
            translationConfig.AddTargetLanguage(Language.DE);
            // Create recognizer using subscription key.
            using (var translationRecognizer = new TranslationRecognizer(translationConfig, audioInput))
            {
                // We cannot really test whether recognizer works, since there is no endpoint available which supports no authentication.
                Assert.IsTrue(string.IsNullOrEmpty(translationRecognizer.Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token)));
                Assert.IsTrue(string.IsNullOrEmpty(translationRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key)));
            }

            var synthesisConfig = SpeechConfig.FromEndpoint(new Uri("wss://westus.tts.speech.microsoft.com/cognitiveservices/websocket/v1"));
            using (var speechSynthesizer = new SpeechSynthesizer(synthesisConfig, null))
            {
                // We cannot really test whether synthesizer works, since there is no test endpoint available which supports no authentication.
                Assert.IsTrue(string.IsNullOrEmpty(speechSynthesizer.Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token)));
                Assert.IsTrue(string.IsNullOrEmpty(speechSynthesizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key)));
            }
        }

        [TestMethod]
        public void PropertiesSetAndGet()
        {
            int initialSilenceTimeout = 6000;
            int endSilenceTimeout = 10000;
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs, initialSilenceTimeout.ToString());
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, endSilenceTimeout.ToString());
            Assert.AreEqual(initialSilenceTimeout, Convert.ToInt32(this.defaultConfig.GetProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs)));
            Assert.AreEqual(endSilenceTimeout, Convert.ToInt32(this.defaultConfig.GetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs)));

            int threshold = 15;
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, threshold.ToString());
            Assert.AreEqual(threshold, Convert.ToInt32(this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold)));

            var valStr = "detailed";
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_OutputFormatOption, valStr);
            Assert.AreEqual(valStr, this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_OutputFormatOption));

            var profanity = "raw";
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_ProfanityOption, profanity);
            Assert.AreEqual(profanity, this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_ProfanityOption));

            var falseStr = "false";
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging, falseStr);
            Assert.AreEqual(falseStr, this.defaultConfig.GetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));

            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, falseStr);
            Assert.AreEqual(falseStr, this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));

            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult, falseStr);
            Assert.AreEqual(falseStr, this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult));

            var trueText = "TrueText";
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, trueText);
            Assert.AreEqual(trueText, this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption));

            using (var recognizer = new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath())))
            {
                Assert.AreEqual(initialSilenceTimeout, Convert.ToInt32(recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs)));
                Assert.AreEqual(endSilenceTimeout, Convert.ToInt32(recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs)));
                Assert.AreEqual(threshold, Convert.ToInt32(recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold)));
                Assert.AreEqual(valStr, recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_OutputFormatOption));
                Assert.AreEqual(profanity, recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
                Assert.AreEqual(falseStr, recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));
                Assert.AreEqual(falseStr, recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));
                Assert.AreEqual(falseStr, recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult));
                Assert.AreEqual(trueText, recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption));
            }
        }

        [TestMethod]
        public void PropertiesDirectSetAndGet()
        {
            this.defaultConfig.SetProfanity(ProfanityOption.Removed);
            this.defaultConfig.EnableAudioLogging();
            this.defaultConfig.RequestWordLevelTimestamps();
            this.defaultConfig.EnableDictation();

            using (var recognizer = new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath())))
            {
                Assert.AreEqual("DICTATION", this.defaultConfig.GetProperty(PropertyId.SpeechServiceConnection_RecoMode));
                Assert.AreEqual("DICTATION", recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoMode));
                Assert.AreEqual("removed", this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
                Assert.AreEqual("removed", recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
                Assert.AreEqual("true", this.defaultConfig.GetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));
                Assert.AreEqual("true", recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));
                Assert.AreEqual("true", this.defaultConfig.GetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));
                Assert.AreEqual("true", recognizer.Properties.GetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));
            }
        }

        [TestMethod]
        public async Task SpeechConfigPropertiesSetAndCheckUrl()
        {
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs, "5000");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, "12000");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, "5");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_OutputFormatOption, "detailed");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_ProfanityOption, "removed");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging, "false");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "false");
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");

            // This one is for Translation, should not be picked up by SpeechRecognizer.
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult, "true");

            // Set one via SetServiceProperty, which should be picked up anyway.
            this.defaultConfig.SetServiceProperty("clientId", "1234", ServicePropertyChannel.UriQueryParameter);

            string connectionUrl;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                // Check no word-level timestamps included, but only detailed output.
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                Assert.IsFalse(jsonResult.Contains("Words"), "Word-level timestamps not expected. Returned JSON: " + jsonResult);
                Assert.IsTrue(result.Best().Count() >= 0, "Best results expected. Returned: " + jsonResult);
            }
            Assert.IsTrue(connectionUrl.Length > 0);

            Assert.IsTrue(connectionUrl.Contains("initialSilenceTimeoutMs=5000"), "mismatch initialSilencetimeout in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("endSilenceTimeoutMs=12000"), "mismatch endSilencetimeout in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("stableIntermediateThreshold=5"), "mismatch stableIntermediateThreshold in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("format=detailed"), "mismatch format in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("profanity=removed"), "mismatch profanity in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("storeAudio=false"), "mismatch storeAudio in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("wordLevelTimestamps=false"), "mismatch wordLevelTimestamps in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("postprocessing=TrueText"), "mismatch postprocessing in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("clientId=1234"), "mismatch clientId in " + connectionUrl);

            Assert.IsTrue(connectionUrl.Contains("language=en-us"), "mismatch language in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("stableTranslation="), "unexpected stableTranslation in " + connectionUrl);
        }

        [TestMethod]
        public async Task SpeechConfigPropertiesDirectSetAndCheckUrl()
        {
            this.defaultConfig.SetProfanity(ProfanityOption.Masked);
            this.defaultConfig.EnableAudioLogging();
            this.defaultConfig.RequestWordLevelTimestamps();
            this.defaultConfig.EnableDictation();
            this.defaultConfig.SpeechRecognitionLanguage = "de-DE";

            // Set one via SetServiceProperty, which should be picked up anyway.
            this.defaultConfig.SetServiceProperty("clientConnectionId", "myClient", ServicePropertyChannel.UriQueryParameter);

            string connectionUrl;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, result.Text);
                // Check word-level timestamps as well as best results are included.
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                Assert.IsTrue(jsonResult.Contains("Words"), "No word-level timestamps. Returned JSON: " + jsonResult);
                Assert.IsTrue(result.Best().Count() >= 0, "Best results expected. Returned: " + jsonResult);
            }
            Assert.IsTrue(connectionUrl.Length > 0);

            Assert.IsTrue(connectionUrl.Contains("speech/recognition/dictation/cognitiveservices"), "mismatch dictation mode in " + connectionUrl);
            // Word-level timestamps will set format to detailed.
            Assert.IsTrue(connectionUrl.Contains("format=detailed"), "mismatch format in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("profanity=masked"), "mismatch profanity in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("storeAudio=true"), "mismatch storeAudio in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("wordLevelTimestamps=true"), "mismatch wordLevelTimestamps in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("language=de-DE"), "mismatch language in " + connectionUrl);

            Assert.IsTrue(connectionUrl.Contains("clientConnectionId=myClient"), "mismatch clientId in " + connectionUrl);

            Assert.IsFalse(connectionUrl.Contains("initialSilenceTimeoutMs="), "unexpected initialSilencetimeout in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("endSilenceTimeoutMs="), "unexpected endSilencetimeout in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("stableIntermediateThreshold="), "unexpected stableIntermediateThreshold in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("postprocessing="), "unexpected postprocessing in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("stableTranslation="), "unexpected stableTranslation in " + connectionUrl);
        }

        [TestMethod]
        public async Task SpeechConfigPropertiesNegativeInteger()
        {
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs, "-50");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.Canceled, result.Reason);
                var cancellation = CancellationDetails.FromResult(result);
                Assert.AreEqual(CancellationReason.Error, cancellation.Reason);
                Assert.AreEqual(CancellationErrorCode.RuntimeError, cancellation.ErrorCode);
                AssertHelpers.AssertStringContains(cancellation.ErrorDetails, "SPXERR_INVALID_ARG");
            }
        }

        [TestMethod]
        public async Task SpeechConfigPropertiesInvalidInteger()
        {
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs, "A50");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.Canceled, result.Reason);
                var cancellation = CancellationDetails.FromResult(result);
                Assert.AreEqual(CancellationReason.Error, cancellation.Reason);
                Assert.AreEqual(CancellationErrorCode.RuntimeError, cancellation.ErrorCode);
            }
        }

        [TestMethod]
        public async Task SpeechConfigPropertiesInvalidBool()
        {
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "nontrue");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.Canceled, result.Reason);
                var cancellation = CancellationDetails.FromResult(result);
                Assert.AreEqual(CancellationReason.Error, cancellation.Reason);
                Assert.AreEqual(CancellationErrorCode.RuntimeError, cancellation.ErrorCode);
                AssertHelpers.AssertStringContains(cancellation.ErrorDetails, "SPXERR_INVALID_ARG");
            }
        }

        [TestMethod]
        public async Task SpeechConfigOutputFormatOptionOverwrite()
        {
            this.defaultConfig.OutputFormat = OutputFormat.Detailed;
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_OutputFormatOption, "simple");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("format=simple"), "mismatch format in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                Assert.IsTrue(result.Best().Count() == 0, "Best results not expected. Returned: " + result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult));
            }
        }

        [TestMethod]
        public async Task SpeechConfigWordLevelTimestampsOverwriteOutputFormatOption()
        {
            this.defaultConfig.RequestWordLevelTimestamps();
            this.defaultConfig.SetProperty(PropertyId.SpeechServiceResponse_OutputFormatOption, "simple");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("format=detailed"), "mismatch format in " + connectionUrl);
                Assert.IsTrue(connectionUrl.Contains("wordLevelTimestamps=true"), "mismatch wordLevelTimestamps in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                Assert.IsTrue(jsonResult.Contains("Words"), "No word-level timestamps. Returned JSON: " + jsonResult);
                Assert.IsTrue(result.Best().Count() >= 0, "Best results expected. Returned: " + jsonResult);
            }
        }

        [TestMethod]
        public async Task SpeechConfigWordLevelTimestampsOverwriteOutputFormatProperty()
        {
            this.defaultConfig.RequestWordLevelTimestamps();
            this.defaultConfig.OutputFormat = OutputFormat.Simple;

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("format=detailed"), "mismatch format in " + connectionUrl);
                Assert.IsTrue(connectionUrl.Contains("wordLevelTimestamps=true"), "mismatch wordLevelTimestamps in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                Assert.IsTrue(jsonResult.Contains("Words"), "No word-level timestamps. Returned JSON: " + jsonResult);
                Assert.IsTrue(result.Best().Count() >= 0, "Best results expected. Returned: " + jsonResult);
            }
        }

        [TestMethod]
        public async Task DefaultLanguageSpeechReco()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("language=en-us"), "Incorrect default language (should be en-us) in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        [TestMethod]
        public async Task DefaultLanguageWithSpeechReco()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.IsTrue(connectionUrl.Contains("language=en-us"), "Incorrect default language (should be en-us) in " + connectionUrl);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        [TestMethod]
        public void ObjectErrorThreshold()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            using (var recognizer1 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer2 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer3 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer4 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer5 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer6 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer7 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer8 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer9 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            using (var recognizer10 = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                this.defaultConfig.SetProperty("SPEECH-ObjectCountErrorThreshold", "10");

                Assert.ThrowsException<ApplicationException>(() => { new SpeechRecognizer(this.defaultConfig, audioInput); }, "Was able to create a recognizer when over object limit");

            }
        }
    }
}

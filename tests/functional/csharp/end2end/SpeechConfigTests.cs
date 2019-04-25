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
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;

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
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
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
            var token = await Config.GetToken(subscriptionKey, region);
            var configWithToken = SpeechConfig.FromAuthorizationToken(token, region);

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithToken, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(token, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task SetAuthorizationTokenOnConfig()
        {
            var invalidToken = "InvalidToken";
            var configWithToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            Assert.AreEqual(invalidToken, configWithToken.AuthorizationToken);

            var newToken = await Config.GetToken(subscriptionKey, region);
            configWithToken.AuthorizationToken = newToken;
            Assert.AreEqual(newToken, configWithToken.AuthorizationToken);

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithToken, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();
                Assert.AreEqual(newToken, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task SetAuthorizationTokenOnRecognizer()
        {
            var invalidToken = "InvalidToken";
            var configWithToken = SpeechConfig.FromAuthorizationToken(invalidToken, region);
            Assert.AreEqual(invalidToken, configWithToken.AuthorizationToken);

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithToken, audioInput)))
            {
                var newToken = await Config.GetToken(subscriptionKey, region);
                speechRecognizer.AuthorizationToken = newToken;
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();
                Assert.AreEqual(newToken, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task SubscriptionKeyAndInvalidAuthToken()
        {
            var invalidToken = "InvalidToken";
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                speechRecognizer.AuthorizationToken = invalidToken;

                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(subscriptionKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(invalidToken, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task SubscriptionKeyAndValidAuthToken()
        {
            var token = await Config.GetToken(subscriptionKey, region);
            this.defaultConfig.AuthorizationToken = token;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(subscriptionKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(token, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task SubscriptionKeyAndExpiredAuthToken()
        {
            var expiredToken = "eyJhbGciOiJodHRwOi8vd3d3LnczLm9yZy8yMDAxLzA0L3htbGRzaWctbW9yZSNobWFjLXNoYTI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ1cm46bXMuY29nbml0aXZlc2VydmljZXMiLCJleHAiOiIxNTU0MzE1Nzk5IiwicmVnaW9uIjoibm9ydGhldXJvcGUiLCJzdWJzY3JpcHRpb24taWQiOiIwNmZlNjU2MWVkZTM0NDdiYTg2NDY5Njc4YTIwNTNkYiIsInByb2R1Y3QtaWQiOiJTcGVlY2hTZXJ2aWNlcy5TMCIsImNvZ25pdGl2ZS1zZXJ2aWNlcy1lbmRwb2ludCI6Imh0dHBzOi8vYXBpLmNvZ25pdGl2ZS5taWNyb3NvZnQuY29tL2ludGVybmFsL3YxLjAvIiwiYXp1cmUtcmVzb3VyY2UtaWQiOiIvc3Vic2NyaXB0aW9ucy8zYTk2ZWY1Ni00MWE5LTQwYTAtYjBmMy1mYjEyNWMyYjg3OTgvcmVzb3VyY2VHcm91cHMvY3NzcGVlY2hzZGstY2FyYm9uL3Byb3ZpZGVycy9NaWNyb3NvZnQuQ29nbml0aXZlU2VydmljZXMvYWNjb3VudHMvc3BlZWNoc2Rrbm9ydGhldXJvcGUiLCJzY29wZSI6InNwZWVjaHNlcnZpY2VzIiwiYXVkIjoidXJuOm1zLnNwZWVjaHNlcnZpY2VzLm5vcnRoZXVyb3BlIn0.hVAWT2YHjknFI6qLhnjmjzoNgOgxKWguuFhJLlyDxLU";
            this.defaultConfig.AuthorizationToken = expiredToken;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(subscriptionKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(expiredToken, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task InvalidSubscriptionKeyAndValidAuthToken()
        {
            var invalidKey = "InvalidKey";
            var token = await Config.GetToken(subscriptionKey, region);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var configWithInvalidKey = SpeechConfig.FromSubscription(invalidKey, region);
            // Create recognizer using subscription key.
            using (var speechRecognizer = TrackSessionId(new SpeechRecognizer(configWithInvalidKey, audioInput)))
            {
                speechRecognizer.AuthorizationToken = token;

                SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

                Assert.AreEqual(invalidKey, speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key));
                Assert.AreEqual(token, speechRecognizer.AuthorizationToken);
                AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
            }
        }

        [TestMethod]
        public async Task InvalidSubscriptionKeyAndInvalidAuthToken()
        {
            var invalidKey = "InvalidKey";
            var invalidToken = "InvalidToken";
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
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
            }
        }

        [TestMethod]
        public void FromEndpointWithoutSubscriptionKeyAndAuthToken()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var speechConfig = SpeechConfig.FromEndpoint(new Uri("wss://westus.stt.speech.microsoft.com/speech/recognition/dictation/cognitiveservices/v1"));
            // Create recognizer using subscription key.
            using (var speechRecognizer = new SpeechRecognizer(speechConfig, audioInput))
            {
                // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
                Assert.IsTrue(string.IsNullOrEmpty(speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token)));
                Assert.IsTrue(string.IsNullOrEmpty(speechRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key)));
            }

            var translationConfig = SpeechTranslationConfig.FromEndpoint(new Uri("wss://westus.s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1"));
            translationConfig.SpeechRecognitionLanguage = "en-us";
            translationConfig.AddTargetLanguage("de");
            // Create recognizer using subscription key.
            using (var translationRecognizer = new TranslationRecognizer(translationConfig, audioInput))
            {
                // We cannot really test whether recognizer works, since there is no endpoint available which supports no authentication.
                Assert.IsTrue(string.IsNullOrEmpty(translationRecognizer.Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token)));
                Assert.IsTrue(string.IsNullOrEmpty(translationRecognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Key)));
            }
        }
    }
}

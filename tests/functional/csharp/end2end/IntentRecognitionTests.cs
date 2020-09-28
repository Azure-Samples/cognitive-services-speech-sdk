//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static AssertHelpers;
    using static SPXTEST;
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class IntentRecognitionTests : LoggingTestBase
    {
        private static string languageUnderstandingSubscriptionKey;
        private static string languageUnderstandingServiceRegion;
        private static string languageUnderstandingHomeAutomationAppId;
        private static string endpointInString;
        private static Uri endpointUrl;
        private static string hostInString;
        private static Uri hostUrl;
        private static Config _config;

        private SpeechConfig config;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            _config = new Config(context);

            languageUnderstandingSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Key;
            languageUnderstandingServiceRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Region;
            languageUnderstandingHomeAutomationAppId = DefaultSettingsMap[DefaultSettingKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID];
            var intentRegionInUrl = MapToIntentServiceRegion(languageUnderstandingServiceRegion);
            endpointInString = String.Format("wss://speech.platform.bing.com/speech/{0}/recognition/interactive/cognitiveservices/v1", intentRegionInUrl);
            endpointUrl = new Uri(endpointInString);
            hostInString = String.Format("wss://speech.platform.bing.com");
            hostUrl = new Uri(hostInString);
        }

        [ClassCleanup]
        public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void Initialize()
        {
            config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);
        }

        public static string MapToIntentServiceRegion(string speechServiceRegion)
        {
            var regionMap = new Dictionary<string, string>
            {
                {"westus", "uswest"},
                {"westus2", "uswest2"},
                {"southcentralus", "ussouthcentral"},
                {"westcentralus", "uswestcentral"},
                {"eastus", "useast"},
                {"eastus2", "useast2"},
                {"westeurope", "europewest"},
                {"northeurope", "europenorth"},
                {"southbrazil", "brazilsouth"},
                {"eastaustralia", "australiaeast"},
                {"southeastasia", "asiasoutheast"},
                {"eastasia", "asiaeast"}
            };

            SPXTEST_ISTRUE(regionMap.ContainsKey(speechServiceRegion), "Cannot map speech service region to intent service region.");
            return regionMap[speechServiceRegion];
        }

        public static IntentRecognizer TrackSessionId(IntentRecognizer recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };

            recognizer.Canceled += (s, e) =>
            {
                if (e.Reason == CancellationReason.Error)
                {
                    Console.WriteLine($"CancellationReason.Error: ErrorCode {e.ErrorCode}, ErrorDetails {e.ErrorDetails}");
                }
            };
            return recognizer;
        }

        [RetryTestMethod]
        [DataRow("", "", "HomeAutomation.TurnOn")]
        [DataRow("", "my-custom-intent-id-string", "my-custom-intent-id-string")]
        [DataRow("HomeAutomation.TurnOn", "", "HomeAutomation.TurnOn")]
        [DataRow("HomeAutomation.TurnOn", "my-custom-intent-id-string", "my-custom-intent-id-string")]
        [DataRow("intent-name-that-doesnt-exist", "", "")]
        [DataRow("intent-name-that-doesnt-exist", "my-custom-intent-id-string", "")]
        public async Task RecognizeIntent(string intentName, string intentId, string expectedIntentId)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                if (string.IsNullOrEmpty(intentName) && string.IsNullOrEmpty(intentId))
                {
                    recognizer.AddAllIntents(model);
                }
                else if (string.IsNullOrEmpty(intentName))
                {
                    recognizer.AddAllIntents(model, intentId);
                }
                else if (string.IsNullOrEmpty(intentId))
                {
                    recognizer.AddIntent(model, intentName);
                }
                else
                {
                    recognizer.AddIntent(model, intentName, intentId);
                }

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                SPXTEST_ARE_EQUAL(
                    string.IsNullOrEmpty(expectedIntentId) ? ResultReason.RecognizedSpeech : ResultReason.RecognizedIntent,
                    result.Reason);
                SPXTEST_ARE_EQUAL(expectedIntentId, result.IntentId);
                SPXTEST_ARE_EQUAL(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text, result.Text);
                var json = result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
                SPXTEST_ISFALSE(string.IsNullOrEmpty(json), "Empty JSON from intent recognition");
                // TODO check JSON validity
            }
        }

        [RetryTestMethod]
        [Ignore] // TODO: This is being worked on by Ryan
        public async Task RecognizeIntentSpecialCharacters()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_SPECIAL_CHARACTER].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model);
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var json = result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
                SPXTEST_ISTRUE(json.Contains("AT&T"), $"Could not find AT&T in JSON response: {json}");
            }
        }

        [RetryTestMethod]
        public void TestSetAndGetAuthTokenOnIntent()
        {
            var token = "x";
            var config = SpeechConfig.FromAuthorizationToken(token, "westus");
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            using (var recognizer = new IntentRecognizer(config, audioInput))
            {
                SPXTEST_ARE_EQUAL(token, recognizer.AuthorizationToken);

                var newToken = "y";
                recognizer.AuthorizationToken = newToken;
                SPXTEST_ARE_EQUAL(token, config.AuthorizationToken);
                SPXTEST_ARE_EQUAL(newToken, recognizer.AuthorizationToken);
            }
        }

        [RetryTestMethod]
        public async Task TestSetAuthorizationTokenOnIntentRecognizer()
        {
            var invalidToken = "InvalidToken";
            var configWithToken = SpeechConfig.FromAuthorizationToken(invalidToken, languageUnderstandingServiceRegion);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            using (var recognizer = TrackSessionId(new IntentRecognizer(configWithToken, audioInput)))
            {
                SPXTEST_ARE_EQUAL(invalidToken, recognizer.AuthorizationToken);

                var newToken = await GetToken(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);
                recognizer.AuthorizationToken = newToken;

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                SPXTEST_ARE_EQUAL(newToken, recognizer.AuthorizationToken);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            }
        }

        [RetryTestMethod]
        [DataRow(false, false)]
        [DataRow(false, true)]
        [DataRow(true, false)]
        [DataRow(true, true)]
        public async Task RecognizeIntentSimplePhrase(bool matchingPhrase, bool singleArgument)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath());

            var phrase = matchingPhrase ? AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text : "do not match this";
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                var someId = "id1";
                var expectedId = matchingPhrase ? (singleArgument ? phrase : someId) : "";
                if (singleArgument)
                {
                    recognizer.AddIntent(phrase);
                }
                else
                {
                    recognizer.AddIntent(phrase, someId);
                }
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(string.IsNullOrEmpty(expectedId) ? ResultReason.RecognizedSpeech : ResultReason.RecognizedIntent, result.Reason);
                SPXTEST_ARE_EQUAL(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text, result.Text);
                SPXTEST_ARE_EQUAL(expectedId, result.IntentId,
                    $"Unexpected intent ID for singleArgument={singleArgument} matchingPhrase={matchingPhrase}: is {result.IntentId}, expected {expectedId}");
            }
        }

        [RetryTestMethod]
        public async Task IntentRecognizerConnectedEvent()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath());
            int connectedEventCount = 0;
            int disconnectedEventCount = 0;
            EventHandler<ConnectionEventArgs> myConnectedHandler = (s, e) =>
            {
                connectedEventCount++;
            };
            EventHandler<ConnectionEventArgs> myDisconnectedHandler = (s, e) =>
            {
                disconnectedEventCount++;
            };
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                    recognizer.AddIntent(model, "HomeAutomation.TurnOn", "my-custom-intent-id-string");

                    var tcs = new TaskCompletionSource<int>();
                    recognizer.SessionStopped += (s, e) =>
                    {
                        tcs.TrySetResult(0);
                    };
                    recognizer.Canceled += (s, e) =>
                    {
                        Console.WriteLine("Canceled: " + e.SessionId);
                        tcs.TrySetResult(0);
                    };
                    connection.Connected += myConnectedHandler;
                    connection.Disconnected += myDisconnectedHandler;

                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromMinutes(2)));

                    connection.Connected -= myConnectedHandler;
                    connection.Disconnected -= myDisconnectedHandler;

                    Console.WriteLine($"ConnectedEventCount: {connectedEventCount}, DisconnectedEventCount: {disconnectedEventCount}");
                    SPXTEST_ISTRUE(connectedEventCount > 0, AssertOutput.ConnectedEventCountMustNotBeZero);
                    SPXTEST_ISTRUE(connectedEventCount == disconnectedEventCount || connectedEventCount == disconnectedEventCount + 1, AssertOutput.ConnectedDisconnectedEventUnmatch);

                    SPXTEST_ARE_EQUAL(ResultReason.RecognizedIntent, result.Reason);
                }
                }
        }

        [RetryTestMethod]
        public void IntentRecognizerUsingConnectionOpen()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                using (var connection = Connection.FromRecognizer(recognizer))
                {
                    var ex = SPXTEST_THROWS<ApplicationException>(() => connection.Open(false));
                    AssertStringContains(ex.Message, "Exception with an error code: 0x1f");
                }
            }
        }

        [RetryTestMethod]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task AsyncRecognitionAfterDisposingIntentRecognizer()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath());
            var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput));
            recognizer.AddIntent(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text);
            recognizer.Dispose();
            await recognizer.StartContinuousRecognitionAsync();
        }

        [RetryTestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void DisposingIntentRecognizerWhileAsyncRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath());
            var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput));
            recognizer.AddIntent(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text);
            recognizer = DoAsyncRecognitionNotAwaited(recognizer);
        }

        [RetryTestMethod]
        public async Task FromEndpointIntentModel()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, languageUnderstandingSubscriptionKey);
            configFromEndpoint.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = TrackSessionId(new IntentRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath()))))
            {
                var phrase = AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text;
                var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                SPXTEST_ARE_EQUAL(ResultReason.RecognizedIntent, result.Reason);
                SPXTEST_ARE_EQUAL(phrase, result.Text);
                SPXTEST_ARE_EQUAL("HomeAutomation.TurnOn", result.IntentId, $"Unexpected intent ID: actual: {result.IntentId}, expected {phrase}");

                var json = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                SPXTEST_ISFALSE(string.IsNullOrEmpty(json), "Empty JSON from intent recognition");
                SPXTEST_ISTRUE(json.Contains("ITN"), "Detailed result does not contain ITN.");
                SPXTEST_ISTRUE(json.Contains("Lexical"), "Detailed result does not contain Lexical.");
                SPXTEST_ISTRUE(json.Contains("MaskedITN"), "Detailed result does not contain MaskedITN.");
                SPXTEST_ISTRUE(json.Contains("Display"), "Detailed result does not contain Text.");
            }
        }

        [RetryTestMethod]
        public async Task FromEndpointGermanPhraseIntent()
        {
            var configFromEndpoint = SpeechConfig.FromEndpoint(endpointUrl, languageUnderstandingSubscriptionKey);
            configFromEndpoint.SpeechRecognitionLanguage = Language.DE_DE;
            // Although we set output format to simple, the intent recognizer will set it to detailed for checking intents.
            configFromEndpoint.OutputFormat = OutputFormat.Simple;
            using (var recognizer = TrackSessionId(new IntentRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath()))))
            {
                var phrase = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text;
                recognizer.AddIntent(phrase);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                SPXTEST_ARE_EQUAL(ResultReason.RecognizedIntent, result.Reason);
                AssertFuzzyMatching(phrase, result.Text);
                SPXTEST_ARE_EQUAL(phrase, result.IntentId, $"Unexpected intent ID: actual: {result.IntentId}, expected {phrase}");

                var json = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                SPXTEST_ISFALSE(string.IsNullOrEmpty(json), "Empty JSON from intent recognition");
                SPXTEST_ISTRUE(json.Contains("ITN"), "Detailed result does not contain ITN.");
                SPXTEST_ISTRUE(json.Contains("Lexical"), "Detailed result does not contain Lexical.");
                SPXTEST_ISTRUE(json.Contains("MaskedITN"), "Detailed result does not contain MaskedITN.");
                SPXTEST_ISTRUE(json.Contains("Display"), "Detailed result does not contain Text.");
            }
        }

        [RetryTestMethod]
        public async Task FromEndpointPropertyOverwriteIntent()
        {
            var endpointWithProperty = endpointInString + "?language=en-us&format=detailed";
            var configFromEndpoint = SpeechConfig.FromEndpoint(new Uri(endpointWithProperty), languageUnderstandingSubscriptionKey);

            // The property should not overwrite the query parameter in url.
            configFromEndpoint.SpeechRecognitionLanguage = "Invalid-Language";
            configFromEndpoint.OutputFormat = OutputFormat.Simple;

            using (var recognizer = TrackSessionId(new IntentRecognizer(configFromEndpoint, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath()))))
            {
                var phrase = AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text;
                var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                SPXTEST_ARE_EQUAL(ResultReason.RecognizedIntent, result.Reason);
                SPXTEST_ARE_EQUAL(phrase, result.Text);
                SPXTEST_ARE_EQUAL("HomeAutomation.TurnOn", result.IntentId, $"Unexpected intent ID: actual: {result.IntentId}, expected {phrase}");

                var json = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                SPXTEST_ISFALSE(string.IsNullOrEmpty(json), "Empty JSON from intent recognition");
                SPXTEST_ISTRUE(json.Contains("ITN"), "Detailed result does not contain ITN.");
                SPXTEST_ISTRUE(json.Contains("Lexical"), "Detailed result does not contain Lexical.");
                SPXTEST_ISTRUE(json.Contains("MaskedITN"), "Detailed result does not contain MaskedITN.");
                SPXTEST_ISTRUE(json.Contains("Display"), "Detailed result does not contain Text.");
            }
        }

        [RetryTestMethod]
        public async Task RecognizeIntentDefaultLanguage()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_WITH_SPECIAL_CHARACTER].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                var recoLanguage = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
                SPXTEST_ISTRUE(String.IsNullOrEmpty(recoLanguage), "RecoLanguage should not be set here. RecoLanguage: " + recoLanguage);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("language=en-us"), "Incorrect default language (should be en-us) in " + connectionUrl);
            }
        }

        [RetryTestMethod]
        [Ignore("This is failing intermitantly with Websocket upgrade failure.")]
        public async Task TestIntentConfigFromHost()
        {
            var configFromHost = SpeechConfig.FromHost(hostUrl, languageUnderstandingSubscriptionKey);
            configFromHost.SpeechRecognitionLanguage = Language.EN;
            using (var recognizer = TrackSessionId(new IntentRecognizer(configFromHost, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].FilePath.GetRootRelativePath()))))
            {
                var phrase = AudioUtterancesMap[AudioUtteranceKeys.INTENT_UTTERANCE].Utterances[Language.EN][0].Text;
                var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedIntent, result.Reason);
            }
        }

        IntentRecognizer DoAsyncRecognitionNotAwaited(IntentRecognizer rec)
        {
            using (var recognizer = rec)
            {
                recognizer.RecognizeOnceAsync();
                Thread.Sleep(100);
                return recognizer;
            }
        }
    }
}

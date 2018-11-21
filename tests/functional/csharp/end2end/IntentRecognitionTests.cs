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

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using System.Threading;
    using static AssertHelpers;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class IntentRecognitionTests
    {
        private static string languageUnderstandingSubscriptionKey;
        private static string languageUnderstandingServiceRegion;
        private static string languageUnderstandingHomeAutomationAppId;

        private SpeechConfig config;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            languageUnderstandingSubscriptionKey = Config.GetSettingByKey<String>(context, "LanguageUnderstandingSubscriptionKey");
            languageUnderstandingServiceRegion = Config.GetSettingByKey<String>(context, "LanguageUnderstandingServiceRegion");
            languageUnderstandingHomeAutomationAppId = Config.GetSettingByKey<String>(context, "LanguageUnderstandingHomeAutomationAppId");

            var inputDir = Config.GetSettingByKey<String>(context, "InputDir");
            TestData.AudioDir = Path.Combine(inputDir, "audio");
        }

        [TestInitialize]
        public void Initialize()
        {
            config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);
        }

        public static IntentRecognizer TrackSessionId(IntentRecognizer recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return recognizer;
        }

        [DataTestMethod]
        [DataRow("", "", "HomeAutomation.TurnOn")]
        [DataRow("", "my-custom-intent-id-string", "my-custom-intent-id-string")]
        [DataRow("HomeAutomation.TurnOn", "", "HomeAutomation.TurnOn")]
        [DataRow("HomeAutomation.TurnOn", "my-custom-intent-id-string", "my-custom-intent-id-string")]
        [DataRow("intent-name-that-doesnt-exist", "", "")]
        [DataRow("intent-name-that-doesnt-exist", "my-custom-intent-id-string", "")]
        public async Task RecognizeIntent(string intentName, string intentId, string expectedIntentId)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.HomeAutomation.TurnOn.AudioFile);
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
                Assert.AreEqual(expectedIntentId, result.IntentId);
                Assert.AreEqual(TestData.English.HomeAutomation.TurnOn.Utterance, result.Text);
                var json = result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
                Assert.IsFalse(string.IsNullOrEmpty(json), "Empty JSON from intent recognition");
                // TODO check JSON validity
            }
        }

        [TestMethod]
        public async Task RecognizeIntentSimplePhrase()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.HomeAutomation.TurnOn.AudioFile);

            foreach (bool matchingPhrase in new bool[] { false, true })
                foreach (bool singleArgument in new bool[] { false, true })
                {
                    var phrase = matchingPhrase ? TestData.English.HomeAutomation.TurnOn.Utterance : "do not match this";
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
                        Assert.AreEqual(TestData.English.HomeAutomation.TurnOn.Utterance, result.Text);
                        Assert.AreEqual(expectedId, result.IntentId,
                            $"Unexpected intent ID for singleArgument={singleArgument} matchingPhrase={matchingPhrase}: is {result.IntentId}, expected {expectedId}");
                    }
                }
        }

        [TestMethod]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task AsyncRecognitionAfterDisposingIntentRecognizer()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.HomeAutomation.TurnOn.AudioFile);
            var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput));
            recognizer.AddIntent(TestData.English.HomeAutomation.TurnOn.Utterance);
            recognizer.Dispose();
            await recognizer.StartContinuousRecognitionAsync();
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void DisposingIntentRecognizerWhileAsyncRecognition()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.HomeAutomation.TurnOn.AudioFile);
            var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput));
            recognizer.AddIntent(TestData.English.HomeAutomation.TurnOn.Utterance);
            recognizer = DoAsyncRecognitionNotAwaited(recognizer);
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

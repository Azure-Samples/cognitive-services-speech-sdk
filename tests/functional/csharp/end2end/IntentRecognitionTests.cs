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
        [DataRow("localIntent", "NonExistingOne", "")]
        [DataRow("", "", "HomeAutomation.TurnOn")]
        [DataRow("DontCare", "", "HomeAutomation.TurnOn")]
        [Ignore] // Service flaky? VSTS 1411509
        public async Task RecognizeIntent(string localIntent, string modelIntent, string expectedIntent)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.HomeAutomation.TurnOn.AudioFile);
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddIntent(model, modelIntent, localIntent);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(TestData.English.HomeAutomation.TurnOn.Utterance, result.Text);
                Assert.AreEqual(expectedIntent, result.IntentId);
            }
        }

        [DataTestMethod]
        [DataRow("localIntent", "HomeAutomation.TurnOn", "localIntent")]
        [Ignore] // Service flaky? VSTS 1411509
        public async Task RecognizeIntentValidInputs(string localIntent, string modelIntent, string expectedIntent)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.HomeAutomation.TurnOn.AudioFile);
            using (var recognizer = TrackSessionId(new IntentRecognizer(config, audioInput)))
            {
                var model = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddIntent(model, modelIntent, localIntent);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(TestData.English.HomeAutomation.TurnOn.Utterance, result.Text);
                Assert.IsFalse(string.IsNullOrEmpty(result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult)));
                Assert.AreEqual(expectedIntent, result.IntentId);
            }
        }
    }
}

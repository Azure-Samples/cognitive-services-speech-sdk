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
    public class SpeechFactoryTests : RecognitionTestBase
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
            var speechRecognizer = factory.CreateSpeechRecognizerFromConfig(audioInput);
            Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
            speechRecognizer.Dispose();

            var targetLanguages = new List<string>() { Language.DE, Language.ES };
            var sourceLanguage = Language.EN;
            var translationRecognizer = factory.CreateTranslationRecognizerFromConfig(audioInput, sourceLanguage, targetLanguages);
            Assert.IsInstanceOfType(translationRecognizer, typeof(TranslationRecognizer));
            translationRecognizer.Dispose();

            var intentRecognizer = factory.CreateIntentRecognizerFromConfig(audioInput);
            Assert.IsInstanceOfType(intentRecognizer, typeof(IntentRecognizer));
            intentRecognizer.Dispose();
        }

        [TestMethod]
        public void TestGetters()
        {
            Assert.AreEqual(subscriptionKey, factory.SubscriptionKey);
            Assert.AreEqual(region, factory.Region);
        }

        [TestMethod]
        public async Task FactoryFromAuthorizationToken()
        {
            var token = await Config.GetToken(subscriptionKey);
            var factoryFromToken = SpeechFactory.FromAuthorizationToken(token, region);

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var speechRecognizer = factoryFromToken.CreateSpeechRecognizerFromConfig(audioInput);
            SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

            Assert.AreEqual(token, factoryFromToken.AuthorizationToken, "Set of authentication token did not work as expected");
            AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
        }

        [TestMethod]
        public async Task SetAuthorizationTokenOnFactory()
        {
            var token = await Config.GetToken(subscriptionKey);
            var factoryFromToken = SpeechFactory.FromAuthorizationToken(token, region);
            var newToken = await Config.GetToken(subscriptionKey);
            factoryFromToken.AuthorizationToken = newToken;

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var speechRecognizer = factoryFromToken.CreateSpeechRecognizerFromConfig(audioInput);
            SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

            Assert.AreEqual(newToken, factoryFromToken.AuthorizationToken, "Set of authentication token did not work as expected");
            AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
        }
    }
}

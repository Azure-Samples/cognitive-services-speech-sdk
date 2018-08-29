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
            var speechRecognizer = factory.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile);
            Assert.IsInstanceOfType(speechRecognizer, typeof(SpeechRecognizer));
            speechRecognizer.Dispose();

            var targetLanguages = new List<string>() { Language.DE, Language.ES };
            var sourceLanguage = Language.EN;
            var translationRecognizer = factory.CreateTranslationRecognizerWithFileInput(TestData.English.Weather.AudioFile, sourceLanguage, targetLanguages);
            Assert.IsInstanceOfType(translationRecognizer, typeof(TranslationRecognizer));
            translationRecognizer.Dispose();

            var intentRecognizer = factory.CreateIntentRecognizerWithFileInput(TestData.English.Weather.AudioFile);
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

            var speechRecognizer = factoryFromToken.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile);
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

            var speechRecognizer = factoryFromToken.CreateSpeechRecognizerWithFileInput(TestData.English.Weather.AudioFile);
            SpeechRecognitionTestsHelper helper = new SpeechRecognitionTestsHelper();

            Assert.AreEqual(newToken, factoryFromToken.AuthorizationToken, "Set of authentication token did not work as expected");
            AssertMatching(TestData.English.Weather.Utterance, await helper.GetFirstRecognizerResult(speechRecognizer));
        }
    }
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    [TestClass]
    public sealed class TranslationTests : RecognitionTestBase
    {
        private TranslationTestsHelper translationHelper;
        private static string synthesisDir;
        
        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
            synthesisDir = Path.Combine(inputDir, "synthesis");
        }

        [TestInitialize]
        public void TestInitalize()
        {
            this.translationHelper = new TranslationTestsHelper(factory);
        }

        [Ignore] // TODO ENABLE once again translation works.
        [TestMethod]
        public void TestLanguageProperties()
        {
            var toLanguages = new List<string>() { Language.DE };
            var fromLanguage = Language.EN;
            var factory = SpeechFactory.FromSubscription(subscriptionKey, region);
            using (var translationRecognizer = factory.CreateTranslationRecognizerWithFileInput(TestData.English.Weather.AudioFile, fromLanguage, toLanguages))
            {
                Assert.AreEqual(translationRecognizer.SourceLanguage, fromLanguage);
                CollectionAssert.AreEqual(translationRecognizer.TargetLanguages, toLanguages);
                Assert.AreEqual(translationRecognizer.OutputVoiceName, String.Empty);
            }
        }

        [TestMethod]
        public void TestLanguagePropertiesMultiTargets()
        {
            var toLanguages = new List<string>() { Language.DE, Language.ES };
            var fromLanguage = Language.EN;
            var factory = SpeechFactory.FromSubscription(subscriptionKey, region);
            using (var translationRecognizer = factory.CreateTranslationRecognizerWithFileInput(TestData.English.Weather.AudioFile, fromLanguage, toLanguages))
            {
                Assert.AreEqual(translationRecognizer.SourceLanguage, fromLanguage);
                CollectionAssert.AreEqual(translationRecognizer.TargetLanguages, toLanguages);
                Assert.AreEqual(translationRecognizer.OutputVoiceName, String.Empty);
            }
        }

        [TestMethod]
        public void TestOutputVoiceName()
        {
            var toLanguages = new List<string>() { Language.DE };
            var fromLanguage = Language.EN;
            var voice = Voice.DE;
            var factory = SpeechFactory.FromSubscription(subscriptionKey, region);
            using (var translationRecognizer = factory.CreateTranslationRecognizerWithFileInput(TestData.English.Weather.AudioFile, fromLanguage, toLanguages, voice))
            {
                Assert.AreEqual(translationRecognizer.SourceLanguage, fromLanguage);
                CollectionAssert.AreEqual(translationRecognizer.TargetLanguages, toLanguages);
                Assert.AreEqual(translationRecognizer.OutputVoiceName, voice);
            }
        }

        [TestMethod]
        [Ignore] // Bug 1294947
        public async Task TranslationBatmanEnToDeFinalTextResult()
        {
            var toLanguages = new List<string>() { Language.DE };

            Console.WriteLine(TestData.English.Batman.AudioFile);
            var actualTranslation = await this.translationHelper.GetTranslationFinalResult(TestData.English.Batman.AudioFile, Language.EN, toLanguages);
            Assert.IsNotNull(actualTranslation);

            Assert.AreEqual(TestData.English.Batman.Utterances[0], actualTranslation.Text);
            Assert.AreEqual(TestData.German.Batman.Utterances[0], actualTranslation.Translations[Language.DE]);
        }

        [TestMethod]
        [Ignore]
        public async Task TranslationBatmanEntoFrAndEsFinalTextResult()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var actualTranslation = await this.translationHelper.GetTranslationFinalResult(TestData.English.Batman.AudioFile, Language.EN, toLanguages);
            Assert.IsNotNull(actualTranslation);
            Assert.AreEqual(TestData.English.Batman.Utterances[0], actualTranslation.Text);

            Assert.AreEqual(TestData.French.Batman.Utterances[0], actualTranslation.Translations[Language.FR]);
            Assert.AreEqual(TestData.Spanish.Batman.Utterances[0], actualTranslation.Translations[Language.ES]);
        }

        [Ignore] // TODO ENABLE AFTER FIXING BROKEN SERVICE
        [TestMethod]
        public async Task TranslationBatmanEnToDeFinalTextResultContinuous()
        {
            List<string> toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationFinalResultContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages);
            Assert.AreEqual(14, actualTranslations[ResultType.Text].Count);

            var actualRecognitionTextResults = actualTranslations[ResultType.Text].Cast<TranslationTextResultEventArgs>().Select(t => t.Result.Text).ToList();
            var actualTranslationsTextResults = actualTranslations[ResultType.Text].Cast<TranslationTextResultEventArgs>().Select(t => t.Result.Translations[Language.DE]).ToList();

            CollectionAssert.AreEquivalent(TestData.English.Batman.Utterances, actualRecognitionTextResults);
            CollectionAssert.AreEquivalent(TestData.German.Batman.Utterances, actualTranslationsTextResults);
        }

        [TestMethod]
        [Ignore]
        public async Task TranslationFirstOneDeToFrAndEsFinalTextResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var actualTranslations = await this.translationHelper.GetTranslationFinalResultContinuous(TestData.German.FirstOne.AudioFile, Language.DE_DE, toLanguages);
            Assert.AreEqual(actualTranslations[ResultType.Text].Count, 1);
            var actualTranslationRecognition = (TranslationTextResultEventArgs)actualTranslations[ResultType.Text].Single();
            Assert.IsNotNull(actualTranslationRecognition);

            Assert.AreEqual(TranslationStatus.Success, actualTranslationRecognition.Result.TranslationStatus);
            Assert.AreEqual(TestData.German.FirstOne.Utterance, actualTranslationRecognition.Result.Text);

            Assert.AreEqual(TestData.French.FirstOne.Utterance, actualTranslationRecognition.Result.Translations[Language.FR]);
            Assert.AreEqual(TestData.Spanish.FirstOne.Utterance, actualTranslationRecognition.Result.Translations[Language.ES]);
        }

        [TestMethod]
        [Ignore]
        public async Task TranslationBatmanEnToDeKatjaSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationFinalResultContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages, Voice.DE);
            Assert.AreNotEqual(actualTranslations[ResultType.Synthesis].Count, 0);
            var actualSynthesisByteResults = actualTranslations[ResultType.Synthesis].Cast<TranslationSynthesisResultEventArgs>().Select(t => t.Result.Audio).ToList();

            var expectedSynthesisByteResults = Config.GetByteArraysForFilesWithSamePrefix(synthesisDir, "de_katja_batman_utterance");

            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[0], actualSynthesisByteResults[0]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[1], actualSynthesisByteResults[1]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[2], actualSynthesisByteResults[2]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[3], actualSynthesisByteResults[3]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[4], actualSynthesisByteResults[4]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[5], actualSynthesisByteResults[5]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[6], actualSynthesisByteResults[6]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[7], actualSynthesisByteResults[7]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[8], actualSynthesisByteResults[8]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[9], actualSynthesisByteResults[9]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[10], actualSynthesisByteResults[10]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[11], actualSynthesisByteResults[11]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[12], actualSynthesisByteResults[12]);
            CollectionAssert.AreEquivalent(expectedSynthesisByteResults[13], actualSynthesisByteResults[13]);
        }

        [TestMethod]
        [Ignore]
        public async Task TranslationWeatherEnToFrCarolineSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR };

            var actualTranslations = await this.translationHelper.GetTranslationFinalResultContinuous(TestData.English.Weather.AudioFile, Language.EN, toLanguages, Voice.FR);
            Assert.AreEqual(1, actualTranslations[ResultType.Synthesis].Count);

            var expectedSynthesisByteResult = Config.GetByteArraysForFilesWithSamePrefix(synthesisDir, "fr_caroline_weather");
            var actualSynthesisByteResult = (TranslationSynthesisResultEventArgs)actualTranslations[ResultType.Synthesis].Single();
            CollectionAssert.AreEqual(expectedSynthesisByteResult[0], actualSynthesisByteResult.Result.Audio);
        }

        [TestMethod]
        public async Task TranslationWeatherEnToFrCarolineTextAndSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR };

            var actualTranslations = await this.translationHelper.GetTranslationFinalResultContinuous(TestData.English.Weather.AudioFile, Language.EN, toLanguages, Voice.FR);
            Assert.AreEqual(actualTranslations[ResultType.Text].Count, 1);
            Assert.AreEqual(actualTranslations[ResultType.Synthesis].Count, 1);

            var actualTextReuslt = (TranslationTextResultEventArgs)actualTranslations[ResultType.Text].Single();
            Assert.AreEqual(TestData.English.Weather.Utterance, actualTextReuslt.Result.Text);

            var expectedSynthesisByteResult = Config.GetByteArraysForFilesWithSamePrefix(synthesisDir, "fr_caroline_weather");
            var actualSynthesisByteResult = (TranslationSynthesisResultEventArgs)actualTranslations[ResultType.Synthesis].Single();
            CollectionAssert.AreEqual(expectedSynthesisByteResult[0], actualSynthesisByteResult.Result.Audio);
        }

        [TestMethod]
        public async Task TranslationWeatherEnToTrIntermediateResultContinuous()
        {
            var toLanguages = new List<string>() { Language.TR };

            var actualTranslations = await this.translationHelper.GetTranslationIntermediateResultContinuous(TestData.English.Weather.AudioFile, Language.EN, toLanguages);
            Assert.AreNotEqual(actualTranslations.Count, 0);

            Assert.IsTrue(actualTranslations[0].Last().Result.Text.Contains("What"));
        }

        [TestMethod]
        [Ignore]
        public async Task TranslationAsyncRecognizerEventReceived()
        {
            var toLanguages = new List<string>() { Language.DE };
            var observedEvent = await this.translationHelper.GetTranslationFinalResultEvents(TestData.English.Batman.AudioFile, Language.EN, toLanguages);
            Assert.IsNull(observedEvent);
        }
    }
}

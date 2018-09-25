//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static TranslationTestsHelper;
    using static SpeechRecognitionTestsHelper;

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
            this.translationHelper = new TranslationTestsHelper(RecognitionTestBase.subscriptionKey, RecognitionTestBase.region);
        }

        [TestMethod]
        public void TestLanguageProperties()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                Assert.AreEqual(translationRecognizer.SpeechRecognitionLanguage, Language.EN);
                CollectionAssert.AreEqual(translationRecognizer.TargetLanguages.ToList(), new List<string> { Language.DE });
                Assert.AreEqual(translationRecognizer.VoiceName, String.Empty);
            }
        }

        [TestMethod]
        public void TestLanguagePropertiesMultiTargets()
        {
            var toLanguages = new List<string>() { Language.DE, Language.ES };
            var fromLanguage = Language.EN;
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = fromLanguage;

            toLanguages.ForEach(l => config.AddTargetLanguage(l));
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                Assert.AreEqual(translationRecognizer.SpeechRecognitionLanguage, fromLanguage);
                CollectionAssert.AreEqual(translationRecognizer.TargetLanguages.ToList(), toLanguages);
                Assert.AreEqual(translationRecognizer.VoiceName, String.Empty);
            }
        }

        [TestMethod]
        public void TestVoiceName()
        {
            var toLanguages = new List<string>() { Language.DE };
            var fromLanguage = Language.EN;
            var voice = Voice.DE;
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = fromLanguage;
            config.VoiceName = voice;

            toLanguages.ForEach(l => config.AddTargetLanguage(l));
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                Assert.AreEqual(translationRecognizer.SpeechRecognitionLanguage, fromLanguage);
                CollectionAssert.AreEqual(translationRecognizer.TargetLanguages.ToList(), toLanguages);
                Assert.AreEqual(translationRecognizer.VoiceName, voice);
            }
        }

        [TestMethod]
        public async Task TranslationWeatherEnToDeFinalTextResult()
        {
            var toLanguages = new List<string>() { Language.DE, Language.ZH };
            var result = await this.translationHelper.GetTranslationFinalResult(TestData.English.Weather.AudioFile, Language.EN, toLanguages);

            Assert.IsNotNull(result, "Translation should not be null");
            Console.WriteLine(result.ToString());

            var errorDetails = result.Reason == ResultReason.Canceled ? CancellationDetails.FromResult(result).ErrorDetails : "";
            Console.WriteLine($"Reason: {result.Reason}, ErrorDetails: {errorDetails}");
            
            Assert.AreEqual(TestData.English.Weather.Utterance, result.Text);
            Assert.AreEqual(TestData.German.Weather.Utterance, result.Translations[Language.DE]);
            Assert.AreEqual(TestData.Chinese.Weather.Utterance, result.Translations[Language.ZH]);
        }

        [TestMethod]
        [Ignore]
        public async Task TranslationWeatherEntoFrAndEsFinalTextResult()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var result = await this.translationHelper.GetTranslationFinalResult(TestData.English.Weather.AudioFile, Language.EN, toLanguages);

            Assert.IsNotNull(result, "Translation should not be null");
            Console.WriteLine(result.ToString());

            var errorDetails = result.Reason == ResultReason.Canceled ? CancellationDetails.FromResult(result).ErrorDetails : "";
            Console.WriteLine($"Reason: {result.Reason}, ErrorDetails: {errorDetails}");

            Assert.AreEqual(TestData.English.Weather.Utterance, result.Text);
            Assert.AreEqual(TestData.French.Weather.Utterance, result.Translations[Language.FR]);
            Assert.AreEqual(TestData.Spanish.Weather.Utterance, result.Translations[Language.ES]);
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task TranslationBatmanEnToDeFinalTextResultContinuous()
        {
            List<string> toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages);
            Assert.AreEqual(TestData.German.Batman.Utterances.Length, actualTranslations[ResultType.Text].Count);

            var actualRecognitionTextResults = actualTranslations[ResultType.Text].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Text).ToList();
            var actualTranslationsTextResults = actualTranslations[ResultType.Text].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Translations[Language.DE]).ToList();
            for (var i = 0; i < actualTranslations.Count; i++)
            {
                AssertMatching(TestData.English.Batman.Utterances[i], actualRecognitionTextResults[i]);
            }
        }

        [TestMethod]
        [Ignore]
        public async Task TranslationFirstOneDeToFrAndEsFinalTextResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.German.FirstOne.AudioFile, Language.DE_DE, toLanguages);
            Assert.AreEqual(actualTranslations[ResultType.Text].Count, 1);
            var actualTranslationRecognition = (TranslationRecognitionEventArgs)actualTranslations[ResultType.Text].Single();
            Assert.IsNotNull(actualTranslationRecognition);

            Assert.AreNotEqual(ResultReason.Canceled, actualTranslationRecognition.Result.Reason);
            Assert.AreEqual(TestData.German.FirstOne.Utterance, actualTranslationRecognition.Result.Text);

            Assert.AreEqual(TestData.French.FirstOne.Utterance, actualTranslationRecognition.Result.Translations[Language.FR]);
            Assert.AreEqual(TestData.Spanish.FirstOne.Utterance, actualTranslationRecognition.Result.Translations[Language.ES]);
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task TranslationBatmanEnToDeKatjaSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages, Voice.DE);
            Assert.AreNotEqual(actualTranslations[ResultType.Synthesis].Count, 0);
            var actualSynthesisByteResults = actualTranslations[ResultType.Synthesis].Cast<TranslationSynthesisEventArgs>().ToList();
            const int MinSize = 20000;
            foreach (var s in actualSynthesisByteResults)
            {
                Console.WriteLine($"Audio.Length: {s.Result.GetAudio().Length}");
                Assert.IsTrue(s.Result.GetAudio().Length > MinSize, $"Expects audio size {s.Result.GetAudio().Length} to be greater than {MinSize}");
            }
        }

        [TestMethod]
        public async Task SynthesisWeatherEnToFrCarolineShortVoice()
        {
            await TranslationWeatherEnToFrCarolineSynthesis(Voice.FR);
        }

        [TestMethod]
        public async Task SynthesisWeatherEnToFrCarolineFullVoice()
        {
            await TranslationWeatherEnToFrCarolineSynthesis("Microsoft Server Speech Text to Speech Voice (fr-FR, Julie, Apollo)");
        }

        public async Task TranslationWeatherEnToFrCarolineSynthesis(string voice)
        {
            var toLanguages = new List<string>() { Language.FR };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Weather.AudioFile, Language.EN, toLanguages, voice);
            Assert.AreEqual(1, actualTranslations[ResultType.Synthesis].Count);

            var actualSynthesisByteResult = (TranslationSynthesisEventArgs)actualTranslations[ResultType.Synthesis].Single();
            const int MinSize = 50000;
            Assert.IsTrue(actualSynthesisByteResult.Result.GetAudio().Length > MinSize,
                $"Received response for speech synthesis is less than {MinSize}: {actualSynthesisByteResult.Result.GetAudio().Length}.");
        }

        [TestMethod]
        public async Task TranslationWeatherEnToFrCarolineTextAndSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Weather.AudioFile, Language.EN, toLanguages, Voice.FR);
            Assert.AreEqual(1, actualTranslations[ResultType.Text].Count);
            Assert.AreEqual(1, actualTranslations[ResultType.Synthesis].Count);

            var actualTextResult = (TranslationRecognitionEventArgs)actualTranslations[ResultType.Text].Single();
            Assert.AreEqual(TestData.English.Weather.Utterance, actualTextResult.Result.Text);

            var actualSynthesisByteResult = (TranslationSynthesisEventArgs)actualTranslations[ResultType.Synthesis].Single();
            const int MinSize = 50000;
            Assert.IsTrue(actualSynthesisByteResult.Result.GetAudio().Length > MinSize,
                $"Received response for speech synthesis is less than {MinSize}: {actualSynthesisByteResult.Result.GetAudio().Length}.");
        }

        [TestMethod]
        public async Task TranslationWeatherEnToTrRecognizingContinuous()
        {
            var toLanguages = new List<string>() { Language.TR };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizingContinuous(TestData.English.Weather.AudioFile, Language.EN, toLanguages);
            Assert.AreNotEqual(actualTranslations.Count, 0);

            Assert.IsTrue(actualTranslations[0].Last().Result.Text.Contains("What"));
        }

        [TestMethod]
        public async Task TestInitialSilenceTimeout()
        {
            var toLanguages = new List<string>() { Language.DE };
            var result = await this.translationHelper.GetTranslationFinalResult(TestData.English.Silence.AudioFile, Language.EN, toLanguages);

            Assert.IsNotNull(result, "Translation should not be null");
            Console.WriteLine(result.ToString());

            var errorDetails = result.Reason == ResultReason.Canceled ? CancellationDetails.FromResult(result).ErrorDetails : "";
            Console.WriteLine($"Reason: {result.Reason}, ErrorDetails: {errorDetails}");
            Assert.AreEqual(ResultReason.NoMatch, result.Reason);
            var noMatch = NoMatchDetails.FromResult(result);
            Assert.AreEqual(NoMatchReason.InitialSilenceTimeout, noMatch.Reason);
        }

    }
}

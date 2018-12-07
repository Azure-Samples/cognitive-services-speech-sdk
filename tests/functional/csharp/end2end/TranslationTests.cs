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
        public async Task TestInvalidTargetLanguageWithRecognizedOnce()
        {
            var toLanguages = new List<string>() { "invalidLanguages" };
            var result = await this.translationHelper.GetTranslationFinalResult(TestData.English.Weather.AudioFile, Language.EN, toLanguages);

            Assert.IsNotNull(result, "Translation should not be null");
            var errorDetails = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails, "");
            Console.WriteLine($"Result: {result.ToString()}, Error details: ErrorDetails: {errorDetails}");
            Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason, "Unexpected result reason.");
            Assert.AreEqual(TestData.English.Weather.Utterance, result.Text, "Unmatched recognized text.");
            Assert.AreEqual(0, result.Translations.Count, "Unmatched translation results.");
            Assert.AreEqual(TestData.ExpectedErrorDetails.InvalidTargetLanaguageErrorMessage, errorDetails, "Unmatched error details.");
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task TestInvalidTargetLanguageWithContinuousRecognition()
        {
            var toLanguages = new List<string>() { "invalidLanguages" };
            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages, voice:null, requireTranslatedSpeech:false);
            Assert.AreEqual(TestData.German.Batman.Utterances.Length, actualTranslations[ResultType.RecognizedText].Count, "Unmatched number of recognized utterances");
            var actualTranslationsTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result).ToList();
            for (var i = 0; i < actualTranslationsTextResults.Count; i++)
            {
                var result = actualTranslationsTextResults[i];
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason, "Unmatched result reason.");
                AssertMatching(TestData.English.Batman.Utterances[i], result.Text);
                Assert.AreEqual(0, result.Translations.Count, "Unmatched translation results");
                var errorDetails = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails, "");
                Assert.AreEqual(TestData.ExpectedErrorDetails.InvalidTargetLanaguageErrorMessage, errorDetails, "Unmatched error details");
            }
        }

        [TestMethod]
        public async Task TestInvalidVoice()
        {
            var toLanguages = new List<string>() { Language.FR };
            using (var recognizer = TrackSessionId(this.translationHelper.CreateTranslationRecognizer(TestData.English.Weather.AudioFile, Language.EN, toLanguages, "InvalidVoice")))
            {
                var tcs = new TaskCompletionSource<bool>();
                bool receivedSynthesizingEvent = false;
                CancellationErrorCode errorCode = CancellationErrorCode.NoError;
                string errorDetails = string.Empty;

                recognizer.Canceled += (s, e) =>
                {
                    if (e.Reason == CancellationReason.Error)
                    {
                        errorCode = CancellationErrorCode.ServiceError;
                        errorDetails = e.ErrorDetails;
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    Assert.AreEqual(ResultReason.TranslatedSpeech, e.Result.Reason, "Unmatched result reason.");
                    Assert.AreEqual(TestData.English.Weather.Utterance, e.Result.Text, "Unmatched recognized text.");
                    Assert.AreEqual(TestData.French.Weather.Utterance, e.Result.Translations[toLanguages[0]], "Unmatched translation result.");
                };

                recognizer.Synthesizing += (s, e) =>
                {
                    receivedSynthesizingEvent = true;
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Received stopped session event: {e.ToString()}");
                    tcs.TrySetResult(true);
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromSeconds(30)));
                await recognizer.StopContinuousRecognitionAsync();

                Assert.AreEqual(CancellationErrorCode.ServiceError, errorCode, "Unmatched error code.");
                Assert.AreEqual(TestData.ExpectedErrorDetails.InvalidVoiceNameErrorMessage, errorDetails, "Unmatched error message.");
                Assert.IsFalse(receivedSynthesizingEvent, "Received unexpected synthesizing event.");
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
            Assert.AreEqual(TestData.German.Batman.Utterances.Length, actualTranslations[ResultType.RecognizedText].Count);

            var actualRecognitionTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Text).ToList();
            var actualTranslationsTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Translations[Language.DE]).ToList();
            for (var i = 0; i < actualTranslations.Count; i++)
            {
                AssertMatching(TestData.English.Batman.Utterances[i], actualRecognitionTextResults[i]);
                AssertMatching(TestData.German.Batman.Utterances[i], actualTranslationsTextResults[i]);
            }
        }

        [TestMethod]
        public async Task TranslationFirstOneDeToFrAndEsFinalTextResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.German.FirstOne.AudioFile, Language.DE_DE, toLanguages);
            Assert.AreEqual(actualTranslations[ResultType.RecognizedText].Count, 1);
            var actualTranslationRecognition = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();
            Assert.IsNotNull(actualTranslationRecognition);

            Assert.AreNotEqual(ResultReason.Canceled, actualTranslationRecognition.Result.Reason);
            Assert.AreEqual(TestData.German.FirstOne.Utterance, actualTranslationRecognition.Result.Text);

            Assert.AreEqual(TestData.French.FirstOne.Utterance, actualTranslationRecognition.Result.Translations[Language.FR]);
            Assert.AreEqual(TestData.Spanish.FirstOne.Utterance, actualTranslationRecognition.Result.Translations[Language.ES]);
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task TranslationBatmanEnToDeHeddaRUSSynthesisResultContinuous()
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
            Assert.AreEqual(1, actualTranslations[ResultType.RecognizedText].Count);
            Assert.AreEqual(1, actualTranslations[ResultType.Synthesis].Count);

            var actualTextResult = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();
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
            Assert.AreNotEqual(actualTranslations[ResultType.RecognizedText].Count, 0);
            Assert.IsTrue(actualTranslations[ResultType.RecognizedText][0].Result.Text.Contains("What"));

            if (actualTranslations[ResultType.RecognizingText].Count > 0)
            {
                Assert.IsTrue(actualTranslations[ResultType.RecognizingText].Last().Result.Text.Contains("What"));
            }
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

        [TestMethod]
        public async Task TranslationFromCatalanToGerman()
        {
            var toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.Catalan.AudioFile, Language.CA_ES, toLanguages);
            Assert.AreEqual(actualTranslations[ResultType.RecognizedText].Count, 1);
            var actualTranslationRecognition = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();
            Assert.IsNotNull(actualTranslationRecognition);

            Assert.AreNotEqual(ResultReason.Canceled, actualTranslationRecognition.Result.Reason);
            Assert.AreEqual(TestData.Catalan.HowIsYourWork.Utterance, actualTranslationRecognition.Result.Text);

            Assert.AreEqual(TestData.German.HowIsYourWork.Utterance, actualTranslationRecognition.Result.Translations[Language.DE]);
        }

        [TestMethod]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task AsyncRecognitionAfterDisposingTranslationRecognizer()
        {
            var toLanguages = new List<string>() { Language.DE };
            var recognizer = this.translationHelper.CreateTranslationRecognizer(TestData.Catalan.AudioFile, Language.CA_ES, toLanguages);
            recognizer.Dispose();
            await recognizer.StopContinuousRecognitionAsync();
        }

        [TestMethod]
        public void DisposingTranslationRecognizerWhileAsyncRecognition()
        {
            var toLanguages = new List<string>() { Language.DE };
            using (var recognizer = this.translationHelper.CreateTranslationRecognizer(TestData.English.Batman.AudioFile, Language.CA_ES, toLanguages))
            {
                Task singleShot = null;
                Assert.ThrowsException<InvalidOperationException>(() =>
                {
                    singleShot = recognizer.RecognizeOnceAsync();
                    Thread.Sleep(100);
                    recognizer.Dispose();
                });
                singleShot.Wait();
            }
        }
    }
}

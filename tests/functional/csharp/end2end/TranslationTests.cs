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
    using static AssertHelpers;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public sealed class TranslationTests : RecognitionTestBase
    {
        private TranslationTestsHelper translationHelper;
        private static string synthesisDir;
        private static string deploymentId;
        private static string endpointInString;
        private static Uri endpointUrl;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
            synthesisDir = Path.Combine(inputDir, "synthesis");
            deploymentId = Config.GetSettingByKey<String>(context, "DeploymentId");
            endpointInString = String.Format("wss://{0}.s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1", region);
            endpointUrl = new Uri(endpointInString);
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
                Assert.AreEqual(Language.EN, translationRecognizer.SpeechRecognitionLanguage);
                CollectionAssert.AreEqual(new List<string> { Language.DE }, translationRecognizer.TargetLanguages.ToList());
                Assert.AreEqual(String.Empty, translationRecognizer.VoiceName);
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
                Assert.AreEqual(fromLanguage, translationRecognizer.SpeechRecognitionLanguage);
                CollectionAssert.AreEqual(toLanguages, translationRecognizer.TargetLanguages.ToList());
                Assert.AreEqual(String.Empty, translationRecognizer.VoiceName);
            }
        }

        [TestMethod]
        public async Task TestNoFromInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await translationRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.Canceled, result.Reason);
                var errorCode = CancellationDetails.FromResult(result).ErrorCode;
                Assert.AreEqual(CancellationErrorCode.BadRequest, errorCode);
            }
        }

        [TestMethod]
        public async Task TestNoToInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await translationRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason);
                Assert.AreEqual(TestData.English.Weather.Utterance, result.Text);
                var errorDetails = CancellationDetails.FromResult(result);
                // Only translation error. 
                Assert.AreEqual(CancellationErrorCode.NoError, errorDetails.ErrorCode);
                Assert.IsTrue(errorDetails.ErrorDetails.Contains("The target language is not valid"), "Actual error:'" + errorDetails.ErrorDetails + "' does not contain expected string.");
            }
        }

        [TestMethod]
        public async Task TestDefaultOutputInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await translationRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                Assert.AreEqual(ResultReason.TranslatedSpeech, result.Reason);
                Assert.AreEqual(TestData.English.Weather.Utterance, result.Text);
                Assert.AreEqual(1, result.Translations.Count);
                AssertMatching(TestData.German.Weather.Utterance, result.Translations[Language.DE]);
                // Default output format is simple.
                AssertDetailedOutput(result, false);
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
                Assert.AreEqual(fromLanguage, translationRecognizer.SpeechRecognitionLanguage);
                CollectionAssert.AreEqual(toLanguages, translationRecognizer.TargetLanguages.ToList());
                Assert.AreEqual(voice, translationRecognizer.VoiceName);
            }
        }


        [TestMethod]
        public async Task TestInvalidTargetLanguageWithRecognizedOnce()
        {
            var toLanguages = new List<string>() { "invalidLanguages" };
            var result = await this.translationHelper.GetTranslationFinalResult(TestData.English.Weather.AudioFile, Language.EN, toLanguages);

            Assert.IsNotNull(result, "Translation should not be null");
            var errorDetails = CancellationDetails.FromResult(result).ErrorDetails;
            Assert.IsFalse(String.IsNullOrEmpty(errorDetails), "Error details cannot be empty when translation language is invalid.");
            Console.WriteLine($"Result: {result.ToString()}, Error details: ErrorDetails: {errorDetails}");
            Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason, "Unexpected result reason.");
            AssertMatching(TestData.English.Weather.Utterance, result.Text);
            Assert.AreEqual(0, result.Translations.Count, "Unmatched translation results.");
            AssertMatching(TestData.ExpectedErrorDetails.InvalidTargetLanaguageErrorMessage, errorDetails);
        }

        [TestMethod]
        public async Task TestInvalidTargetLanguageWithContinuousRecognition()
        {
            var toLanguages = new List<string>() { "invalidLanguages" };
            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages, voice: null, requireTranslatedSpeech: false);

            // only look at non-empty results
            var actualTranslationsTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result).ToArray();
            actualTranslationsTextResults = actualTranslationsTextResults.Where(r => !string.IsNullOrEmpty(r.Text)).ToArray();

            var expectedUtterances = TestData.English.Batman.UtterancesTranslation.Where(t => !string.IsNullOrEmpty(t)).ToArray();

            Assert.AreEqual(expectedUtterances.Count(), actualTranslationsTextResults.Count());
            for (var i = 0; i < actualTranslationsTextResults.Count(); i++)
            {
                var result = actualTranslationsTextResults[i];
                Assert.AreEqual(ResultReason.RecognizedSpeech, result.Reason, "Unmatched result reason.");

                Assert.AreEqual(0, result.Translations.Count, "Unmatched translation results");
                var errorDetails = CancellationDetails.FromResult(result).ErrorDetails;
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
                TranslationRecognitionResult result = null;

                recognizer.Canceled += (s, e) =>
                {
                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"Canceled event. ErrorCode:{e.ErrorCode}, ErrorDetails:{e.ErrorDetails}");
                        errorCode = e.ErrorCode;
                        errorDetails = e.ErrorDetails;
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Recognized: Reason: {e.Result.Reason}, Text: {e.Result.Text}, Translation: {e.Result.Translations[toLanguages[0]]}.");
                    result = e.Result;
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

                Assert.IsTrue(result != null, "No result received.");
                Assert.AreEqual(CancellationErrorCode.ServiceError, errorCode, "Unmatched error code.");
                AssertMatching(TestData.ExpectedErrorDetails.InvalidVoiceNameErrorMessage, errorDetails);
                Assert.IsFalse(receivedSynthesizingEvent, "Received unexpected synthesizing event.");
                Assert.AreEqual(ResultReason.TranslatedSpeech, result.Reason, "Unmatched result reason.");
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                AssertMatching(TestData.French.Weather.Utterance, result.Translations[toLanguages[0]]);
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

            AssertMatching(TestData.English.Weather.Utterance, result.Text);
            AssertMatching(TestData.French.Weather.Utterance, result.Translations[Language.FR]);
            AssertOneEqual(TestData.Spanish.Weather.PossibleUtterances, result.Translations[Language.ES]);
        }

        [TestMethod]
        public async Task TranslationBatmanEnToDeFinalTextResultContinuous()
        {
            List<string> toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages);

            var actualRecognitionTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Text).ToArray();
            actualRecognitionTextResults = actualRecognitionTextResults.Where(t => !string.IsNullOrEmpty(t)).ToArray();
            var actualTranslationsTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Translations[Language.DE]).ToArray();
            actualTranslationsTextResults = actualTranslationsTextResults.Where(t => !string.IsNullOrEmpty(t)).ToArray();

            var expectedUtterances = TestData.English.Batman.UtterancesTranslation.Where(t => !string.IsNullOrEmpty(t)).ToArray();
            var expectedTranslations = TestData.German.Batman.Utterances.Where(t => !string.IsNullOrEmpty(t)).ToArray();

            // compare only nonempty utterances, either expected or actual
            Assert.AreEqual(expectedUtterances.Count(), actualTranslationsTextResults.Count());
            for (var i = 0; i < actualTranslations.Count; i++)
            {
                AssertStringWordEditPercentage(Normalize(expectedUtterances[i]), Normalize(actualRecognitionTextResults[i]), 5, 2);
                AssertStringWordEditPercentage(Normalize(expectedTranslations[i]), Normalize(actualTranslationsTextResults[i]), 5, 2);
            }
        }

        [TestMethod]
        public async Task TranslationFirstOneDeToFrAndEsFinalTextResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.German.FirstOne.AudioFile, Language.DE_DE, toLanguages);
            Assert.AreEqual(1, actualTranslations[ResultType.RecognizedText].Count);
            var actualTranslationRecognition = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();
            Assert.IsNotNull(actualTranslationRecognition);

            Assert.AreNotEqual(ResultReason.Canceled, actualTranslationRecognition.Result.Reason);
            AssertMatching(TestData.German.FirstOne.Utterance, actualTranslationRecognition.Result.Text);

            AssertMatching(TestData.French.FirstOne.Utterance, actualTranslationRecognition.Result.Translations[Language.FR]);
            AssertOneEqual(TestData.Spanish.FirstOne.PossibleUtterances, actualTranslationRecognition.Result.Translations[Language.ES]);
        }

        [TestMethod]
        public async Task TranslationBatmanEnToDeHeddaRUSSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Batman.AudioFile, Language.EN, toLanguages, voice: Voice.DE);
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

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(TestData.English.Weather.AudioFile, Language.EN, toLanguages, voice: voice);
            Assert.AreEqual(1, actualTranslations[ResultType.Synthesis].Count);

            var actualSynthesisByteResult = (TranslationSynthesisEventArgs)actualTranslations[ResultType.Synthesis].Single();
            const int MinSize = 49000;
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
            Assert.AreEqual(1, actualTranslations[ResultType.RecognizedText].Count);
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

        [DataTestMethod, TestCategory(TestCategory.LongRunning)]
        [DynamicData(nameof(Language.LocaleAndLang), typeof(Language), DynamicDataSourceType.Property)]
        public async Task TranslateFromEachLocaletoEachTextLang(string locale, string lang)
        {
            var result = await this.translationHelper.GetTranslationFinalResult(TestData.English.Weather.AudioFile, locale, new List<string> { lang });
            Assert.IsNotNull(result, "Translation should not be null");
            Assert.AreEqual(ResultReason.TranslatedSpeech, result.Reason, "Unmatched result reason.");
            Assert.IsTrue(String.IsNullOrEmpty(CancellationDetails.FromResult(result).ErrorDetails));
            Assert.IsFalse(String.IsNullOrEmpty(result.Text), $"locale: {locale}, language: {lang}, result: {result.ToString()}");
            Assert.AreEqual(1, result.Translations.Count);
        }

        [DataTestMethod, TestCategory(TestCategory.LongRunning)]
        [DynamicData(nameof(Voice.LangAndSynthesis), typeof(Voice), DynamicDataSourceType.Property)]
        public async Task TranslateFromENtoEachLangWithSynthesis(string lang, string voice)
        {
            var tuple = await this.translationHelper.GetTranslationSynthesisAndFinalResult(TestData.English.Weather.AudioFile, Language.EN, new List<string> { lang }, voice);
            foreach (var e in tuple.Item1)
            {
                if (e.Result.GetAudio().Length == 0)
                {
                    Assert.AreEqual(e.Result.Reason, ResultReason.SynthesizingAudioCompleted, $"Synthesizing event failure: Reason:{0} Audio.Length={1}", e.Result.Reason, e.Result.GetAudio().Length);
                }
            }

            var result = tuple.Item2;
            Assert.IsNotNull(result, "Translation result should not be null");
            Assert.IsFalse(String.IsNullOrEmpty(result.Text), $"locale: { Language.EN }, language: { lang }, result: { result.ToString() }");
        }

        [TestMethod]
        public async Task TranslateFromENtoTongan()
        {
            var result = await this.translationHelper.GetTranslationFinalResult(TestData.English.Weather.AudioFile, Language.EN, new List<string> { "to" });
            Assert.IsNotNull(result, "Failed to recognize and translate From English audio file to Tongal.");
            Assert.AreEqual(TestData.English.Weather.Utterance, result.Text, "Failed to recognize text correctly.");
            Assert.AreEqual(ResultReason.TranslatedSpeech, result.Reason, $"Unexpected result reason. Cancellation error details: { CancellationDetails.FromResult(result).ErrorDetails }");
            Assert.AreEqual(1, result.Translations.Count, "Unmatched translation results.");
        }

        [TestMethod]
        public void TestSetAndGetAuthToken()
        {
            var token = "x";
            var config = SpeechTranslationConfig.FromAuthorizationToken(token, "westus");
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            var recognizer = new TranslationRecognizer(config, audioInput);
            Assert.AreEqual(token, recognizer.AuthorizationToken);

            var newToken = "y";
            recognizer.AuthorizationToken = newToken;
            Assert.AreEqual(token, config.AuthorizationToken);
            Assert.AreEqual(newToken, recognizer.AuthorizationToken);
        }

        [TestMethod]
        public void TestSetAndGetSubKey()
        {
            var config = SpeechTranslationConfig.FromSubscription("x", "westus");
            Assert.AreEqual("x", config.SubscriptionKey);
        }

        [TestMethod]
        public async Task ContinuousValidCustomTranslation()
        {
            var toLanguages = new List<string>() { Language.DE };
            var actualTranslations = await this.translationHelper.GetTranslationRecognizingContinuous(TestData.English.Weather.AudioFile, "", toLanguages, deploymentId);
            Assert.AreNotEqual(actualTranslations[ResultType.RecognizedText].Count, 0, "Number of translations should not be zero.");
            AssertMatching(TestData.German.Weather.Utterance, actualTranslations[ResultType.RecognizedText].Last().Result.Translations[Language.DE]);
        }

        [TestMethod]
        public async Task ContinuousInvalidCustomTranslation()
        {
            var toLanguages = new List<string>() { Language.DE };
            var actualTranslations = await this.translationHelper.GetTranslationRecognizingContinuous(TestData.English.Weather.AudioFile, "", toLanguages, "invalidid", true);
            Assert.AreEqual(0, actualTranslations[ResultType.RecognizedText].Count, "Number of translations should be zero.");
            Assert.AreEqual(0, actualTranslations[ResultType.RecognizingText].Count, "Number of translations should be zero.");
            Assert.AreEqual(1, actualTranslations[ResultType.Cancelled].Count, "Number of cancelled events should not be zero.");
            var errorDetails = actualTranslations[ResultType.Cancelled].Cast<TranslationRecognitionCanceledEventArgs>().Last().ErrorDetails;
            Assert.IsTrue(errorDetails.Contains("bad request"));
        }

        [TestMethod]
        [ExpectedException(typeof(ApplicationException), "Cannot change log filename after the first recognizer has been created.")]
        public void SetAndRenameLogFilename()
        {
            var toLanguages = new List<string>() { Language.DE };
            var config = this.translationHelper.GetConfig(Language.EN, toLanguages, "");
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);

            var logFilename = "test_filename.txt";
            var logFilename2 = "test_filename2.txt";
            config.SetProperty(PropertyId.Speech_LogFilename, logFilename);
            Assert.AreEqual(logFilename, config.GetProperty(PropertyId.Speech_LogFilename));
            var recognizer = new TranslationRecognizer(config, audioInput);
            Assert.IsTrue(File.Exists(logFilename), "log file must exist when recognizer is created");

            config.SetProperty(PropertyId.Speech_LogFilename, logFilename2);
            Assert.AreEqual(logFilename2, config.GetProperty(PropertyId.Speech_LogFilename));
            Assert.IsFalse(File.Exists(logFilename2));
            
            try
            {
                var recognizer2 = new TranslationRecognizer(config, audioInput);
            }
            finally
            {
                Assert.IsTrue(new FileInfo(logFilename).Length > 0, "log file must contain logs after recognizer created");
            }
        }

        [TestMethod]
        public void SetLogFilename()
        {
            var toLanguages = new List<string>() { Language.DE };
            var config = this.translationHelper.GetConfig(Language.EN, toLanguages, "");
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);

            var logFilename = "test_filename.txt";
            config.SetProperty(PropertyId.Speech_LogFilename, logFilename);
            Assert.AreEqual(logFilename, config.GetProperty(PropertyId.Speech_LogFilename));
            var recognizer = new TranslationRecognizer(config, audioInput);
            Assert.IsTrue(new FileInfo(logFilename).Length > 0, "log file must contain logs after recognizer created");
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentException), "Cannot set a property to whitespace")]
        public void SetPropertyToWhiteSpace()
        {
            var toLanguages = new List<string>() { Language.DE };
            var config = this.translationHelper.GetConfig(Language.EN, toLanguages, "");
            config.SetProperty(PropertyId.Speech_LogFilename, " ");
        }

        [TestMethod]
        public async Task FromEndpointDeToFrTranslationWithVoice()
        {
            var configFromEndpoint = SpeechTranslationConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.SpeechRecognitionLanguage = Language.DE_DE;
            configFromEndpoint.AddTargetLanguage(Language.FR);
            configFromEndpoint.AddTargetLanguage(Language.ES);
            configFromEndpoint.VoiceName = Voice.FR;

            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
           
            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResultEvents = await this.translationHelper.DoTranslationAsync(recognizer, true);

                Assert.AreEqual(1, allResultEvents[ResultType.RecognizedText].Count);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResultEvents[ResultType.RecognizedText].Single();
                Assert.IsNotNull(translationTextEvent);
                var translationTextResult = translationTextEvent.Result;
                Assert.AreEqual(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, translationTextResult.Text);
                Assert.AreEqual(2, translationTextResult.Translations.Count);
                AssertMatching(TestData.French.FirstOne.Utterance, translationTextResult.Translations[Language.FR]);
                AssertOneEqual(TestData.Spanish.FirstOne.PossibleUtterances, translationTextResult.Translations[Language.ES]);

                Assert.AreNotEqual(allResultEvents[ResultType.Synthesis].Count, 0);
                var synthesisResultEvents = allResultEvents[ResultType.Synthesis].Cast<TranslationSynthesisEventArgs>().ToList();
                const int MinSize = 50000;
                foreach (var s in synthesisResultEvents)
                {
                    Console.WriteLine($"Audio.Length: {s.Result.GetAudio().Length}");
                    Assert.IsTrue(s.Result.GetAudio().Length > MinSize, $"Expects audio size {s.Result.GetAudio().Length} to be greater than {MinSize}");
                }
            }
        }

        [TestMethod]
        public async Task FromEndpointCustomSpeechModelDetailedFormatEnToFrTranslation()
        {
            var configFromEndpoint = SpeechTranslationConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.EndpointId = deploymentId;
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;
            configFromEndpoint.AddTargetLanguage(Language.FR);

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResults = await this.translationHelper.DoTranslationAsync(recognizer, true);

                Assert.AreEqual(1, allResults[ResultType.RecognizedText].Count);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResults[ResultType.RecognizedText].Single();
                Assert.IsNotNull(translationTextEvent);
                var translationTextResult = translationTextEvent.Result;
                Assert.AreEqual(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertMatching(TestData.English.Weather.Utterance, translationTextResult.Text);
                Assert.AreEqual(1, translationTextResult.Translations.Count);
                AssertMatching(TestData.French.Weather.Utterance, translationTextResult.Translations[Language.FR]);
                AssertDetailedOutput(translationTextResult, true);
            }
        }

        [TestMethod]
        public async Task FromEndpointPropertyOverwriteTranslation()
        {
            var endpointWithParameters = endpointInString + "?format=simple&from=de-DE&to=fr&features=texttospeech&voice=Microsoft%20Server%20Speech%20Text%20to%20Speech%20Voice%20(fr-CA,%20Caroline)";
            var configFromEndpoint = SpeechTranslationConfig.FromEndpoint(new Uri(endpointWithParameters), subscriptionKey);
            configFromEndpoint.SpeechRecognitionLanguage = "Invalid source language";
            configFromEndpoint.AddTargetLanguage("Invalid target language");
            configFromEndpoint.VoiceName = "Invalid voice name";
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;

            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);
            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResultEvents = await this.translationHelper.DoTranslationAsync(recognizer, true);

                Assert.AreEqual(1, allResultEvents[ResultType.RecognizedText].Count);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResultEvents[ResultType.RecognizedText].Single();
                Assert.IsNotNull(translationTextEvent);
                var translationTextResult = translationTextEvent.Result;
                Assert.AreEqual(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, translationTextResult.Text);
                Assert.AreEqual(1, translationTextResult.Translations.Count);
                AssertMatching(TestData.French.FirstOne.Utterance, translationTextResult.Translations[Language.FR]);

                Assert.AreNotEqual(allResultEvents[ResultType.Synthesis].Count, 0);
                var synthesisResultEvents = allResultEvents[ResultType.Synthesis].Cast<TranslationSynthesisEventArgs>().ToList();
                const int MinSize = 50000;
                foreach (var s in synthesisResultEvents)
                {
                    Console.WriteLine($"Audio.Length: {s.Result.GetAudio().Length}");
                    Assert.IsTrue(s.Result.GetAudio().Length > MinSize, $"Expects audio size {s.Result.GetAudio().Length} to be greater than {MinSize}");
                }

                AssertDetailedOutput(translationTextResult, false);
            }
        }

        [TestMethod]
        public async Task FromEndpointWithoutSettingFromToProperty()
        {
            var endpointWithParameters = endpointInString + "?from=de-DE&to=fr";
            var configFromEndpoint = SpeechTranslationConfig.FromEndpoint(new Uri(endpointWithParameters), subscriptionKey);
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;

            var audioInput = AudioConfig.FromWavFileInput(TestData.German.FirstOne.AudioFile);

            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResultEvents = await this.translationHelper.DoTranslationAsync(recognizer, true);

                Assert.AreEqual(1, allResultEvents[ResultType.RecognizedText].Count);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResultEvents[ResultType.RecognizedText].Single();
                Assert.IsNotNull(translationTextEvent);
                var translationTextResult = translationTextEvent.Result;
                Assert.AreEqual(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertMatching(TestData.German.FirstOne.Utterance, translationTextResult.Text);
                Assert.AreEqual(1, translationTextResult.Translations.Count);
                AssertMatching(TestData.French.FirstOne.Utterance, translationTextResult.Translations[Language.FR]);

                AssertDetailedOutput(translationTextResult, true);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyTranslation()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SetServiceProperty("from", "en-us", ServicePropertyChannel.UriQueryParameter);
            config.SetServiceProperty("format", "detailed", ServicePropertyChannel.UriQueryParameter);
            config.AddTargetLanguage(Language.FR);

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var allResults = await this.translationHelper.DoTranslationAsync(recognizer, true);

                Assert.AreEqual(1, allResults[ResultType.RecognizedText].Count);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResults[ResultType.RecognizedText].Single();
                Assert.IsNotNull(translationTextEvent);
                var translationTextResult = translationTextEvent.Result;
                Assert.AreEqual(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertMatching(TestData.English.Weather.Utterance, translationTextResult.Text);
                Assert.AreEqual(1, translationTextResult.Translations.Count);
                AssertMatching(TestData.French.Weather.Utterance, translationTextResult.Translations[Language.FR]);

                AssertDetailedOutput(translationTextResult, true);
            }
        }
    }
}

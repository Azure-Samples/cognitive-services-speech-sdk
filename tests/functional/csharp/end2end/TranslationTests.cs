//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;
    using static SpeechRecognitionTestsHelper;
    using static TranslationTestsHelper;

    [TestClass]
    public sealed class TranslationTests : RecognitionTestBase
    {
        private TranslationTestsHelper translationHelper;
        private static string synthesisDir;
        private static string deploymentId;
        private static string endpointInString;
        private static Uri endpointUrl;
        private static string hostInString;
        private static Uri hostUrl;

        public TranslationTests() : base(collectNativeLogs: true)
        {
        }

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            BaseClassInit(context);

            synthesisDir = "synthesis".GetRootRelativePath();
            deploymentId = DefaultSettingsMap[DefaultSettingKeys.DEPLOYMENT_ID];
            endpointInString = String.Format("wss://{0}.s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1", region);
            endpointUrl = new Uri(endpointInString);
            hostInString = String.Format("wss://{0}.s2s.speech.microsoft.com", region);
            hostUrl = new Uri(hostInString);
        }

        [ClassCleanup]
        new public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void TestInitalize()
        {
            this.translationHelper = new TranslationTestsHelper(subscriptionKey, region);
        }

        [TestMethod]
        public void TestLanguageProperties()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                SPXTEST_ARE_EQUAL(Language.EN, translationRecognizer.SpeechRecognitionLanguage);
                SPXTEST_ARE_EQUAL(new List<string> { Language.DE }, translationRecognizer.TargetLanguages.ToList());
                SPXTEST_ARE_EQUAL(String.Empty, translationRecognizer.VoiceName);
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                SPXTEST_ARE_EQUAL(fromLanguage, translationRecognizer.SpeechRecognitionLanguage);
                SPXTEST_ARE_EQUAL(toLanguages, translationRecognizer.TargetLanguages.ToList());
                SPXTEST_ARE_EQUAL(String.Empty, translationRecognizer.VoiceName);
            }
        }

        [TestMethod]
        public async Task TestNoFromInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await translationRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.Canceled, result.Reason);
                var errorCode = CancellationDetails.FromResult(result).ErrorCode;
                SPXTEST_ARE_EQUAL(CancellationErrorCode.BadRequest, errorCode);
            }
        }

        [TestMethod]
        public async Task TestNoToInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await translationRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                var errorDetails = CancellationDetails.FromResult(result);
                // Only translation error.
                SPXTEST_ISTRUE(errorDetails.ErrorDetails.Contains("Please verify the provided subscription details and language information"), "Actual error:'" + errorDetails.ErrorDetails + "' does not contain expected string.");
                SPXTEST_ARE_EQUAL(CancellationErrorCode.BadRequest, errorDetails.ErrorCode);
                SPXTEST_ARE_EQUAL(ResultReason.Canceled, result.Reason);
                SPXTEST_ARE_EQUAL("", result.Text);
            }
        }

        [TestMethod]
        public async Task TestDefaultOutputInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await translationRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                SPXTEST_ARE_EQUAL(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                SPXTEST_ARE_EQUAL(1, result.Translations.Count);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.DE][0].Text, result.Translations[Language.DE]);
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
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                SPXTEST_ARE_EQUAL(fromLanguage, translationRecognizer.SpeechRecognitionLanguage);
                SPXTEST_ARE_EQUAL(toLanguages, translationRecognizer.TargetLanguages.ToList());
                SPXTEST_ARE_EQUAL(voice, translationRecognizer.VoiceName);
            }
        }


        [TestMethod]
        public async Task TestInvalidTargetLanguageWithRecognizedOnce()
        {
            var toLanguages = new List<string>() { "invalidLanguages" };
            var result = await this.translationHelper.GetTranslationFinalResult(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages);

            SPXTEST_ISNOTNULL(result, AssertOutput.TranslationShouldNotBeNull);
            var errorDetails = CancellationDetails.FromResult(result).ErrorDetails;
            SPXTEST_ISFALSE(String.IsNullOrEmpty(errorDetails), "Error details cannot be empty when translation language is invalid.");
            Console.WriteLine($"Result: {result.ToString()}, Error details: ErrorDetails: {errorDetails}");
            SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason, "Unexpected result reason.");
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            SPXTEST_ARE_EQUAL(0, result.Translations.Count, "Unmatched translation results.");
            AssertMatching(TestData.ExpectedErrorDetails.InvalidTargetLanaguageErrorMessage, errorDetails);
        }

        [TestMethod]
        public async Task TestInvalidTargetLanguageWithContinuousRecognition()
        {
            var toLanguages = new List<string>() { "invalidLanguages" };
            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages, voice: null, requireTranslatedSpeech: false);

            // only look at non-empty results
            var actualTranslationsTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result).ToArray();
            actualTranslationsTextResults = actualTranslationsTextResults.Where(r => !string.IsNullOrEmpty(r.Text)).ToArray();

            var expectedUtterances = AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Where(t => !string.IsNullOrEmpty(t.Text)).ToArray();

            SPXTEST_ARE_EQUAL(expectedUtterances.Count(), actualTranslationsTextResults.Count());
            for (var i = 0; i < actualTranslationsTextResults.Count(); i++)
            {
                var result = actualTranslationsTextResults[i];
                SPXTEST_ARE_EQUAL(ResultReason.RecognizedSpeech, result.Reason, "Unmatched result reason.");

                SPXTEST_ARE_EQUAL(0, result.Translations.Count, "Unmatched translation results");
                var errorDetails = CancellationDetails.FromResult(result).ErrorDetails;
                AssertMatching(TestData.ExpectedErrorDetails.InvalidTargetLanaguageErrorMessage, errorDetails);
            }
        }

        [TestMethod]
        public async Task TestInvalidVoice()
        {
            var toLanguages = new List<string>() { Language.FR };
            using (var recognizer = TrackSessionId(this.translationHelper.CreateTranslationRecognizer(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages, "InvalidVoice")))
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

                SPXTEST_ISTRUE(result != null, "No result received.");
                SPXTEST_ARE_EQUAL(CancellationErrorCode.ServiceError, errorCode, "Unmatched error code.");
                AssertMatching(TestData.ExpectedErrorDetails.InvalidVoiceNameErrorMessage, errorDetails);
                SPXTEST_ISFALSE(receivedSynthesizingEvent, "Received unexpected synthesizing event.");
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason, "Unmatched result reason.");
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.FR][0].Text, result.Translations[toLanguages[0]]);
            }
        }

        [TestMethod]
        public async Task TranslationWeatherEnToDeFinalTextResult()
        {
            var toLanguages = new List<string>() { Language.DE, Language.ZH_CN };
            var result = await this.translationHelper.GetTranslationFinalResult(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages);

            SPXTEST_ISNOTNULL(result, AssertOutput.TranslationShouldNotBeNull);
            Console.WriteLine(result.ToString());

            var errorDetails = result.Reason == ResultReason.Canceled ? CancellationDetails.FromResult(result).ErrorDetails : "";
            Console.WriteLine($"Reason: {result.Reason}, ErrorDetails: {errorDetails}");

            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.DE][0].Text, result.Translations[Language.DE]);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.ZH_CN][0].Text, result.Translations[Language.ZH]);

        }

        [TestMethod]
        public async Task TranslationWeatherEntoFrAndEsFinalTextResult()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var result = await this.translationHelper.GetTranslationFinalResult(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages);

            SPXTEST_ISNOTNULL(result, AssertOutput.TranslationShouldNotBeNull);
            Console.WriteLine(result.ToString());

            var errorDetails = result.Reason == ResultReason.Canceled ? CancellationDetails.FromResult(result).ErrorDetails : "";
            Console.WriteLine($"Reason: {result.Reason}, ErrorDetails: {errorDetails}");

            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.FR][0].Text, result.Translations[Language.FR]);
            AssertOneEqual(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.ES].Select(x => x.Text).ToArray(), result.Translations[Language.ES]);
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task TranslationBatmanEnToDeFinalTextResultContinuous()
        {
            List<string> toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages);

            var actualRecognitionTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Text).ToArray();
            actualRecognitionTextResults = actualRecognitionTextResults.Where(t => !string.IsNullOrEmpty(t)).ToArray();
            var actualTranslationsTextResults = actualTranslations[ResultType.RecognizedText].Cast<TranslationRecognitionEventArgs>().Select(t => t.Result.Translations[Language.DE]).ToArray();
            actualTranslationsTextResults = actualTranslationsTextResults.Where(t => !string.IsNullOrEmpty(t)).ToArray();

            var expectedUtterances = AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.EN].Where(t => !string.IsNullOrEmpty(t.Text)).ToArray();
            var expectedTranslations = AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].Utterances[Language.DE].Where(t => !string.IsNullOrEmpty(t.Text)).ToArray();

            // compare only nonempty utterances, either expected or actual
            SPXTEST_ARE_EQUAL(expectedUtterances.Count(), actualTranslationsTextResults.Count());
            for (var i = 0; i < actualTranslations.Count; i++)
            {
                Assert.IsTrue(LevenshteinRatio(Normalize(expectedUtterances[i].Text), Normalize(actualRecognitionTextResults[i])) > TestData.Levenshtein.SimilarityScoreThreshold);
                Assert.IsTrue(LevenshteinRatio(Normalize(expectedTranslations[i].Text), Normalize(actualTranslationsTextResults[i])) > TestData.Levenshtein.SimilarityScoreThreshold);
            }
        }

        [TestMethod]
        public async Task TranslationFirstOneDeToFrAndEsFinalTextResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR, Language.ES };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath(), Language.DE_DE, toLanguages);
            SPXTEST_ARE_EQUAL(1, actualTranslations[ResultType.RecognizedText].Count);
            var actualTranslationRecognition = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();

            SPXTEST_ARE_NOT_EQUAL(ResultReason.Canceled, actualTranslationRecognition.Result.Reason);
            AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, actualTranslationRecognition.Result.Text);

            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.FR][0].Text, actualTranslationRecognition.Result.Translations[Language.FR]);
            AssertOneEqual(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.ES].Select(x => x.Text).ToArray(), actualTranslationRecognition.Result.Translations[Language.ES]);
        }

        [TestMethod]
        public async Task TranslationBatmanEnToDeHeddaRUSSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages, voice: Voice.DE);
            SPXTEST_ARE_NOT_EQUAL(actualTranslations[ResultType.Synthesis].Count, 0);
            var actualSynthesisByteResults = actualTranslations[ResultType.Synthesis].Cast<TranslationSynthesisEventArgs>().ToList();
            const int MinSize = 20000;
            foreach (var s in actualSynthesisByteResults)
            {
                Console.WriteLine($"Audio.Length: {s.Result.GetAudio().Length}");
                SPXTEST_ISTRUE(s.Result.GetAudio().Length > MinSize, $"Expects audio size {s.Result.GetAudio().Length} to be greater than {MinSize}");
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

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages, voice: voice);
            SPXTEST_ARE_EQUAL(1, actualTranslations[ResultType.Synthesis].Count);

            var actualSynthesisByteResult = (TranslationSynthesisEventArgs)actualTranslations[ResultType.Synthesis].Single();
            const int MinSize = 49000;
            SPXTEST_ISTRUE(actualSynthesisByteResult.Result.GetAudio().Length > MinSize,
                $"Received response for speech synthesis is less than {MinSize}: {actualSynthesisByteResult.Result.GetAudio().Length}.");
        }

        [TestMethod]
        public async Task TranslationWeatherEnToFrCarolineTextAndSynthesisResultContinuous()
        {
            var toLanguages = new List<string>() { Language.FR };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages, Voice.FR);
            SPXTEST_ARE_EQUAL(1, actualTranslations[ResultType.RecognizedText].Count);
            SPXTEST_ARE_EQUAL(1, actualTranslations[ResultType.Synthesis].Count);

            var actualTextResult = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();
            SPXTEST_ARE_EQUAL(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, actualTextResult.Result.Text);

            var actualSynthesisByteResult = (TranslationSynthesisEventArgs)actualTranslations[ResultType.Synthesis].Single();
            const int MinSize = 50000;
            SPXTEST_ISTRUE(actualSynthesisByteResult.Result.GetAudio().Length > MinSize,
                $"Received response for speech synthesis is less than {MinSize}: {actualSynthesisByteResult.Result.GetAudio().Length}.");
        }

        [TestMethod]
        public async Task TranslationWeatherEnToTrRecognizingContinuous()
        {
            var toLanguages = new List<string>() { Language.TR };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizingContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages);
            SPXTEST_ARE_NOT_EQUAL(actualTranslations[ResultType.RecognizedText].Count, 0, "Empty translation received");
            var finalResultText = actualTranslations[ResultType.RecognizedText][0].Result.Text;
            SPXTEST_ISTRUE(finalResultText.Contains("What"), $"Final result does not contain expected string \"What\", but is {finalResultText}");

            if (actualTranslations[ResultType.RecognizingText].Count > 0)
            {
                var lastIntermediateResultText = actualTranslations[ResultType.RecognizingText].Last().Result.Text;
                SPXTEST_ISTRUE(lastIntermediateResultText.Contains("What"), $"Last intermediate result does not contain expected string \"What\", but is {lastIntermediateResultText}");
            }
        }

        [TestMethod]
        public async Task TestInitialSilenceTimeout()
        {
            var toLanguages = new List<string>() { Language.DE };
            var result = await this.translationHelper.GetTranslationFinalResult(AudioUtterancesMap[AudioUtteranceKeys.SHORT_SILENCE].FilePath.GetRootRelativePath(), Language.EN, toLanguages);

            SPXTEST_ISNOTNULL(result, AssertOutput.TranslationShouldNotBeNull);
            Console.WriteLine(result.ToString());

            var errorDetails = result.Reason == ResultReason.Canceled ? CancellationDetails.FromResult(result).ErrorDetails : "";
            Console.WriteLine($"Reason: {result.Reason}, ErrorDetails: {errorDetails}");

            SPXTEST_ARE_EQUAL(ResultReason.NoMatch, result.Reason);
            SPXTEST_ISTRUE(result.OffsetInTicks > 0 || result.Duration.Ticks > 0, $"Bad offset: {result.OffsetInTicks} or duration: {result.Duration}");
            SPXTEST_ISTRUE(string.IsNullOrEmpty(result.Text), $"Bad result text: {result.Text}");

            var noMatch = NoMatchDetails.FromResult(result);
            SPXTEST_ARE_EQUAL(NoMatchReason.InitialSilenceTimeout, noMatch.Reason);
        }

        [TestMethod]
        public async Task TranslationFromCatalanToGerman()
        {
            var toLanguages = new List<string>() { Language.DE };

            var actualTranslations = await this.translationHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CATALAN].FilePath.GetRootRelativePath(), Language.CA_ES, toLanguages);
            SPXTEST_ARE_EQUAL(1, actualTranslations[ResultType.RecognizedText].Count, AssertOutput.WrongTranslatedUtterancesCount);
            var actualTranslationRecognition = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();

            SPXTEST_ARE_NOT_EQUAL(ResultReason.Canceled, actualTranslationRecognition.Result.Reason);
            SPXTEST_ISTRUE(LevenshteinRatio(Normalize(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CATALAN].Utterances[Language.CA_ES][0].Text),
                Normalize(actualTranslationRecognition.Result.Text)) > TestData.Levenshtein.SimilarityScoreThreshold);

            SPXTEST_ISTRUE(LevenshteinRatio(Normalize(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CATALAN].Utterances[Language.DE][0].Text),
                Normalize(actualTranslationRecognition.Result.Translations[Language.DE])) > TestData.Levenshtein.SimilarityScoreThreshold);
        }

        [Ignore]
        [TestMethod]
        public async Task TranslationFromCatalanToGermanInSovereignCloud()
        {
            var toLanguages = new List<string>() { Language.DE };

            var mooncakeSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Key;
            var mooncakeRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Region;

            TranslationTestsHelper trHelper = new TranslationTestsHelper(mooncakeSubscriptionKey, mooncakeRegion);
            var actualTranslations = await trHelper.GetTranslationRecognizedContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CATALAN].FilePath.GetRootRelativePath(), Language.CA_ES, toLanguages);
            SPXTEST_ARE_EQUAL(1, actualTranslations[ResultType.RecognizedText].Count, AssertOutput.WrongTranslatedUtterancesCount);
            var actualTranslationRecognition = (TranslationRecognitionEventArgs)actualTranslations[ResultType.RecognizedText].Single();

            SPXTEST_ARE_NOT_EQUAL(ResultReason.Canceled, actualTranslationRecognition.Result.Reason);
            SPXTEST_ISTRUE(LevenshteinRatio(Normalize(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CATALAN].Utterances[Language.CA_ES][0].Text),
                Normalize(actualTranslationRecognition.Result.Text)) > TestData.Levenshtein.SimilarityScoreThreshold);
            SPXTEST_ISTRUE(LevenshteinRatio(Normalize(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CATALAN].Utterances[Language.DE][0].Text),
                Normalize(actualTranslationRecognition.Result.Translations[Language.DE])) > TestData.Levenshtein.SimilarityScoreThreshold);
        }

        [TestMethod]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task AsyncRecognitionAfterDisposingTranslationRecognizer()
        {
            var toLanguages = new List<string>() { Language.DE };
            var recognizer = this.translationHelper.CreateTranslationRecognizer(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CATALAN].FilePath.GetRootRelativePath(), Language.CA_ES, toLanguages);
            recognizer.Dispose();
            await recognizer.StopContinuousRecognitionAsync();
        }

        [TestMethod]
        public void DisposingTranslationRecognizerWhileAsyncRecognition()
        {
            var toLanguages = new List<string>() { Language.DE };
            using (var recognizer = this.translationHelper.CreateTranslationRecognizer(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.CA_ES, toLanguages))
            {
                Task singleShot = null;
                SPXTEST_THROWS<InvalidOperationException>(() =>
                {
                    singleShot = recognizer.RecognizeOnceAsync();
                    Thread.Sleep(100);
                    recognizer.Dispose();
                });
                singleShot.Wait();
            }
        }

        [DataTestMethod, TestCategory(TestCategory.LongRunning)]
        [DynamicData(nameof(Voice.LangAndSynthesis), typeof(Voice), DynamicDataSourceType.Property)]
        public async Task TranslateFromENtoEachLangWithSynthesis(string translateTo, string voice)
        {
            Log("Test starting");

            TimeSpan timeout = TimeSpan.FromMinutes(1);
            string speechLang = Language.EN;

            TranslationRecognizer recognizer = null;

            try
            {
                recognizer = this.translationHelper.CreateTranslationRecognizer(
                    AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(),
                    speechLang,
                    new List<string> { translateTo },
                    voice);

                var callbacks = new TranslationRecognitionTestRun(recognizer, this)
                    .AddEventHandler(nameof(TranslationRecognizer.Canceled))
                    .AddEventHandler(nameof(TranslationRecognizer.SessionStarted))
                    .AddEventHandler(nameof(TranslationRecognizer.SessionStopped))
                    .AddEventHandler(nameof(TranslationRecognizer.Synthesizing));

                Log("Starting recognize once async");

                // Do the single recognition
                var result = await recognizer.RecognizeOnceAsync();
                LogEvent(result, "Recognize once async result");

                // wait for a session stopped event, or an error cancellation event with a timeout
                await callbacks.WaitForCompletion(timeout);

                Log("Done waiting for completion");

                // there should be no canceled events with an error, or user cancelled
                var badCanceledEvents = callbacks.Canceled
                    .Where(c => c.Reason != CancellationReason.EndOfStream)
                    .Select(c => c.ToString())
                    .ToArray();

                var badEvents = string.Join("\n", badCanceledEvents);
                SPXTEST_ISTRUE(badCanceledEvents.Length == 0, $"Cancelled events were raised that indicate an error occurred.\n{badEvents}");

                SPXTEST_ISFALSE(string.IsNullOrEmpty(result?.Text), $"locale: { speechLang }, translateTo: { translateTo }, result: { result?.ToString() ?? "<<NULL>>" }");
                SPXTEST_ISTRUE(callbacks.Synthesizing.Count > 0, "No synthesizing events were received");
                for (int i = 0; i < callbacks.Synthesizing.Count; i++)
                {
                    var synthResult = callbacks.Synthesizing[i]?.Result;
                    if (i < callbacks.Synthesizing.Count - 1)
                    {
                        SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudio, synthResult.Reason, $"Wrong result reason at synthesizing event at index {i}");
                        SPXTEST_ISTRUE(synthResult.GetAudio().Length > 0, "Synthesizing audio result at index {i} has no audio data");
                    }
                    else
                    {
                        SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, synthResult.Reason, $"Wrong result reason at synthesizing event at index {i}");
                        SPXTEST_ISTRUE(synthResult.GetAudio().Length == 0, $"Synthesizing audio completed result at index {i} should have no audio data");
                    }
                }
            }
            finally
            {
                Log("Disposing recognizer");
                if (recognizer != null)
                {
                    recognizer.Dispose();
                }

                Log("Test done");
            }
        }

        [TestMethod]
        public async Task TranslateFromENtoTongan()
        {
            var result = await this.translationHelper.GetTranslationFinalResult(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, new List<string> { "to" });
            SPXTEST_ISNOTNULL(result, "Failed to recognize and translate From English audio file to Tongal.");
            SPXTEST_ARE_EQUAL(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text, "Failed to recognize text correctly.");
            SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason, $"Unexpected result reason. Cancellation error details: { CancellationDetails.FromResult(result).ErrorDetails }");
            SPXTEST_ARE_EQUAL(1, result.Translations.Count, "Unmatched translation results.");
        }

        [TestMethod]
        public void TestSetAndGetAuthToken()
        {
            var token = "x";
            var config = SpeechTranslationConfig.FromAuthorizationToken(token, "westus");
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var recognizer = new TranslationRecognizer(config, audioInput);
            SPXTEST_ARE_EQUAL(token, recognizer.AuthorizationToken);

            var newToken = "y";
            recognizer.AuthorizationToken = newToken;
            SPXTEST_ARE_EQUAL(token, config.AuthorizationToken);
            SPXTEST_ARE_EQUAL(newToken, recognizer.AuthorizationToken);
        }

        [TestMethod]
        public void TestSetAndGetSubKey()
        {
            var config = SpeechTranslationConfig.FromSubscription("x", "westus");
            SPXTEST_ARE_EQUAL("x", config.SubscriptionKey);
        }

        [TestMethod]
        public async Task ContinuousValidCustomTranslation()
        {
            var toLanguages = new List<string>() { Language.DE };
            var actualTranslations = await this.translationHelper.GetTranslationRecognizingContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), Language.EN, toLanguages, deploymentId);
            SPXTEST_ARE_NOT_EQUAL(actualTranslations[ResultType.RecognizedText].Count, 0, "Number of translations should not be zero.");
            AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.DE][0].Text, actualTranslations[ResultType.RecognizedText].Last().Result.Translations[Language.DE]);
        }

        [TestMethod]
        public async Task ContinuousInvalidCustomTranslation()
        {
            var toLanguages = new List<string>() { Language.DE };
            var actualTranslations = await this.translationHelper.GetTranslationRecognizingContinuous(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath(), "", toLanguages, "invalidid", true);
            SPXTEST_ARE_EQUAL(0, actualTranslations[ResultType.RecognizedText].Count, "Number of translations should be zero.");
            SPXTEST_ARE_EQUAL(0, actualTranslations[ResultType.RecognizingText].Count, "Number of translations should be zero.");
            SPXTEST_ARE_EQUAL(1, actualTranslations[ResultType.Cancelled].Count, "Number of cancelled events should not be zero.");
            var errorDetails = actualTranslations[ResultType.Cancelled].Cast<TranslationRecognitionCanceledEventArgs>().Last().ErrorDetails;
            SPXTEST_ISTRUE(errorDetails.ToLowerInvariant().Contains("bad request"));
        }

        [TestMethod]
        public void SetLogFilename()
        {
            var toLanguages = new List<string>() { Language.DE };
            var config = this.translationHelper.GetConfig(Language.EN, toLanguages, "");
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            var tempPath = Path.GetTempPath();
            var logFilename = Path.Combine(tempPath, "Speech-" + Guid.NewGuid().ToString() + ".log");
            File.Create(logFilename + ".1");
            Console.WriteLine($"Log file name {logFilename}");

            config.SetProperty(PropertyId.Speech_LogFilename, logFilename);
            SPXTEST_ARE_EQUAL(logFilename, config.GetProperty(PropertyId.Speech_LogFilename));
            using (var recognizer = new TranslationRecognizer(config, audioInput))
            { recognizer.RecognizeOnceAsync().Wait(); }

            Thread.CurrentThread.Join(TimeSpan.FromSeconds(10));

            var files = Directory.EnumerateFiles(tempPath);
            try
            {
                var stream = new FileStream(logFilename, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                var sr = new StreamReader(stream);
                var contents = sr.ReadToEnd();
                SPXTEST_ISTRUE(stream.Length > 0, "log file must contain logs after recognizer created '" + contents + "'");
                Console.WriteLine(contents);
                stream.Dispose();
            }
            catch (Exception e)
            {
                Console.WriteLine($"Exception {e}");
                Console.WriteLine("Directory Contents:");
                foreach(var file in files)
                {
                    Console.WriteLine(file);
                }

            }
            
            config.SetProperty("SPEECH-LogFilename", "");
            using (var recognizer = new TranslationRecognizer(config, audioInput))
            { recognizer.RecognizeOnceAsync().Wait(); }

            File.Delete(logFilename);
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

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());

            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResultEvents = await this.translationHelper.DoTranslationAsync(recognizer, true);

                SPXTEST_ARE_EQUAL(1, allResultEvents[ResultType.RecognizedText].Count, "Bad count of translation events");
                var translationTextEvent = (TranslationRecognitionEventArgs)allResultEvents[ResultType.RecognizedText].Single();
                var translationTextResult = translationTextEvent.Result;
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, translationTextResult.Text);
                SPXTEST_ARE_EQUAL(2, translationTextResult.Translations.Count, "Bad count of translation utterances");
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.FR][0].Text, translationTextResult.Translations[Language.FR]);
                var expected = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.ES].Select(x => x.Text).ToArray();
                Console.WriteLine("Expected spanish one of:");
                foreach (string result in expected)
                {
                    Console.WriteLine($"     {result}");
                }
                Console.WriteLine($"Actually was {translationTextResult.Translations[Language.ES]} in spanish");
                AssertOneEqual(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.ES].Select(x => x.Text).ToArray(), translationTextResult.Translations[Language.ES]);

                SPXTEST_ARE_NOT_EQUAL(allResultEvents[ResultType.Synthesis].Count, 0, "There should not be zero Synthesis events");
                var synthesisResultEvents = allResultEvents[ResultType.Synthesis].Cast<TranslationSynthesisEventArgs>().ToList();
                const int MinSize = 50000;
                foreach (var s in synthesisResultEvents)
                {
                    Console.WriteLine($"Audio.Length: {s.Result.GetAudio().Length}");
                    SPXTEST_ISTRUE(s.Result.GetAudio().Length > MinSize, $"Expects audio size {s.Result.GetAudio().Length} to be greater than {MinSize}");
                }
            }
        }

        [TestMethod]
        public async Task FromEndpointCustomSpeechModelDetailedFormatEnToFrTranslation()
        {
            var configFromEndpoint = SpeechTranslationConfig.FromEndpoint(endpointUrl, subscriptionKey);
            configFromEndpoint.EndpointId = deploymentId;
            configFromEndpoint.SpeechRecognitionLanguage = Language.EN;
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;
            configFromEndpoint.AddTargetLanguage(Language.FR);

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResults = await this.translationHelper.DoTranslationAsync(recognizer, true);

                SPXTEST_ARE_EQUAL(1, allResults[ResultType.RecognizedText].Count, AssertOutput.WrongRecognizedUtterancesCount);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResults[ResultType.RecognizedText].Single();
                var translationTextResult = translationTextEvent.Result;
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, translationTextResult.Text);
                SPXTEST_ARE_EQUAL(1, translationTextResult.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.FR][0].Text, translationTextResult.Translations[Language.FR]);
                AssertDetailedOutput(translationTextResult, true);
            }
        }

        [TestMethod]
        [Ignore]
        public async Task FromEndpointPropertyOverwriteTranslation()
        {
            var endpointWithParameters = endpointInString + "?format=simple&from=de-DE&to=fr&features=texttospeech&voice=Microsoft%20Server%20Speech%20Text%20to%20Speech%20Voice%20(fr-CA,%20Caroline)";
            var configFromEndpoint = SpeechTranslationConfig.FromEndpoint(new Uri(endpointWithParameters), subscriptionKey);
            configFromEndpoint.SpeechRecognitionLanguage = "Invalid source language";
            configFromEndpoint.AddTargetLanguage("Invalid target language");
            configFromEndpoint.VoiceName = "Invalid voice name";
            configFromEndpoint.OutputFormat = OutputFormat.Detailed;

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResultEvents = await this.translationHelper.DoTranslationAsync(recognizer, true);

                SPXTEST_ARE_EQUAL(1, allResultEvents[ResultType.RecognizedText].Count, AssertOutput.WrongRecognizedUtterancesCount);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResultEvents[ResultType.RecognizedText].Single();
                var translationTextResult = translationTextEvent.Result;
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, translationTextResult.Text);
                SPXTEST_ARE_EQUAL(1, translationTextResult.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.FR][0].Text, translationTextResult.Translations[Language.FR]);

                SPXTEST_ARE_NOT_EQUAL(allResultEvents[ResultType.Synthesis].Count, 0, AssertOutput.WrongSynthesizedUtterancesCount);
                var synthesisResultEvents = allResultEvents[ResultType.Synthesis].Cast<TranslationSynthesisEventArgs>().ToList();
                const int MinSize = 50000;
                foreach (var s in synthesisResultEvents)
                {
                    Console.WriteLine($"Audio.Length: {s.Result.GetAudio().Length}");
                    SPXTEST_ISTRUE(s.Result.GetAudio().Length > MinSize, $"Expects audio size {s.Result.GetAudio().Length} to be greater than {MinSize}");
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

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].FilePath.GetRootRelativePath());

            using (var recognizer = TrackSessionId(new TranslationRecognizer(configFromEndpoint, audioInput)))
            {
                var allResultEvents = await this.translationHelper.DoTranslationAsync(recognizer, true);

                SPXTEST_ARE_EQUAL(1, allResultEvents[ResultType.RecognizedText].Count, AssertOutput.WrongRecognizedUtterancesCount);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResultEvents[ResultType.RecognizedText].Single();
                var translationTextResult = translationTextEvent.Result;
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertFuzzyMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.DE][0].Text, translationTextResult.Text);
                SPXTEST_ARE_EQUAL(1, translationTextResult.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_GERMAN].Utterances[Language.FR][0].Text, translationTextResult.Translations[Language.FR]);

                AssertDetailedOutput(translationTextResult, true);
            }
        }

        [TestMethod]
        public async Task SetServicePropertyTranslation()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SetServiceProperty("from", Language.EN, ServicePropertyChannel.UriQueryParameter);
            config.SetServiceProperty("format", "detailed", ServicePropertyChannel.UriQueryParameter);
            config.AddTargetLanguage(Language.FR);

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var allResults = await this.translationHelper.DoTranslationAsync(recognizer, true);

                SPXTEST_ARE_EQUAL(1, allResults[ResultType.RecognizedText].Count, AssertOutput.WrongRecognizedUtterancesCount);
                var translationTextEvent = (TranslationRecognitionEventArgs)allResults[ResultType.RecognizedText].Single();
                var translationTextResult = translationTextEvent.Result;
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, translationTextResult.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, translationTextResult.Text);
                SPXTEST_ARE_EQUAL(1, translationTextResult.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.FR][0].Text, translationTextResult.Translations[Language.FR]);

                AssertDetailedOutput(translationTextResult, true);
            }
        }

        [TestMethod]
        public async Task ProfanityTranslation()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].FilePath.GetRootRelativePath());
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            config.SetProfanity(ProfanityOption.Removed);
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                SPXTEST_ISFALSE(result.Text.Contains(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityRaw));
                SPXTEST_ARE_EQUAL(1, result.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                SPXTEST_ISFALSE(string.IsNullOrEmpty(result.Translations[Language.DE]));
                WarnIfNotContains(result.Translations[Language.DE], AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.DE][0].ProfanityRemoved);
            }

            config.SetProfanity(ProfanityOption.Masked);
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                AssertIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityMaskedPattern);
                WarnIfNotContains(result.Text, AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityMasked);
                SPXTEST_ARE_EQUAL(1, result.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                AssertIfNotContains(result.Translations[Language.DE], AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.DE][0].ProfanityMaskedPattern);
                WarnIfNotContains(result.Translations[Language.DE], AudioUtterancesMap[AudioUtteranceKeys.PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1].Utterances[Language.DE][0].ProfanityMasked);
            }

            config.SetProfanity(ProfanityOption.Raw);
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].Text, result.Text);
                SPXTEST_ARE_EQUAL(1, result.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                // this is the RAW output, so the RawUtteranceTranslation should be there.
                AssertIfNotContains(result.Translations[Language.DE], AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.DE][0].Text);
            }

            config.SetProfanity(ProfanityOption.Masked);
            config.SetServiceProperty("profanityMarker", "Tag", ServicePropertyChannel.UriQueryParameter);
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                AssertIfNotContains(result.Text.ToLower(), AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.EN][0].ProfanityTagged.ToLower());
                SPXTEST_ARE_EQUAL(1, result.Translations.Count, AssertOutput.WrongTranslatedUtterancesCount);
                AssertIfNotContains(result.Translations[Language.DE], AudioUtterancesMap[AudioUtteranceKeys.PROFANITY_SINGLE_UTTERANCE_ENGLISH_2].Utterances[Language.DE][0].ProfanityTagged);
            }
        }

        [TestMethod]
        public async Task ChangeLanguageOutsideTurn()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var targetLangs = config.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
            SPXTEST_ARE_EQUAL(Language.DE, targetLangs);

            config.AddTargetLanguage(Language.FR);
            targetLangs = config.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
            SPXTEST_ARE_EQUAL(Language.DE + "," + Language.FR, targetLangs);

            config.RemoveTargetLanguage(Language.DE);
            targetLangs = config.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
            SPXTEST_ARE_EQUAL(Language.FR, targetLangs);

            config.AddTargetLanguage(Language.DE);
            targetLangs = config.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
            SPXTEST_ARE_EQUAL(Language.FR + "," + Language.DE, targetLangs);

            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                recognizer.AddTargetLanguage(Language.ES);
                targetLangs = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
                SPXTEST_ARE_EQUAL(Language.FR + "," + Language.DE + "," + Language.ES, targetLangs);

                var result = await recognizer.RecognizeOnceAsync();
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                SPXTEST_ISTRUE(result.Text.Length > 0, $"result.Text.Length({result.Text.Length}) !> 0");
                SPXTEST_ISTRUE(result.Translations[Language.DE].Length > 0, "result.Translations[Language.DE].Length !> 0");
                SPXTEST_ISTRUE(result.Translations[Language.FR].Length > 0, "result.Translations[Language.FR].Length !> 0");
                SPXTEST_ISTRUE(result.Translations[Language.ES].Length > 0, "result.Translations[Language.ES].Length !> 0");

                recognizer.RemoveTargetLanguage(Language.DE);
                recognizer.RemoveTargetLanguage(Language.ES);
                targetLangs = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
                SPXTEST_ARE_EQUAL(Language.FR, targetLangs);

                recognizer.AddTargetLanguage(Language.ZH_CN);
                targetLangs = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
                SPXTEST_ARE_EQUAL(Language.FR + "," + Language.ZH_CN, targetLangs);

                result = await recognizer.RecognizeOnceAsync();
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                SPXTEST_ISTRUE(result.Text.Length > 0, "result.Text.Length !> 0");
                SPXTEST_ISTRUE(result.Translations[Language.FR].Length > 0, $"result.Translations[Language.FR].Length({result.Translations[Language.FR].Length}) !> 0");
                SPXTEST_ISTRUE(result.Translations[Language.ZH].Length > 0, $"result.Translations[Language.ZH].Length({result.Translations[Language.ZH].Length}) !> 0");
                SPXTEST_ISFALSE(result.Translations.ContainsKey(Language.ES));
                SPXTEST_ISFALSE(result.Translations.ContainsKey(Language.DE));
            }
        }

        [TestMethod]
        public async Task ChangeLanguageInsideTurn()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            var recognizedResults = new List<string>();
            var translatedResultsDe = new List<string>();
            var translatedResultsEs = new List<string>();
            var translatedResultsFr = new List<string>();
            var errorResults = new List<string>();

            var tcs = new TaskCompletionSource<bool>();

            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                recognizer.Canceled += (s, e) =>
                {
                    if (e.Reason != CancellationReason.EndOfStream)
                    {
                        errorResults.Add(string.IsNullOrEmpty(e.ErrorDetails) ? "Errors" : e.ErrorDetails);
                        Console.WriteLine($"Canceled event. ErrorCode:{e.ErrorCode}, ErrorDetails:{e.ErrorDetails}");
                        tcs.TrySetResult(false);
                    }
                    else
                    {
                        Console.WriteLine("Received EndOfStream.");
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.TranslatedSpeech)
                    {
                        recognizedResults.Add(e.Result.Text);
                        translatedResultsDe.Add(e.Result.Translations[Language.DE]);
                        if (e.Result.Translations.ContainsKey(Language.FR))
                        {
                            translatedResultsFr.Add(e.Result.Translations[Language.FR]);

                            if (!e.Result.Translations.ContainsKey(Language.ES))
                            {
                                recognizer.AddTargetLanguage(Language.ES);
                            }
                        }
                        else
                        {
                            recognizer.AddTargetLanguage(Language.FR);
                        }

                        if (e.Result.Translations.ContainsKey(Language.ES))
                        {
                            translatedResultsEs.Add(e.Result.Translations[Language.ES]);
                        }

                    }
                    else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        errorResults.Add("Translation Error.");
                        tcs.TrySetResult(false);
                    }
                    else
                    {
                        errorResults.Add("Recognized Error.");
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Received stopped session event: {e.ToString()}");
                    tcs.TrySetResult(true);
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(TimeSpan.FromSeconds(300)));
                await recognizer.StopContinuousRecognitionAsync();

                SPXTEST_ARE_EQUAL(recognizedResults.Count, translatedResultsDe.Count, $"recognized English count {recognizedResults.Count} not equal to translated German count {translatedResultsDe.Count}");
                SPXTEST_ARE_EQUAL(errorResults.Count, 0, $"errorResults count {errorResults.Count} not equal 0");
                SPXTEST_ISTRUE(recognizedResults.Count > 0, $"recognizedResults.Count({recognizedResults.Count}) not equal to zero.");
                SPXTEST_ISTRUE(translatedResultsFr.Count > 0, $"translatedResultsFr.Count({translatedResultsFr.Count}) !> zero.");
                SPXTEST_ISTRUE(translatedResultsEs.Count > 0, $"translatedResultsEs.Count({translatedResultsEs.Count}) !> zero.");
                SPXTEST_ISTRUE(translatedResultsFr.Count < translatedResultsDe.Count, $"translatedResultsFr.Count({translatedResultsFr.Count}) !< translatedResultsDe.Count({translatedResultsDe.Count})");
                SPXTEST_ISTRUE(translatedResultsEs.Count < translatedResultsDe.Count, $"translatedResultsEs.Count({translatedResultsEs.Count}) !< translatedResultsDe.Count({translatedResultsDe.Count})");
            }
        }

        [TestMethod]
        public async Task TestTranslationConfigFromHost()
        {
            var config = SpeechTranslationConfig.FromHost(new Uri(hostInString), subscriptionKey);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            using (var translationRecognizer = TrackSessionId(new TranslationRecognizer(config, audioInput)))
            {
                var result = await translationRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                SPXTEST_ARE_EQUAL(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                SPXTEST_ARE_EQUAL(1, result.Translations.Count);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.DE][0].Text, result.Translations[Language.DE]);
                // Default output format is simple.
                AssertDetailedOutput(result, false);
            }
        }
    }
}

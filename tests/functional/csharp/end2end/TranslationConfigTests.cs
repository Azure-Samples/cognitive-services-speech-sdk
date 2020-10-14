//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;
    using static SpeechRecognitionTestsHelper;
    using static TranslationTestsHelper;

    [TestClass]
    public sealed class TranslationConfigTests : RecognitionTestBase
    {
        [RetryTestMethod]
        public void TestVoiceNameInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            config.VoiceName = Voice.DE;

            SPXTEST_ARE_EQUAL(config.VoiceName, Voice.DE);
        }

        [RetryTestMethod]
        public void TestEmptyVoiceNameInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            config.VoiceName = "";

            SPXTEST_ARE_EQUAL(config.VoiceName, "");
        }

        [RetryTestMethod]
        public void TestDefaultVoiceNameInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            SPXTEST_ARE_EQUAL("", config.VoiceName);
        }

        [RetryTestMethod]
        public async Task TranslationConfigPropertiesAndCheckUrl()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            config.SetProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs, "5000");
            config.SetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, "12000");
            config.SetProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, "5");
            config.SetProperty(PropertyId.SpeechServiceResponse_OutputFormatOption, "detailed");
            config.SetProperty(PropertyId.SpeechServiceResponse_ProfanityOption, "masked");
            config.SetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging, "false");
            config.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "false");
            config.SetProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult, "true");

            // This one is for SpeechRecognizer, should not be picked up by TranslationRecognizer.
            config.SetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");

            string connectionUrl;
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.DE][0].Text, result.Translations[Language.DE]);
                // Check no word-level timestamps included, but only detailed output.
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                SPXTEST_ISFALSE(jsonResult.Contains("Words"), "Word-level timestamps not expected. Returned JSON: " + jsonResult);
                AssertDetailedOutput(result, true);
            }
            SPXTEST_ISTRUE(connectionUrl.Length > 0);

            SPXTEST_ISTRUE(connectionUrl.Contains("initialSilenceTimeoutMs=5000"), "mismatch initialSilencetimeout in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("endSilenceTimeoutMs=12000"), "mismatch endSilencetimeout in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("stableIntermediateThreshold=5"), "mismatch stableIntermediateThreshold in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("format=detailed"), "mismatch format in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("profanity=masked"), "mismatch profanity in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("storeAudio=false"), "mismatch storeAudio in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("wordLevelTimestamps=false"), "mismatch wordLevelTimestamps in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("stableTranslation=true"), "mismatch stableTranslation in " + connectionUrl);

            SPXTEST_ISFALSE(connectionUrl.Contains("postprocessing="), "unexpected postprocessing in " + connectionUrl);
        }

        [RetryTestMethod]
        public async Task TranslationConfigPropertiesDirectSetAndCheckUrl()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            config.SetProfanity(ProfanityOption.Raw);
            config.EnableAudioLogging();
            config.RequestWordLevelTimestamps();
            // It does not have effect on translation.
            config.EnableDictation();

            string connectionUrl;
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.DE][0].Text, result.Translations[Language.DE]);
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                SPXTEST_ISTRUE(jsonResult.Contains("Words"), "No word-level timestamps. Returned JSON: " + jsonResult);
                AssertDetailedOutput(result, true);
            }
            SPXTEST_ISTRUE(connectionUrl.Length > 0);

            // Word-level timestamps will set format to detailed.
            SPXTEST_ISTRUE(connectionUrl.Contains("format=detailed"), "unexpected format in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("profanity=raw"), "mismatch profanity in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("storeAudio=true"), "mismatch storeAudio in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("wordLevelTimestamps=true"), "mismatch wordLevelTimestamps in " + connectionUrl);

            SPXTEST_ISFALSE(connectionUrl.Contains("dictation"), "unexpected dictation mode in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("initialSilenceTimeoutMs="), "unexpected initialSilencetimeout in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("endSilenceTimeoutMs="), "unexpected endSilencetimeout in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("stableIntermediateThreshold="), "unexpected stableIntermediateThreshold in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("postprocessing="), "unexpected postprocessing in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("stableTranslation="), "unexpected stableTranslation in " + connectionUrl);
        }

        [RetryTestMethod]
        public async Task TranslationConfigStablePartialResultCheckInUrl()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            config.SetProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult, "true");

            // This one is for SpeechRecognizer, should not be picked up by TranslationRecognizer.
            config.SetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");

            string connectionUrl;
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ARE_EQUAL(ResultReason.TranslatedSpeech, result.Reason);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, result.Text);
                AssertMatching(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.DE][0].Text, result.Translations[Language.DE]);
            }
            SPXTEST_ISTRUE(connectionUrl.Length > 0);

            SPXTEST_ISTRUE(connectionUrl.Contains("from=" + Language.EN), "mismatch from in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("to=" + Language.DE), "mismtach to in " + connectionUrl);
            SPXTEST_ISTRUE(connectionUrl.Contains("stableTranslation=true"), "mismatch stableTranslation in " + connectionUrl);

            SPXTEST_ISFALSE(connectionUrl.Contains("initialSilenceTimeoutMs="), "unexpected initialSilencetimeout in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("endSilenceTimeoutMs="), "unexpected endSilencetimeout in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("stableIntermediateThreshold="), "unexpected stableIntermediateThreshold in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("format="), "unexpected format in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("profanity="), "unexpected profanity in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("storeAudio="), "unexpected storeAudio in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("wordLevelTimestamps="), "unexpected wordLevelTimestamps in " + connectionUrl);
            SPXTEST_ISFALSE(connectionUrl.Contains("postprocessing="), "unexpected postprocessing in " + connectionUrl);
        }
    }
}

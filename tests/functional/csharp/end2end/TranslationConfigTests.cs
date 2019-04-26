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
    public sealed class TranslationConfigTests : RecognitionTestBase
    {
        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestMethod]
        public void TestVoiceNameInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            config.VoiceName = Voice.DE;

            Assert.AreEqual(config.VoiceName, Voice.DE);
        }

        [TestMethod]
        public void TestEmptyVoiceNameInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);
            config.VoiceName = "";

            Assert.AreEqual(config.VoiceName, "");
        }

        [TestMethod]
        public void TestDefaultVoiceNameInTranslationConfig()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            Assert.AreEqual("", config.VoiceName);
        }

        [TestMethod]
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
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.AreEqual(ResultReason.TranslatedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                AssertMatching(TestData.German.Weather.Utterance, result.Translations[Language.DE]);
                // Check no word-level timestamps included, but only detailed output.
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                Assert.IsFalse(jsonResult.Contains("Words"), "Word-level timestamps not expected. Returned JSON: " + jsonResult);
                AssertDetailedOutput(result, true);
            }
            Assert.IsTrue(connectionUrl.Length > 0);

            Assert.IsTrue(connectionUrl.Contains("initialSilenceTimeoutMs=5000"), "mismatch initialSilencetimeout in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("endSilenceTimeoutMs=12000"), "mismatch endSilencetimeout in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("stableIntermediateThreshold=5"), "mismatch stableIntermediateThreshold in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("format=detailed"), "mismatch format in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("profanity=masked"), "mismatch profanity in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("storeAudio=false"), "mismatch storeAudio in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("wordLevelTimestamps=false"), "mismatch wordLevelTimestamps in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("stableTranslation=true"), "mismatch stableTranslation in " + connectionUrl);

            Assert.IsFalse(connectionUrl.Contains("postprocessing="), "unexpected postprocessing in " + connectionUrl);
        }

        [TestMethod]
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
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.AreEqual(ResultReason.TranslatedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                AssertMatching(TestData.German.Weather.Utterance, result.Translations[Language.DE]);
                var jsonResult = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                Assert.IsTrue(jsonResult.Contains("Words"), "No word-level timestamps. Returned JSON: " + jsonResult);
                AssertDetailedOutput(result, true);
            }
            Assert.IsTrue(connectionUrl.Length > 0);

            // Word-level timestamps will set format to detailed.
            Assert.IsTrue(connectionUrl.Contains("format=detailed"), "unexpected format in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("profanity=raw"), "mismatch profanity in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("storeAudio=true"), "mismatch storeAudio in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("wordLevelTimestamps=true"), "mismatch wordLevelTimestamps in " + connectionUrl);

            Assert.IsFalse(connectionUrl.Contains("dictation"), "unexpected dictation mode in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("initialSilenceTimeoutMs="), "unexpected initialSilencetimeout in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("endSilenceTimeoutMs="), "unexpected endSilencetimeout in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("stableIntermediateThreshold="), "unexpected stableIntermediateThreshold in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("postprocessing="), "unexpected postprocessing in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("stableTranslation="), "unexpected stableTranslation in " + connectionUrl);
        }

        [TestMethod]
        public async Task TranslationConfigStablePartialResultCheckInUrl()
        {
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            config.SetProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult, "true");

            // This one is for SpeechRecognizer, should not be picked up by TranslationRecognizer.
            config.SetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");

            string connectionUrl;
            using (var recognizer = TrackSessionId(new TranslationRecognizer(config, AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile))))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                connectionUrl = recognizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                Assert.AreEqual(ResultReason.TranslatedSpeech, result.Reason);
                AssertMatching(TestData.English.Weather.Utterance, result.Text);
                AssertMatching(TestData.German.Weather.Utterance, result.Translations[Language.DE]);
            }
            Assert.IsTrue(connectionUrl.Length > 0);

            Assert.IsTrue(connectionUrl.Contains("from=" + Language.EN), "mismatch from in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("to=" + Language.DE), "mismtach to in " + connectionUrl);
            Assert.IsTrue(connectionUrl.Contains("stableTranslation=true"), "mismatch stableTranslation in " + connectionUrl);

            Assert.IsFalse(connectionUrl.Contains("initialSilenceTimeoutMs="), "unexpected initialSilencetimeout in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("endSilenceTimeoutMs="), "unexpected endSilencetimeout in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("stableIntermediateThreshold="), "unexpected stableIntermediateThreshold in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("format="), "unexpected format in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("profanity="), "unexpected profanity in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("storeAudio="), "unexpected storeAudio in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("wordLevelTimestamps="), "unexpected wordLevelTimestamps in " + connectionUrl);
            Assert.IsFalse(connectionUrl.Contains("postprocessing="), "unexpected postprocessing in " + connectionUrl);
        }
    }
}

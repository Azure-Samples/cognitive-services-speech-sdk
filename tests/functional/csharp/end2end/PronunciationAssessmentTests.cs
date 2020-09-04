//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.PronunciationAssessment;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using System.IO;
    using static AssertHelpers;
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class PronunciationAssessmentTests : RecognitionTestBase
    {
        private SpeechRecognitionTestsHelper helper;

        [ClassInitialize]
        public static void TestClassInitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
        }

        [TestMethod]
        public void TestPronunciationAssessmentConfig()
        {
            var config = new PronunciationAssessmentConfig("reference", GradingSystem.HundredMark);
            Assert.IsNotNull(config);
            Assert.AreEqual("reference", config.ReferenceText);
            var jo = JObject.Parse(config.ToJson());
            Assert.AreEqual(config.ReferenceText, jo["referenceText"]);
            Assert.AreEqual("HundredMark", jo["gradingSystem"]);
            Assert.AreEqual("Phoneme", jo["granularity"]);
            Assert.AreEqual("Comprehensive", jo["dimension"]);

            config = new PronunciationAssessmentConfig("reference", GradingSystem.HundredMark, Granularity.Word, true, "id");
            config.ReferenceText = "new reference";
            jo = JObject.Parse(config.ToJson());
            Assert.AreEqual("new reference", jo["referenceText"]);
            Assert.AreEqual("HundredMark", jo["gradingSystem"]);
            Assert.AreEqual("Word", jo["granularity"]);
            Assert.AreEqual("Comprehensive", jo["dimension"]);
            Assert.IsTrue(jo["enableMiscue"].ToObject<bool>());
            Assert.AreEqual("id", jo["scenarioId"]);

            var config2 = PronunciationAssessmentConfig.FromJson(config.ToJson());
            Assert.IsNotNull(config2);
            Assert.AreEqual(config.ToJson(), config2.ToJson());

            Assert.ThrowsException<ApplicationException>(() =>
            {
                var pronunciationAssessmentConfig = new PronunciationAssessmentConfig("reference", (GradingSystem) 8);
            });

            Assert.ThrowsException<ApplicationException>(() =>
            {
                var pronunciationAssessmentConfig = new PronunciationAssessmentConfig("reference",
                        GradingSystem.HundredMark, (Granularity) 8);
            });

            Assert.ThrowsException<ApplicationException>(() =>
            {
                var pronunciationAssessmentConfig = PronunciationAssessmentConfig.FromJson("invalid json");
            });
        }

        [TestMethod]
        public void TestPronunciationAssessmentConfigApplyToRecognizer()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var config = new PronunciationAssessmentConfig("reference");
                config.ApplyTo(recognizer);
                Assert.AreEqual(config.ToJson(), recognizer.Properties.GetProperty(PropertyId.PronunciationAssessment_Params));
            }
        }

        [TestMethod]
        // [DataRow(true)] enable this when pronunciation assessment deployed to north europe
        [DataRow(false)]
        public async Task TestPronunciationAssessment(bool useReferenceText)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE].FilePath.GetRootRelativePath());
            var pronunciationAssessmentConfig = new PronunciationAssessmentConfig(
                useReferenceText ? AudioUtterancesMap[AudioUtteranceKeys.PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE].Utterances["zh-CN"][0].Text : string.Empty,
                GradingSystem.HundredMark, Granularity.Phoneme);

            var speechConfig = this.defaultConfig;
            speechConfig.SpeechRecognitionLanguage = "zh-CN";

            using (var recognizer = TrackSessionId(new SpeechRecognizer(speechConfig, audioInput)))
            {
                pronunciationAssessmentConfig.ApplyTo(recognizer);
                Assert.AreEqual(pronunciationAssessmentConfig.ToJson(),
                    recognizer.Properties.GetProperty(PropertyId.PronunciationAssessment_Params));
                var result = await helper.CompleteRecognizeOnceAsync(recognizer).ConfigureAwait(false);
                Assert.IsTrue(LevenshteinRatio(
                    AudioUtterancesMap[AudioUtteranceKeys.PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE]
                        .Utterances["zh-CN"][0].Text, result.Text) >= TestData.Levenshtein.PronunciationSimilarityScoreThreshold);
                var pronResult = PronunciationAssessmentResult.FromResult(result);

                Assert.IsTrue(pronResult.AccuracyScore > 0);
                Assert.IsTrue(pronResult.PronunciationScore > 0);
                Assert.IsTrue(pronResult.CompletenessScore > 0);
                Assert.IsTrue(pronResult.FluencyScore > 0);

                Assert.IsNotNull(pronResult.Words);
                Assert.IsTrue(pronResult.Words.First().AccuracyScore > 0);
                Assert.IsNotNull(pronResult.Words.First().Phonemes);
                Assert.IsTrue(pronResult.Words.First().Phonemes.First().AccuracyScore > 0);
            }
        }
    }
}

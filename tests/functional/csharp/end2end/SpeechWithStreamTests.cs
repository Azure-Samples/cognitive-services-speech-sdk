//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    [TestClass]
    public sealed class SpeechWithStreamTests : RecognitionTestBase
    {
        private SpeechWithStreamHelper speechHelper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestInitialize]
        public void TestInitalize()
        {
            speechHelper = new SpeechWithStreamHelper(factory);
        }

        [TestMethod]
        [Ignore("because of the test not retunning any result for long period of time occasionally")]
        public async Task SimpleRecognitionBatman()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(TestData.English.Batman.AudioFile);
            Assert.IsTrue(Config.AreResultsMatching(result.Text, TestData.English.Batman.Utterances[0]), result.ToString());
        }

        [TestMethod]
        [Ignore("because of the bug in VSO BUG 1293606")]
        public async Task DetailedRecognitionBatman()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionResult(TestData.English.Batman.AudioFile, Language.EN, SpeechOutputFormat.Detailed);
            var detailedRecognitionText = String.Join(" ", result.Best().Select(r => r.Text).ToList());
            var detailedRecognitionNormalizedForm = String.Join(" ", result.Best().Select(r => r.NormalizedForm).ToList());
            var detailedRecognitionLexicalForm = String.Join(" ", result.Best().Select(r => r.NormalizedForm).ToList());
            
            Assert.IsTrue(Config.AreResultsMatching(detailedRecognitionText, TestData.English.Batman.Utterances[0]), detailedRecognitionText);
            Assert.IsTrue(Config.AreResultsMatching(detailedRecognitionNormalizedForm, TestData.English.Batman.Utterances[0]), detailedRecognitionNormalizedForm);
            Assert.IsTrue(Config.AreResultsMatching(detailedRecognitionLexicalForm, TestData.English.Batman.Utterances[0]), detailedRecognitionLexicalForm);
        }

        [TestMethod]
        [Ignore("because of the bug detailed in VSO BUG 1293499")]
        public async Task SimpleRecognitionBatmanContinuous()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(TestData.English.Batman.AudioFile);
            Assert.AreNotEqual(result.Count, 0);

            Assert.IsTrue(Config.AreResultsMatching(result[0].Result.Text, TestData.English.Batman.Utterances[0]), result[0].Result.Text);
            Assert.IsTrue(Config.AreResultsMatching(result.Last().Result.Text, TestData.English.Batman.Utterances.Last()), result.Last().Result.Text);

            string actualText = String.Join(" ", result.Select(r => r.Result.Text).ToList());
            Assert.IsTrue(actualText.Contains("The most dangerous man on Earth"), actualText);
            Assert.IsTrue(actualText.Contains("if not the world's greatest crimes"), actualText);
            Assert.IsTrue(actualText.Contains("Batman has been repeatedly described as having genius level intellect"), actualText);
            Assert.IsTrue(actualText.Contains("is nearly unparalleled"), actualText);
            Assert.IsTrue(actualText.Contains("describes superman as"), actualText);
            Assert.IsTrue(actualText.Contains("often gathering information under"), actualText);
            Assert.IsTrue(actualText.Contains("and to break free"), actualText);
        } 

        [TestMethod]
        [Ignore("because of the bug detailed in VSO BUG 1290780 and 1293499")]
        public async Task DetailedRecognitionBatmanContinuous()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(TestData.English.Batman.AudioFile, Language.EN, SpeechOutputFormat.Detailed);
            Assert.AreNotEqual(result.Count, 0); 

            var firstUtteranceText = String.Join(" ", result[0].Result.Best().Select(r => r.Text).ToList());
            var firstUtteranceNormalizedForm = String.Join(" ", result[0].Result.Best().Select(r => r.NormalizedForm).ToList());
            var firstUtteranceLexicalForm = String.Join(" ", result[0].Result.Best().Select(r => r.NormalizedForm).ToList());

            Assert.IsTrue(Config.AreResultsMatching(firstUtteranceText, TestData.English.Batman.Utterances[0]), firstUtteranceText);
            Assert.IsTrue(Config.AreResultsMatching(firstUtteranceNormalizedForm, TestData.English.Batman.Utterances[0]), firstUtteranceNormalizedForm);
            Assert.IsTrue(Config.AreResultsMatching(firstUtteranceLexicalForm, TestData.English.Batman.Utterances[0]), firstUtteranceLexicalForm);

            var lastUtteranceText = String.Join(" ", result.Last().Result.Best().Select(r => r.Text).ToList());
            var lastUtteranceNormalizedForm = String.Join(" ", result.Last().Result.Best().Select(r => r.NormalizedForm).ToList());
            var lastUtteranceLexicalForm = String.Join(" ", result.Last().Result.Best().Select(r => r.NormalizedForm).ToList());

            Assert.IsTrue(Config.AreResultsMatching(lastUtteranceText, TestData.English.Batman.Utterances.Last()), lastUtteranceText);
            Assert.IsTrue(Config.AreResultsMatching(lastUtteranceNormalizedForm, TestData.English.Batman.Utterances.Last()), lastUtteranceNormalizedForm);
            Assert.IsTrue(Config.AreResultsMatching(lastUtteranceLexicalForm, TestData.English.Batman.Utterances.Last()), lastUtteranceLexicalForm);

            string actualText = String.Join(" ", result.Select(r => r.Result.Text).ToList());
            Assert.IsTrue(actualText.Contains("The most dangerous man on Earth"), actualText);
            Assert.IsTrue(actualText.Contains("if not the world's greatest crimes"), actualText);
            Assert.IsTrue(actualText.Contains("Batman has been repeatedly described as having genius level intellect"), actualText);
            Assert.IsTrue(actualText.Contains("is nearly unparalleled"), actualText);
            Assert.IsTrue(actualText.Contains("describes superman as"), actualText);
            Assert.IsTrue(actualText.Contains("often gathering information under"), actualText);
            Assert.IsTrue(actualText.Contains("and to break free"), actualText);
        }

        [TestMethod]
        [Ignore("because of the bug detailed in VSO BUG 1290780")]
        public async Task DetailedRecognitionBatmanContinuousBest()
        {
            var result = await this.speechHelper.GetSpeechFinalRecognitionContinuous(TestData.English.Batman.AudioFile, Language.EN, SpeechOutputFormat.Detailed);
            Assert.AreNotEqual(result.Count, 0);

            var firstUtteranceText = String.Join(" ", result[0].Result.Best().Select(r => r.Text).ToList());
            Assert.IsFalse(String.IsNullOrEmpty(firstUtteranceText), firstUtteranceText);
        }
    }
}

// <copyright file="UnitTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.IO;
    using System.Linq;

    using Connector;

    using Microsoft.Extensions.Logging;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    using Moq;

    using Newtonsoft.Json;

    [TestClass]
    public class UnitTests
    {
        private static Mock<ILogger> Logger { get; set; }

        [TestInitialize]
        public virtual void TestInitialize()
        {
            Logger = new Mock<ILogger>();
        }

        [TestMethod]
        [TestCategory(TestCategories.UnitTest)]
        public void GetSpeechObjectFromJson()
        {
            var body = File.ReadAllText(@"TestFiles/transcriptSample.json");
            var speechTranscript = JsonConvert.DeserializeObject<SpeechTranscript>(body);
            Assert.IsTrue(speechTranscript != null);
            Assert.IsTrue(speechTranscript.CombinedRecognizedPhrases.Count() == 1);
            Assert.IsTrue(speechTranscript.RecognizedPhrases.Count() == 3);
            Assert.IsTrue(speechTranscript.Duration.Equals("PT7.48S", StringComparison.Ordinal));
        }

        [TestMethod]
        [TestCategory(TestCategories.UnitTest)]
        public void GetHTMLFromJson()
        {
            var body = File.ReadAllText(@"TestFiles/transcriptSample.json");
            var transcription = JsonConvert.DeserializeObject<SpeechTranscript>(body);

            var html = TranscriptionToHtml.ToHtml(transcription, "testfile");
            Assert.IsTrue(!string.IsNullOrEmpty(html));
            Assert.IsTrue(html.StartsWith("<html lang=", StringComparison.OrdinalIgnoreCase));
        }

        [TestMethod]
        [TestCategory(TestCategories.UnitTest)]
        public void GetCostEstimationWithEmptyDuration()
        {
            var cost = CostEstimation.GetCostEstimation(TimeSpan.Zero, 1, true, Connector.Enums.SentimentAnalysisSetting.None, Connector.Enums.PiiRedactionSetting.None);
            Assert.AreEqual(cost, 0);
        }
    }
}
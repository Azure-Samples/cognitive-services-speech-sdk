// <copyright file="UnitTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using Connector;
    using Microsoft.Extensions.Logging;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;
    using Newtonsoft.Json;
    using RealtimeTranscription;

    [TestClass]
    public class UnitTests
    {
        private const string TestSasUri = "https://contoso.blob.core.windows.net/testContainer/testfolder/test.wav";

        private static Mock<ILogger> Logger { get; set; }

        [TestInitialize]
        public virtual void TestInitialize()
        {
            Logger = new Mock<ILogger>();
        }

        [TestMethod]
        [TestCategory(TestCategories.UnitTest)]
        public void ConvertRealtimeResultToBatchFormat()
        {
            var realtimeResultString = File.ReadAllText(@"testFiles/realtimeresult.json");
            var fileResult = JsonConvert.DeserializeObject<List<JsonResult>>(realtimeResultString);

            var speechTranscript = ResultConversionHelper.CreateBatchResultFromRealtimeResults("test", fileResult, Logger.Object);

            Assert.AreEqual(speechTranscript.Duration, "PT1.7S");
            Assert.IsTrue(speechTranscript.CombinedRecognizedPhrases.Any());
            Assert.IsTrue(speechTranscript.RecognizedPhrases.Any());
        }

        [TestMethod]
        [TestCategory(TestCategories.UnitTest)]
        public void GetContainerFromSasTest()
        {
            var containerName = StorageConnector.GetContainerNameFromUri(new Uri(TestSasUri));
            var fileName = StorageConnector.GetFileNameFromUri(new Uri(TestSasUri));
            var fileNameWithoutExtension = StorageConnector.GetFileNameWithoutExtension(fileName);
            Assert.AreEqual("testContainer", containerName);
            Assert.AreEqual("testfolder/test.wav", fileName);
            Assert.AreEqual("testfolder/test", fileNameWithoutExtension);
        }

        [TestMethod]
        [TestCategory(TestCategories.UnitTest)]
        public void GetSpeechObjectFromJson()
        {
            var body = File.ReadAllText(@"testFiles/transcriptSample.json");
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
            var body = File.ReadAllText(@"testFiles/transcriptSample.json");
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
// <copyright file="OverallTests.cs" company="Microsoft Corporation">
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

    [TestClass]
    public class OverallTests
    {
        private const string TestSasUri = "https://contoso.blob.core.windows.net/testContainer/testfolder/test.wav";

        private static Mock<ILogger> Logger { get; set; }

        [TestInitialize]
        public virtual void TestInitialize()
        {
            Logger = new Mock<ILogger>();
        }

        [TestMethod]
        public void GetContainerFromSasTest()
        {
            var containerName = StorageUtilities.GetContainerNameFromPath(TestSasUri);
            var fileName = StorageUtilities.GetFileNameFromPath(TestSasUri);
            var fileNameWithoutExtension = StorageUtilities.GetFileNameWithoutExtension(fileName);
            Assert.AreEqual("testContainer", containerName);
            Assert.AreEqual("testfolder/test.wav", fileName);
            Assert.AreEqual("testfolder/test", fileNameWithoutExtension);
        }

        [TestMethod]
        public void GetAudioDetailsWavTest()
        {
            var audioBytes = File.ReadAllBytes(@"testFiles/test_audio.wav");
            var audioDetails = StorageUtilities.GetAudioDetails(audioBytes, "testFiles/test_audio.wav", "en-US", DateTime.UtcNow, false, false, false, Logger.Object);

            Assert.AreEqual(1, audioDetails.Channels);
        }

        [TestMethod]
        public void GetAudioDetailsMp3Test()
        {
            var audioBytes = File.ReadAllBytes(@"testFiles/test_audio.mp3");
            var audioDetails = StorageUtilities.GetAudioDetails(audioBytes, "testFiles/test_audio.mp3", "en-US", DateTime.UtcNow, false, false, false, Logger.Object);

            Assert.AreEqual(1, audioDetails.Channels);
        }

        [TestMethod]
        public void GetAudioDetailsMissingHeaderTest()
        {
            var audioBytes = File.ReadAllBytes(@"testFiles/missing_wav_header.wav");
            var audioDetails = StorageUtilities.GetAudioDetails(audioBytes, "testFiles/missing_wav_header.wav", "en-US", DateTime.UtcNow, false, false, false, Logger.Object);

            Assert.AreEqual(0, audioDetails.Channels);
            Assert.AreEqual(TimeSpan.Zero, audioDetails.AudioLength);
        }

        [TestMethod]
        public void GetAudioDetailsInvalidFileTest()
        {
            var audioBytes = File.ReadAllBytes(@"testFiles/transcriptSample.json");
            var audioDetails = StorageUtilities.GetAudioDetails(audioBytes, "testFiles/transcriptSample.json", "en-US", DateTime.UtcNow, false, false, false, Logger.Object);

            Assert.AreEqual(0, audioDetails.Channels);
            Assert.AreEqual(TimeSpan.Zero, audioDetails.AudioLength);
        }

        [TestMethod]
        public void AudioFileNameTest()
        {
            Assert.IsTrue(StorageUtilities.IsItAudio("test.wav"));
            Assert.IsFalse(StorageUtilities.IsItAudio("test.jpg"));
        }

        [TestMethod]
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
        public void GetHTMLFromJson()
        {
            var body = File.ReadAllText(@"testFiles/transcriptSample.json");
            var transcription = JsonConvert.DeserializeObject<SpeechTranscript>(body);

            var html = TranscriptionToHtml.ToHTML(transcription, "testfile");
            Assert.IsTrue(!string.IsNullOrEmpty(html));
            Assert.IsTrue(html.StartsWith("<html lang=", StringComparison.OrdinalIgnoreCase));
        }
    }
}
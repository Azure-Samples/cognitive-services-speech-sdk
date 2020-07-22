// <copyright file="OverallTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Threading.Tasks;
    using Connector;
    using FetchTranscriptionFunction;
    using Microsoft.Extensions.Logging;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;
    using Newtonsoft.Json;

    [TestClass]
    public class OverallTests
    {
        private const string TestSasUri = "https://contoso.blob.core.windows.net/testContainer/test.wav";

        private const string SubscriptionKey = "TestSubscriptionKey";
        private const string SubscriptionRegion = "testregion";
        private const string TranscriptionLocation = "TestLocation";
        private const string TestAudioFileName = "test.wav";
        private const string TestAudioFileUrl = "https://contoso.com";

        private static Mock<ILogger> Logger { get; set; }

        [TestInitialize]
        public virtual void TestInitialize()
        {
            Logger = new Mock<ILogger>();
        }

        [TestMethod]
        public void GetContainerFromSasTest()
        {
            var containerPath = StorageUtilities.GetContainerPathFromSAS(TestSasUri, Logger.Object);
            Assert.AreEqual("testContainer", containerPath);
        }

        [TestMethod]
        public void GetAudioDetailsWavTest()
        {
            var wavAudioBytes = File.ReadAllBytes(@"testFiles/test_audio.wav");
            (TimeSpan duration, int channels, double estimatedCost) = StorageUtilities.GetAudioDetailsFromBytes(wavAudioBytes, ".wav", Logger.Object);

            Assert.AreEqual(1, channels);
            Assert.AreEqual(0.0002d, Math.Round(estimatedCost, 4));
        }

        [TestMethod]
        public void GetAudioDetailsMp3Test()
        {
            var mp3AudioBytes = File.ReadAllBytes(@"testFiles/test_audio.mp3");
            (TimeSpan duration, int channels, double estimatedCost) = StorageUtilities.GetAudioDetailsFromBytes(mp3AudioBytes, ".mp3", Logger.Object);

            Assert.AreEqual(1, channels);
            Assert.AreEqual(0.0002d, Math.Round(estimatedCost, 4));
        }

        [TestMethod]
        public void GetAudioDetailsMissingHeaderTest()
        {
            var audioBytes = File.ReadAllBytes(@"testFiles/missing_wav_header.wav");
            (TimeSpan duration, int channels, double estimatedCost) = StorageUtilities.GetAudioDetailsFromBytes(audioBytes, ".wav", Logger.Object);

            Assert.AreEqual(TimeSpan.Zero, duration);
            Assert.AreEqual(0, channels);
            Assert.AreEqual(0d, estimatedCost);
        }

        [TestMethod]
        public void GetAudioDetailsInvalidFileTest()
        {
            var audioBytes = File.ReadAllBytes(@"testFiles/transcriptSample.json");
            (TimeSpan duration, int channels, double estimatedCost) = StorageUtilities.GetAudioDetailsFromBytes(audioBytes, ".txt", Logger.Object);

            Assert.AreEqual(TimeSpan.Zero, duration);
            Assert.AreEqual(0, channels);
            Assert.AreEqual(0d, estimatedCost);
        }

        [TestMethod]
        public void ParseServiceBusMessageTest()
        {
            var subscription = new Subscription(SubscriptionKey, SubscriptionRegion);
            var createTranscriptionMessage = new TranscriptionServiceBusMessage(
                subscription,
                TranscriptionLocation,
                TestAudioFileName,
                TestAudioFileUrl,
                DateTime.UtcNow.ToString(CultureInfo.InvariantCulture),
                TimeSpan.Zero.ToString(),
                1,
                0.5d,
                "en-US",
                0,
                3);
            var messageAsString = createTranscriptionMessage.CreateMessageString();
            var fetchTranscriptionMessage = TranscriptionServiceBusMessage.ParseMessageFromString(messageAsString);

            Assert.AreEqual(createTranscriptionMessage.Subscription.SubscriptionKey, fetchTranscriptionMessage.Subscription.SubscriptionKey);
            Assert.AreEqual(createTranscriptionMessage.Subscription.LocationUri, fetchTranscriptionMessage.Subscription.LocationUri);
            Assert.AreEqual(createTranscriptionMessage.TranscriptionLocation, fetchTranscriptionMessage.TranscriptionLocation);
            Assert.AreEqual(createTranscriptionMessage.BlobName, fetchTranscriptionMessage.BlobName);
            Assert.AreEqual(createTranscriptionMessage.CreatedTime, fetchTranscriptionMessage.CreatedTime);
            Assert.AreEqual(createTranscriptionMessage.AudioLength, fetchTranscriptionMessage.AudioLength);
            Assert.AreEqual(createTranscriptionMessage.Channels, fetchTranscriptionMessage.Channels);
            Assert.AreEqual(createTranscriptionMessage.EstimatedCost, fetchTranscriptionMessage.EstimatedCost);
            Assert.AreEqual(createTranscriptionMessage.Locale, fetchTranscriptionMessage.Locale);
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
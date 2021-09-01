// <copyright file="EndToEndTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Threading.Tasks;
    using Microsoft.CognitiveServices.Speech;
    using Microsoft.Extensions.Logging;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;
    using RealtimeTranscription;

    [TestClass]
    public class EndToEndTests
    {
        private static IDictionary<string, object> TestProperties;

        private static Mock<ILogger> Logger { get; set; }

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            context = context ?? throw new ArgumentNullException(nameof(context));
            TestProperties = context.Properties;
            Logger = new Mock<ILogger>();
        }

        [TestMethod]
        [TestCategory(TestCategories.EndToEndTest)]
        public async Task TestMultiChannelFromSasTestAsync()
        {
            var region = TestProperties["SpeechServicesRegion"].ToString();
            var subscriptionKey = TestProperties["SpeechServicesSubscriptionKey"].ToString();
            var conf = SpeechConfig.FromEndpoint(
                new Uri($"wss://{region}.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1?setfeature=multichannel2&initialSilenceTimeoutMs=600000&endSilenceTimeoutMs=600000"),
                subscriptionKey);
            conf.OutputFormat = OutputFormat.Detailed;

            var stereoFile = File.ReadAllBytes(@"testFiles/test_audio_stereo.wav");
            var jsonResults = await RealtimeTranscriptionHelper.TranscribeAsync(stereoFile, conf, Logger.Object).ConfigureAwait(false);

            Assert.IsTrue(jsonResults.Any());
            Assert.IsTrue(!string.IsNullOrEmpty(jsonResults.First().SpeakerId));
            Assert.IsTrue(jsonResults.First().NBest.Any());

            var firstNBest = jsonResults.First().NBest.First();
            Assert.AreEqual(firstNBest.Lexical, "hello");
        }
    }
}

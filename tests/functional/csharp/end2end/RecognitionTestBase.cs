//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    [TestClass]
    public class RecognitionTestBase
    {
        public static string inputDir, subscriptionKey, region, conversationTranscriptionEndpoint, conversationTranscriptionPPEKey, conversationTranscriptionPRODKey, speechRegionForConversationTranscription;
        public SpeechConfig defaultConfig;
        private static Config _config;

        public static void BaseClassInit(TestContext context)
        {
            _config = new Config(context);

            subscriptionKey = Config.UnifiedSpeechSubscriptionKey;
            region = Config.Region;
            inputDir = Config.InputDir;
            conversationTranscriptionEndpoint = Config.ConversationTranscriptionEndpoint;
            conversationTranscriptionPPEKey = Config.ConversationTranscriptionPPEKey;
            conversationTranscriptionPRODKey = Config.ConversationTranscriptionPRODKey;
            speechRegionForConversationTranscription = Config.SpeechRegionForConversationTranscription;

            TestData.AudioDir = Path.Combine(inputDir, "audio");
            TestData.KwsDir = Path.Combine(inputDir, "kws");

            Console.WriteLine("region: " + region);
            Console.WriteLine("input directory: " + inputDir);
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            defaultConfig = SpeechConfig.FromSubscription(subscriptionKey, region);
        }
    }
}

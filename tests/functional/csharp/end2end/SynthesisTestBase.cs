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
    public class SynthesisTestBase
    {
        public const long EmptyWaveFileSize = 46;
        public const int GuidLength = 32;
        public const string DefaultLanguage = "en-US";
        public const string DefaultVoice = "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)";
        public const int MockAudioSize = 32000;
        public const int MockAudioChunkSize = 3200;
        public const string SsmlTemplate = "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='{0}'><voice name='{1}'>{2}</voice></speak>";
        public const byte BinaryCompareTolerance = 0;

        public static string subscriptionKey, region;
        public SpeechConfig config;
        public SpeechConfig mockConfig;

        public static void BaseClassInit(TestContext context)
        {
            subscriptionKey = Config.GetSettingByKey<String>(context, "UnifiedSpeechSubscriptionKey");
            region = Config.GetSettingByKey<String>(context, "Region");

            Console.WriteLine("region: " + region);
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            config = SpeechConfig.FromSubscription(subscriptionKey, region);
            mockConfig = SpeechConfig.FromSubscription("None", "None");
            mockConfig.SetProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");
        }
    }
}

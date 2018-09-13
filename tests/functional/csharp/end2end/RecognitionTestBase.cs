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
        public static string inputDir, subscriptionKey, region;
        public SpeechConfig config;

        public static void BaseClassInit(TestContext context)
        {
            subscriptionKey = Config.GetSettingByKey<String>(context, "UnifiedSpeechSubscriptionKey");
            region = Config.GetSettingByKey<String>(context, "Region");
            inputDir = Config.GetSettingByKey<String>(context, "InputDir");

            TestData.AudioDir = Path.Combine(inputDir, "audio");

            Console.WriteLine("region: " + region);
            Console.WriteLine("input directory: " + inputDir);
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            config = SpeechConfig.FromSubscription(subscriptionKey, region);
        }
    }
}

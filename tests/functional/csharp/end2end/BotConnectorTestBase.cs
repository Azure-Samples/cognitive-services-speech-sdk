//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.CognitiveServices.Speech.Dialog;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    [TestClass]
    public class BotConnectorTestBase
    {
        public static string inputDir, subscriptionKey, region, botConnectionId;
        public BotConnectorConfig botConnectorConfig;

        [ClassInitialize]
        public static void BaseClassInitialize(TestContext context)
        {
            // Ignore for now, using AutoReply
            botConnectionId = Config.GetSettingByKey<String>(context, "BotConnectionId");
            subscriptionKey = Config.GetSettingByKey<String>(context, "BotSubscriptionKey");
            region = Config.GetSettingByKey<String>(context, "BotRegion");
            inputDir = Config.GetSettingByKey<String>(context, "InputDir");

            TestData.AudioDir = Path.Combine(inputDir, "audio");

            Console.WriteLine("region: " + region);
            Console.WriteLine("input directory: " + inputDir);
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            botConnectorConfig = BotConnectorConfig.FromSecretKey(botConnectionId, subscriptionKey, region);
            botConnectorConfig.SpeechRecognitionLanguage = "en-us";
            botConnectorConfig.SetProperty("Conversation_Communication_Type", "AutoReply");
        }
    }
}

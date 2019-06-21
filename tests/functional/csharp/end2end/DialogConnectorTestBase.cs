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
    public class DialogConnectorTestBase
    {
        public static string inputDir, subscriptionKey, region, botSecret;
        public DialogConfig dialogConfig;

        [ClassInitialize]
        public static void BaseClassInitialize(TestContext context)
        {
            // Ignore for now, using AutoReply
            botSecret = Config.GetSettingByKey<String>(context, "DialogFunctionalTestBot");
            subscriptionKey = Config.GetSettingByKey<String>(context, "DialogSubscriptionKey");
            region = Config.GetSettingByKey<String>(context, "DialogRegion");
            inputDir = Config.GetSettingByKey<String>(context, "InputDir");

            TestData.AudioDir = Path.Combine(inputDir, "audio");

            Console.WriteLine("region: " + region);
            Console.WriteLine("input directory: " + inputDir);
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            dialogConfig = DialogConfig.FromBotSecret(botSecret, subscriptionKey, region);
            dialogConfig.SpeechRecognitionLanguage = "en-us";
            dialogConfig.SetProperty("Conversation_Communication_Type", "AutoReply");
        }
    }
}

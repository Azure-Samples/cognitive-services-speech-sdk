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
    public class DialogServiceConnectorTestBase
    {
        public static string inputDir, subscriptionKey, region, botSecret;
        public DialogServiceConfig dialogServiceConfig;

        [ClassInitialize]
        public static void BaseClassInitialize(TestContext context)
        {
            // Ignore for now, using AutoReply
            botSecret = Config.DialogFunctionalTestBot;
            subscriptionKey = Config.DialogSubscriptionKey;
            region = Config.DialogRegion;
            inputDir = Config.InputDir;

            TestData.AudioDir = Path.Combine(inputDir, "audio");

            Console.WriteLine("region: " + region);
            Console.WriteLine("input directory: " + inputDir);
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            dialogServiceConfig = BotFrameworkConfig.FromSubscription(subscriptionKey, region);
            dialogServiceConfig.SetProperty(PropertyId.Conversation_ApplicationId, botSecret);
            dialogServiceConfig.Language = "en-us";
            dialogServiceConfig.SetProperty("Conversation_Communication_Type", "AutoReply");
        }
    }
}

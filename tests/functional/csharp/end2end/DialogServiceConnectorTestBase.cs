//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Dialog;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static Config;
    using static SPXTEST;

    [TestClass]
    public class DialogServiceConnectorTestBase : LoggingTestBase
    {
        public static string inputDir;
        public static string subscriptionKey;
        public static string region;
        public static string botSecret;
        private static Config config;

        public DialogServiceConfig dialogServiceConfig;

        [ClassInitialize]
        public static void BaseClassInitialize(TestContext context)
        {
            LoggingTestBaseInit(context);

            // Ignore for now, using AutoReply
            config = new Config(context);
            botSecret = DefaultSettingsMap[DefaultSettingKeys.DIALOG_FUNCTIONAL_TEST_BOT];
            subscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.DIALOG_SUBSCRIPTION].Key;
            region = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.DIALOG_SUBSCRIPTION].Region;
            inputDir = DefaultSettingsMap[DefaultSettingKeys.INPUT_DIR];

            SPXTEST_ISTRUE(!string.IsNullOrEmpty(botSecret) && !string.IsNullOrEmpty(subscriptionKey) && !string.IsNullOrEmpty(region),
                String.Format("Subscription, region, and bot app id are all expected to be non-empty. Actual lengths are "
                    + "{0}, {1}, and {2}. Is the json file with subscription information updated?",
                        subscriptionKey.Length,
                        region.Length,
                        botSecret.Length));
                        
            Console.WriteLine("region: " + region);
            Console.WriteLine("input directory: " + inputDir);
        }

        [ClassCleanup]
        public static void BaseClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            dialogServiceConfig = BotFrameworkConfig.FromSubscription(subscriptionKey, region);
            dialogServiceConfig.SetProperty(PropertyId.Conversation_ApplicationId, botSecret);
            dialogServiceConfig.Language = Language.EN;
            dialogServiceConfig.SetProperty("Conversation_Communication_Type", "AutoReply");
        }
    }
}

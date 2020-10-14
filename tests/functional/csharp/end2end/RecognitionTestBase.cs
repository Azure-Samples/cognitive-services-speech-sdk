//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;

    [TestClass]
    public class RecognitionTestBase : LoggingTestBase
    {
        public static string subscriptionKey;
        public static string region;
        public static string conversationTranscriptionEndpoint;
        public static string conversationTranscriptionPPEKey;
        public static string conversationTranscriptionPPERegion;
        public static string conversationTranscriptionPRODKey;
        public static string conversationTranscriptionPRODRegion;

        public SpeechConfig defaultConfig;
        public SpeechConfig hostConfig;
        public SpeechConfig offlineConfig;

        private static Config _config;
        
        [ClassInitialize(InheritanceBehavior.BeforeEachDerivedClass)]
        public static void BaseClassInit(TestContext context)
        {
            _config = new Config(context);

            subscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION].Key;
            region = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION].Region;
            conversationTranscriptionEndpoint = DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSCRIPTION_ENDPOINT];
            conversationTranscriptionPPEKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION].Key;
            conversationTranscriptionPPERegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION].Region;
            conversationTranscriptionPRODKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSCRIPTION_PROD_SUBSCRIPTION].Key;
            conversationTranscriptionPRODRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSCRIPTION_PROD_SUBSCRIPTION].Region;

            SPXTEST_ISTRUE(DefaultSettingsMap.Values.Any(value => !String.IsNullOrEmpty(value)),
                "All default settings are empty. Has the settings .json been updated?");
            SPXTEST_ISTRUE(SubscriptionsRegionsMap.Values.Any(value => !String.IsNullOrEmpty(value.Key) && !String.IsNullOrEmpty(value.Region)),
                "All configured subscriptions are empty. Has the settings .json been updated?");

            Console.WriteLine("region: " + region);
            Console.WriteLine("input directory: " + DefaultSettingsMap[DefaultSettingKeys.INPUT_DIR]);
        }

        [TestInitialize]
        public void BaseTestInit()
        {
            defaultConfig = SpeechConfig.FromSubscription(subscriptionKey, region);

            var host = $"wss://{region}.stt.speech.microsoft.com";
            hostConfig = SpeechConfig.FromHost(new Uri(host), subscriptionKey);

            offlineConfig = SpeechConfig.FromSubscription(subscriptionKey, region);
            offlineConfig.SetProperty("CARBON-INTERNAL-UseRecoEngine-Unidec", "true");
            offlineConfig.SetProperty("CARBON-INTERNAL-SPEECH-RecoLocalModelPathRoot", TestData.OfflineUnidec.LocalModelPathRoot);
            offlineConfig.SetProperty("CARBON-INTERNAL-SPEECH-RecoLocalModelLanguage", TestData.OfflineUnidec.LocalModelLanguage);
        }
    }
}

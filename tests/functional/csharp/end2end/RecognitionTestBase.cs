//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Runtime.CompilerServices;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static Config;

    [TestClass]
    public class RecognitionTestBase
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

        public TestContext TestContext { get; set; }

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
            // Uncomment below to enable logs
            //offlineConfig.SetProperty(PropertyId.Speech_LogFilename, "logfile-" + DateTime.Now.ToString("HH-mm-ss") + ".txt");
        }

        protected void WriteLine(string msg, [CallerMemberName] string caller = null, [CallerLineNumber] int line = 0, [CallerFilePath] string file = null)
        {
            string logMessage = $"({DateTime.UtcNow.ToString("yyyy-MM-dd HH::mm::ss.ff")}) [{Path.GetFileName(file)}:{caller}:{line}] {msg}";
            TestContext.WriteLine(logMessage);
        }

        protected void DumpLine(string msg)
        {
            TestContext.WriteLine(msg);
        }
    }
}

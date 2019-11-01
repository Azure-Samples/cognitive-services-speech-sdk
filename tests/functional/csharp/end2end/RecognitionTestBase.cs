//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Runtime.CompilerServices;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    [TestClass]
    public class RecognitionTestBase
    {
        public static string inputDir, subscriptionKey, region, conversationTranscriptionEndpoint, conversationTranscriptionPPEKey, conversationTranscriptionPRODKey, speechRegionForConversationTranscription;
        public SpeechConfig defaultConfig, hostConfig, offlineConfig;
        private static Config _config;

        public TestContext TestContext { get; set; }

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

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Audio;

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static AssertHelpers;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class SpeechRecognizerDynamicGrammarTests
    {
        private static string luisRegion, luisKey;
        private static string inputDir;

        private static bool useTrumanUntilSkymanReady = true;
        private static string unifiedRegion, unifiedKey;

        private SpeechConfig config;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            luisRegion = Config.GetSettingByKey<String>(context, "LanguageUnderstandingServiceRegion");
            luisKey = Config.GetSettingByKey<String>(context, "LanguageUnderstandingSubscriptionKey");

            inputDir = Config.GetSettingByKey<String>(context, "InputDir");
            TestData.AudioDir = Path.Combine(inputDir, "audio");

            unifiedRegion = Config.GetSettingByKey<String>(context, "Region");
            unifiedKey = Config.GetSettingByKey<String>(context, "UnifiedSpeechSubscriptionKey");

            Console.WriteLine(useTrumanUntilSkymanReady ? ("luisRegion: " + luisRegion) : ("unifiedRegion: " + unifiedRegion));
            Console.WriteLine("input directory: " + inputDir);
        }

        [TestInitialize]
        public void Initialize()
        {
            var overrideEndpoint = $"wss://speech.platform.bing.com/speech/uswest/recognition/interactive/cognitiveservices/v1?format=detailed&language=en-us";
            config = useTrumanUntilSkymanReady
                ? SpeechConfig.FromEndpoint(new Uri(overrideEndpoint), luisKey)
                : SpeechConfig.FromSubscription(unifiedKey, unifiedRegion);
        }

        [DataTestMethod]
        [DataRow(false, 0, 0, 0, 0, 0)]
        [DataRow(true, 1, 0, 0, 0, 0)]
        [DataRow(true, 1, 2, 0, 0, 0)]
        [DataRow(true, 0, 0, 1, 2, 0)]
        [DataRow(true, 0, 0, 1, 0, 1)]
        [DataRow(true, 2, 0, 1, 0, 1)]
        [DataRow(true, 2, 3, 1, 0, 1)]
        [DataRow(true, 1, 0, 2, 0, 1)]
        [DataRow(true, 1, 3, 2, 0, 1)]
        [DataRow(true, 1, 2, 3, 0, 1)]
        public async Task TestPhraseListGrammar(bool usePhraseList, int addUnrelated1At, int addUnrelated2At, int addCorrectTextAt, int clearPhraseListAt, int expected)
        {
            var defaultRecoText = TestData.English.DgiWreckANiceBeach.DefaultRecoText;
            var correctRecoText = TestData.English.DgiWreckANiceBeach.CorrectRecoText;
            var recoTextExpected = (expected == 0) ? defaultRecoText : correctRecoText;

            var unrelatedPhrase1 = "This is a test of the emergency broadcast system.";
            var unrelatedPhrase2 = "This is not the right transcript.";

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.DgiWreckANiceBeach.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                PhraseListGrammar phraselist = usePhraseList
                    ? PhraseListGrammar.FromRecognizer(recognizer)
                    : null;

                for (int i = 1; i <= 3; i++)
                {
                    if (i == addUnrelated1At) phraselist.AddPhrase(unrelatedPhrase1);
                    if (i == addUnrelated2At) phraselist.AddPhrase(unrelatedPhrase2);
                    if (i == addCorrectTextAt) phraselist.AddPhrase(correctRecoText);
                    if (i == clearPhraseListAt) phraselist.Clear();
                }

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                AssertMatching(recoTextExpected, result.Text);
            }
        }
    }
}

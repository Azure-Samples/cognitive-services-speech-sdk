//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class SpeechRecognizerDynamicGrammarTests : LoggingTestBase
    {
        private static string unifiedRegion;
        private static string unifiedKey;
        private static string officeEndpoint;

        private SpeechConfig config;
        private static Config _config;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            _config = new Config(context);

            unifiedRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION].Region;
            unifiedKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION].Key;
            officeEndpoint = DefaultSettingsMap[DefaultSettingKeys.OFFICE_PRODUCTION_ENDPOINT];

            var officeFlight = DefaultSettingsMap[DefaultSettingKeys.OFFICE_FLIGHT];
            if (!string.IsNullOrEmpty(officeFlight))
            {
                officeEndpoint += (officeEndpoint.Contains("?") ? "&" : "?") + "setflight=" + officeFlight;
            }

            Console.WriteLine("unifiedRegion: " + unifiedRegion);
            Console.WriteLine("input directory: " + DefaultSettingsMap[DefaultSettingKeys.INPUT_DIR]);
        }

        [ClassCleanup]
        public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void Initialize()
        {
            config = SpeechConfig.FromSubscription(unifiedKey, unifiedRegion);
        }

        [RetryTestMethod]
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
            var defaultRecoText = TestData.DgiWreckANiceBeach.DefaultRecoText;
            var correctRecoText = AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].Utterances[Language.EN][0].Text;
            var recoTextExpected = (expected == 0) ? defaultRecoText : correctRecoText;

            var unrelatedPhrase1 = "This is a test of the emergency broadcast system.";
            var unrelatedPhrase2 = "This is not the right transcript.";

            bool triedOnce = false;

            while (true)
            {
                var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].FilePath.GetRootRelativePath());
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
                    try
                    {
                        AssertMatching(recoTextExpected, result.Text);
                        break;
                    }
                    catch (Exception)
                    {
                        // We'll allow a 2nd attempt to work around failures caused by service bug
                        // https://msasg.visualstudio.com/Skyman/_workitems/edit/1893773
                        if (recoTextExpected != correctRecoText || triedOnce)
                        {
                            throw;
                        }

                        Console.WriteLine("Re-trying phrase list matching.");
                        triedOnce = true;
                    }
                }
            }
        }

        [RetryTestMethod]
        [Ignore]
        public async Task TestSetRecognitionFactor()
        {
            var defaultRecoText = TestData.DgiWreckANiceBeach.DefaultRecoText;
            var correctRecoText = AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].Utterances[Language.EN][0].Text;
            var speechConfig = SpeechConfig.FromEndpoint(new Uri(officeEndpoint));
            speechConfig.AuthorizationToken = "token";

            speechConfig.SpeechRecognitionLanguage = Language.EN;

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(speechConfig, audioInput)))
            {
                PhraseListGrammar phraselist = PhraseListGrammar.FromRecognizer(recognizer);
                phraselist.AddPhrase("wreck a large nice beach");

                var gl = GrammarList.FromRecognizer(recognizer);
                gl.SetRecognitionFactor(700, RecognitionFactorScope.PartialPhrase);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                AssertMatching(defaultRecoText, result.Text);
            }
        }

        [RetryTestMethod]
        public void TestSetRecognitionFactorError()
        {
            var defaultRecoText = TestData.DgiWreckANiceBeach.DefaultRecoText;
            var correctRecoText = AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].Utterances[Language.EN][0].Text;

            config.SpeechRecognitionLanguage = Language.EN;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                PhraseListGrammar phraselist = PhraseListGrammar.FromRecognizer(recognizer);
                phraselist.AddPhrase(correctRecoText);

                var gl = GrammarList.FromRecognizer(recognizer);
                SPXTEST_THROWS<ApplicationException>(() => gl.SetRecognitionFactor(-1, RecognitionFactorScope.PartialPhrase));
            }
        }

        [RetryTestMethod]
        public async Task TestSetRecognitionFactorStringErrorRange()
        {
            var defaultRecoText = TestData.DgiWreckANiceBeach.DefaultRecoText;
            var correctRecoText = AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].Utterances[Language.EN][0].Text;

            config.SpeechRecognitionLanguage = Language.EN;
            config.SetProperty("SPEECH-WordLevelRecognitionFactor", "-1");

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                PhraseListGrammar phraselist = PhraseListGrammar.FromRecognizer(recognizer);
                phraselist.AddPhrase(correctRecoText);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ISTRUE(result.Reason == ResultReason.Canceled);

                var canceled = CancellationDetails.FromResult(result);
                SPXTEST_ISTRUE(canceled.Reason == CancellationReason.Error);
                SPXTEST_ISTRUE(canceled.ErrorCode == CancellationErrorCode.RuntimeError);
            }
        }

        [RetryTestMethod]
        public async Task TestSetRecognitionFactorStringError()
        {
            var defaultRecoText = TestData.DgiWreckANiceBeach.DefaultRecoText;
            var correctRecoText = AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].Utterances[Language.EN][0].Text;

            config.SpeechRecognitionLanguage = Language.EN;
            config.SetProperty("SPEECH-WordLevelRecognitionFactor", "NotANumber");

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.AMBIGUOUS_SPEECH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                PhraseListGrammar phraselist = PhraseListGrammar.FromRecognizer(recognizer);
                phraselist.AddPhrase(correctRecoText);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                SPXTEST_ISTRUE(result.Reason == ResultReason.Canceled);

                var canceled = CancellationDetails.FromResult(result);
                SPXTEST_ISTRUE(canceled.Reason == CancellationReason.Error);
                SPXTEST_ISTRUE(canceled.ErrorCode == CancellationErrorCode.RuntimeError);
            }
        }

        [RetryTestMethod]
        public async Task TestPhraseListGrammarInChinese()
        {
            var recoTextExpected = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CHINESE].Utterances[Language.ZH_CN][0].Text;

            var unrelatedPhrase1 = "你好"; // "Hello"
            var unrelatedPhrase2 = "键盘"; // "Keyboard"

            config.SpeechRecognitionLanguage = Language.ZH_CN;
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CHINESE].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                PhraseListGrammar phraselist = PhraseListGrammar.FromRecognizer(recognizer);

                phraselist.AddPhrase(unrelatedPhrase1);
                phraselist.AddPhrase(unrelatedPhrase2);
                phraselist.AddPhrase(recoTextExpected);

                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                AssertMatching(recoTextExpected, result.Text);
            }
        }
    }
}

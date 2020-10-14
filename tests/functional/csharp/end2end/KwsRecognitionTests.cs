//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class KwsRecognitionTests : RecognitionTestBase
    {
        private static int kwsFoundTimeoutDelay = 30000;

        private static string deploymentId;
        private SpeechRecognitionTestsHelper helper;

        private static string languageUnderstandingSubscriptionKey;
        private static string languageUnderstandingServiceRegion;
        private static string languageUnderstandingHomeAutomationAppId;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            deploymentId = DefaultSettingsMap[DefaultSettingKeys.DEPLOYMENT_ID];

            languageUnderstandingSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Key;
            languageUnderstandingServiceRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Region;
            languageUnderstandingHomeAutomationAppId = DefaultSettingsMap[DefaultSettingKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID];
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterStartAndStopWithInvalidSubscription()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            var stopRecognition = new TaskCompletionSource<int>();

            var invalidConfig = SpeechConfig.FromSubscription("invalid", "invalid");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(invalidConfig, audioInput)))
            {
                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                recognizer.Canceled += (s, e) =>
                {
                    stopRecognition.TrySetResult(0);
                };
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);
                Task.WaitAny(new[] { stopRecognition.Task }, kwsFoundTimeoutDelay);
                SPXTEST_ISTRUE(stopRecognition.Task.IsCompleted, "Canceled event not received");
                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechSpeechKeywordspotterStartStop()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                // just wait some "random" time
                await Task.Delay(1000);

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterNonExisting()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                // since we request the ddk kws implementation, this must fail
                // on our build servers.
                recognizer.Properties.SetProperty("CARBON-INTERNAL-UseKwsEngine-Ddk", "true");
                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);

                // so, check that this really fails
                try
                {
                    recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false).GetAwaiter().GetResult();
                    SPXTEST_FAIL("recognizer did not throw expected exception");
                }
                catch (ApplicationException ex)
                {
                    Console.WriteLine("Got Exception: " + ex.Message.ToString());
                    SPXTEST_ISTRUE(ex.Message.Contains("EXTENSION_LIBRARY_NOT_FOUND"), "not the expected error message: " + ex.Message);
                }

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterComputerFoundFromWavFile()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromWavFileInput(str);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }
        
        [RetryTestMethod]
        [Ignore]
        public async Task TestSpeechKeywordspotterComputer2PassFoundFromWavFile()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromWavFileInput(str);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer2Pass.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer2Pass.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay * 2));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterEventsFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var idx = 0;
            var tcsFoundRecognizing = -1;
            var tcsFoundRecognized = -1;
            var error = String.Empty;

            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                    {
                        if (tcsFoundRecognizing >= 0) error = "tcsFoundRecognizing already set";
                        tcsFoundRecognizing = idx++;
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                    {
                        if (tcsFoundRecognized >= 0) error = "tcsFoundRecognized already set";
                        tcsFoundRecognized = idx++;
                        tcs.TrySetResult(true);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_NOT_EQUAL(-1, tcsFoundRecognizing, $"tcsFoundRecognizing not detected within timeout ({sessionid})");
                SPXTEST_ARE_NOT_EQUAL(-1, tcsFoundRecognized, $"tcsFoundRecognized not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcsFoundRecognizing < tcsFoundRecognized, $"tcsFoundRecognized event ordering incorrect ({sessionid})");
                SPXTEST_ISTRUE(error.Length == 0, $"{error} ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechSpeechKeywordspotterSecretFound()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.SECRET_KEYWORDS].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Secret.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Secret.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        [Ignore("This test is crashing a lot of the time on a background thread.")]
        public async Task TestSpeechKeywordspotterSecretAndComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            bool tcsSecret = false;
            bool tcsComputer = false;

            var audioInputSecret = AudioConfig.FromStreamInput(AudioInputStream.CreatePullStream(new RealTimeMultiFileStream(AudioUtterancesMap[AudioUtteranceKeys.SECRET_KEYWORDS].FilePath.GetRootRelativePath())));
            var recognizerSecret = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInputSecret));
            {
                recognizerSecret.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizerSecret.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Secret.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED(Secret.ModelKeyword): " + e.Result.Text.ToString());
                        tcsSecret = true;
                        if (tcsSecret && tcsComputer)
                        {
                            tcs.TrySetResult(true);
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED(Secret.ModelKeyword): " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizerSecret.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };
            }

            var audioInputComputer = AudioConfig.FromStreamInput(AudioInputStream.CreatePullStream(new RealTimeMultiFileStream(AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath())));
            var recognizerComputer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInputComputer));
            {
                recognizerComputer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizerComputer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED(Computer.ModelKeyword): " + e.Result.Text.ToString());
                        tcsComputer = true;
                        if (tcsSecret && tcsComputer)
                            tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED(Computer.ModelKeyword): " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizerComputer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };
            }

            var modelSecret = KeywordRecognitionModel.FromFile(TestData.Kws.Secret.ModelFile);
            var taskStartRecognizerSecret = recognizerSecret.StartKeywordRecognitionAsync(modelSecret);

            var modelComputer = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
            var taskStartRecognizerComputer = recognizerComputer.StartKeywordRecognitionAsync(modelComputer);

            await taskStartRecognizerSecret.ConfigureAwait(false);
            await taskStartRecognizerComputer.ConfigureAwait(false);

            var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay));
            Console.WriteLine();

            var sessionid1 = recognizerSecret.Properties.GetProperty(PropertyId.Speech_SessionId);
            var sessionid2 = recognizerComputer.Properties.GetProperty(PropertyId.Speech_SessionId);
            SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid1}, {sessionid2})");
            SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid1}, {sessionid2})");

            await recognizerSecret.StopKeywordRecognitionAsync().ConfigureAwait(false);
            await recognizerComputer.StopKeywordRecognitionAsync().ConfigureAwait(false);
        }

        [RetryTestMethod]
        [Ignore("This test is crashing a lot of the time on a background thread.")]
        public async Task TestSpeechKeywordspotterSecretAndComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var tcsSecret = false;
            var tcsComputer = false;
            var numCanceledCalled = 0;

            var audioInputSecret = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SECRET_KEYWORDS].FilePath.GetRootRelativePath());
            var recognizerSecret = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInputSecret));
            {
                recognizerSecret.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcsSecret = true;
                    tcs.TrySetResult(true);
                };

                recognizerSecret.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result EXPECTED(Secret.ModelKeyword): " + e.Result.Text.ToString());
                    tcsSecret = true;
                    tcs.TrySetResult(true);
                };

                recognizerSecret.Canceled += (s, e) =>
                {
                    Console.WriteLine("Secret Canceled: " + e.ToString());

                    if (2 == Interlocked.Increment(ref numCanceledCalled))
                        tcsCanceled.TrySetResult(true);
                };
            }

            var audioInputComputer = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath());
            var recognizerComputer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInputComputer));
            {
                recognizerComputer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcsComputer = true;
                    tcs.TrySetResult(true);
                };

                recognizerComputer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result EXPECTED(Computer.ModelKeyword): " + e.Result.Text.ToString());
                    tcsComputer = true;
                    tcs.TrySetResult(true);
                };

                recognizerComputer.Canceled += (s, e) =>
                {
                    Console.Write("Computer Canceled: " + e.ToString());

                    if (2 == Interlocked.Increment(ref numCanceledCalled))
                        tcsCanceled.TrySetResult(true);
                };
            }

            var modelSecret = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
            var taskStartRecognizerSecret = recognizerSecret.StartKeywordRecognitionAsync(modelSecret);

            var modelComputer = KeywordRecognitionModel.FromFile(TestData.Kws.Secret.ModelFile);
            var taskStartRecognizerComputer = recognizerComputer.StartKeywordRecognitionAsync(modelComputer);

            await taskStartRecognizerSecret.ConfigureAwait(false);
            await taskStartRecognizerComputer.ConfigureAwait(false);

            var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(kwsFoundTimeoutDelay));
            SPXTEST_ARE_EQUAL(1, hasCompleted, "keyword detected within timeout is unexpected");
            SPXTEST_ISFALSE(tcs.Task.IsCompleted, "keyword detected task within timeout is unexpected");
            SPXTEST_ISFALSE(tcs.Task.IsCanceled, "keyword detected task within timeout is unexpected");

            SPXTEST_ARE_EQUAL(2, numCanceledCalled, "num closed sessions mismatch");

            SPXTEST_ISFALSE(tcsSecret, "secret keyword detected that should not happen");
            SPXTEST_ISFALSE(tcsComputer, "computer keyword detected that should not happen");

            await recognizerSecret.StopKeywordRecognitionAsync().ConfigureAwait(false);
            await recognizerComputer.StopKeywordRecognitionAsync().ConfigureAwait(false);
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterComputerFound2Utterances()
        {
            var tcs = new TaskCompletionSource<bool>();
            var count = 0;

            var stream = new RealTimeMultiFileStream(AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath(),
                AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath());

            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(stream));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());

                        if (Interlocked.Increment(ref count) == 2)
                        {
                            tcs.TrySetResult(true);
                            return;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                        return;
                    }

                    stream.NextFile();
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay * 2));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(true, tcs.Task.Result, $"2x keyword not detected within timeout ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterComputerFound2DifferentUtterances()
        {
            var tcs = new TaskCompletionSource<bool>();
            var count = 0;
            var error = String.Empty;

            var stream = new RealTimeMultiFileStream(AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath(),
                AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2].FilePath.GetRootRelativePath());

            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(stream));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());

                        if (Interlocked.Increment(ref count) == 2)
                        {
                            if (!e.Result.Text.Contains("old"))
                                error = "text did not contain old";
                            tcs.TrySetResult(true);
                            return;
                        }
                        else
                        {
                            if (!e.Result.Text.Contains("weather"))
                                error = "text did not contain what";
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                        return;
                    }

                    stream.NextFile();
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine(DateTime.Now + "Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay * 2));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(true, tcs.Task.Result, $"2x keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(0, error.Length, $"{error} ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechKeywordspotterComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            var numCanceledCalled = 0;

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcs.TrySetResult(true);
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result: " + e.ToString());
                    tcs.TrySetResult(true);
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());

                    if (e.Reason == CancellationReason.EndOfStream)
                        Interlocked.Increment(ref numCanceledCalled);
                    tcsCanceled.TrySetResult(e.Reason == CancellationReason.EndOfStream);
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(kwsFoundTimeoutDelay));
                SPXTEST_ARE_EQUAL(1, hasCompleted, "did expect a result (for canceled)");
                SPXTEST_ARE_EQUAL(1, numCanceledCalled, "unexpected num close sessions");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestSpeechMockKeywordspotterStartStop()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                // TODO: use some explicit model file, not re-use the wave here
                recognizer.Properties.SetProperty("CARBON-INTERNAL-UseKwsEngine-Mock", "true");
                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);

                recognizer.Canceled += (s, e) => { Console.WriteLine($"Recognition Canceled: {e.ToString()}"); };
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);
                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestIntentKeywordspotterStartStop()
        {
            var config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);

            var str = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = new IntentRecognizer(config, audioInput))
            {
                var model2 = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model2);

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                // just wait some "random" time
                await Task.Delay(1000);

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestIntentKeywordspotterComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            var numCanceledCalled = 0;

            var config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);

            using (var recognizer = new IntentRecognizer(config, audioInput))
            {
                var model2 = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model2);

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcs.TrySetResult(true);
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result: " + e.ToString());
                    tcs.TrySetResult(true);
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled EXPECTED: " + e.ToString());
                    if (e.Reason == CancellationReason.EndOfStream)
                        Interlocked.Increment(ref numCanceledCalled);
                    tcsCanceled.TrySetResult(e.Reason == CancellationReason.EndOfStream);
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(kwsFoundTimeoutDelay));
                SPXTEST_ARE_EQUAL(1, hasCompleted, "did expect a result (for canceled)");
                SPXTEST_ARE_EQUAL(1, numCanceledCalled, "unexpected num close sessions");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestIntentKeywordspotterComputerFound2DifferentUtterances()
        {
            var count = 0;
            var error = String.Empty;
            var tcs = new TaskCompletionSource<bool>();
            var tcsKeywordRecognizing = 0;
            var tcsKeywordRecognized = 0;
            var config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);

            var stream = new RealTimeMultiFileStream(AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath(),
                AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2].FilePath.GetRootRelativePath());

            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(stream));

            using (var recognizer = new IntentRecognizer(config, audioInput))
            {
                var model2 = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model2);

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        Interlocked.Increment(ref tcsKeywordRecognizing);
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                    {
                        Interlocked.Increment(ref tcsKeywordRecognized);
                        return; // ignore keyword events for now
                    }

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString() + ", " + e.Result.IntentId);

                        if (Interlocked.Increment(ref count) == 2)
                        {
                            if (!e.Result.Text.Contains("old"))
                                error = "text does not contain old - " + e.Result.Text;
                            tcs.TrySetResult(true);
                            return;
                        }
                        else
                        {
                            if (!e.Result.Text.Contains("weather"))
                                error = "text does not contain what - " + e.Result.Text;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString() + ", " + e.Result.IntentId);
                        tcs.TrySetResult(false);
                        return;
                    }

                    stream.NextFile();
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay * 2));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(true, tcs.Task.Result, $"2x keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(2, tcsKeywordRecognizing, $"2x tcsKeywordRecognizing not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(2, tcsKeywordRecognized, $"2x tcsKeywordRecognized not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(0, error.Length, $"{error} ({sessionid})");
                stream.Close();
                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestIntentKeywordspotterComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);

            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = new IntentRecognizer(config, audioInput))
            {
                var model2 = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model2);

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword) &&
                        !String.IsNullOrWhiteSpace(e.Result.IntentId))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString() + ", " + e.Result.IntentId);
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED: " + e.Result.Text.ToString() + ", " + e.Result.IntentId);
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                    Console.WriteLine("ErrorDetails: " + e.ErrorDetails);
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestTranslationKeywordspotterComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            var numCanceledCalled = 0;

            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            using (var recognizer = new TranslationRecognizer(config, audioInput))
            {
                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcs.TrySetResult(true);
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result: " + e.ToString());
                    tcs.TrySetResult(true);
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled EXPECTED: " + e.ToString());
                    if (e.Reason == CancellationReason.EndOfStream)
                        Interlocked.Increment(ref numCanceledCalled);
                    tcsCanceled.TrySetResult(e.Reason == CancellationReason.EndOfStream);
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(kwsFoundTimeoutDelay));
                SPXTEST_ARE_EQUAL(1, hasCompleted, "did expect a result (for canceled)");
                SPXTEST_ARE_EQUAL(1, numCanceledCalled, "unexpected num close sessions");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestTranslationKeywordspotterComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            using (var recognizer = new TranslationRecognizer(config, audioInput))
            {
                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionStarted: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());
                        tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ISTRUE(tcs.Task.Result, $"keyword not detected within timeout ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [RetryTestMethod]
        public async Task TestTranslationKeywordspotterComputerFound2DifferentUtterances()
        {
            var count = 0;
            var tcs = new TaskCompletionSource<bool>();
            var error = String.Empty;

            var stream = new RealTimeMultiFileStream(AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath(),
                AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2].FilePath.GetRootRelativePath());

            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(stream));

            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = Language.EN;
            config.AddTargetLanguage(Language.DE);

            using (var recognizer = new TranslationRecognizer(config, audioInput))
            {
                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionStarted with SessionId: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine("Recognizing \"Intermediate\" result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine("Final Result.Reason: " + e.Result.Reason.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                    {
                        return; // ignore initial keyword detected events.
                    }

                    Console.WriteLine("Final result.Text.StartsWith() EXPECTED: " + TestData.Kws.Computer.ModelKeyword);
                    Console.WriteLine("Final result.Text ACTUAL: " + e.Result.Text.ToString());
                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        if (Interlocked.Increment(ref count) == 2)
                        {
                            if (!e.Result.Text.Contains("old"))
                            {
                                error = "text did not contain old - " + e.Result.Text;
                            }

                            tcs.TrySetResult(true);
                            return;
                        }
                        else
                        {
                            if (!e.Result.Text.Contains("weather"))
                            {
                                error = "text did not contain what - " + e.Result.Text;
                            }
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                        return;
                    }

                    stream.NextFile();
                };

                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.ToString());
                    Console.WriteLine("ErrorDetails: " + e.ErrorDetails);
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(kwsFoundTimeoutDelay * 2));
                Console.WriteLine();

                var sessionid = recognizer.Properties.GetProperty(PropertyId.Speech_SessionId);
                SPXTEST_ARE_EQUAL(0, hasCompleted, $"keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(true, tcs.Task.Result, $"2x keyword not detected within timeout ({sessionid})");
                SPXTEST_ARE_EQUAL(0, error.Length, $"{error} ({sessionid})");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        #region Dispose Timing Tests
        // Tests that start recognition in various modes and for different events allow the recognizer to fall out of scope and be disposed
        // mid recognition

        [RetryTestMethod]
        public async Task CloseOnSessonStart()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.SessionStarted += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [RetryTestMethod]
        public async Task CloseOnRecognizing()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognizing += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [RetryTestMethod]
        public async Task CloseOnRecognized()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.Recognized += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [RetryTestMethod]
        public async Task CloseOnSpeechStart()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.SpeechStartDetected += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        [RetryTestMethod]
        public async Task CloseOnConnected()
        {
            var str = AudioUtterancesMap[AudioUtteranceKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].FilePath.GetRootRelativePath();
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            ManualResetEvent disposeEvent = new ManualResetEvent(false);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);

                connection.Connected += (sender, e) =>
                {
                    disposeEvent.Set();
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                disposeEvent.WaitOne();
            }
        }

        #endregion
    }
}

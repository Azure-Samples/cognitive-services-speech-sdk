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
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.CognitiveServices.Speech.Audio;

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using System.Threading;
    using static AssertHelpers;
    using static SpeechRecognitionTestsHelper;

    // provides an input stream that does playback a file in "realtime", i.e.,
    // returns 32000bytes/sec assuming our standard mono/16bits/16000hz
    // wav file format.
    public class RealTimeAudioInputStream : PullAudioInputStreamCallback
    {
        FileStream fs;
        DateTime notBefore;

        public RealTimeAudioInputStream(string filename)
        {
            fs = File.OpenRead(filename);
            notBefore = DateTime.Now;
        }

        public override int Read(byte[] dataBuffer, uint size)
        {
            var now = DateTime.Now;

            // calculates the deadline when the next data should arrive assuming
            // a stream of mono/16bits/16000hz audio input.
            var newNotBefore = notBefore.AddMilliseconds((1000 * size) / 32000);

            var delay = notBefore - now;
            if (delay > TimeSpan.Zero)
            {
                Thread.Sleep((int)delay.TotalMilliseconds);
            }

            notBefore = newNotBefore;
            return fs.Read(dataBuffer, 0, (int)size);
        }
    }

    [TestClass]
    public class KwsRecognitionTests : RecognitionTestBase
    {
        private static string deploymentId;
        private SpeechRecognitionTestsHelper helper;

        private static string languageUnderstandingSubscriptionKey;
        private static string languageUnderstandingServiceRegion;
        private static string languageUnderstandingHomeAutomationAppId;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
            deploymentId = Config.GetSettingByKey<String>(context, "DeploymentId");

            languageUnderstandingSubscriptionKey = Config.GetSettingByKey<String>(context, "LanguageUnderstandingSubscriptionKey");
            languageUnderstandingServiceRegion = Config.GetSettingByKey<String>(context, "LanguageUnderstandingServiceRegion");
            languageUnderstandingHomeAutomationAppId = Config.GetSettingByKey<String>(context, "LanguageUnderstandingHomeAutomationAppId");
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
        }

        [TestMethod]
        public async Task TestSpeechSpeechKeywordspotterStartStop()
        {
            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                // just wait some "random" time
                await Task.Delay(1000);

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterNonExisting()
        {
            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                // since we request the ddk kws implementation, this must fail
                // on our build servers.
                recognizer.Properties.SetProperty("CARBON-INTERNAL-UseKwsEngine-Ddk", "true");
                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);

                // so, check that this really fails
                try
                {
                    recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false).GetAwaiter().GetResult();
                    Assert.Fail("recognizer did not throw expected exception");
                }
                catch (ApplicationException ex)
                {
                    Console.WriteLine("Got Exception: " + ex.Message.ToString());
                    Assert.IsTrue(ex.Message.Contains("EXTENSION_LIBRARY_NOT_FOUND"), "not the expected error message: " + ex.Message);
                }

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) => {
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

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(30000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterComputerFoundFromWavFile()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromWavFileInput(str);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) => {
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

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(30000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterEventsFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var idx = 0;
            var tcsFoundRecognizing = -1;
            var tcsFoundRecognized = -1;
            var error = String.Empty;

            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                    {
                        if (tcsFoundRecognizing >= 0) error = "tcsFoundRecognizing already set";
                        tcsFoundRecognizing = idx++;
                    }
                };

                recognizer.Recognized += (s, e) => {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                    {
                        if (tcsFoundRecognized >= 0) error = "tcsFoundRecognized already set";
                        tcsFoundRecognized = idx++;
                        tcs.TrySetResult(true);
                    }
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(30000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");
                Assert.AreNotEqual(-1, tcsFoundRecognizing, "tcsFoundRecognizing not detected within timeout");
                Assert.AreNotEqual(-1, tcsFoundRecognized, "tcsFoundRecognized not detected within timeout");
                Assert.IsTrue(tcsFoundRecognizing < tcsFoundRecognized, "tcsFoundRecognized event ordering incorrect");
                Assert.IsTrue(error.Length == 0, error);

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechSpeechKeywordspotterSecretFound()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = TestData.Kws.Secret.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) => {
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

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Secret.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(30000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterSecretAndComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            bool tcsSecret = false;
            bool tcsComputer = false;

            var audioInputSecret = AudioConfig.FromWavFileInput(TestData.Kws.Secret.AudioFile);
            var recognizerSecret = TrackSessionId(new SpeechRecognizer(this.config, audioInputSecret));
            {
                recognizerSecret.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizerSecret.Recognized += (s, e) => {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Secret.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED(Secret.ModelKeyword): " + e.Result.Text.ToString());
                        tcsSecret = true;
                        if(tcsSecret && tcsComputer)
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
            }

            var audioInputComputer = AudioConfig.FromWavFileInput(TestData.Kws.Computer.AudioFile);
            var recognizerComputer = TrackSessionId(new SpeechRecognizer(this.config, audioInputComputer));
            {
                recognizerComputer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizerComputer.Recognized += (s, e) => {
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
            }

            var modelSecret = KeywordRecognitionModel.FromFile(TestData.Kws.Secret.ModelFile);
            var taskStartRecognizerSecret = recognizerSecret.StartKeywordRecognitionAsync(modelSecret);

            var modelComputer = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
            var taskStartRecognizerComputer = recognizerComputer.StartKeywordRecognitionAsync(modelComputer);

            await taskStartRecognizerSecret.ConfigureAwait(false);
            await taskStartRecognizerComputer.ConfigureAwait(false);

            var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(30000));
            Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
            Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

            await recognizerSecret.StopKeywordRecognitionAsync().ConfigureAwait(false);
            await recognizerComputer.StopKeywordRecognitionAsync().ConfigureAwait(false);
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterSecretAndComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var tcsSecret = false;
            var tcsComputer = false;
            var numCanceledCalled = 0;

            var audioInputSecret = AudioConfig.FromWavFileInput(TestData.Kws.Secret.AudioFile);
            var recognizerSecret = TrackSessionId(new SpeechRecognizer(this.config, audioInputSecret));
            {
                recognizerSecret.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcsSecret = true;
                    tcs.TrySetResult(true);
                };

                recognizerSecret.Recognized += (s, e) => {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result EXPECTED(Secret.ModelKeyword): " + e.Result.Text.ToString());
                    tcsSecret = true;
                    tcs.TrySetResult(true);
                };

                recognizerSecret.Canceled += (s, e) =>
                {
                    if (2 == Interlocked.Increment(ref numCanceledCalled))
                        tcsCanceled.TrySetResult(true);
                };
            }

            var audioInputComputer = AudioConfig.FromWavFileInput(TestData.Kws.Computer.AudioFile);
            var recognizerComputer = TrackSessionId(new SpeechRecognizer(this.config, audioInputComputer));
            {
                recognizerComputer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcsComputer = true;
                    tcs.TrySetResult(true);
                };

                recognizerComputer.Recognized += (s, e) => {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result EXPECTED(Computer.ModelKeyword): " + e.Result.Text.ToString());
                    tcsComputer = true;
                    tcs.TrySetResult(true);
                };

                recognizerComputer.Canceled += (s, e) =>
                {
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

            var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(20000));
            Assert.AreEqual(1, hasCompleted, "keyword detected within timeout is unexpected");
            Assert.IsFalse(tcs.Task.IsCompleted, "keyword detected task within timeout is unexpected");
            Assert.IsFalse(tcs.Task.IsCanceled, "keyword detected task within timeout is unexpected");

            Assert.AreEqual(2, numCanceledCalled, "num closed sessions mismatch");

            Assert.IsFalse(tcsSecret, "secret keyword detected that should not happen");
            Assert.IsFalse(tcsComputer, "computer keyword detected that should not happen");

            await recognizerSecret.StopKeywordRecognitionAsync().ConfigureAwait(false);
            await recognizerComputer.StopKeywordRecognitionAsync().ConfigureAwait(false);
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterComputerFound2Utterances()
        {
            var tcs = new TaskCompletionSource<bool>();
            var count = 0;

            var str = TestData.Kws.Computer2.AudioFileTranslate;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) => {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());

                        if (Interlocked.Increment(ref count) == 2)
                            tcs.TrySetResult(true);
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) => {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(60000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.AreEqual(true, tcs.Task.Result, "2x keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [Ignore] // TODO temporarily disabled
        [TestMethod]
        public async Task TestSpeechKeywordspotterComputerFound2DifferentUtterances()
        {
            var tcs = new TaskCompletionSource<bool>();
            var count = 0;
            var error = String.Empty;

            var str = TestData.Kws.Computer2.AudioFileTranslate;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) => {
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
                        }
                        else
                        {
                            if (!e.Result.Text.Contains("what"))
                                error = "text did not contain what";
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) => {
                    Console.WriteLine(DateTime.Now + "Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(60000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.AreEqual(true, tcs.Task.Result, "2x keyword not detected within timeout");
                Assert.AreEqual(0, error.Length, error);

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechKeywordspotterComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var str = TestData.English.Weather.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            var numCanceledCalled = 0;

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcs.TrySetResult(true);
                };

                recognizer.Recognized += (s, e) => {
                    Console.WriteLine("Final result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    Console.WriteLine("Final result: " + e.ToString());
                    tcs.TrySetResult(true);
                };

                recognizer.Canceled += (s, e) =>
                {
                    if (e.Reason == CancellationReason.EndOfStream)
                        Interlocked.Increment(ref numCanceledCalled);
                    tcsCanceled.TrySetResult(e.Reason == CancellationReason.EndOfStream);
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(20000));
                Assert.AreEqual(1, hasCompleted, "did expect a result (for canceled)");
                Assert.AreEqual(1, numCanceledCalled, "unexpected num close sessions");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestSpeechMockKeywordspotterStartStop()
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.config, audioInput)))
            {
                // TODO: use some explicit model file, not re-use the wave here
                recognizer.Properties.SetProperty("CARBON-INTERNAL-UseKwsEngine-Mock", "true");
                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);

                recognizer.Canceled += (s, e) => { Console.WriteLine($"Recognition Canceled: {e.ToString()}"); };
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);
                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestIntentKeywordspotterStartStop()
        {
            var config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);

            var str = TestData.Kws.Computer.AudioFile;
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

        [TestMethod]
        public async Task TestIntentKeywordspotterComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var str = TestData.English.Weather.AudioFile;
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

                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcs.TrySetResult(true);
                };

                recognizer.Recognized += (s, e) => {
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

                var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(20000));
                Assert.AreEqual(1, hasCompleted, "did expect a result (for canceled)");
                Assert.AreEqual(1, numCanceledCalled, "unexpected num close sessions");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestIntentKeywordspotterComputerFound2DifferentUtterances()
        {
            var count = 0;
            var error = String.Empty;
            var tcs = new TaskCompletionSource<bool>();
            var tcsKeywordRecognizing = 0;
            var tcsKeywordRecognized = 0;
            var config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);

            var str = TestData.Kws.Computer2.AudioFileIntent;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = new IntentRecognizer(config, audioInput))
            {
                var model2 = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model2);

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        Interlocked.Increment(ref tcsKeywordRecognizing);
                };

                recognizer.Recognized += (s, e) => {
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
                        }
                        else
                        {
                            if (!e.Result.Text.Contains("what"))
                                error = "text does not contain what - " + e.Result.Text;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString() + ", " + e.Result.IntentId);
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) => {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(60000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.AreEqual(true, tcs.Task.Result, "2x keyword not detected within timeout");
                Assert.AreEqual(2, tcsKeywordRecognizing, "2x tcsKeywordRecognizing not detected within timeout");
                Assert.AreEqual(2, tcsKeywordRecognized, "2x tcsKeywordRecognized not detected within timeout");
                Assert.AreEqual(0, error.Length, error);

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestIntentKeywordspotterComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var config = SpeechConfig.FromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);

            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            using (var recognizer = new IntentRecognizer(config, audioInput))
            {
                var model2 = LanguageUnderstandingModel.FromAppId(languageUnderstandingHomeAutomationAppId);
                recognizer.AddAllIntents(model2);

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognized += (s, e) => {
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

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(30000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestTranslationKeywordspotterComputerNotFound()
        {
            var tcs = new TaskCompletionSource<bool>();
            var tcsCanceled = new TaskCompletionSource<bool>();

            var str = TestData.English.Weather.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));
            var numCanceledCalled = 0;

            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = "en-US";
            config.AddTargetLanguage("de");

            using (var recognizer = new TranslationRecognizer(config, audioInput))
            {
                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());

                    if (e.Result.Reason == ResultReason.RecognizingKeyword)
                        return; // ignore keyword events

                    tcs.TrySetResult(true);
                };

                recognizer.Recognized += (s, e) => {
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

                var hasCompleted = Task.WaitAny(tcs.Task, tcsCanceled.Task, Task.Delay(20000));
                Assert.AreEqual(1, hasCompleted, "did expect a result (for canceled)");
                Assert.AreEqual(1, numCanceledCalled, "unexpected num close sessions");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [TestMethod]
        public async Task TestTranslationKeywordspotterComputerFound()
        {
            var tcs = new TaskCompletionSource<bool>();

            var str = TestData.Kws.Computer.AudioFile;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = "en-US";
            config.AddTargetLanguage("de");

            using (var recognizer = new TranslationRecognizer(config, audioInput))
            {
                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine("SessionStarted: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Canceled += (s, e) => {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                recognizer.Recognized += (s, e) => {
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

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(30000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.IsTrue(tcs.Task.Result, "keyword not detected within timeout");

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }

        [Ignore] // TODO temporarily disabled
        [TestMethod]
        public async Task TestTranslationKeywordspotterComputerFound2DifferentUtterances()
        {
            var count = 0;
            var tcs = new TaskCompletionSource<bool>();
            var error = String.Empty;

            var str = TestData.Kws.Computer2.AudioFileTranslate;
            var audioInput = AudioConfig.FromStreamInput(new PullAudioInputStream(new RealTimeAudioInputStream(str)));

            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = "en-US";
            config.AddTargetLanguage("de");

            using (var recognizer = new TranslationRecognizer(config, audioInput))
            {
                recognizer.SessionStarted += (s, e) => {
                    Console.WriteLine("SessionId: " + e.SessionId);
                };

                recognizer.Recognizing += (s, e) => {
                    Console.WriteLine("Intermediate result: " + e.ToString());
                };

                recognizer.Recognized += (s, e) => {
                    Console.WriteLine("Final result: " + e.Result.Reason.ToString());

                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        return; // ignore keyword events

                    if (e.Result.Text.ToLowerInvariant().StartsWith(TestData.Kws.Computer.ModelKeyword))
                    {
                        Console.WriteLine("Final result EXPECTED: " + e.Result.Text.ToString());

                        if (Interlocked.Increment(ref count) == 2)
                        {
                            if (!e.Result.Text.Contains("old"))
                                error = "text did not contain old - " + e.Result.Text;
                            tcs.TrySetResult(true);
                        }
                        else
                        {
                            if (!e.Result.Text.Contains("what"))
                                error = "text did not contain what - " + e.Result.Text;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Final result UNEXPECTED : " + e.Result.Text.ToString());
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Canceled += (s, e) => {
                    Console.WriteLine("Canceled: " + e.ToString());
                };

                var model = KeywordRecognitionModel.FromFile(TestData.Kws.Computer.ModelFile);
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                var hasCompleted = Task.WaitAny(tcs.Task, Task.Delay(90000));
                Assert.AreEqual(0, hasCompleted, "keyword not detected within timeout");
                Assert.AreEqual(true, tcs.Task.Result, "2x keyword not detected within timeout");
                Assert.AreEqual(0, error.Length, error);

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
        }
    }
}

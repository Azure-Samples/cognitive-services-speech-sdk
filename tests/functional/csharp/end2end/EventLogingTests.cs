//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Test;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Concurrent;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class EventLoggingTests : RecognitionTestBase
    {
        private SpeechRecognitionTestsHelper helper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
        }

        #region Diagnostic Tests
        [TestMethodWindowsOnly]
        public async Task EnableLogging()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ConcurrentBag<string> logMessages = new ConcurrentBag<string>();

            Test.Internal.LogMessageCallbackFunctionDelegate logCallback = (string log) =>
            {
                logMessages.Add(log);
            };

            Diagnostics.SetLogMessageCallback(logCallback);
            try
            {
                using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
                {
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                }

                Assert.IsTrue(logMessages.Count > 0, "No messages were logged");
            }
            finally
            {
                Diagnostics.SetLogMessageCallback(null);
            }
        }

        [TestMethodWindowsOnly]
        public async Task DisableLogging()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ConcurrentBag<string> logMessages = new ConcurrentBag<string>();

            Test.Internal.LogMessageCallbackFunctionDelegate logCallback = (string log) =>
            {
                logMessages.Add(log);
            };

            Diagnostics.SetLogMessageCallback(logCallback);
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            }

            Assert.IsTrue(logMessages.Count > 0);

            int logMessageCount = logMessages.Count;
            Console.WriteLine($"{logMessageCount} before clearing callback.");

            Diagnostics.SetLogMessageCallback(null);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            }

            Assert.IsTrue(logMessageCount == logMessages.Count, $"Log message count differ '{logMessageCount}' != '{logMessages.Count}'");
        }

        [TestMethodWindowsOnly]
        public async Task FilterLogging()
        {
            var filePath = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath();

            var audioInput = AudioConfig.FromWavFileInput(filePath);

            ConcurrentBag<string> logMessages = new ConcurrentBag<string>();

            Test.Internal.LogMessageCallbackFunctionDelegate logCallback = (string log) =>
            {
                logMessages.Add(log);
            };

            Diagnostics.SetLogMessageFilter(filePath);
            Diagnostics.SetLogMessageCallback(logCallback);
            try
            {
                using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
                {
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    Console.WriteLine("Recognition done");
                }

                Assert.IsTrue(logMessages.Count > 0, "No messages were logged");

                foreach (var line in logMessages)
                {
                    Assert.IsTrue(line.Contains(filePath), $"Line '{line}' did not contain '{filePath}'");
                }
            }
            finally
            {
                Diagnostics.SetLogMessageCallback(null);
            }
        }

        [TestMethodWindowsOnly]
        public async Task LambdaLogging()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            ConcurrentBag<string> logMessages = new ConcurrentBag<string>();

            Diagnostics.SetLogMessageCallback((string log) =>
            {
                logMessages.Add(log);
            });

            // Force a full GC.
            GC.Collect(GC.MaxGeneration);

            try
            {
                using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
                {
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                }

                Assert.IsTrue(logMessages.Count > 0, "No messages were logged");
            }
            finally
            {
                Diagnostics.SetLogMessageCallback(null);
            }
        }
    }
    #endregion

    internal class TestMethodWindowsOnly : TestMethodAttribute
    {
        public override TestResult[] Execute(ITestMethod testMethod)
        {
            if (Environment.OSVersion.Platform != PlatformID.Win32NT)
            {
                return new[]
                {
                    new TestResult
                    {
                        Outcome = UnitTestOutcome.NotRunnable,
                        TestFailureException = new AssertInconclusiveException("Test only valid on windows.")
                    }
                };
            }
            else
            {
                return base.Execute(testMethod);
            }
        }
    }
}

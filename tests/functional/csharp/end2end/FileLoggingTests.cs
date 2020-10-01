//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class FileLoggingTests : RecognitionTestBase
    {
        private SpeechRecognitionTestsHelper helper;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            BaseClassInit(context);
        }

        [ClassCleanup]
        new public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new SpeechRecognitionTestsHelper();
        }

        #region Diagnostic Tests
        [RetryTestMethod]
        public void SwitchLogFileNames()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            var baseName = "logifile";

            for (int i = 0; i <= 5; i++)
            {
                this.defaultConfig.SetProperty("SPEECH-LogFilename", baseName + i);
            }
        }

        [RetryTestMethod]
        public async Task StartAndStopLogging()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            var baseName = Path.GetTempFileName();

            this.defaultConfig.SetProperty("SPEECH-LogFilename", baseName);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            }

            audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            long size = 0;
            FileInfo fi;

            this.defaultConfig.SetProperty("SPEECH-LogFilename", "");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                // That should have closed the file and flushed it, so now check the length.
                fi = new FileInfo(baseName);
                size = fi.Length;
                Console.WriteLine("Size after first file closed = " + size);
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            }

            fi = new FileInfo(baseName);
            Console.WriteLine("File size while closed: " + fi.Length);
            SPXTEST_ARE_EQUAL(size, fi.Length, "File should not have changed size");

            this.defaultConfig.SetProperty("SPEECH-LogFilename", baseName);
            this.defaultConfig.SetProperty("SPEECH-AppendToLogFile", "1");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            }

            // Create a recognizer to force the file to close and flush.
            this.defaultConfig.SetProperty("SPEECH-LogFilename", "");
            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
            }
            fi = new FileInfo(baseName);
            Console.WriteLine("File size after being re-enabled: " + fi.Length);
            SPXTEST_ISTRUE(size < fi.Length, "File should have grown.");
        }

        [RetryTestMethod]
        public async Task FileSizeCap()
        {
            ManualResetEvent stoppedEvent = new ManualResetEvent(false);

            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            var baseName = Path.GetTempFileName();
            var query = Path.GetFileNameWithoutExtension(baseName) + "*" + Path.GetExtension(baseName);
            foreach (var file in Directory.EnumerateFiles(Path.GetTempPath(), query))
            {
                File.Delete(file);
            }

            this.defaultConfig.SetProperty("SPEECH-LogFilename", baseName);
            this.defaultConfig.SetProperty("SPEECH-FileLogSizeMB", "1");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                recognizer.SessionStopped += (o, e) => { stoppedEvent.Set(); };
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                if (!stoppedEvent.WaitOne(TimeSpan.FromMinutes(1)))
                {
                    Console.WriteLine("Failed to get session stopped event in one minute. Continuing");
                }
            }
            audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            }

            var tempDir = Path.GetDirectoryName(baseName);

            var files = Directory.EnumerateFiles(tempDir, query);
            var fileCount = files.Count();
            SPXTEST_ISTRUE(fileCount > 1, "Not enough files created");
        }

        [RetryTestMethod]
        public async Task TestFileLogFilter()
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            var baseName = Path.GetTempFileName();

            this.defaultConfig.SetProperty("SPEECH-LogFilename", baseName);
            this.defaultConfig.SetProperty("SPEECH-LogFileFilters", AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text);

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            }

            this.defaultConfig.SetProperty("SPEECH-LogFilename", "");

            using (var recognizer = TrackSessionId(new SpeechRecognizer(this.defaultConfig, audioInput)))
            {
            }

            foreach(var line in File.ReadAllLines(baseName))
            {
                SPXTEST_ISTRUE(line.Contains(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text));
            }
        }

        #endregion
    }
}
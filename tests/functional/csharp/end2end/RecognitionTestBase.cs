//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.CognitiveServices.Speech.Tests;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using System;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text.RegularExpressions;

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

        private static readonly JsonSerializerSettings SERIALIZER_SETTINGS = new JsonSerializerSettings()
        {
            Formatting = Formatting.Indented,
            Converters = new[]
            {
                new Newtonsoft.Json.Converters.StringEnumConverter()
            },
            DateFormatString = "o"
        };

        private static Config _config;

        protected bool _collectNativeLogs;
        private string _logFilename;

        public RecognitionTestBase() : this(false)
        {
        }

        protected RecognitionTestBase(bool collectNativeLogs)
        {
            this._collectNativeLogs = collectNativeLogs;
        }

        public TestContext TestContext { get; set; }

        public static void BaseClassInit(TestContext context)
        {
            LoggingTestBaseInit(context);
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

        [ClassCleanup]
        public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
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

            if (_collectNativeLogs)
            {
                // start logging to a file. This will be read back and dumped to the trace logs at the end of the
                // test execution
                _logFilename = $"Carbon_{TestContext.TestName.FileNameSanitize()}.txt";
                IntPtr res = this.ResetLogging();
                if (res != IntPtr.Zero)
                {
                    Log($"Failed to reset logging. Cause: {res.ToInt64()}");
                }

                res = this.StartLogging(_logFilename);
                if (res != IntPtr.Zero)
                {
                    Log($"Failed to start logging to {_logFilename}. Cause: {res.ToInt64()}");
                }
                else
                {
                    Log($"Started logging to {_logFilename}");
                }
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "Don't care about exceptions here")]
        [TestCleanup]
        public void BaseTestCleanup()
        {
            if (_logFilename != null)
            {
                try
                {
                    // just in case there are any logs that still need to be written
                    System.Threading.Thread.Sleep(250);

                    var logFile = new System.IO.FileInfo(_logFilename);
                    if (!logFile.Exists)
                    {
                        Log("Log file did not exist");
                        return;
                    }

                    // force the log file to close otherwise we can't access it here
                    IntPtr res = this.StopLogging();
                    if (res != IntPtr.Zero)
                    {
                        Log($"Failed to stop logging to {_logFilename}: {res.ToInt64()}");
                    }

                    LogRaw(Environment.NewLine);

                    // dump log file so the output stream
                    foreach (var line in System.IO.File.ReadLines(logFile.FullName))
                    {
                        LogRaw(line);
                    }

                    logFile.Delete();
                }
                catch (Exception e)
                {
                    Log($"Encountered an exception when trying to read {_logFilename}. {e.GetType().FullName}: {e}");
                }
                finally
                {
                    this.ResetLogging();
                }
            }
        }

        internal void Log(string msg, [CallerMemberName] string caller = null, [CallerLineNumber] int line = 0, [CallerFilePath] string file = null)
        {
            string logMessage = $"({DateTime.UtcNow.ToString("yyyy-MM-dd HH::mm::ss.ff")}) [{Path.GetFileName(file)}:{caller}:{line}] {msg}";
            TestContext.WriteLine(logMessage);
        }

        internal void LogRaw(string msg)
        {
            TestContext.WriteLine(msg);
        }

        internal T LogEvent<T>(T eventArg, string msg =  null, [CallerMemberName] string caller = null, [CallerLineNumber] int line = 0, [CallerFilePath] string file = null)
        {
            string json = JsonConvert.SerializeObject(eventArg, SERIALIZER_SETTINGS) ?? string.Empty;
            json = Regex.Replace(json, "^", "    ", RegexOptions.Multiline);

            Log($"{msg} {json}", caller, line, file);
            return eventArg;
        }
    }
}

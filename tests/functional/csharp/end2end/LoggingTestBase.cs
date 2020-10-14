//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech.Test;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Text.RegularExpressions;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    /// <summary>
    /// Base class for C# tests that enables native logging capture
    /// </summary>
    [TestClass]
    public class LoggingTestBase
    {
        private static readonly JsonSerializerSettings SERIALIZER_SETTINGS = new JsonSerializerSettings()
        {
            Formatting = Formatting.Indented,
            Converters = new[]
            {
                new Newtonsoft.Json.Converters.StringEnumConverter()
            },
            DateFormatString = "o"
        };

        private static Lazy<bool> memoryLoggerEnabled = new Lazy<bool>(() =>
        {
            var loggingEnvVar = Environment.GetEnvironmentVariable("SPEECHSDK_TEST_LOGGING");
            var enableMemoryLogging = string.IsNullOrEmpty(loggingEnvVar) || loggingEnvVar.Contains("memory");

            return enableMemoryLogging;
        });

        /// <summary>
        /// Gets whether or not the memory logger is enabled
        /// </summary>
        public static bool IsMemoryLoggerEnabled => memoryLoggerEnabled.Value;

        /// <summary>
        /// The context for the current test. This is automatically set by VSTest during execution
        /// </summary>
        public TestContext TestContext { get; set; }

        /// <summary>
        /// Set this to true if you've already dumped the memory logs
        /// </summary>
        protected static bool HasDumpedLogs { get; set; }

        /// <summary>
        /// Runs once at the start of test classes that inherit from <see cref="LoggingTestBase"/>
        /// </summary>
        [ClassInitialize(InheritanceBehavior.BeforeEachDerivedClass)]
        public static void LoggingTestBaseInit(TestContext context)
        {
            if (IsMemoryLoggerEnabled)
            {
                Diagnostics.StartMemoryLogging();
            }
            else
            {
                context.WriteLine("Memory logger is not enabled");
            }
        }

        /// <summary>
        /// Runs once at the end of test classes that inherit from <see cref="LoggingTestBase"/>
        /// </summary>
        [ClassCleanup(InheritanceBehavior.BeforeEachDerivedClass)]
        public static void LoggingTestBaseCleanup()
        {
            if (IsMemoryLoggerEnabled)
            {
                Diagnostics.StopMemoryLogging();
            }
        }

        /// <summary>
        /// Dumps the memory logs if they have not already been dumped for the current test
        /// </summary>
        public static void TryDumpMemoryLogs()
        {
            if (!IsMemoryLoggerEnabled || HasDumpedLogs)
            {
                return;
            }
            else
            {
                HasDumpedLogs = true;
            }

            Diagnostics.DumpMemoryLog(
                filename: null,
                linePrefix: "CRBN",
                emitToStdOut: false,
                emitToStdErr: true);
        }

        /// <summary>
        /// Runs before each test
        /// </summary>
        [TestInitialize]
        public void BeforeTest()
        {
            HasDumpedLogs = false;
            Diagnostics.SPX_TRACE_TEST_START(TestContext.TestName);
        }

        /// <summary>
        /// Runs after each test
        /// </summary>
        [TestCleanup]
        public void AfterTest()
        {
            switch (TestContext?.CurrentTestOutcome)
            {
                case null:
                case UnitTestOutcome.Passed:
                case UnitTestOutcome.InProgress:
                case UnitTestOutcome.NotRunnable:
                    // nothing to do
                    break;

                default:
                    TryDumpMemoryLogs();
                    break;
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

        internal T LogEvent<T>(T eventArg, string msg = null, [CallerMemberName] string caller = null, [CallerLineNumber] int line = 0, [CallerFilePath] string file = null)
        {
            string json = JsonConvert.SerializeObject(eventArg, SERIALIZER_SETTINGS) ?? string.Empty;
            json = Regex.Replace(json, "^", "    ", RegexOptions.Multiline);

            Log($"{msg} {json}", caller, line, file);
            return eventArg;
        }
    }
}

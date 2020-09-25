
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
    using static Config;

    [TestClass]
    public class LoggingTestBase
    {
        [ClassInitialize]
        public static void LoggingTestBaseInit(TestContext context)
        {
            if (testContext == null)
            {
                testContext = context;

                var logging = Environment.GetEnvironmentVariable("SPEECHSDK_TEST_LOGGING");
                var enableMemoryLogging = string.IsNullOrEmpty(logging) || logging.Contains("memory");
                if (enableMemoryLogging) Microsoft.CognitiveServices.Speech.Test.Diagnostics.StartMemoryLogging();
            }
        }

        [ClassCleanup]
        public static void LoggingTestBaseCleanup()
        {
            if (testContext?.CurrentTestOutcome != UnitTestOutcome.Passed)
            {
                Microsoft.CognitiveServices.Speech.Test.Diagnostics.DumpMemoryLog(
                    filename: null,
                    linePrefix: "CLEANUP",
                    emitToStdOut: false,
                    emitToStdErr: true);
                testContext = null;
            }
        }

        protected static TestContext testContext;
    }
}

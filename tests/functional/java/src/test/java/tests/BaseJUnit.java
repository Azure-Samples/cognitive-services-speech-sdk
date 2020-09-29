// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

package tests;

import org.junit.*;
import org.junit.rules.TestWatcher;
import java.util.EnumSet;
import com.microsoft.cognitiveservices.speech.Diagnostics;

/**
 * Base class for all the JUnit tests
 */
public class BaseJUnit {
    private static final boolean MEMORY_LOGGER_ENABLED = isMemoryLoggerEnabled(); 

    /**
     * Uses the test watcher to start/stop the memory logger, and dump the contents to the output
     * stream on test failure
     */
    @Rule
    public TestWatcher watcher = new TestWatcher()
    {
        @Override
        protected void starting(org.junit.runner.Description description) {
            if (MEMORY_LOGGER_ENABLED) {
                Diagnostics.startMemoryLogging();
            }
        };

        @Override
        protected void failed(Throwable e, org.junit.runner.Description description) {
            if (MEMORY_LOGGER_ENABLED) {
                Diagnostics.dumpMemoryLog(null, "CRBN", false, true);
            }
        };

        @Override
        protected void finished(org.junit.runner.Description description) {
            if (MEMORY_LOGGER_ENABLED) {
                Diagnostics.stopMemoryLogging();
            }
        };
    };

    private static boolean isMemoryLoggerEnabled()
    {
        // debug builds already write the native logs to the stderr stream so no need to enable
        // the memory logger
        String buildConfig = System.getProperty("BUILD_CONFIGURATION");
        boolean isDebugBuild = buildConfig != null && buildConfig.toUpperCase().equals("DEBUG");

        // Also check the environment variables to see if memory logging has been enabled. By default
        // it is enabled
        String env = System.getenv("SPEECHSDK_TEST_LOGGING");
        boolean isMemoryLoggerEnabled = env == null || env.isEmpty() || env.toUpperCase().contains("MEMORY");

        return !isDebugBuild && isMemoryLoggerEnabled;
    }
}

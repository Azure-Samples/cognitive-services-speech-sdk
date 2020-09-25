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
        String env = System.getenv("SPEECHSDK_TEST_LOGGING");
        return env == null || env.isEmpty() || env.toUpperCase().contains("MEMORY");
    }
}

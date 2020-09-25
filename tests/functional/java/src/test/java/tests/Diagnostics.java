//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package com.microsoft.cognitiveservices.speech;

import java.util.EnumSet;
import java.util.Set;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Native logging and other diagnostics
 */
public final class Diagnostics {
    public enum MemoryLoggerOptions
    {
        NONE(0),
        FILE(1),
        STANDARD_OUT(2),
        STANDARD_ERROR(4)
        ;

        private final int val;

        MemoryLoggerOptions(int val) {
            this.val = val;
        }

        public int getValue() { return this.val; }
        public static int getValue(Set<MemoryLoggerOptions> set) {
            int value = 0;
            for (MemoryLoggerOptions f : set) {
                value |= f.getValue();
            }
            return value;
        }
    }

    /**
     * Enables the native memory logger
     */
    public static void startMemoryLogging() {
        logMemoryStartLogging();
    }

    /**
     * Stops the native memory logger
     */
    public static void stopMemoryLogging() {
        logMemoryStopLogging();
    }

    /**
     * Dumps the contents of the memory logger
     * @param filename The name of the file to write to. Set this to an empty string if not needed
     * @param linePrefix The prefix to apply to each line of logged memory content, e.g. "CRBN"
     * @param emitToStdOut Whether the log should be emitted to standard output in addition to any other targets
     * @param emitToStdErr Whether the log should be emitted to standard error in addition to any other targets
     */
    public static void dumpMemoryLog(String filename, String linePrefix, boolean emitToStdOut, boolean emitToStdErr) {
        Contracts.throwIfFail(logMemoryDump(filename, linePrefix, emitToStdOut, emitToStdErr));
    }

    private final native static long logMemoryStartLogging();
    private final native static long logMemoryStopLogging();
    private final native static long logMemoryDump(String filename, String linePrefix, boolean emitToStdOut, boolean emitToStdErr);
}

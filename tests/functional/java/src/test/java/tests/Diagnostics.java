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
     * @param option Where to dump the contents of the memory logger to
     */
    public static void dumpMemoryLogToFile(String filename, MemoryLoggerOptions option) {
        int optionsVal = option.getValue();
        Contracts.throwIfFail(logMemoryDumpToFile(filename, optionsVal));
    }

    /**
     * Dumps the contents of the memory logger
     * @param filename The name of the file to write to. Set this to an empty string if not needed
     * @param options Where to dump the contents of the memory logger to
     */
    public static void dumpMemoryLogToFile(String filename, EnumSet<MemoryLoggerOptions> options) {
        int optionsVal = MemoryLoggerOptions.getValue(options);
        Contracts.throwIfFail(logMemoryDumpToFile(filename, optionsVal));
    }
    
    private final native static long logMemoryStartLogging();
    private final native static long logMemoryStopLogging();
    private final native static long logMemoryDumpToFile(String filename, int options);
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Native logging and other diagnostics
 */
public final class Diagnostics {
    /**
     * Enables the native memory logger
     */
    public static void startMemoryLogging() {
        Contracts.throwIfFail(logMemoryStartLogging());
    }

    /**
     * Stops the native memory logger
     */
    public static void stopMemoryLogging() {
        Contracts.throwIfFail(logMemoryStopLogging());
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

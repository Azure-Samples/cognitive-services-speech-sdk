//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package com.microsoft.cognitiveservices.speech.samples.console;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.net.URI;
import java.net.URISyntaxException;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.diagnostics.logging.EventLogger;
import com.microsoft.cognitiveservices.speech.diagnostics.logging.FileLogger;
import com.microsoft.cognitiveservices.speech.diagnostics.logging.MemoryLogger;
import com.microsoft.cognitiveservices.speech.diagnostics.logging.SpxTrace;
import com.microsoft.cognitiveservices.speech.diagnostics.logging.Level;

// Shows how to enable Speech SDK trace logging. Microsoft may ask you to collect logs in order
// to investigate an issue you reported
@SuppressWarnings("resource") // scanner
public class SpeechDiagnosticsLoggingSamples {

    // Enable Speech SDK trace logging to a file
    public static void fileLoggerWithoutFilter() throws InterruptedException, ExecutionException, URISyntaxException {
        // Define the full path and name of the log file on your local disk
        String logFile = "speech-sdk-log.txt";

        // Start logging to the given file. By default it will create
        // a new file, but you have the option of appending to an existing one
        FileLogger.start(logFile);

        // Do your Speech SDK calls here... for example:
        try (SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");
             SpeechRecognizer recognizer = new SpeechRecognizer(config)) {

            // ...

            // Stop logging to the file
            FileLogger.stop();
        }

        // Now look at the log file that was created..
    }

    // Enabled Speech SDK trace logging to a file with a filter
    public static void fileLoggerWithFilter() throws InterruptedException, ExecutionException, URISyntaxException {
        // Define the full path and name of the log file
        String logFile = "speech-sdk-log.txt";

        // Start logging to the given file, but Log only traces
        // that contain either one of the filter strings provided.
        // By default it will create a new file, but you have the option of
        // appending to an existing one.
        String[] filters = { "YourFirstString", "YourSecondString" };

        FileLogger.setFilters(filters);
        FileLogger.start(logFile);

        // Do your Speech SDK calls here... for example:
        try (SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");
             SpeechRecognizer recognizer = new SpeechRecognizer(config)) {

            // ...

            // Stop logging to the file
            FileLogger.stop();
            FileLogger.setFilters();
        }

        // Now look at the log file that was created..
    }

    // Enable Speech SDK trace logging to a subscribed event handler
    public static void eventLoggerWithoutFilter() throws InterruptedException, ExecutionException, URISyntaxException {
        final Object lockObject = new Object();
        List<String> messages = new ArrayList<>();

        // Register a callback that will get invoked by Speech SDK on every new log message
        EventLogger.setCallback((message) -> {
            // Copy the string and store it for further processing. Do not do any processing in the event thread!
            synchronized (lockObject) {
                messages.add(message);
            }
        });

        // Do your Speech SDK calls... for example:
        try (SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");
             SpeechRecognizer recognizer = new SpeechRecognizer(config)) {

            // ...

            // Stop logging by setting an empty callback
            EventLogger.setCallback();
        }

        // See resulting logs on the console
        for (String message : messages) {
            System.out.print(message);
        }
    }

    // Enable Speech SDK trace logging to a subscribed event handler with a filter
    public static void eventLoggerWithFilter() throws InterruptedException, ExecutionException, URISyntaxException {
        final Object lockObject = new Object();
        List<String> messages = new ArrayList<>();

        // Register a callback that will get invoked by Speech SDK on every new log message
        EventLogger.setCallback((message) -> {
            // Copy the string and store it for further processing. Do not do any processing in the event thread!
            synchronized (lockObject) {
                messages.add(message);
            }
        });

        // Set an event filter, such that the callback will be invoked only for traces
        // that contain either one of the filter strings provided.
        String[] filters = { "YourFirstString", "YourSecondString" };
        EventLogger.setFilters(filters);

        // Do your Speech SDK calls... for example:
        try (SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");
             SpeechRecognizer recognizer = new SpeechRecognizer(config)) {

            // ...

            // Stop logging by setting an empty callback
            EventLogger.setCallback();
            EventLogger.setFilters();
        }

        // See resulting logs on the console
        for (String message : messages) {
            System.out.print(message);
        }
    }

    // Enable Speech SDK trace logging to memory buffer with or without a filter
    public static void memoryLoggerWithOrWithoutFilter() throws InterruptedException, ExecutionException, URISyntaxException {
        // Optional - Apply a filter, such that only traces that contain either one of the
        // filter strings will be logged. Microsoft will provide the filter when relevant.
        
        // String[] filters = { "YourFirstString", "YourSecondString" };
        // MemoryLogger.setFilters(filters);

        // Set the level of logging to be captured in memory and start logging
        MemoryLogger.setLevel(Level.Info);
        MemoryLogger.start();

        // Do your Speech SDK calls here... for example:
        try (SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");
             SpeechRecognizer recognizer = new SpeechRecognizer(config)) {

            // Define the full path and name of a log file on your local disk
            String logFile = "speech-sdk-log.txt";

            // At any time (while still logging or after logging is stopped) you can dump the
            // recent traces from memory to a file:
            MemoryLogger.dump(logFile);
        
            // Or dump to any object that is derived from java.io.Writer. For example, System.out,
            // which will enable logging to your console windows:
            try {
                MemoryLogger.dump(System.out);
            } catch (IOException ex) {
                ex.printStackTrace();
            }

            // Or dump to a list of strings, and see the results on the console:
            List<String> messages = MemoryLogger.dump();

            for (String message : messages) {
                System.out.print(message);
            }

            // Stop logging to memory
            MemoryLogger.stop();
            
            // Optional - Reset the filters
            // MemoryLogger.setFilters();
        }
    }

    // Ingest self-defined trace into Speech SDK trace and log the mixed trace to memory buffer such that 
    // the self-defined trace can be used as markers to facliitate the investigation of the issue
    public static void selfDefinedSpxTraceLogging() throws InterruptedException, ExecutionException, URISyntaxException {
        // Set the level of logging to be captured in memory and start logging
        MemoryLogger.setLevel(Level.Info);
        MemoryLogger.start();

        SpxTrace.SPX_TRACE_INFO("### This is my trace info -- START ###");

        // Do your Speech SDK calls here... for example:
        try (SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");
             SpeechRecognizer recognizer = new SpeechRecognizer(config)) {

            // Define the full path and name of a log file on your local disk
            String logFile = "speech-sdk-log.txt";

            SpxTrace.SPX_TRACE_INFO("### This is my trace info -- STOP ###");

            // At any time (while still logging or after logging is stopped) you can dump the
            // recent traces from memory to a file:
            MemoryLogger.dump(logFile);
        
            // Or dump to any object that is derived from java.io.Writer. For example, System.out,
            // which will enable logging to your console windows:
            try {
                MemoryLogger.dump(System.out);
            } catch (IOException ex) {
                ex.printStackTrace();
            }

            // Or dump to a list of strings, and see the results on the console:
            List<String> messages = MemoryLogger.dump();

            for (String message : messages) {
                System.out.print(message);
            }

            // Stop logging to memory
            MemoryLogger.stop();
        }
    }
}

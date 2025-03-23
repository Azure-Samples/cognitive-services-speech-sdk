//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Diagnostics.Logging;


namespace MicrosoftSpeechSDKSamples
{
    // Shows how to enabled Speech SDK trace logging to a file. Microsoft may ask you to collect logs
    // in order to investigate an issue you reported.
    public class SpeechDiagnosticsLoggingSamples
    {
        public static void FileLoggerWithoutFilter()
        {
            // Define the full path and name of the log file on your local disk
            string logFile = "speech-sdk-log.txt";

            // Start logging to the given file. By default it will create
            // a new file, but you have the option of appending to an existing one
            FileLogger.Start(logFile);

            // Do your Speech SDK calls here... for example:
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            SpeechRecognizer recognizer = new SpeechRecognizer(config);

            // Stop logging
            FileLogger.Stop();

            // Now look at the log file that was created.. 
        }

        // Shows how to enabled Speech SDK trace logging to a file, after applying a filter that results in
        // only a subset of the logs caputured. Microsoft may ask you to collect logs
        // in order to investigate an issue you reported, and they will provide the a filter if relevant.
        public static void FileLoggerWithFilter()
        {
            // Define the full path and name of the log file
            string logFile = "speech-sdk-log.txt";

            // Start logging to the given file, but Log only traces
            // that contain either one of the filter strings provided.
            // By default it will create a new file, but you have the option of
            // appending to an existing one.
            string[] filters = { "YourFirstString", "YourSecondString" };
            FileLogger.SetFilters(filters);
            FileLogger.Start(logFile);

            // Do your Speech SDK calls here... for example:
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            SpeechRecognizer recognizer = new SpeechRecognizer(config);

            // Stop logging
            FileLogger.Stop();

            // Clear filters
            FileLogger.SetFilters();

            // Now look at the log file that was created.. 
        }

        // These variables and method are used by the EvenLogger tests below. The method
        // will be invoked on every new log message.
        private static readonly object lockObject = new object();
        private static List<string> eventMessages = new List<string>();
        private static void OnMessageEvent(object sender, string message)
        {
            // Copy the string somewhere under lock to save it and return immediately. Do not do any
            // processing on network calls on the thread that invoked this method.
            lock (lockObject)
            {
                // Here we add it to a list of strings to be looked at later or processed
                // by another thread
                eventMessages.Add(message);
            }
        }

        // Shows how to enabled Speech SDK trace logging events. Your application subscribes to this event
        // to get notified whenever there is a new log string. Microsoft may ask you to collect logs
        // in order to investigate an issue you reported.
        public static void EventLoggerWithoutFilter()
        {
            // Clears the list that will hold log messages
            eventMessages.Clear();

            // Subscribe an event that will get invoked by Speech SDK on every new log message
            EventLogger.OnMessage += OnMessageEvent;

            try
            {
                // Do your Speech SDK calls here... for example:
                // Creates an instance of a speech config with specified endpoint and subscription key.
                // Replace with your own endpoint and subscription key.
                var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

                SpeechRecognizer recognizer = new SpeechRecognizer(config);
            }
            finally
            {
                // Unsubscribe to stop getting events
                EventLogger.OnMessage -= OnMessageEvent;
            }

            // See resulting logs on the console
            Console.WriteLine("Here are the logs we captured:");
            foreach (string message in eventMessages)
            {
                Console.Write(message);
            }
        }

        // Shows how to enabled Speech SDK trace logging events, while applying a filter to the logs.
        // Your application subscribes to this even to get notified whenever there is a new log string.
        // Microsoft may ask you to collect logs in order to investigate an issue you reported, and may
        // provide a filter for you to use.
        public static void EventLoggerWithFilter()
        {
            // Set up your filters, such that the event will only fire for 
            // log traces that contain either one of the filter strings provided.
            string[] filters = { "YourFirstString", "YourSecondString" };
            EventLogger.SetFilters(filters);

            // Clears the list that will hold the filtered log messages
            eventMessages.Clear();

            // Subscribe an event that will get invoked by Speech SDK on every new log message
            EventLogger.OnMessage += OnMessageEvent;

            try
            {
                // Creates an instance of a speech config with specified endpoint and subscription key.
                // Replace with your own endpoint and subscription key.
                var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

                SpeechRecognizer recognizer = new SpeechRecognizer(config);
            }
            finally
            {
                // Unsubscribe to stop getting events
                EventLogger.OnMessage -= OnMessageEvent;

                // Clear filters
                EventLogger.SetFilters();
            }

            // See resulting logs on the console
            Console.WriteLine("Here are the logs we captured:");
            foreach (string message in eventMessages)
            {
                Console.Write(message);
            }
        }

        // Shows how to enable Speech SDK trace logging to memory. Memory logger keeps logging into a fixed-size
        // memory buffer (a "ring" buffer). At any pointer in time, when an issue occurs, the application
        // can dump the whole content of the memory buffer in one of several formats. For example, an
        // application using Speech SDK for speech recognition, may want to dump the content of the
        // logging memory buffer to file for one or more unexpected CancellationErrorCode values received
        // (instead of a recognition result).
        // Microsoft may ask you to collect logs using MemoryLogger in order to investigate an issue you reported,
        // and will guide you on when to do the dump.
        public static void MemoryLoggerWithOrWithoutFilter()
        {
            // Optional - Apply a filter, such that only traces that contain either one of the
            // filter strings will be logged. Microsoft will provide the filter when relevant.
            /*
            string[] filters = { "YourFirstString", "YourSecondString" };
            MemoryLogger.SetFilters(filters);
            */

            // Turn on logging to memory
            MemoryLogger.Start();

            // Do your Speech SDK calls here... for example:
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            SpeechRecognizer recognizer = new SpeechRecognizer(config);

            // Define the full path and name of a log file on your local disk
            string logFile = "speech-sdk-log.txt";

            // At any time (while still logging or after logging is stopped) you can dump the
            // recent traces from memory to a file:
            MemoryLogger.Dump(logFile);

            // Or dump to any object that is derived from System.IO.TextWriter. For example, System.Console.Out,
            // which will enable logging to your console windows:
            MemoryLogger.Dump(System.Console.Out);

            // Or dump to a vector of strings, and see the results on the console:
            List<string> messages = MemoryLogger.Dump().ToList<string>();

            Console.WriteLine("Here are the logs we captured:");
            foreach (string message in messages)
            {
                Console.Write(message);
            }

            // Stop logging to memory
            MemoryLogger.Stop();

            // Clear filters if previously set
            MemoryLogger.SetFilters();
        }
    }
}

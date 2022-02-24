//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "stdafx.h"
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Diagnostics::Logging;

// Shows how to enabled Speech SDK trace logging to a file. Microsoft may ask you to collect logs
// in order to investigate an issue you reported.
void DiagnosticsLoggingFileLoggerWithoutFilter()
{
    // Define the name of a log file on your local disk
    std::string logFile = "speech-sdk-log.txt";

    // Start logging to the given file. By default it will create
    // a new file, but you have the option of appending to an existing one
    FileLogger::Start(logFile);

    // Do your Speech SDK calls... for example:
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    auto recognizer = SpeechRecognizer::FromConfig(config);

    // Stop logging
    FileLogger::Stop();

    // Now look at the log file that was created.. 
}

// Shows how to enabled Speech SDK trace logging to a file, while applying a filter. Microsoft may ask you to collect logs
// in order to investigate an issue you reported. The suggested filter will be provided by Microsoft.
void DiagnosticsLoggingFileLoggerWithFilter()
{
    // Define the name of a log file on your local disk
    std::string logFile = "speech-sdk-log.txt";

    // Start logging to the given file, but Log only traces
    // that contain either one of the filter strings provided.
    // By default it will create a new file, but you have the option of
    // appending to an existing one.
    std::initializer_list<std::string> filters = { "YourFirstString", "YourSecondString" };
    FileLogger::SetFilters(filters);
    FileLogger::Start(logFile);

    // Do your Speech SDK calls... for example:
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    auto recognizer = SpeechRecognizer::FromConfig(config);

    // Stop logging and clear filters
    FileLogger::Stop();

    // Clear filters
    FileLogger::SetFilters();

    // Now look at the log file that was created.. 
}

// Shows how to enabled Speech SDK trace logging events, while applying a filter. Microsoft may ask you to collect logs
// in order to investigate an issue you reported. The suggested filter will be provided by Microsoft.
void DiagnosticsLoggingEventLoggerWithoutFilter()
{
    std::mutex mtx;
    std::vector<std::string> messages;

    // Register a callback that will get invoked by Speech SDK on every new log message
    EventLogger::SetCallback([&messages, &mtx](std::string message) {
            // Copy the string and store it for further processing. Do not do any processing in the event thread!
            std::unique_lock<std::mutex> lock(mtx);
            messages.push_back(message);
        });

    // Do your Speech SDK calls... for example:
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    auto recognizer = SpeechRecognizer::FromConfig(config);

    // Stop logging by setting an empty callback
    EventLogger::SetCallback();

    // See resulting logs on the console
    std::cout << "Here are the logs we captured:\n";
    for (std::string message : messages)
    {
        std::cout << message;
    }
}

// Shows how to enabled Speech SDK trace logging events. Microsoft may ask you to collect logs
// in order to investigate an issue you reported
void DiagnosticsLoggingEventLoggerWithFilter()
{
    std::mutex mtx;
    std::vector<std::string> messages;

    // Register a callback that will get invoked by Speech SDK on every new log message
    EventLogger::SetCallback([&messages, &mtx](std::string message) {
            // Copy the string and store it for further processing. Do not do any processing in the event thread!
            std::unique_lock<std::mutex> lock(mtx);
            messages.push_back(message);
        });

    // Set an event filter, such that the callback will be invoked only for traces
    // that contain either one of the filter strings provided.
    std::initializer_list<std::string> filters = { "YourFirstString", "YourSecondString" };
    EventLogger::SetFilters(filters);

    // Do your Speech SDK calls... for example:
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    auto recognizer = SpeechRecognizer::FromConfig(config);

    // Stop logging and clear filters by setting an empty callback
    EventLogger::SetCallback();

    // Clear filters
    EventLogger::SetFilters();

    // See resulting logs on the console
    std::cout << "Here are the logs we captured:\n";
    for (std::string message : messages)
    {
        std::cout << message;
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
void DiagnosticsLoggingMemoryLogger()
{
    // Optional - Apply a filter, such that only traces that contain either one of the
    // filter strings will be logged
    /*
    std::initializer_list<std::string> filters = { "YourFirstString", "YourSecondString" };
    MemoryLogger::SetFilters(filters);
    */

    // Turn on logging to memory
    MemoryLogger::Start();

    // Do your Speech SDK calls... for example:
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    auto recognizer = SpeechRecognizer::FromConfig(config);

    // Define the full path and name of a log file on your local disk
    std::string logFile = "speech-sdk-log.txt";

    // At any time (while still logging or after logging is stopped) you can dump the traces
    // in memory to a file:
    MemoryLogger::Dump(logFile);

    // Or to any stream object that is derived from std::ostream. For example, std::cout.
    // Using std::cout will enable logging to the standard output stream (your console Windows):
    MemoryLogger::Dump(std::cout);

    // Or to a vector of strings, and see the results on the console:
    std::vector<std::string> messages = MemoryLogger::Dump();

    std::cout << "Here are the logs we captured:\n";
    for (std::string message : messages)
    {
        std::cout << message;
    }

    // Stop logging to memory
    MemoryLogger::Stop();

    // Clear filters if previously set
    MemoryLogger::SetFilters();
}

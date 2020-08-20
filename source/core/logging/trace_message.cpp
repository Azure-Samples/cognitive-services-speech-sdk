//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// trace_message.cpp: SpxTraceMessage() implementation definition
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "trace_message.h"
#include "speechapi_c_diagnostics.h"
#include "file_utils.h"
#include "file_logger.h"
#include "event_logger.h"
#include "exception.h"
#include "memory_logger.h"
#include "string_utils.h"
#include <chrono>
#include <stdio.h>
#include <sstream>
#include <iostream>

// Note: in case of android, log to logcat
#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#endif

#define SPX_CONFIG_INCLUDE_TRACE_THREAD_ID      1
#define SPX_CONFIG_INCLUDE_TRACE_HIRES_CLOCK    1
// #define SPX_CONFIG_INCLUDE_TRACE_WINDOWS_DEBUGGER   1

#ifdef SPX_CONFIG_INCLUDE_TRACE_WINDOWS_DEBUGGER
#include <windows.h>
#endif // SPX_CONFIG_INCLUDE_TRACE_WINDOWS_DEBUGGER

decltype(std::chrono::high_resolution_clock::now()) __g_spx_trace_message_time0 = std::chrono::high_resolution_clock::now();

SPX_EXTERN_C void SpxTraceMessage1(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...)
{
    UNUSED(level);
    try
    {
        va_list argptr;
        va_start(argptr, pszFormat);
        SpxTraceMessage2(level, pszTitle, fileName, lineNumber, pszFormat, argptr);
        va_end(argptr);
    }
    catch(...)
    {
    }
}

SPX_EXTERN_C void SpxTraceMessage2(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr)
{
    bool logToConsole = false;
#if defined(DEBUG) || defined(_DEBUG)
    logToConsole = true;
#endif

    bool logToFile = FileLogger::Instance().IsFileLoggingEnabled();
    bool logToEvents = EventLogger::Instance().IsLoggingEnabled();
    bool logToMemory = MemoryLogger::Instance().IsLoggingEnabled();

    if (!logToConsole && !logToFile && !logToEvents && !logToMemory)
    {
        return;
    }

    char sz[4096];
    SpxFormatMessage(sz, 4096, level, pszTitle, fileName, lineNumber, pszFormat, argptr);

#ifdef SPX_CONFIG_INCLUDE_TRACE_WINDOWS_DEBUGGER
    OutputDebugStringA(sz);
#endif // SPX_CONFIG_INCLUDE_TRACE_WINDOWS_DEBUGGER

#if defined(ANDROID) || defined(__ANDROID__)
    int androidPrio = ANDROID_LOG_ERROR;
    switch (level)
    {
    case __SPX_TRACE_LEVEL_INFO:    androidPrio = ANDROID_LOG_INFO;     break; // Trace_Info
    case __SPX_TRACE_LEVEL_WARNING: androidPrio = ANDROID_LOG_WARN;     break; // Trace_Warning
    case __SPX_TRACE_LEVEL_ERROR:   androidPrio = ANDROID_LOG_ERROR;    break; // Trace_Error
    case __SPX_TRACE_LEVEL_VERBOSE: androidPrio = ANDROID_LOG_VERBOSE;  break; // Trace_Verbose
    default: androidPrio = ANDROID_LOG_FATAL; break;
    }

    if (logToConsole)
    {
        __android_log_write(androidPrio, "SpeechSDK", sz);
    }
    if (logToFile)
    {
        FileLogger::Instance().LogToFile(std::move(sz));
    }
#else
    if (logToConsole)
    {
        fprintf(stderr, "%s", sz);
    }
    if (logToFile)
    {
        FileLogger::Instance().LogToFile(sz);
    }
    if (logToEvents)
    {
        EventLogger::Instance().LogToEvent(sz);
    }
    if (logToMemory)
    {
        MemoryLogger::Instance().LogToMemory(sz);
    }
#endif
}

SPX_EXTERN_C void SpxFormatMessage(char *buffer, size_t bufferSize, int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr)
{
    UNUSED(level);

    std::string format;
    if (SPX_CONFIG_INCLUDE_TRACE_THREAD_ID)
    {
        auto threadHash = std::hash<std::thread::id>{}(std::this_thread::get_id());
        format += "(" + std::to_string(threadHash % 1000000) + "): ";
    }

    if (SPX_CONFIG_INCLUDE_TRACE_HIRES_CLOCK)
    {
        auto now = std::chrono::high_resolution_clock::now();
        unsigned long delta = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(now - __g_spx_trace_message_time0).count();
        format += std::to_string(delta) + "ms ";
    }

    while (*pszFormat == '\n' || *pszFormat == '\r')
    {
        if (*pszFormat == '\r')
        {
            pszTitle = nullptr;
        }

        format += *pszFormat++;
    }

    if (pszTitle != nullptr)
    {
        format += pszTitle;
    }

    std::string fileNameOnly(fileName);
    std::replace(fileNameOnly.begin(), fileNameOnly.end(), '\\', '/');

    std::ostringstream fileNameLineNumber;
    fileNameLineNumber << " " << fileNameOnly.substr(fileNameOnly.find_last_of('/', std::string::npos) + 1) << ":" << lineNumber << " ";

    format += fileNameLineNumber.str();

    format += pszFormat;
    if (format.length() < 1 || format[format.length() - 1] != '\n')
    {
        format += "\n";
    }

    vsnprintf(buffer, bufferSize, format.c_str(), argptr);

    static constexpr char prefix[] = "Microsoft::CognitiveServices::Speech::Impl::";
    static constexpr auto prefixLength = sizeof(prefix) - 1;
    auto start = buffer;
    while (true)
    {
        auto found = strstr(start, prefix);
        if (!found) break;
        strcpy(found, &found[prefixLength]);
        start = found;
    }
}

SPX_EXTERN_C void _xlogging_log_function_spx_trace_message_wrapper(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...)
{
    UNUSED(func);
    UNUSED(options);

    va_list args;
    va_start(args, format);

    switch (log_category)
    {
    case AZ_LOG_INFO:
        SpxTraceMessage2(__SPX_TRACE_LEVEL_INFO, "SPX_TRACE_INFO: AZ_LOG_INFO: ", file, line, format, args);
        break;

    case AZ_LOG_ERROR:
        SpxTraceMessage2(__SPX_TRACE_LEVEL_INFO, "SPX_TRACE_ERROR: AZ_LOG_ERROR: ", file, line, format, args);
        SPX_TRACE_ERROR("Error: File:%s Func:%s Line:%d ", file, func, line);
        break;

    default:
        break;
    }

    va_end(args);
}

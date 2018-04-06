//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>
#include <fstream>
#include <random>
#include <signal.h>
#include "string_utils.h"
#include <iostream>
#include <functional>
#include <signal.h>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <WinBase.h>
#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include "DbgHelp.h"
#pragma warning(pop)
#undef min
#else
#include <execinfo.h>
#include <cxxabi.h>
#endif


inline bool exists(const std::wstring& name) {
    return std::ifstream(PAL::ToString(name).c_str()).good();
}

inline std::ifstream get_stream(const std::wstring& name) {
    return std::ifstream(PAL::ToString(name).c_str(), std::ifstream::binary);
}

typedef std::linear_congruential_engine<uint_fast32_t, 1664525, 1013904223, UINT_FAST32_MAX> random_engine;


// What follows was lifted from CNTK (see ExceptionWithCallStack.cpp). Long live CNTK!

#ifdef _WIN32
inline std::string FormatWin32Error(DWORD error)
{
    wchar_t buf[1024] = { 0 };
    ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, "", error, 0, buf, sizeof(buf) / sizeof(*buf) - 1, NULL);
    std::wstring res(buf);
    // eliminate newlines (and spaces) from the end
    size_t last = res.find_last_not_of(L" \t\r\n");
    if (last != std::string::npos)
        res.erase(last + 1, res.length());
    return PAL::ToString(res);
}
#endif

// make the unmangled name a bit more readable
// Insert spaces around the main function name for better visual parsability; and double-spaces between function arguments.
// This uses some heuristics for C++ names that may be fragile, but that's OK since this only adds/removes spaces.
inline std::string MakeFunctionNameStandOut(std::string origName)
{
    try // guard against exception, since this is used for exception reporting
    {
        auto name = origName;
        // strip off modifiers for parsing (will be put back at the end)
        std::string modifiers;
        auto pos = name.find_last_not_of(" abcdefghijklmnopqrstuvwxyz");
        if (pos != std::string::npos)
        {
            modifiers = name.substr(pos + 1);
            name = name.substr(0, pos + 1);
        }
        bool hasArgList = !name.empty() && name.back() == ')';
        size_t angleDepth = 0;
        size_t parenDepth = 0;
        bool hitEnd = !hasArgList; // hit end of function name already?
        bool hitStart = false;
        // we parse the function name from the end; escape nested <> and ()
        // We look for the end and start of the function name itself (without namespace qualifiers),
        // and for commas separating function arguments.
        for (size_t i = name.size(); i--> 0;)
        {
            // account for nested <> and ()
            if (name[i] == '>')
                angleDepth++;
            else if (name[i] == '<')
                angleDepth--;
            else if (name[i] == ')')
                parenDepth++;
            else if (name[i] == '(')
                parenDepth--;
            // space before '>'
            if (name[i] == ' ' && i + 1 < name.size() && name[i + 1] == '>')
                name.erase(i, 1); // remove
                                  // commas
            if (name[i] == ',')
            {
                if (i + 1 < name.size() && name[i + 1] == ' ')
                    name.erase(i + 1, 1);  // remove spaces after comma
                if (!hitEnd && angleDepth == 0 && parenDepth == 1)
                    name.insert(i + 1, "  "); // except for top-level arguments, we separate them by 2 spaces for better readability
            }
            // function name
            if ((name[i] == '(' || name[i] == '<') &&
                parenDepth == 0 && angleDepth == 0 &&
                (i == 0 || name[i - 1] != '>') &&
                !hitEnd && !hitStart) // we hit the start of the argument list
            {
                hitEnd = true;
                name.insert(i, "  ");
            }
            else if ((name[i] == ' ' || name[i] == ':' || name[i] == '>') && hitEnd && !hitStart && i > 0) // we hit the start of the function name
            {
                if (name[i] != ' ')
                    name.insert(i + 1, " ");
                name.insert(i + 1, " "); // in total insert 2 spaces
                hitStart = true;
            }
        }
        return name + modifiers;
    }
    catch (...)
    {
        return origName;
    }
}

/// <summary>This function collects the stack tracke and writes it through the provided write function
/// <param name="write">Function for writing the text associated to a the callstack</param>
/// </summary>
inline void CollectCallStack(size_t skipLevels, bool makeFunctionNamesStandOut, const std::function<void(std::string)>& write)
{
    static const int MAX_CALLERS = 62;
    static const unsigned short MAX_CALL_STACK_DEPTH = 20;

    write("\n[CALL STACK]\n");

#ifdef _WIN32


    // RtlCaptureStackBackTrace() is a kernel API without default binding, we must manually determine its function pointer.
    typedef USHORT(WINAPI * CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
    CaptureStackBackTraceType RtlCaptureStackBackTrace = (CaptureStackBackTraceType)(GetProcAddress(LoadLibrary("kernel32.dll"), "RtlCaptureStackBackTrace"));
    if (RtlCaptureStackBackTrace == nullptr) // failed somehow
        return write("Failed to generate CALL STACK. GetProcAddress(\"RtlCaptureStackBackTrace\") failed with error " + FormatWin32Error(GetLastError()) + "\n");

    HANDLE process = GetCurrentProcess();
    if (!SymInitialize(process, nullptr, TRUE))
        return write("Failed to generate CALL STACK. SymInitialize() failed with error " + FormatWin32Error(GetLastError()) + "\n");

    // get the call stack
    void* callStack[MAX_CALLERS];
    unsigned short frames;
    frames = RtlCaptureStackBackTrace(0, MAX_CALLERS, callStack, nullptr);

    SYMBOL_INFO* symbolInfo = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1); // this is a variable-length structure, can't use vector easily
    symbolInfo->MaxNameLen = 255;
    symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    frames = std::min(frames, MAX_CALL_STACK_DEPTH);

    // format and emit
    size_t firstFrame = skipLevels + 1; // skip CollectCallStack()
    for (size_t i = firstFrame; i < frames; i++)
    {
        std::string callStackLine;
        if (i == firstFrame)
            callStackLine = "    > ";
        else
            callStackLine = "    - ";

        if (SymFromAddr(process, (DWORD64)(callStack[i]), 0, symbolInfo))
        {
            callStackLine += makeFunctionNamesStandOut ? MakeFunctionNameStandOut(symbolInfo->Name) : symbolInfo->Name;
            write(callStackLine + "\n");
        }
        else
        {
            DWORD error = GetLastError();
            char buf[17];
            sprintf_s(buf, "%p", callStack[i]);
            callStackLine += buf;
            write(callStackLine + " (SymFromAddr() error: " + FormatWin32Error(error) + ")\n");
        }
    }

    write("\n");

    free(symbolInfo);

    SymCleanup(process);

#else // Linux

    const unsigned int MAX_NUM_FRAMES = 1024;
    void* backtraceAddresses[MAX_NUM_FRAMES];
    unsigned int numFrames = backtrace(backtraceAddresses, MAX_NUM_FRAMES);
    char** symbolList = backtrace_symbols(backtraceAddresses, numFrames);

    for (size_t i = skipLevels; i < numFrames; i++)
    {
        char* beginName = NULL;
        char* beginOffset = NULL;
        char* beginAddress = NULL;

        // Find parentheses and +address offset surrounding the mangled name
        for (char* p = symbolList[i]; *p; ++p)
        {
            if (*p == '(')      // function name begins here
                beginName = p;
            else if (*p == '+') // relative address ofset
                beginOffset = p;
            else if ((*p == ')') && (beginOffset || beginName)) // absolute address
                beginAddress = p;
        }
        const int buf_size = 1024;
        char buffer[buf_size];

        if (beginName && beginAddress && (beginName < beginAddress))
        {
            *beginName++ = '\0';
            *beginAddress++ = '\0';
            if (beginOffset) // has relative address
                *beginOffset++ = '\0';

            // Mangled name is now in [beginName, beginOffset) and caller offset in [beginOffset, beginAddress).
            int status = 0;
            const unsigned int MAX_FUNCNAME_SIZE = 4096;
            size_t funcNameSize = MAX_FUNCNAME_SIZE;
            char funcName[MAX_FUNCNAME_SIZE]; // working buffer
            const char* ret = abi::__cxa_demangle(beginName, funcName, &funcNameSize, &status);
            std::string fName;
            if (status == 0)
                fName = makeFunctionNamesStandOut ? MakeFunctionNameStandOut(ret) : ret; // make it a bit more readable
            else
                fName = beginName; // failed: fall back

                                   // name of source file--not printing since it is not super-useful
                                   //string sourceFile = symbolList[i];
                                   //static const size_t sourceFileWidth = 20;
                                   //if (sourceFile.size() > sourceFileWidth)
                                   //    sourceFile = "..." + sourceFile.substr(sourceFile.size() - (sourceFileWidth-3));
            while (*beginAddress == ' ') // eat unnecessary space
                beginAddress++;
            std::string pcOffset = beginOffset ? std::string(" + ") + beginOffset : std::string();
            snprintf(buffer, buf_size, "%-20s%-50s%s\n", beginAddress, fName.c_str(), pcOffset.c_str());
        }
        else // Couldn't parse the line. Print the whole line as it came.
            snprintf(buffer, buf_size, "%s\n", symbolList[i]);

        write(buffer);
    }

    free(symbolList);

#endif
}

inline void signal_handler()
{
    CollectCallStack(1/*skip this function*/, false, [](std::string stack)
    {
        std::cerr << stack;
    });

    exit(1);
}

inline void signal_handler(int sig) {
    std::cerr << std::endl << "Received an error signal: " << sig << std::endl;

    signal_handler();
}

inline void add_signal_handlers()
{
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);

    std::set_terminate(signal_handler);
}
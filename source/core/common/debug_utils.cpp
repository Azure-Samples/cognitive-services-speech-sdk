//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_helpers.cpp: Implementation definitions for *Http* helper methods
//

#include "stdafx.h"
#include "debug_utils.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <functional>
#include "string_utils.h"
#include <algorithm>
#include <iostream>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <WinBase.h>
#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include "DbgHelp.h"
#pragma warning(pop)
#undef min
#elif defined(ANDROID) || defined(__ANDROID__) // execinfo.h not available on Android.
#include <iostream>
#include <iomanip>
#include <unwind.h>
#include <dlfcn.h>

struct AndroidStackFrame
{
    void** current;
    void** end;
    };

static _Unwind_Reason_Code AndroidUnwindCallback(struct _Unwind_Context* context, void* arg)
{
    AndroidStackFrame* state = static_cast<AndroidStackFrame*>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc) {
        if (state->current == state->end) {
            return _URC_END_OF_STACK;
        }
        else {
            *state->current++ = reinterpret_cast<void*>(pc);
        }
    }
    return _URC_NO_REASON;
}

#else
#include <execinfo.h>
#include <cxxabi.h>
#endif


namespace Debug {

// What follows was lifted from CNTK (see ExceptionWithCallStack.cpp).
#ifdef _WIN32
static std::string FormatWin32Error(DWORD error)
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

#if !defined(ANDROID) && !defined(__ANDROID__)
// make the unmangled name a bit more readable
// Insert spaces around the main function name for better visual parsability; and double-spaces between function arguments.
// This uses some heuristics for C++ names that may be fragile, but that's OK since this only adds/removes spaces.
static std::string MakeFunctionNameStandOut(std::string origName)
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
#endif

/// <summary>This function collects the stack tracke and writes it through the provided write function
/// <param name="write">Function for writing the text associated to a the callstack</param>
/// </summary>
static void CollectCallStack(size_t skipLevels, bool makeFunctionNamesStandOut, const std::function<void(std::string)>& write)
{
    write("\n[CALL STACK]\n");

#ifdef _WIN32
    static const int MAX_CALLERS = 62;
    static const unsigned short MAX_CALL_STACK_DEPTH = 20;

    // RtlCaptureStackBackTrace() is a kernel API without default binding, we must manually determine its function pointer.
    typedef USHORT(WINAPI * CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
    auto kernelLib = LoadLibrary("kernel32.dll");
    CaptureStackBackTraceType RtlCaptureStackBackTrace = nullptr;
    if (kernelLib != NULL) {
        RtlCaptureStackBackTrace = (CaptureStackBackTraceType)(GetProcAddress(kernelLib, "RtlCaptureStackBackTrace"));
    }
    if (RtlCaptureStackBackTrace == nullptr) // failed somehow
    {
        return write("Failed to generate CALL STACK. GetProcAddress(\"RtlCaptureStackBackTrace\") failed with error " + FormatWin32Error(GetLastError()) + "\n");
    }

    HANDLE process = GetCurrentProcess();
    if (!SymInitialize(process, nullptr, TRUE))
    {
        return write("Failed to generate CALL STACK. SymInitialize() failed with error " + FormatWin32Error(GetLastError()) + "\n");
    }

    // get the call stack
    void* callStack[MAX_CALLERS];
    unsigned short frames;
    frames = RtlCaptureStackBackTrace(0, MAX_CALLERS, callStack, nullptr);

    SYMBOL_INFO* symbolInfo = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1); // this is a variable-length structure, can't use vector easily
    if (symbolInfo == nullptr) // failed somehow 
    {
        return write("Failed to allocated SYMBOL_INFO struct.\n");
    }

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

#elif !defined(ANDROID) && !defined(__ANDROID__)

    const unsigned int MAX_NUM_FRAMES = 1024;
    void* backtraceAddresses[MAX_NUM_FRAMES];
    unsigned int numFrames = backtrace(backtraceAddresses, MAX_NUM_FRAMES);
    char** symbolList = backtrace_symbols(backtraceAddresses, numFrames);

    for (size_t i = skipLevels; i < numFrames; i++)
    {
        // Find parentheses and +address offset surrounding the mangled name

        std::string current(symbolList[i]);

        auto beginName = current.find('(');
        auto beginOffset = current.find('+');
        auto endOffset = current.find(')');

        std::ostringstream buffer;

        if (beginName != std::string::npos && beginOffset != std::string::npos && endOffset != std::string::npos && (beginName < beginOffset))
        {
            auto mangled_name = current.substr(beginName + 1, beginOffset - beginName - 1);
            auto offset = current.substr(beginOffset + 1, endOffset - beginOffset - 1);

            // Mangled name is now in [beginName, beginOffset) and caller offset in [beginOffset, beginAddress).
            int status = 0;
            char* ret = abi::__cxa_demangle(mangled_name.c_str(), NULL, NULL, &status);
            std::string fName(mangled_name);
            if (status == 0)
                fName = makeFunctionNamesStandOut ? MakeFunctionNameStandOut(ret) : ret; // make it a bit more readable
            
            free(ret);

            buffer << std::setw(20) << std::left << current.substr(endOffset + 1)
                << std::setw(50) << std::left << fName
                << "+" << offset;
        }
        else // Couldn't parse the line. Print the whole line as it came.
            buffer << current;

        write(buffer.str());
    }

    free(symbolList);
#else
    UNUSED(skipLevels);
    UNUSED(makeFunctionNamesStandOut);

    void *androidStackFrames[32];
    AndroidStackFrame state = { &androidStackFrames[0], &androidStackFrames [31] };
    _Unwind_Backtrace(AndroidUnwindCallback, &state);

    std::ostringstream os;
    size_t numStackFramesValid = state.current - androidStackFrames;

    for (size_t idx = skipLevels; idx < numStackFramesValid; ++idx)
    {
        const void* addr = androidStackFrames[idx];
        const char* symbol = "???";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname)
        {
            symbol = info.dli_sname;
        }

        os << "  #" << std::setw(2) << (idx - skipLevels) << ": " << addr << "  " << symbol << "\n";
    }
    write(os.str().c_str());
#endif
}


std::string GetCallStack(size_t skipLevels/* = 0*/, bool makeFunctionNamesStandOut/* = false*/) 
{
    try
    {
        std::ostringstream buffer;
        CollectCallStack(skipLevels + 1/*skip this function*/, makeFunctionNamesStandOut, [&](std::string stack)
        {
            buffer << stack << std::endl;
        });
        return  buffer.str();
    }
    catch (...) // since we run as part of error reporting, don't get hung up on our own error
    {
        return std::string();
    }
}

static void ErrorHandler(size_t skipLevels)
{
    auto callstack = Debug::GetCallStack(skipLevels + 1, false);

    SPX_TRACE_VERBOSE(callstack.c_str());

    exit(1);
}

#ifdef _WIN32
LONG WINAPI WinSignalHandler(EXCEPTION_POINTERS * /*ExceptionInfo*/)
{
    ErrorHandler(1);
#ifdef _DEBUG
    return EXCEPTION_CONTINUE_SEARCH;
#endif
}
#endif

static void SignalHandler(int sig) {
    SPX_TRACE_VERBOSE("\nReceived an error signal: %d\n", sig);
    ErrorHandler(2);
}


void HookSignalHandlers()
{

#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG);
    SetUnhandledExceptionFilter(WinSignalHandler);
    AddVectoredExceptionHandler(1, WinSignalHandler);
#endif

    signal(SIGSEGV, SignalHandler);
    signal(SIGABRT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    std::set_terminate([] { ErrorHandler(1); });
}

}

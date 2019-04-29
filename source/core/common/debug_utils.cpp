//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#if defined(ANDROID) || defined(__ANDROID__)
#define _ANDROID_LEGACY_SIGNAL_INLINES_H_
#endif

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

/// <summary>This function collects the stack tracke and writes it through the provided write function
/// <param name="write">Function for writing the text associated to a the callstack</param>
/// </summary>
static void CollectCallStack(size_t skipLevels, const std::function<void(std::string)>& write)
{
    write("\n[CALL STACK BEGIN]\n");

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
            callStackLine += symbolInfo->Name;
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
        auto beginOffset = current.find('+', beginName);

        std::ostringstream buffer;

        if (beginName != std::string::npos && beginOffset != std::string::npos && beginName < beginOffset)
        {
            buffer << current.substr(0, beginName + 1);
            auto mangled_name = current.substr(beginName + 1, beginOffset - beginName - 1);
            int status = 0;
            char* ret = abi::__cxa_demangle(mangled_name.c_str(), NULL, NULL, &status);
            if (status == 0)
                buffer << ret;
            else
                buffer << mangled_name;
            free(ret);
            buffer << current.substr(beginOffset);
        }
        else // Nothing to demangle. Print the whole line as it came.
            buffer << current;

        write(buffer.str());
    }

    free(symbolList);
#else
    UNUSED(skipLevels);

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

    write("[CALL STACK END]\n");
}


std::string GetCallStack(size_t skipLevels/* = 0*/)
{
    try
    {
        std::ostringstream buffer;
        CollectCallStack(skipLevels + 1/*skip this function*/, [&](std::string stack)
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

static void SignalHandler()
{
    auto callstack = Debug::GetCallStack(1);

    SPX_TRACE_VERBOSE("%s", callstack.c_str());

    exit(1);
}

// In case of Android, we claim to run with API level 19 and later.
// The problem with this is that some functions were not availble
// on the earlier versions (19-21) while part of the libc in later
// api versions.
// So, to support the old as well as the newer api levels, we provide
// stub functions with "weak" attributes for the linker to link against
// in case no other function is found.
// Please note that this is not a 100% solution since the Android
// linker takes "the first" function with a given name, not the first
// non-weak, only falling back to the weak function if no other has
// been found.
// That means, even if the libc provides the functions below, we could
// still end up with our stubs.
// Second note: signal() is only used for debugging and to hide
// pipe signals in the networking code.
// The tc*etattr() are only used to configure the terminal - which is
// not used in our library.
// The pthread_setclock() is used to select a particular clock used for
// timestamps.
#if defined(ANDROID) || defined(__ANDROID__)
#if __ANDROID_API__ < 21
extern "C" sighandler_t __attribute__((weak)) __attribute__((visibility("default"))) signal(int, void(*)(int))
{
    SPX_TRACE_VERBOSE("signal called on weak reference. probably running on api level 19");
    return (sighandler_t)0;
}

extern "C" void __attribute__((weak)) __attribute__((visibility("default"))) tcsetattr(int, int, void*)
{
    SPX_TRACE_VERBOSE("tcsetattr called on weak reference. probably running on api level 19");
}

extern "C" int __attribute__((weak)) __attribute__((visibility("default"))) tcgetattr(int, void*)
{
    SPX_TRACE_VERBOSE("tcgetattr called on weak reference. probably running on api level 19");
    return -1;
}

extern "C" void __attribute__((weak)) __attribute__((visibility("default"))) pthread_condattr_setclock(void *, int)
{
    SPX_TRACE_VERBOSE("pthread_condattr_setclock called on weak reference. probably running on api level 19");
}
#endif
#endif


static void SignalHandler(int sig) {
    SPX_TRACE_VERBOSE("\nReceived an error signal: %d\n", sig);
    SignalHandler();
}

void HookSignalHandlers()
{
    signal(SIGSEGV, SignalHandler);
    signal(SIGABRT, SignalHandler);

    std::set_terminate(SignalHandler);
}

}

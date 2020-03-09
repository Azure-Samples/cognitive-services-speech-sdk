//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// dllmain.cpp : Defines the entry point for the DLL application.


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TraceLoggingProvider.h>

// forward-declare
TRACELOGGING_DECLARE_PROVIDER(tracingEventProvider);
#endif

#include "stdafx.h"
#include "debug_utils.h"
#include "handle_table.h"
#include "trace_message.h"
#include "create_module_object.h"

SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName);
void InitLogging();

using namespace Microsoft::CognitiveServices::Speech::Impl;

#ifdef _MSC_VER

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    UNUSED(hModule);
    UNUSED(lpReserved);
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            InitLogging();
#ifdef _WIN32
            TraceLoggingRegister(tracingEventProvider);
#endif
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
#ifdef _WIN32
            TraceLoggingUnregister(tracingEventProvider);
#endif
            CSpxSharedPtrHandleTableManager::Term();
            break;
    }
    return TRUE;
}
#else

__attribute__((constructor)) static void LibLoad(int argc, char** argv, char** envp)
{
    UNUSED(argc);
    UNUSED(argv);
    UNUSED(envp);
    InitLogging();
}

__attribute__((destructor)) static void LibUnload()
{
    CSpxSharedPtrHandleTableManager::Term();
}

#endif

SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    return PrimaryCarbon_CreateModuleObject(className, interfaceName);
}

void InitLogging()
{
    xlogging_set_log_function(_xlogging_log_function_spx_trace_message_wrapper);
}

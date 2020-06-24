//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <azure_c_shared_utility_xlogging_wrapper.h>
#include <handle_table.h>

using namespace Microsoft::CognitiveServices::Speech;

static void CommonInit();
static void CommonDeinit();

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
            CommonInit();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            CommonDeinit();
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

    CommonInit();
}

__attribute__((destructor)) static void LibUnload()
{
    CommonDeinit();
}

#endif

SPX_EXTERN_C void _xlogging_log_function_spx_trace_message_wrapper(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...)
{
    UNUSED(func);
    UNUSED(options);

    va_list args;
    va_start(args, format);

    switch (log_category)
    {
        case AZ_LOG_INFO:
            diagnostics_log_trace_message2(__SPX_TRACE_LEVEL_INFO, "SPX_TRACE_INFO: AZ_LOG_INFO: ", file, line, format, args);
            break;

        case AZ_LOG_ERROR:
            diagnostics_log_trace_message2(__SPX_TRACE_LEVEL_ERROR, "SPX_TRACE_ERROR: AZ_LOG_ERROR: ", file, line, format, args);
            break;

        default:
            break;
    }

    va_end(args);
}

static void CommonInit()
{
    xlogging_set_log_function(_xlogging_log_function_spx_trace_message_wrapper);
}

static void CommonDeinit()
{
    Impl::CSpxSharedPtrHandleTableManager::Term();
}

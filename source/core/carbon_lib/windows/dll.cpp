// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "azure_c_shared_utility\xlogging.h"


using namespace CARBON_IMPL_NAMESPACE();


void InitLogging()
{
#ifndef _DEBUG

    LOGGER_LOG tracelog = [](LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...) {
        UNUSED(log_category);
        UNUSED(file);
        UNUSED(func);
        UNUSED(line);
        UNUSED(options);
        UNUSED(format);
    };

    UNUSED(tracelog);

    xlogging_set_log_function(nullptr);

#endif // DEBUG
}

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
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            CSpxSharedPtrHandleTableManager::Term();
            break;
    }
    return TRUE;
}
#else

__attribute__((constructor)) static void LibLoad(int argc, char** argv, char** envp)
{
    InitLogging();
}

__attribute__((destructor)) static void LibUnload()
{
    CSpxSharedPtrHandleTableManager::Term();
}

#endif
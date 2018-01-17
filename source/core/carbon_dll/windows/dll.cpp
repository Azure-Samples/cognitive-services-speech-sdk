// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "azure_c_shared_utility\xlogging.h"


using namespace CARBON_IMPL_NAMESPACE();


void InitLogging()
{
    #ifndef _DEBUG

    LOGGER_LOG tracelog = [](LOG_CATEGORY log_category, unsigned int options, const char* format, ...) {
    };

    xlogging_set_log_function(nullptr);

    #endif // DEBUG
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
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

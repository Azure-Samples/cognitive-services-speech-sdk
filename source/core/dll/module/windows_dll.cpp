// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<CSpxRecognitionResult>>, SPXASYNCHANDLE>()->Term();
            CSpxSharedPtrHandleTableManager::Get<CSpxRecognitionResult, SPXRESULTHANDLE>()->Term();
            CSpxSharedPtrHandleTableManager::Get<CSpxRecognizer, SPXRECOHANDLE>()->Term();
            break;
	}
	return TRUE;
}

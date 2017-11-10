// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
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
        //CSpxHandleTableManager::Term();
        CSpxHandleTableManager::Get<CSpxRecognizer, SPXRECOHANDLE>()->Term();
        CSpxHandleTableManager::Get<CSpxRecognitionResult, SPXRESULTHANDLE>()->Term();
        break;
	}
	return TRUE;
}


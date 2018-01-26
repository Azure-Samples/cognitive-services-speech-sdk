#include "stdafx.h"
#include "event_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {
    

SPXAPI_PRIVATE Recognizer_SessionEvent_SetCallback(ISpxRecognizerEvents::SessionEvent_Type ISpxRecognizerEvents::*psessionEvent, SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto pfn = [=](std::shared_ptr<ISpxSessionEventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSessionEventArgs, SPXEVENTHANDLE>();
            auto hevent = (*eventhandles)[e.get()];
            (*pCallback)(hreco, hevent, pvContext);
        };

        auto pISpxRecognizerEvents = std::dynamic_pointer_cast<ISpxRecognizerEvents>(recognizer).get();
        (pISpxRecognizerEvents->*psessionEvent).Disconnect(pfn);

        if (pCallback != nullptr)
        {
            (pISpxRecognizerEvents->*psessionEvent).Connect(pfn);
        }
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI_PRIVATE Recognizer_RecoEvent_SetCallback(ISpxRecognizerEvents::RecoEvent_Type ISpxRecognizerEvents::*precoEvent, SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto pfn = [=](std::shared_ptr<ISpxRecognitionEventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
            auto hevent = (*eventhandles)[e.get()];
            (*pCallback)(hreco, hevent, pvContext);
        };

        auto pISpxRecognizerEvents = std::dynamic_pointer_cast<ISpxRecognizerEvents>(recognizer).get();
        (pISpxRecognizerEvents->*precoEvent).Disconnect(pfn);

        if (pCallback != nullptr)
        {
            (pISpxRecognizerEvents->*precoEvent).Connect(pfn);
        }
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}


} // CARBON_IMPL_NAMESPACE()

#include "stdafx.h"
#include "event_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {
    

SPXAPI_PRIVATE Recognizer_Event_SetCallback(ISpxRecognizerEvents::RecoEvent_Type ISpxRecognizerEvents::*precoEvent, SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    SPX_INIT_HR(hr);
    
    try
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto pfn = [=](std::shared_ptr<ISpxRecognitionEventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
            auto hevent = (*eventhandles)[e.get()];
            (*pCallback)(hreco, hevent, pvContext);
        };

        auto recoevents = std::dynamic_pointer_cast<ISpxRecognizerEvents>(recognizer).get();
        (recoevents->*precoEvent).Disconnect(pfn);

        if (pCallback != nullptr)
        {
            (recoevents->*precoEvent).Connect(pfn);
        }
    }
    catch (SPXHR hr)
    {
        SPX_RETURN_HR(hr);
    }
    catch (std::exception ex)
    {
        SPX_RETURN_HR(SPXERR_UNHANDLED_EXCEPTION);
    }

    SPX_RETURN_HR(hr);
}


}; // CARBON_IMPL_NAMESPACE()

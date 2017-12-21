#include "stdafx.h"
#include "event_helpers.h"
#include "handle_helpers.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI_(bool) Recognzier_Handle_IsValid(SPXRECOHANDLE hreco)
{
    return Handle_IsValid<SPXRECOHANDLE, ISpxRecognizer>(hreco);
}

SPXAPI Recognizer_Handle_Close(SPXRECOHANDLE hreco)
{
    return Handle_Close<SPXRECOHANDLE, ISpxRecognizer>(hreco);
}

SPXAPI_(bool) Recognizer_AsyncHandle_IsValid(SPXASYNCHANDLE hasync)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<void>>(hasync);
}

SPXAPI Recognizer_AsyncHandle_Close(SPXASYNCHANDLE hasync)
{
    return Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<void>>(hasync);
}

SPXAPI_(bool) Recognizer_ResultHandle_IsValid(SPXRESULTHANDLE hresult)
{
    return Handle_IsValid<SPXRESULTHANDLE, ISpxRecognitionResult>(hresult);
}

SPXAPI Recognizer_ResultHandle_Close(SPXRESULTHANDLE hresult)
{
    return Handle_Close<SPXRESULTHANDLE, ISpxRecognitionResult>(hresult);
}

SPXAPI_(bool) Recognizer_EventHandle_IsValid(SPXEVENTHANDLE hevent)
{
    return Handle_IsValid<SPXEVENTHANDLE, ISpxRecognitionEventArgs>(hevent);
}

SPXAPI Recognizer_EventHandle_Close(SPXEVENTHANDLE hevent)
{
    return Handle_IsValid<SPXEVENTHANDLE, ISpxRecognitionEventArgs>(hevent);
}

SPXAPI Recognizer_Enable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto precognizer = (*recohandles)[hreco];
        precognizer->Enable();
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_Disable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto precognizer = (*recohandles)[hreco];
        precognizer->Disable();
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_IsEnabled(SPXRECOHANDLE hreco, bool* pfEnabled)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto precognizer = (*recohandles)[hreco];
        *pfEnabled = precognizer->IsEnabled();
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_SetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* value)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_GetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, wchar_t* value, uint32_t cchValue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_SetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t value)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_GetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t* pvalue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_Recognize(SPXRECOHANDLE hreco, SPXRESULTHANDLE* phresult)
{
    SPX_INIT_HR(hr);
    *phresult = SPXHANDLE_INVALID;

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_RecognizeAsync(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_RecognizeAsync_WaitFor(hasync, UINT32_MAX, phresult));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI Recognizer_RecognizeAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPXAPI_INIT_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto precognizer = (*recohandles)[hreco];

        auto asyncop = precognizer->RecognizeAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_RecognizeAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult)
{
    SPXAPI_INIT_TRY(hr)
    {
        *phresult = SPXHANDLE_INVALID;

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        hr = SPXERR_TIMEOUT;
        auto completed = asyncop->WaitFor(milliseconds);

        if (completed)
        {
            auto result = asyncop->Future.get();
            auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
            *phresult = resulthandles->TrackHandle(result);
            hr = SPX_NOERROR;
        }
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_StartContinuousRecognition(SPXRECOHANDLE hreco)
{
    SPX_INIT_HR(hr);   

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StartContinuousRecognitionAsync(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StartContinuousRecognitionAsync_WaitFor(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI Recognizer_StartContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPXAPI_INIT_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto precognizer = (*recohandles)[hreco];

        auto asyncop = precognizer->StartContinuousRecognitionAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<void>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_StartContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        auto completed = asyncop->WaitFor(milliseconds);
        hr = completed ? SPX_NOERROR : SPXERR_TIMEOUT;
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_StopContinuousRecognition(SPXRECOHANDLE hreco)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StopContinuousRecognitionAsync(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StopContinuousRecognitionAsync_WaitFor(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI Recognizer_StopContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPXAPI_INIT_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto precognizer = (*recohandles)[hreco];

        auto asyncop = precognizer->StopContinuousRecognitionAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<void>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_StopContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        auto completed = asyncop->WaitFor(milliseconds);
        hr = completed ? SPX_NOERROR : SPXERR_TIMEOUT;
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_SessionStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_SessionEvent_SetCallback(&ISpxRecognizerEvents::SessionStarted, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_SessionStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_SessionEvent_SetCallback(&ISpxRecognizerEvents::SessionStopped, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_SoundStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    // TODO: RobCh: Next: Implement
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_SoundStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    // TODO: RobCh: Next: Implement
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_IntermediateResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_RecoEvent_SetCallback(&ISpxRecognizerEvents::IntermediateResult, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_FinalResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_RecoEvent_SetCallback(&ISpxRecognizerEvents::FinalResult, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_NoMatch_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_RecoEvent_SetCallback(&ISpxRecognizerEvents::NoMatch, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_Canceled_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_RecoEvent_SetCallback(&ISpxRecognizerEvents::Canceled, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_SessionEvent_GetSessionId(SPXEVENTHANDLE hevent, wchar_t* pszSessionId, uint32_t cchSessionId)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto recoHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto sessionHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxSessionEventArgs, SPXEVENTHANDLE>();
        auto recoEvent = recoHandleTable->IsTracked(hevent)
            ? (*recoHandleTable)[hevent]
            : (*sessionHandleTable)[hevent];

        auto sessionId = recoEvent->GetSessionId();
        wcscpy_s(pszSessionId, cchSessionId, sessionId.c_str());
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Recognizer_RecognitionEvent_GetResult(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult)
{
    SPXAPI_INIT_TRY(hr)
    {
        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto recoEvent = (*eventhandles)[hevent];
        auto result = recoEvent->GetResult();

        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

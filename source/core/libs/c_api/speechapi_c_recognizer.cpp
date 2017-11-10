#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI Recognzier_Handle_IsValid(SPXRECOHANDLE hreco)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_Handle_Close(SPXRECOHANDLE hreco)
{
    SPX_INIT_HR(hr);

    try
    {
        auto recohandles = CSpxHandleTableManager::Get<CSpxRecognizer, SPXRECOHANDLE>();
        recohandles->StopTracking(hreco);
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

SPXAPI Recognizer_AsyncHandle_IsValid(SPXASYNCHANDLE hasync)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_AsyncHandle_Close(SPXASYNCHANDLE hasync)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_ResultHandle_IsValid(SPXRESULTHANDLE hresult)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_ResultHandle_Close(SPXRESULTHANDLE hresult)
{
    SPX_INIT_HR(hr);

    try
    {
        auto recohandles = CSpxHandleTableManager::Get<CSpxRecognitionResult, SPXRESULTHANDLE>();
        recohandles->StopTracking(hresult);
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

SPXAPI Recognizer_EventHandle_IsValid(SPXEVENTHANDLE hevent)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_EventHandle_Close(SPXEVENTHANDLE hevent)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_Enable(SPXRECOHANDLE hreco)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_Disable(SPXRECOHANDLE hreco)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_IsEnabled(SPXRECOHANDLE hreco, bool* pfEnabled)
{
    return SPXERR_NOT_IMPL;
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
    
    try
    {
        *phresult = SPXHANDLE_INVALID;

        auto recohandles = CSpxHandleTableManager::Get<CSpxRecognizer, SPXRECOHANDLE>();
        auto precognizer = (*recohandles)[hreco];
        auto future = precognizer->RecognizeAsync();
        auto result = future.get();
        
        auto resulthandles = CSpxHandleTableManager::Get<CSpxRecognitionResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
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

SPXAPI Recognizer_RecognizeAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_RecognizeAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_StartContinuousRecognition(SPXRECOHANDLE hreco)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_StartContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_StartContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_StopContinuousRecognition(SPXRECOHANDLE hreco)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_StopContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_StopContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_SessionStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_SessionStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_SoundStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_SoundStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_IntermediateResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_FinalResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_NoMatch_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_Canceled_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC* pCallback, void* pvContext)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_SessionEvent_GetSessionId(SPXEVENTHANDLE hevent, wchar_t* pszSessionId, uint32_t cchSessionId)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Recognizer_RecognitionEvent_GetResult(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult)
{
    return SPXERR_NOT_IMPL;
}

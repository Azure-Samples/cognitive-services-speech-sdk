//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_recognizer.cpp: Public API definitions for Recognizer related C methods

#include "stdafx.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "string_utils.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI_(bool) Recognizer_Handle_IsValid(SPXRECOHANDLE hreco)
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
    return Handle_Close<SPXEVENTHANDLE, ISpxRecognitionEventArgs>(hevent);
}

SPXAPI Recognizer_Enable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];
        recognizer->Enable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_Disable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];
        recognizer->Disable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_IsEnabled(SPXRECOHANDLE hreco, bool* pfEnabled)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];
        *pfEnabled = recognizer->IsEnabled();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_GetParameter_Name(Recognizer_Parameter parameter, wchar_t* name, uint32_t cchName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        const wchar_t* parameterName = L"";
        switch (parameter)
        {
            case RecognizerParameter_DeploymentId:
                parameterName = g_SPEECH_ModelId;
                break;

            case RecognizerParameter_OutputFormat:
                parameterName = g_SPEECH_OutputFormat;
                break;

            default:
                hr = SPXERR_INVALID_ARG;
                break;
        }

        PAL::wcscpy(name, cchName, parameterName, wcslen(parameterName), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_SetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        // We do NOT support CRIS Model IDs for Intent Recognizers (yet)... 
        SPX_IFTRUE_THROW_HR(
            wcscmp(g_SPEECH_ModelId, name) == 0 && 
            PAL::GetTypeName(*recognizer.get()).find("CSpxIntentRecognizer") != std::string::npos,
            SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        namedProperties->SetStringValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_GetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        auto tempValue = namedProperties->GetStringValue(name, defaultValue);

        PAL::wcscpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Recognizer_ContainsParameter_String(SPXRECOHANDLE hreco, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        return namedProperties->HasStringValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI Recognizer_SetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        namedProperties->SetNumberValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_GetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t* pvalue, int32_t defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        auto tempValue = namedProperties->GetNumberValue(name, defaultValue);

        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Recognizer_ContainsParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        return namedProperties->HasNumberValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI Recognizer_SetParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name, bool value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        namedProperties->SetBooleanValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_GetParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name, bool* pvalue, bool defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        auto tempValue = namedProperties->GetBooleanValue(name, defaultValue);

        *pvalue = !!tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Recognizer_ContainsParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        return namedProperties->HasBooleanValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
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
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto asyncop = recognizer->RecognizeAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_RecognizeAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phresult = SPXHANDLE_INVALID;

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        hr = SPXERR_TIMEOUT;
        auto completed = asyncop->WaitFor(milliseconds);

        if (completed)
        {
            auto result = asyncop->Future.get();
            if (result == nullptr)
            {
                hr = SPXERR_TIMEOUT;
            }
            else
            {
                auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
                *phresult = resulthandles->TrackHandle(result);
                hr = SPX_NOERROR;
            }
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
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
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto asyncop = recognizer->StartContinuousRecognitionAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<void>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_StartContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        auto completed = asyncop->WaitFor(milliseconds);
        hr = completed ? SPX_NOERROR : SPXERR_TIMEOUT;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
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
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto asyncop = recognizer->StopContinuousRecognitionAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<void>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_StopContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        auto completed = asyncop->WaitFor(milliseconds);
        hr = completed ? SPX_NOERROR : SPXERR_TIMEOUT;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_StartKeywordRecognition(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword)
{
    SPX_INIT_HR(hr);   

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StartKeywordRecognitionAsync(hreco, hkeyword, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StartKeywordRecognitionAsync_WaitFor(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI Recognizer_StartKeywordRecognitionAsync(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword, SPXASYNCHANDLE* phasync)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto keywordhandles =  CSpxSharedPtrHandleTableManager::Get<ISpxKwsModel, SPXKEYWORDHANDLE>();
        auto model = (*keywordhandles)[hkeyword];

        auto asyncop = recognizer->StartKeywordRecognitionAsync(model);
        auto ptr = std::make_shared<CSpxAsyncOp<void>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_StartKeywordRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        auto completed = asyncop->WaitFor(milliseconds);
        hr = completed ? SPX_NOERROR : SPXERR_TIMEOUT;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_StopKeywordRecognition(SPXRECOHANDLE hreco)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StopKeywordRecognitionAsync(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = Recognizer_StopKeywordRecognitionAsync_WaitFor(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI Recognizer_StopKeywordRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto asyncop = recognizer->StopKeywordRecognitionAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<void>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_StopKeywordRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        auto completed = asyncop->WaitFor(milliseconds);
        hr = completed ? SPX_NOERROR : SPXERR_TIMEOUT;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_SessionStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_SessionEvent_SetCallback(&ISpxRecognizerEvents::SessionStarted, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_SessionStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_SessionEvent_SetCallback(&ISpxRecognizerEvents::SessionStopped, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_SpeechStartDetected_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_RecoEvent_SetCallback(&ISpxRecognizerEvents::SpeechStartDetected, hreco, pCallback, pvContext);
}

SPXAPI Recognizer_SpeechEndDetected_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_RecoEvent_SetCallback(&ISpxRecognizerEvents::SpeechEndDetected, hreco, pCallback, pvContext);
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
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recoHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto sessionHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxSessionEventArgs, SPXEVENTHANDLE>();
        auto recoEvent = recoHandleTable->IsTracked(hevent)
            ? (*recoHandleTable)[hevent]
            : (*sessionHandleTable)[hevent];

        auto sessionId = recoEvent->GetSessionId();
        PAL::wcscpy(pszSessionId, cchSessionId, sessionId.c_str(), sessionId.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_RecognitionEvent_GetOffset(SPXEVENTHANDLE hevent, uint64_t* offset)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        if (offset != nullptr)
        {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
            auto recoEvent = (*eventhandles)[hevent];

            auto local_offset = recoEvent->GetOffset();
            *offset = local_offset;
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Recognizer_RecognitionEvent_GetResult(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto recoEvent = (*eventhandles)[hevent];
        auto result = recoEvent->GetResult();

        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_recognizer.cpp: Public API definitions for Recognizer related C methods

#include "stdafx.h"
#include "service_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "string_utils.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) recognizer_handle_is_valid(SPXRECOHANDLE hreco)
{
    return Handle_IsValid<SPXRECOHANDLE, ISpxRecognizer>(hreco);
}

SPXAPI recognizer_handle_release(SPXRECOHANDLE hreco)
{
    return Handle_Close<SPXRECOHANDLE, ISpxRecognizer>(hreco);
}

SPXAPI_(bool) recognizer_async_handle_is_valid(SPXASYNCHANDLE hasync)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<void>>(hasync);
}

SPXAPI recognizer_async_handle_release(SPXASYNCHANDLE hasync)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<void>>(hasync)
        ? Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<void>>(hasync)
        : Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>>(hasync);
}

SPXAPI_(bool) recognizer_result_handle_is_valid(SPXRESULTHANDLE hresult)
{
    return Handle_IsValid<SPXRESULTHANDLE, ISpxRecognitionResult>(hresult);
}

SPXAPI recognizer_result_handle_release(SPXRESULTHANDLE hresult)
{
    return Handle_Close<SPXRESULTHANDLE, ISpxRecognitionResult>(hresult);
}

SPXAPI_(bool) recognizer_event_handle_is_valid(SPXEVENTHANDLE hevent)
{
    return Handle_IsValid<SPXEVENTHANDLE, ISpxRecognitionEventArgs>(hevent) ||
           Handle_IsValid<SPXEVENTHANDLE, ISpxSessionEventArgs>(hevent);
}

SPXAPI recognizer_event_handle_release(SPXEVENTHANDLE hevent)
{
    return Handle_IsValid<SPXEVENTHANDLE, ISpxSessionEventArgs>(hevent)
        ? Handle_Close<SPXEVENTHANDLE, ISpxSessionEventArgs>(hevent)
        : Handle_Close<SPXEVENTHANDLE, ISpxRecognitionEventArgs>(hevent);
}

SPXAPI recognizer_enable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];
        recognizer->Enable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_disable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];
        recognizer->Disable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_is_enabled(SPXRECOHANDLE hreco, bool* pfEnabled)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];
        *pfEnabled = recognizer->IsEnabled();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_recognize_once(SPXRECOHANDLE hreco, SPXRESULTHANDLE* phresult)
{
    SPX_INIT_HR(hr);
    *phresult = SPXHANDLE_INVALID;

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_recognize_once_async(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_recognize_once_async_wait_for(hasync, UINT32_MAX, phresult));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ recognizer_async_handle_release(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI recognizer_recognize_once_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
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

SPXAPI recognizer_recognize_once_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult)
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

SPXAPI recognizer_start_continuous_recognition(SPXRECOHANDLE hreco)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_start_continuous_recognition_async(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_start_continuous_recognition_async_wait_for(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ recognizer_async_handle_release(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI recognizer_start_continuous_recognition_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
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

SPXAPI recognizer_start_continuous_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        hr = SPXERR_TIMEOUT;
        auto completed = asyncop->WaitFor(milliseconds);
        if (completed)
        {
            asyncop->Future.get();
            hr = SPX_NOERROR;
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_stop_continuous_recognition(SPXRECOHANDLE hreco)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_stop_continuous_recognition_async(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_stop_continuous_recognition_async_wait_for(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ recognizer_async_handle_release(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI recognizer_stop_continuous_recognition_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
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

SPXAPI recognizer_stop_continuous_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        hr = SPXERR_TIMEOUT;
        auto completed = asyncop->WaitFor(milliseconds);
        if (completed)
        {
            asyncop->Future.get();
            hr = SPX_NOERROR;
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_start_keyword_recognition(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_start_keyword_recognition_async(hreco, hkeyword, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_start_keyword_recognition_async_wait_for(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ recognizer_async_handle_release(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI recognizer_start_keyword_recognition_async(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword, SPXASYNCHANDLE* phasync)
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

SPXAPI recognizer_start_keyword_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        hr = SPXERR_TIMEOUT;
        auto completed = asyncop->WaitFor(milliseconds);
        if (completed)
        {
            asyncop->Future.get();
            hr = SPX_NOERROR;
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_stop_keyword_recognition(SPXRECOHANDLE hreco)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_stop_keyword_recognition_async(hreco, &hasync));
    }

    if (SPX_SUCCEEDED(hr))
    {
        SPX_REPORT_ON_FAIL(hr = recognizer_stop_keyword_recognition_async_wait_for(hasync, UINT32_MAX));
    }

    if (hasync != SPXHANDLE_INVALID)
    {
        // Don't overwrite error code from earlier function calls when cleaning up async handles
        SPX_REPORT_ON_FAIL(/* hr = */ recognizer_async_handle_release(hasync));
        hasync = SPXHANDLE_INVALID;
    }

    SPX_RETURN_HR(hr);
}

SPXAPI recognizer_stop_keyword_recognition_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
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

SPXAPI recognizer_stop_keyword_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        hr = SPXERR_TIMEOUT;
        auto completed = asyncop->WaitFor(milliseconds);
        if (completed)
        {
            asyncop->Future.get();
            hr = SPX_NOERROR;
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_session_started_set_callback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_session_set_event_callback(&ISpxRecognizerEvents::SessionStarted, hreco, pCallback, pvContext);
}

SPXAPI recognizer_session_stopped_set_callback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_session_set_event_callback(&ISpxRecognizerEvents::SessionStopped, hreco, pCallback, pvContext);
}

SPXAPI recognizer_speech_start_detected_set_callback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_recognition_set_event_callback(&ISpxRecognizerEvents::SpeechStartDetected, hreco, pCallback, pvContext);
}

SPXAPI recognizer_speech_end_detected_set_callback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_recognition_set_event_callback(&ISpxRecognizerEvents::SpeechEndDetected, hreco, pCallback, pvContext);
}

SPXAPI recognizer_recognizing_set_callback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_recognition_set_event_callback(&ISpxRecognizerEvents::IntermediateResult, hreco, pCallback, pvContext);
}

SPXAPI recognizer_recognized_set_callback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_recognition_set_event_callback(&ISpxRecognizerEvents::FinalResult, hreco, pCallback, pvContext);
}

SPXAPI recognizer_canceled_set_callback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_recognition_set_event_callback(&ISpxRecognizerEvents::Canceled, hreco, pCallback, pvContext);
}

SPXAPI recognizer_session_event_get_session_id(SPXEVENTHANDLE hevent, char* pszSessionId, uint32_t cchSessionId)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recoHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto sessionHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxSessionEventArgs, SPXEVENTHANDLE>();
        auto recoEvent = recoHandleTable->IsTracked(hevent)
            ? (*recoHandleTable)[hevent]
            : (*sessionHandleTable)[hevent];

        auto sessionId = PAL::ToString(recoEvent->GetSessionId());
        PAL::strcpy(pszSessionId, cchSessionId, sessionId.c_str(), sessionId.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_recognition_event_get_offset(SPXEVENTHANDLE hevent, uint64_t* offset)
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

SPXAPI recognizer_recognition_event_get_result(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult)
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

SPXAPI recognizer_get_property_bag(SPXRECOHANDLE hreco, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !recognizer_handle_is_valid(hreco));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(recognizer);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

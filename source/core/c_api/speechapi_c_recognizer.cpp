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
#include "async_helpers.h"
#include "function_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) recognizer_handle_is_valid(SPXRECOHANDLE hreco)
{
    return Handle_IsValid<SPXRECOHANDLE, ISpxRecognizer>(hreco);
}

SPXAPI recognizer_handle_release(SPXRECOHANDLE hreco)
{
    // Before releasing the handle, make sure speech is stopped.
    // If we get to a place where > 1 higher level object has and needs to
    // close a handle, this method should revert to a simple close and
    // we'll need to figure out where to ensure speech is stopped before the
    // last handle it closed. Today we have a 1:1 mapping between the higher
    // level objects and the lower ones.

    SPX_INIT_HR(hr);

    hr = recognizer_stop_continuous_recognition(hreco);
    SPX_REPORT_ON_FAIL(hr);

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
           Handle_IsValid<SPXEVENTHANDLE, ISpxSessionEventArgs>(hevent) ||
           Handle_IsValid<SPXEVENTHANDLE, ISpxConnectionEventArgs>(hevent);
}

SPXAPI recognizer_event_handle_release(SPXEVENTHANDLE hevent)
{
    if (Handle_IsValid<SPXEVENTHANDLE, ISpxSessionEventArgs>(hevent))
    {
        return Handle_Close<SPXEVENTHANDLE, ISpxSessionEventArgs>(hevent);
    }
    else if (Handle_IsValid<SPXEVENTHANDLE, ISpxConnectionEventArgs>(hevent))
    {
        return Handle_Close<SPXEVENTHANDLE, ISpxConnectionEventArgs>(hevent);
    }
    else
    {
        return Handle_Close<SPXEVENTHANDLE, ISpxRecognitionEventArgs>(hevent);
    }
}

SPXAPI recognizer_enable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        recognizer->Enable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_disable(SPXRECOHANDLE hreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        recognizer->Disable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_is_enabled(SPXRECOHANDLE hreco, bool* pfEnabled)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pfEnabled == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        *pfEnabled = recognizer->IsEnabled();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_recognize_once(SPXRECOHANDLE hreco, SPXRESULTHANDLE* phresult)
{
    return async_to_sync_with_result(
        hreco,
        phresult,
        recognizer_recognize_once_async,
        recognizer_recognize_once_async_wait_for
    );
}

SPXAPI recognizer_recognize_once_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        launch_async_op(*recognizer, resolveOverload(&ISpxRecognizer::RecognizeAsync), phasync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_recognize_once_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult)
{
    return async_operation_wait_for<ISpxRecognitionResult>(hasync, milliseconds, phresult);
}

SPXAPI recognizer_start_continuous_recognition(SPXRECOHANDLE hreco)
{
    return async_to_sync(
        hreco,
        recognizer_start_continuous_recognition_async,
        recognizer_start_continuous_recognition_async_wait_for
    );
}

SPXAPI recognizer_start_continuous_recognition_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        launch_async_op(*recognizer, &ISpxRecognizer::StartContinuousRecognitionAsync, phasync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_start_continuous_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    return async_operation_wait_for(hasync, milliseconds);
}

SPXAPI recognizer_stop_continuous_recognition(SPXRECOHANDLE hreco)
{
    return async_to_sync(
        hreco,
        recognizer_stop_continuous_recognition_async,
        recognizer_stop_continuous_recognition_async_wait_for
    );
}

SPXAPI recognizer_stop_continuous_recognition_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        launch_async_op(*recognizer, &ISpxRecognizer::StopContinuousRecognitionAsync, phasync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_stop_continuous_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    return async_operation_wait_for(hasync, milliseconds);
}

SPXAPI recognizer_start_keyword_recognition(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword)
{
    return async_to_sync(
        hreco,
        recognizer_start_keyword_recognition_async,
        recognizer_start_keyword_recognition_async_wait_for,
        hkeyword);
}

SPXAPI recognizer_start_keyword_recognition_async(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        auto model = GetInstance<ISpxKwsModel>(hkeyword);
        launch_async_op(*recognizer, &ISpxRecognizer::StartKeywordRecognitionAsync, phasync, model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_start_keyword_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    return async_operation_wait_for(hasync, milliseconds);
}

SPXAPI recognizer_recognize_keyword_once(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword, SPXRESULTHANDLE* phresult)
{
    return async_to_sync_with_result(
        hreco,
        phresult,
        recognizer_recognize_keyword_once_async,
        recognizer_recognize_keyword_once_async_wait_for,
        hkeyword
    );
}

SPXAPI recognizer_recognize_keyword_once_async(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        auto model = GetInstance<ISpxKwsModel>(hkeyword);
        launch_async_op(*recognizer, resolveOverload<std::shared_ptr<ISpxKwsModel>>(&ISpxRecognizer::RecognizeAsync), phasync, model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
SPXAPI recognizer_recognize_keyword_once_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult)
{
    return async_operation_wait_for<ISpxRecognitionResult>(hasync, milliseconds, phresult);
}

SPXAPI recognizer_stop_keyword_recognition(SPXRECOHANDLE hreco)
{
    return async_to_sync(hreco, recognizer_stop_keyword_recognition_async, recognizer_stop_keyword_recognition_async_wait_for);
}

SPXAPI recognizer_stop_keyword_recognition_async(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        launch_async_op(*recognizer, &ISpxRecognizer::StopKeywordRecognitionAsync, phasync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_stop_keyword_recognition_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    return async_operation_wait_for(hasync, milliseconds);
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
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pszSessionId == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        std::shared_ptr<ISpxSessionEventArgs> recoEvent = TryGetInstance<ISpxRecognitionEventArgs>(hevent);
        if (!recoEvent)
        {
            recoEvent = TryGetInstance<ISpxConnectionEventArgs>(hevent);
        }
        if (!recoEvent)
        {
            recoEvent = TryGetInstance<ISpxConversationEventArgs>(hevent);
        }
        if (!recoEvent)
        {
            recoEvent = TryGetInstance<ISpxSessionEventArgs>(hevent);
        }

        SPX_IFTRUE_THROW_HR(recoEvent == nullptr, SPXERR_INVALID_HANDLE);
        auto sessionId = PAL::ToString(recoEvent->GetSessionId());
        PAL::strcpy(pszSessionId, cchSessionId, sessionId.c_str(), sessionId.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_recognition_event_get_offset(SPXEVENTHANDLE hevent, uint64_t* offset)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, offset == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        if (offset != nullptr)
        {
            auto recoEvent = GetInstance<ISpxRecognitionEventArgs>(hevent);

            auto local_offset = recoEvent->GetOffset();
            *offset = local_offset;
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_recognition_event_get_result(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phresult == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recoEvent = GetInstance<ISpxRecognitionEventArgs>(hevent);
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
        auto recognizer = GetInstance<ISpxRecognizer>(hreco);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(recognizer);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

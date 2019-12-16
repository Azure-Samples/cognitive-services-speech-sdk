//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_dialog_service_connector.cpp: Definitions for Dialog Service Connector related C methods.
//

#include "stdafx.h"
#include <limits>
#include <tuple>
#include <type_traits>
#include "event_helpers.h"
#include "handle_helpers.h"
#include "string_utils.h"
#include "service_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI_(bool) dialog_service_connector_handle_is_valid(SPXRECOHANDLE h_connector)
{
    return Handle_IsValid<SPXRECOHANDLE, ISpxDialogServiceConnector>(h_connector);
}

SPXAPI dialog_service_connector_handle_release(SPXRECOHANDLE h_connector)
{
    return Handle_Close<SPXRECOHANDLE, ISpxDialogServiceConnector>(h_connector);
}

SPXAPI_(bool) dialog_service_connector_async_handle_is_valid(SPXASYNCHANDLE h_async)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>>(h_async);
}

SPXAPI dialog_service_connector_async_handle_release(SPXASYNCHANDLE h_async)
{
    return Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>>(h_async);
}

SPXAPI_(bool) dialog_service_connector_async_void_handle_is_valid(SPXASYNCHANDLE h_async)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<void>>(h_async);
}

SPXAPI dialog_service_connector_async_void_handle_release(SPXASYNCHANDLE h_async)
{
    return Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<void>>(h_async);
}

SPXAPI_(bool) dialog_service_connector_async_string_handle_is_valid(SPXASYNCHANDLE h_async)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<std::string>>(h_async);
}

SPXAPI dialog_service_connector_async_string_handle_release(SPXASYNCHANDLE h_async)
{
    return Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<std::string>>(h_async);
}

SPXAPI_(bool) dialog_service_connector_async_reco_result_handle_is_valid(SPXASYNCHANDLE h_async)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>>(h_async);
}

SPXAPI dialog_service_connector_async_reco_result_handle_release(SPXASYNCHANDLE h_async)
{
    return Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>>(h_async);
}

SPXAPI_(bool) dialog_service_connector_activity_received_event_handle_is_valid(SPXEVENTHANDLE h_event)
{
    return Handle_IsValid<SPXEVENTHANDLE, ISpxActivityEventArgs>(h_event);
}

SPXAPI dialog_service_connector_activity_received_event_release(SPXEVENTHANDLE h_event)
{
    return Handle_Close<SPXEVENTHANDLE, ISpxActivityEventArgs>(h_event);
}

SPXAPI dialog_service_connector_get_property_bag(SPXRECOHANDLE h_connector, SPXPROPERTYBAGHANDLE* h_prop_bag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, h_prop_bag == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !dialog_service_connector_handle_is_valid(h_connector));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxDialogServiceConnector, SPXRECOHANDLE>();
        auto connector = (*handles)[h_connector];

        auto properties = SpxQueryService<ISpxNamedProperties>(connector);

        auto prop_handle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *h_prop_bag = prop_handle->TrackHandle(properties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename Fn, typename... Args>
void launch_async_op(SPXRECOHANDLE h_connector, SPXASYNCHANDLE *p_async, Fn fn, Args... args)
{
    SPX_IFTRUE_THROW_HR(p_async == nullptr, SPXERR_INVALID_ARG);

    using async_type = decltype((std::declval<ISpxDialogServiceConnector>().*fn)(std::declval<Args>()...));

    *p_async = SPXHANDLE_INVALID;

    auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxDialogServiceConnector, SPXRECOHANDLE>();
    auto connector = (*handles)[h_connector];

    auto async_op = std::make_shared<async_type>(std::move(((*connector).*fn)(args...)));

    auto async_handles = CSpxSharedPtrHandleTableManager::Get<async_type, SPXASYNCHANDLE>();

    *p_async = async_handles->TrackHandle(async_op);
}


/* The following two functions can be combined once we upgrade to C++17 and have if constexpr support */
SPXHR wait_for_void_async_op(SPXASYNCHANDLE h_async, uint32_t milliseconds)
{
    using async_type = CSpxAsyncOp<void>;
    auto handles = CSpxSharedPtrHandleTableManager::Get<async_type, SPXASYNCHANDLE>();
    auto async_op = (*handles)[h_async];

    SPXHR hr = SPXERR_TIMEOUT;
    auto completed = async_op->WaitFor(milliseconds);

    if (completed)
    {
        hr = SPX_NOERROR;
    }

    return hr;
}

template<typename Result>
std::tuple<SPXHR, Result> wait_for_async_op(SPXASYNCHANDLE h_async, uint32_t milliseconds)
{
    using async_type = CSpxAsyncOp<Result>;
    auto handles = CSpxSharedPtrHandleTableManager::Get<async_type, SPXASYNCHANDLE>();
    auto async_op = (*handles)[h_async];

    SPXHR hr = SPXERR_TIMEOUT;
    auto completed = async_op->WaitFor(milliseconds);

    if (completed)
    {
        return std::make_tuple(SPX_NOERROR, async_op->Future.get());
    }

    return std::make_tuple(hr, Result{});
}

SPXAPI dialog_service_connector_connect(SPXRECOHANDLE h_connector)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE h_async = SPXHANDLE_INVALID;
    hr = dialog_service_connector_connect_async(h_connector, &h_async);
    SPX_REPORT_ON_FAIL(hr);

    if (SPX_SUCCEEDED(hr))
    {
        hr = dialog_service_connector_connect_async_wait_for(h_async, std::numeric_limits<uint32_t>::max());
        SPX_REPORT_ON_FAIL(hr);
    }

    if (h_async != SPXHANDLE_INVALID)
    {
        auto releaseHr = recognizer_async_handle_release(h_async);
        UNUSED(releaseHr);
        SPX_REPORT_ON_FAIL(releaseHr);
    }
    SPX_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_connect_async(SPXRECOHANDLE h_connector, SPXASYNCHANDLE* p_async)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, p_async == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        launch_async_op(h_connector, p_async, &ISpxDialogServiceConnector::ConnectAsync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_connect_async_wait_for(SPXASYNCHANDLE h_async, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        return wait_for_void_async_op(h_async, milliseconds);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_disconnect(SPXRECOHANDLE h_connector)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE h_async = SPXHANDLE_INVALID;
    hr = dialog_service_connector_disconnect_async(h_connector, &h_async);
    SPX_REPORT_ON_FAIL(hr);

    if (SPX_SUCCEEDED(hr))
    {
        hr = dialog_service_connector_disconnect_async_wait_for(h_async, std::numeric_limits<uint32_t>::max());
        SPX_REPORT_ON_FAIL(hr);
    }

    if (h_async != SPXHANDLE_INVALID)
    {
        auto releaseHr = dialog_service_connector_async_void_handle_release(h_async);
        UNUSED(releaseHr);
        SPX_REPORT_ON_FAIL(releaseHr);
    }
    SPX_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_disconnect_async(SPXRECOHANDLE h_connector, SPXASYNCHANDLE* p_async)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, p_async == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        launch_async_op(h_connector, p_async, &ISpxDialogServiceConnector::DisconnectAsync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_disconnect_async_wait_for(SPXASYNCHANDLE h_async, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        return wait_for_void_async_op(h_async, milliseconds);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_send_activity(SPXRECOHANDLE h_connector, const char* activity, char* interaction_id)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, interaction_id == nullptr);

    SPX_INIT_HR(hr);

    SPXASYNCHANDLE h_async = SPXHANDLE_INVALID;
    hr = dialog_service_connector_send_activity_async(h_connector, activity, &h_async);
    SPX_REPORT_ON_FAIL(hr);

    if (SPX_SUCCEEDED(hr))
    {
        hr = dialog_service_connector_send_activity_async_wait_for(h_async, std::numeric_limits<uint32_t>::max(), interaction_id);
        SPX_REPORT_ON_FAIL(hr);
    }

    if (h_async != SPXHANDLE_INVALID)
    {
        auto releaseHr = recognizer_async_handle_release(h_async);
        UNUSED(releaseHr);
        SPX_REPORT_ON_FAIL(releaseHr);
    }
    SPX_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_send_activity_async(SPXRECOHANDLE h_connector, const char* activity, SPXASYNCHANDLE* p_async)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, p_async == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        std::string activity_str{ activity };
        try
        {
            auto json = nlohmann::json::parse(activity_str);
            if (!json.is_object())
            {
                return SPXERR_INVALID_ARG;
            }
        }
        catch (const std::exception& ex)
        {
            SPX_TRACE_ERROR("%s: Parsing received activity failed (what=%s)", __FUNCTION__, ex.what());
            return SPXERR_INVALID_ARG;
        }

        launch_async_op(h_connector, p_async, &ISpxDialogServiceConnector::SendActivityAsync, std::move(activity_str));
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);

}

SPXAPI dialog_service_connector_send_activity_async_wait_for(SPXASYNCHANDLE h_async, uint32_t milliseconds, char* interaction_id)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, interaction_id == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto result = wait_for_async_op<std::string>(h_async, milliseconds);
        hr = std::get<0>(result);
        auto iid = std::move(std::get<1>(result));
        std::memcpy(interaction_id, iid.c_str(), iid.size());
        interaction_id[iid.size()] = 0x00;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_start_keyword_recognition(SPXRECOHANDLE h_connector, SPXKEYWORDHANDLE h_keyword)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE h_async = SPXHANDLE_INVALID;
    hr = dialog_service_connector_start_keyword_recognition_async(h_connector, h_keyword, &h_async);
    SPX_REPORT_ON_FAIL(hr);

    if (SPX_SUCCEEDED(hr))
    {
        hr = dialog_service_connector_start_keyword_recognition_async_wait_for(h_async, std::numeric_limits<uint32_t>::max());
        SPX_REPORT_ON_FAIL(hr);
    }

    if (h_async != SPXHANDLE_INVALID)
    {
        auto releaseHr = recognizer_async_handle_release(h_async);
        UNUSED(releaseHr);
        SPX_REPORT_ON_FAIL(releaseHr);
    }
    SPX_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_start_keyword_recognition_async(SPXRECOHANDLE h_connector, SPXKEYWORDHANDLE h_keyword, SPXASYNCHANDLE* p_async)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, p_async == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto keyword_handles = CSpxSharedPtrHandleTableManager::Get<ISpxKwsModel, SPXKEYWORDHANDLE>();
        auto model = (*keyword_handles)[h_keyword];
        launch_async_op(h_connector, p_async, &ISpxDialogServiceConnector::StartKeywordRecognitionAsync, model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_start_keyword_recognition_async_wait_for(SPXASYNCHANDLE h_async, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        return wait_for_void_async_op(h_async, milliseconds);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_stop_keyword_recognition(SPXRECOHANDLE h_connector)
{
    SPX_INIT_HR(hr);

    SPXASYNCHANDLE h_async = SPXHANDLE_INVALID;
    hr = dialog_service_connector_stop_keyword_recognition_async(h_connector, &h_async);
    SPX_REPORT_ON_FAIL(hr);

    if (SPX_SUCCEEDED(hr))
    {
        hr = dialog_service_connector_stop_keyword_recognition_async_wait_for(h_async, std::numeric_limits<uint32_t>::max());
        SPX_REPORT_ON_FAIL(hr);
    }

    if (h_async != SPXHANDLE_INVALID)
    {
        auto releaseHr = recognizer_async_handle_release(h_async);
        UNUSED(releaseHr);
        SPX_REPORT_ON_FAIL(releaseHr);
    }
    SPX_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_stop_keyword_recognition_async(SPXRECOHANDLE h_connector, SPXASYNCHANDLE* p_async)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, p_async == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        launch_async_op(h_connector, p_async, &ISpxDialogServiceConnector::StopKeywordRecognitionAsync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);

}

SPXAPI dialog_service_connector_stop_keyword_recognition_async_wait_for(SPXASYNCHANDLE h_async, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        return wait_for_void_async_op(h_async, milliseconds);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_listen_once(SPXRECOHANDLE h_connector, SPXRESULTHANDLE* ph_result)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_result == nullptr);
    *ph_result = SPXHANDLE_INVALID;

    SPXASYNCHANDLE h_async = SPXHANDLE_INVALID;

    SPX_RETURN_ON_FAIL(dialog_service_connector_listen_once_async(h_connector, &h_async));

    SPX_RETURN_ON_FAIL(dialog_service_connector_listen_once_async_wait_for(h_async, UINT32_MAX, ph_result));

    SPX_REPORT_ON_FAIL(dialog_service_connector_async_handle_release(h_async));

    return SPX_NOERROR;
}

SPXAPI dialog_service_connector_listen_once_async(SPXRECOHANDLE h_connector, SPXASYNCHANDLE* p_async)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, p_async == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        launch_async_op(h_connector, p_async, &ISpxDialogServiceConnector::ListenOnceAsync);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_listen_once_async_wait_for(SPXASYNCHANDLE h_async, uint32_t milliseconds, SPXRESULTHANDLE* ph_result)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_result == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_result = SPXHANDLE_INVALID;

        auto result = wait_for_async_op<std::shared_ptr<ISpxRecognitionResult>>(h_async, milliseconds);

        SPX_RETURN_ON_FAIL(std::get<0>(result));

        auto result_handles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();

        *ph_result = result_handles->TrackHandle(std::get<1>(result));

        return std::get<0>(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_session_started_set_callback(SPXRECOHANDLE h_connector, PSESSION_CALLBACK_FUNC p_callback, void *pv_context)
{
    return dialog_service_connector_session_set_event_callback(&ISpxRecognizerEvents::SessionStarted, h_connector, p_callback, pv_context);
}

SPXAPI dialog_service_connector_session_stopped_set_callback(SPXRECOHANDLE h_connector, PSESSION_CALLBACK_FUNC p_callback, void *pv_context)
{
    return dialog_service_connector_session_set_event_callback(&ISpxRecognizerEvents::SessionStopped, h_connector, p_callback, pv_context);
}

SPXAPI dialog_service_connector_recognized_set_callback(SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void *pv_context)
{
    return dialog_service_connector_recognition_set_event_callback(&ISpxRecognizerEvents::FinalResult, h_connector, p_callback, pv_context);
}

SPXAPI dialog_service_connector_recognizing_set_callback(SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void *pv_context)
{
    return dialog_service_connector_recognition_set_event_callback(&ISpxRecognizerEvents::IntermediateResult, h_connector, p_callback, pv_context);
}

SPXAPI dialog_service_connector_canceled_set_callback(SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void *pv_context)
{
    return dialog_service_connector_recognition_set_event_callback(&ISpxRecognizerEvents::Canceled, h_connector, p_callback, pv_context);
}

SPXAPI dialog_service_connector_activity_received_set_callback(SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void *pv_context)
{
    return dialog_service_connector_activity_received_set_event_callback(&ISpxDialogServiceConnectorEvents::ActivityReceived, h_connector, p_callback, pv_context);
}

SPXAPI dialog_service_connector_synthesizing_audio_set_callback(SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void *pv_context)
{
    return dialog_service_connector_recognition_set_event_callback(&ISpxRecognizerEvents::TranslationSynthesisResult, h_connector, p_callback, pv_context);
}

SPXAPI dialog_service_connector_activity_received_event_get_activity_size(SPXEVENTHANDLE h_event, size_t* size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, size == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityEventArgs, SPXEVENTHANDLE>();
        auto event = (*handles)[h_event];
        auto& activity_str = event->GetActivity();
        *size = activity_str.size();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_activity_received_event_get_activity(SPXEVENTHANDLE h_event, char* activity, size_t size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, activity == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityEventArgs, SPXEVENTHANDLE>();
        auto event = (*handles)[h_event];
        auto activity_str = event->GetActivity();
        if (size < (activity_str.size() + 1))
        {
            return SPXERR_BUFFER_TOO_SMALL;
        }
        std::copy(activity_str.begin(), activity_str.end(), activity);
        activity[activity_str.size()] = 0;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) dialog_service_connector_activity_received_event_has_audio(SPXEVENTHANDLE h_event)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityEventArgs, SPXEVENTHANDLE>();
        auto event = (*handles)[h_event];
        return event->HasAudio();
    }
    SPXAPI_CATCH_AND_RETURN(hr, false);
}


SPXAPI dialog_service_connector_activity_received_event_get_audio(SPXEVENTHANDLE h_event, SPXAUDIOSTREAMHANDLE* ph_audio)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_audio == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityEventArgs, SPXEVENTHANDLE>();
        auto event = (*handles)[h_event];
        auto audio = event->GetAudio();
        if (audio != nullptr)
        {
            auto audioStream = audio->QueryInterface<ISpxAudioStream>();
            SPX_THROW_HR_IF(SPXERR_INVALID_ARG, audioStream == nullptr);
            auto audioTable = CSpxSharedPtrHandleTableManager::Get<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>();
            *ph_audio = audioTable->TrackHandle(audioStream);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

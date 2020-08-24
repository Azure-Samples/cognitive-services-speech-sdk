//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_audio_stream.cpp: Public API definitions for audio stream related C methods and types

#include "stdafx.h"
#include "common.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"


using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) audio_stream_is_handle_valid(SPXAUDIOSTREAMHANDLE haudioStream)
{
    return Handle_IsValid<SPXAUDIOSTREAMHANDLE, ISpxAudioStream>(haudioStream);
}

SPXAPI audio_stream_create_push_audio_input_stream(SPXAUDIOSTREAMHANDLE* haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioStream = SPXHANDLE_INVALID;

        auto format = CSpxSharedPtrHandleTableManager::GetPtr<SPXWAVEFORMATEX, SPXAUDIOSTREAMFORMATHANDLE>(hformat);
        auto initFormat = SpxCreateObjectWithSite<ISpxAudioStreamInitFormat>("CSpxPushAudioInputStream", SpxGetRootSite());
        initFormat->SetFormat(format.get());

        auto stream = SpxQueryInterface<ISpxAudioStream>(initFormat);
        *haudioStream = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(stream);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_stream_create_pull_audio_input_stream(SPXAUDIOSTREAMHANDLE* haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioStream = SPXHANDLE_INVALID;

        auto format = CSpxSharedPtrHandleTableManager::GetPtr<SPXWAVEFORMATEX, SPXAUDIOSTREAMFORMATHANDLE>(hformat);
        auto initFormat = SpxCreateObjectWithSite<ISpxAudioStreamInitFormat>("CSpxPullAudioInputStream", SpxGetRootSite());
        initFormat->SetFormat(format.get());

        auto stream = SpxQueryInterface<ISpxAudioStream>(initFormat);
        *haudioStream = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(stream);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_stream_create_pull_audio_output_stream(SPXAUDIOSTREAMHANDLE* haudioStream)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioStream = SPXHANDLE_INVALID;

        auto initFormat = SpxCreateObjectWithSite<ISpxAudioStreamInitFormat>("CSpxPullAudioOutputStream", SpxGetRootSite());
        auto stream = SpxQueryInterface<ISpxAudioStream>(initFormat);
        *haudioStream = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(stream);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_stream_create_push_audio_output_stream(SPXAUDIOSTREAMHANDLE* haudioStream)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioStream = SPXHANDLE_INVALID;

        auto initFormat = SpxCreateObjectWithSite<ISpxAudioStreamInitFormat>("CSpxPushAudioOutputStream", SpxGetRootSite());
        auto stream = SpxQueryInterface<ISpxAudioStream>(initFormat);
        *haudioStream = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(stream);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_stream_release(SPXAUDIOSTREAMHANDLE haudioStream)
{
    return Handle_Close<SPXAUDIOSTREAMHANDLE, ISpxAudioStream>(haudioStream);
}

SPXAPI pull_audio_input_stream_set_callbacks(SPXAUDIOSTREAMHANDLE haudioStream, void* pvContext, CUSTOM_AUDIO_PULL_STREAM_READ_CALLBACK readCallback, CUSTOM_AUDIO_PULL_STREAM_CLOSE_CALLBACK closeCallback)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto initCallbacks = SpxQueryInterface<ISpxAudioStreamReaderInitCallbacks>(stream);
        initCallbacks->SetCallbacks(
            [=](uint8_t* buffer, uint32_t size) { return readCallback(pvContext, buffer, static_cast<uint32_t>(size)); },
            [=]() { if (closeCallback != nullptr) { closeCallback(pvContext); } } );
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI pull_audio_input_stream_set_getproperty_callback(SPXAUDIOSTREAMHANDLE haudioStream, void* pvContext, CUSTOM_AUDIO_PULL_STREAM_GET_PROPERTY_CALLBACK getPropertyCallback)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto initCallbacks = SpxQueryInterface<ISpxAudioStreamReaderInitCallbacks>(stream);
        initCallbacks->SetPropertyCallback(
            [=](PropertyId id, uint8_t* result, uint32_t size) { return getPropertyCallback(pvContext, static_cast<int>(id), result, static_cast<uint32_t>(size)); }
        );
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
SPXAPI push_audio_input_stream_write(SPXAUDIOSTREAMHANDLE haudioStream, uint8_t* buffer, uint32_t size)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto pushStream = SpxQueryInterface<ISpxAudioStreamWriter>(stream);
        pushStream->Write(buffer, size);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI push_audio_input_stream_close(SPXAUDIOSTREAMHANDLE haudioStream)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto pushStream = SpxQueryInterface<ISpxAudioStreamWriter>(stream);
        pushStream->Write(nullptr, 0);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI pull_audio_output_stream_read(SPXAUDIOSTREAMHANDLE haudioStream, uint8_t* buffer, uint32_t bufferSize, uint32_t* pfilledSize)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pfilledSize == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, buffer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto pullStream = SpxQueryInterface<ISpxAudioOutputReader>(stream);
        *pfilledSize = pullStream->Read(buffer, bufferSize);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI push_audio_output_stream_set_callbacks(SPXAUDIOSTREAMHANDLE haudioStream, void* pvContext, CUSTOM_AUDIO_PUSH_STREAM_WRITE_CALLBACK writeCallback, CUSTOM_AUDIO_PUSH_STREAM_CLOSE_CALLBACK closeCallback)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto initCallbacks = SpxQueryInterface<ISpxAudioStreamWriterInitCallbacks>(stream);
        initCallbacks->SetCallbacks(
            [=](uint8_t* buffer, uint32_t size) { return writeCallback(pvContext, buffer, size); },
            [=]() { if (closeCallback != nullptr) { closeCallback(pvContext); } });
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) audio_data_stream_is_handle_valid(SPXAUDIOSTREAMHANDLE haudioStream)
{
    return Handle_IsValid<SPXAUDIOSTREAMHANDLE, ISpxAudioDataStream>(haudioStream);
}

SPXAPI audio_data_stream_create_from_file(SPXAUDIOSTREAMHANDLE* haudioStream, const char* fileName)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioStream = SPXHANDLE_INVALID;

        auto audioDataStream = SpxCreateObjectWithSite<ISpxAudioDataStream>("CSpxAudioDataStream", SpxGetRootSite());
        audioDataStream->InitFromFile(fileName);

        *haudioStream = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioDataStream, SPXAUDIOSTREAMHANDLE>(audioDataStream);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_create_from_result(SPXAUDIOSTREAMHANDLE* haudioStream, SPXRESULTHANDLE hresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioStream = SPXHANDLE_INVALID;

        auto result = CSpxSharedPtrHandleTableManager::GetPtr<ISpxSynthesisResult, SPXRESULTHANDLE>(hresult);
        auto audioDataStream = result->GetAudioDataStream();

        *haudioStream = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioDataStream, SPXAUDIOSTREAMHANDLE>(audioDataStream);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_create_from_keyword_result(SPXAUDIOSTREAMHANDLE* audioStreamHandle, SPXRESULTHANDLE keywordResultHandle)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, audioStreamHandle == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto result = CSpxSharedPtrHandleTableManager::GetPtr<ISpxRecognitionResult, SPXRESULTHANDLE>(keywordResultHandle);
        SPX_RETURN_HR_IF(SPXERR_INVALID_RESULT_REASON, ResultReason::RecognizedKeyword != result->GetReason());
        auto audioDataStream = result->GetAudioDataStream();
        SPX_RETURN_HR_IF(SPXERR_INVALID_RECOGNIZER, audioDataStream == nullptr);
        auto retrievable = SpxQueryInterface<ISpxRetrievable>(audioDataStream);
        retrievable->MarkAsRetrieved();
        *audioStreamHandle = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioDataStream, SPXAUDIOSTREAMHANDLE>(audioDataStream);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_get_status(SPXAUDIOSTREAMHANDLE haudioStream, Stream_Status* status)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, status == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        *status = (Stream_Status)stream->GetStatus();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_get_reason_canceled(SPXAUDIOSTREAMHANDLE haudioStream, Result_CancellationReason* reason)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, reason == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        *reason = (Result_CancellationReason)stream->GetCancellationReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_get_canceled_error_code(SPXAUDIOSTREAMHANDLE haudioStream, Result_CancellationErrorCode* errorCode)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, errorCode == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        auto error = stream->GetError();
        auto streamErrorCode = error != nullptr ? error->GetCancellationCode() : CancellationErrorCode::NoError;
        *errorCode = static_cast<Result_CancellationErrorCode>(streamErrorCode);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) audio_data_stream_can_read_data(SPXAUDIOSTREAMHANDLE haudioStream, uint32_t requestedSize)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        return stream->CanReadData(requestedSize);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false);
}

SPXAPI_(bool) audio_data_stream_can_read_data_from_position(SPXAUDIOSTREAMHANDLE haudioStream, uint32_t requestedSize, uint32_t position)
{
    auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);

    return stream->CanReadData(requestedSize, position);
}

SPXAPI audio_data_stream_read(SPXAUDIOSTREAMHANDLE haudioStream, uint8_t* buffer, uint32_t bufferSize, uint32_t* pfilledSize)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pfilledSize == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        *pfilledSize = stream->Read(buffer, bufferSize);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_read_from_position(SPXAUDIOSTREAMHANDLE haudioStream, uint8_t* buffer, uint32_t bufferSize, uint32_t position, uint32_t* pfilledSize)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pfilledSize == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, buffer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        *pfilledSize = stream->Read(buffer, bufferSize, position);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_save_to_wave_file(SPXAUDIOSTREAMHANDLE haudioStream, const char* fileName)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, fileName == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        stream->SaveToWaveFile(PAL::ToWString(fileName).c_str());
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_get_position(SPXAUDIOSTREAMHANDLE haudioStream, uint32_t* position)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, position == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        *position = stream->GetPosition();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_set_position(SPXAUDIOSTREAMHANDLE haudioStream, uint32_t position)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = GetInstance<ISpxAudioDataStream>(haudioStream);
        stream->SetPosition(position);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_detach_input(SPXAUDIOSTREAMHANDLE audioStreamHandle)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto wrapper = QueryInterfaceFromHandle<ISpxAudioDataStream, ISpxAudioDataStreamWrapper>(audioStreamHandle);
        if (wrapper)
        {
            wrapper->DetachInput();
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_data_stream_get_property_bag(SPXAUDIOSTREAMHANDLE haudioStream, SPXPROPERTYBAGHANDLE* hpropbag)
{
    return GetTargetObjectByInterface<ISpxAudioDataStream, ISpxNamedProperties>(haudioStream, hpropbag);
}

SPXAPI audio_data_stream_release(SPXAUDIOSTREAMHANDLE haudioStream)
{
    return Handle_Close<SPXAUDIOSTREAMHANDLE, ISpxAudioDataStream>(haudioStream);
}

SPXAPI push_audio_input_stream_set_property_by_id(SPXAUDIOSTREAMHANDLE haudioStream, int id, const char* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto pushStream = SpxQueryInterface<ISpxAudioStreamWriter>(stream);
        pushStream->SetProperty(static_cast<PropertyId>(id), value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI push_audio_input_stream_set_property_by_name(SPXAUDIOSTREAMHANDLE haudioStream, const char* name, const char* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto pushStream = SpxQueryInterface<ISpxAudioStreamWriter>(stream);
        pushStream->SetProperty(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


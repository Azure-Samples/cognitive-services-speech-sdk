//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_audio_stream.cpp: Public API definitions for audio stream related C methods and types

#include "stdafx.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) audio_stream_is_handle_valid(SPXAUDIOSTREAMHANDLE haudioStream)
{
    return Handle_IsValid<SPXAUDIOSTREAMHANDLE, ISpxAudioStream>(haudioStream);
}

SPXAPI audio_stream_create_push_audio_input_stream(SPXAUDIOSTREAMHANDLE* haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat)
{
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
            [=](uint8_t* buffer, uint32_t size) { return readCallback(pvContext, buffer, size); },
            [=]() { if (closeCallback != nullptr) { closeCallback(pvContext); } } );
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

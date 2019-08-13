//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_audio_config.cpp: Public API definitions for audio configuration C methods and types

#include "stdafx.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"
#include <assert.h>
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) audio_config_is_handle_valid(SPXAUDIOCONFIGHANDLE haudioConfig)
{
    return Handle_IsValid<SPXAUDIOCONFIGHANDLE, ISpxAudioConfig>(haudioConfig);
}

SPXAPI audio_config_create_audio_input_from_default_microphone(SPXAUDIOCONFIGHANDLE* haudioConfig)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioConfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxGetRootSite());
        config->InitFromDefaultDevice();

        *haudioConfig = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_audio_input_from_a_microphone(SPXAUDIOCONFIGHANDLE* haudioConfig, const char* deviceName)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioConfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxGetRootSite());
        auto properties = SpxQueryService<ISpxNamedProperties>(config);
        properties->SetStringValue(GetPropertyName(PropertyId::AudioConfig_DeviceNameForCapture), deviceName);

        *haudioConfig = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_audio_input_from_wav_file_name(SPXAUDIOCONFIGHANDLE* haudioConfig, const char* fileName)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, fileName == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioConfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxGetRootSite());
        config->InitFromFile(PAL::ToWString(fileName).c_str());

        *haudioConfig = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_audio_input_from_stream(SPXAUDIOCONFIGHANDLE* haudioConfig, SPXAUDIOSTREAMHANDLE haudioStream)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioConfig = SPXHANDLE_INVALID;

        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto config = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxGetRootSite());
        config->InitFromStream(stream);

        *haudioConfig = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_push_audio_input_stream(SPXAUDIOCONFIGHANDLE* haudioConfig, SPXAUDIOSTREAMHANDLE* haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_THROW_ON_FAIL(audio_stream_create_push_audio_input_stream(haudioStream, hformat));
        SPX_THROW_ON_FAIL(audio_config_create_audio_input_from_stream(haudioConfig, *haudioStream));
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_pull_audio_input_stream(SPXAUDIOCONFIGHANDLE* haudioConfig, SPXAUDIOSTREAMHANDLE* haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_THROW_ON_FAIL(audio_stream_create_pull_audio_input_stream(haudioStream, hformat));
        SPX_THROW_ON_FAIL(audio_config_create_audio_input_from_stream(haudioConfig, *haudioStream));
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_audio_output_from_default_speaker(SPXAUDIOCONFIGHANDLE* haudioConfig)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioConfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxGetRootSite());
        config->InitFromDefaultDevice();

        *haudioConfig = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_audio_output_from_wav_file_name(SPXAUDIOCONFIGHANDLE* haudioConfig, const char* fileName)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, fileName == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioConfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxGetRootSite());
        config->InitFromFile(PAL::ToWString(fileName).c_str());

        *haudioConfig = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_create_audio_output_from_stream(SPXAUDIOCONFIGHANDLE* haudioConfig, SPXAUDIOSTREAMHANDLE haudioStream)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioConfig == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, haudioStream == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *haudioConfig = SPXHANDLE_INVALID;

        auto stream = CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioStream, SPXAUDIOSTREAMHANDLE>(haudioStream);
        auto config = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxGetRootSite());
        config->InitFromStream(stream);

        *haudioConfig = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI audio_config_release(SPXAUDIOCONFIGHANDLE haudioConfig)
{
    return Handle_Close<SPXAUDIOCONFIGHANDLE, ISpxAudioConfig>(haudioConfig);
}

SPXAPI audio_config_get_property_bag(SPXAUDIOCONFIGHANDLE haudioConfig, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto audioconfigs = CSpxSharedPtrHandleTableManager::Get<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>();
        auto audioconfig = (*audioconfigs)[haudioConfig];
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, audioconfig == nullptr);
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(audioconfig);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

// FUTURE DEVELOPMENT: The config method group could be extended in the future to support additional scenarios, for example:
//
//    SPXAPI audio_config_create_audio_input_from_url(SPXAUDIOCONFIGHANDLE* haudioConfig, const char* url);
//    SPXAPI audio_config_create_audio_input_from_url_stream(SPXAUDIOCONFIGHANDLE* haudioConfig, const char* url, SPXAUDIOSTREAMFORMATHANDLE hformat);
//    SPXAPI audio_config_create_audio_input_from_file_stream(SPXAUDIOCONFIGHANDLE* haudioConfig, const char* fileName, SPXAUDIOSTREAMFORMATHANDLE hformat);
//    SPXAPI audio_config_create_audio_input_from_device(SPXAUDIOCONFIGHANDLE* haudioConfig, SPXAUDIODEVICEHANDLE* hdevice);

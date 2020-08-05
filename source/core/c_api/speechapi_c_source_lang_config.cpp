//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "common.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"
#include "handle_table.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI source_lang_config_from_language(SPXSOURCELANGCONFIGHANDLE* hconfig, const char* language)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, language == nullptr || !(*language));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSourceLanguageConfig>("CSpxSourceLanguageConfig", SpxGetRootSite());
        config->InitFromLanguage(language);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSourceLanguageConfig, SPXSOURCELANGCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI source_lang_config_from_language_and_endpointId(SPXSOURCELANGCONFIGHANDLE* hconfig, const char* language, const char* endpointId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, language == nullptr || !(*language));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, endpointId == nullptr || !(*endpointId));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSourceLanguageConfig>("CSpxSourceLanguageConfig", SpxGetRootSite());
        config->InitFromLanguageAndEndpointId(language, endpointId);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSourceLanguageConfig, SPXSOURCELANGCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) source_lang_config_is_handle_valid(SPXSOURCELANGCONFIGHANDLE hconfig)
{
    return Handle_IsValid<SPXSOURCELANGCONFIGHANDLE, ISpxSourceLanguageConfig>(hconfig);
}

SPXAPI source_lang_config_release(SPXSOURCELANGCONFIGHANDLE hconfig)
{
    return Handle_Close<SPXSOURCELANGCONFIGHANDLE, ISpxSourceLanguageConfig>(hconfig);
}

SPXAPI source_lang_config_get_property_bag(SPXSOURCELANGCONFIGHANDLE hconfig, SPXPROPERTYBAGHANDLE* hpropbag)
{
    return GetTargetObjectByInterface<ISpxSourceLanguageConfig, ISpxNamedProperties>(hconfig, hpropbag);
}

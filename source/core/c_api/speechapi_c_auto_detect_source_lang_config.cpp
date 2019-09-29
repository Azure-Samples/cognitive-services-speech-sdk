//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "speechapi_c_auto_detect_source_lang_config.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"
#include "handle_table.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI auto_detect_source_lang_config_from_languages(SPXAUTODETECTSOURCELANGCONFIGHANDLE* hconfig, const char* languages)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, languages == nullptr || !(*languages));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto autoDetectSourceLangConfig = SpxCreateObjectWithSite<ISpxAutoDetectSourceLangConfig>("CSpxAutoDetectSourceLangConfig", SpxGetRootSite());
        autoDetectSourceLangConfig->InitFromLanguages(languages);

        auto autoDetectSourceLangConfigs = CSpxSharedPtrHandleTableManager::Get<ISpxAutoDetectSourceLangConfig, SPXAUTODETECTSOURCELANGCONFIGHANDLE>();
        *hconfig = autoDetectSourceLangConfigs->TrackHandle(autoDetectSourceLangConfig);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) auto_detect_source_lang_config_is_handle_valid(SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig)
{
    return Handle_IsValid<SPXAUTODETECTSOURCELANGCONFIGHANDLE, ISpxAutoDetectSourceLangConfig>(hconfig);
}

SPXAPI auto_detect_source_lang_config_release(SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig)
{
    return Handle_Close<SPXAUTODETECTSOURCELANGCONFIGHANDLE, ISpxAutoDetectSourceLangConfig>(hconfig);
}

SPXAPI auto_detect_source_lang_config_get_property_bag(SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto configs = CSpxSharedPtrHandleTableManager::Get<ISpxAutoDetectSourceLangConfig, SPXAUTODETECTSOURCELANGCONFIGHANDLE>();
        auto config = (*configs)[hconfig];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(config);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);

    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

#include "stdafx.h"
#include "speechapi_c_speech_config.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"
#include "handle_table.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI_(bool) speech_config_is_handle_valid(SPXSPEECHCONFIGHANDLE hconfig)
{
    return Handle_IsValid<SPXSPEECHCONFIGHANDLE, ISpxSpeechConfig>(hconfig);
}

SPXAPI speech_config_from_subscription(SPXSPEECHCONFIGHANDLE* hconfig, const char* subscription, const char* region)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscription == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, region == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>("CSpxSpeechConfig", SpxGetRootSite());
        config->InitFromSubscription(subscription, region);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_config_from_authorization_token(SPXSPEECHCONFIGHANDLE* hconfig, const char* authToken, const char* region)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, authToken == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, region == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>("CSpxSpeechConfig", SpxGetRootSite());
        config->InitAuthorizationToken(authToken, region);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_config_from_endpoint(SPXSPEECHCONFIGHANDLE* hconfig, const char* endpoint, const char* subscription)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, endpoint == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscription == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>("CSpxSpeechConfig", SpxGetRootSite());
        config->InitFromEndpoint(endpoint, subscription);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_config_release(SPXSPEECHCONFIGHANDLE hconfig)
{
    return Handle_Close<SPXSPEECHCONFIGHANDLE, ISpxSpeechConfig>(hconfig);
}

SPXAPI speech_config_get_property_bag(SPXSPEECHCONFIGHANDLE hconfig, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto configs = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto config = (*configs)[hconfig];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(config);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);

    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


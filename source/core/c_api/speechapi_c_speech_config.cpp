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
#include "audio_format_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI speech_config_from_subscription_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* subscription, const char* region, const char* classname)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscription == nullptr || !(*subscription));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, region == nullptr || !(*region));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>(classname, SpxGetRootSite());
        config->InitFromSubscription(subscription, region);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_config_from_authorization_token_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* authToken, const char* region, const char* classname)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, authToken == nullptr || !(*authToken));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, region == nullptr || !(*region));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>(classname, SpxGetRootSite());
        config->InitAuthorizationToken(authToken, region);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_config_from_endpoint_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* endpoint, const char* subscription, const char* classname)
{
    // subscription can be null.
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, endpoint == nullptr || !(*endpoint));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>(classname, SpxGetRootSite());
        config->InitFromEndpoint(endpoint, subscription);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_config_from_host_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* host, const char* subscription, const char* classname)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, host == nullptr || !(*host));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconfig == nullptr);
    // subscription can be null.

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hconfig = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>(classname, SpxGetRootSite());
        config->InitFromHost(host, subscription);

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        *hconfig = confighandles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) speech_config_is_handle_valid(SPXSPEECHCONFIGHANDLE hconfig)
{
    return Handle_IsValid<SPXSPEECHCONFIGHANDLE, ISpxSpeechConfig>(hconfig);
}

SPXAPI speech_config_from_subscription(SPXSPEECHCONFIGHANDLE* hconfig, const char* subscription, const char* region)
{
    return speech_config_from_subscription_internal(hconfig, subscription, region, "CSpxSpeechConfig");
}

SPXAPI speech_config_from_authorization_token(SPXSPEECHCONFIGHANDLE* hconfig, const char* authToken, const char* region)
{
    return speech_config_from_authorization_token_internal(hconfig, authToken, region, "CSpxSpeechConfig");
}

SPXAPI speech_config_from_endpoint(SPXSPEECHCONFIGHANDLE* hconfig, const char* endpoint, const char* subscription)
{
    return speech_config_from_endpoint_internal(hconfig, endpoint, subscription, "CSpxSpeechConfig");
}

SPXAPI speech_config_from_host(SPXSPEECHCONFIGHANDLE* hconfig, const char* host, const char* subscription)
{
    return speech_config_from_host_internal(hconfig, host, subscription, "CSpxSpeechConfig");
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

SPXAPI speech_config_set_audio_output_format(SPXSPEECHCONFIGHANDLE hconfig, Speech_Synthesis_Output_Format formatId)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto configs = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto config = (*configs)[hconfig];
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(config);
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthOutputFormat), GetAudioFormatName(static_cast<SpeechSynthesisOutputFormat>(formatId)));
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

static_assert((int)SpeechConfig_ServicePropertyChannel_UriQueryParameter == (int)ServicePropertyChannel::UriQueryParameter, "SpeechConfig_ServicePropertyChannel_* enum values == ServicePropertyChannel::* enum values");

SPXAPI speech_config_set_service_property(SPXSPEECHCONFIGHANDLE configHandle, const char* propertyName, const char* propertyValue, SpeechConfig_ServicePropertyChannel channel)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, propertyName == nullptr || propertyName[0] == '\0');
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, propertyValue == nullptr || propertyValue[0] == '\0');
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, channel != SpeechConfig_ServicePropertyChannel_UriQueryParameter);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto configs = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto config = (*configs)[configHandle];
        config->SetServiceProperty(propertyName, propertyValue, (ServicePropertyChannel)channel);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

static_assert((int)SpeechConfig_ProfanityMasked == (int)ProfanityOption::Masked, "Profanity_* enum values == ProfanityOption::* enum values");
static_assert((int)SpeechConfig_ProfanityRemoved == (int)ProfanityOption::Removed, "Profanity_* enum values == ProfanityOption::* enum values");
static_assert((int)SpeechConfig_ProfanityRaw == (int)ProfanityOption::Raw, "Profanity_* enum values == ProfanityOption::* enum values");

SPXAPI speech_config_set_profanity(SPXSPEECHCONFIGHANDLE configHandle, SpeechConfig_ProfanityOption profanity)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto configs = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto config = (*configs)[configHandle];
        config->SetProfanity((ProfanityOption)profanity);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

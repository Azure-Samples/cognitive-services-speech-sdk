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

SPXAPI_(bool) speech_config_is_handle_valid(SPXSPEECHCONFIGHANDLE hconfig)
{
    return Handle_IsValid<SPXSPEECHCONFIGHANDLE, ISpxSpeechConfig>(hconfig);
}

SPXAPI speech_config_from_subscription(SPXSPEECHCONFIGHANDLE* hconfig, const char* subscription, const char* region)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscription == nullptr || !(*subscription));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, region == nullptr || !(*region));
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
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, authToken == nullptr || !(*authToken));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, region == nullptr || !(*region));
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
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, endpoint == nullptr || !(*endpoint));
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

SPXAPI speech_config_set_audio_output_format(SPXSPEECHCONFIGHANDLE hconfig, Speech_Synthesis_Output_Format formatId)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPXPROPERTYBAGHANDLE hpropbag = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_get_property_bag(hconfig, &hpropbag));
        auto formatName = GetAudioFormatName(static_cast<SpeechSynthesisOutputFormat>(formatId));
        SPX_THROW_ON_FAIL(property_bag_set_string(hpropbag, static_cast<int>(PropertyId::SpeechServiceConnection_SynthOutputFormat), nullptr, formatName));
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

static_assert((int)SpeechConfig_ServicePropertyChannel_UriQueryParameter == (int)ServicePropertyChannel::UriQueryParameter, "SpeechConfig_ServicePropertyChannel_* enum values == ServicePropertyChannel::* enum values");

SPXAPI speech_config_set_service_property(SPXSPEECHCONFIGHANDLE configHandle, const char* propertyName, const char* propertyValue, SpeechConfig_ServicePropertyChannel channel)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(propertyName == nullptr || propertyName[0] == '\0', SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(propertyValue == nullptr || propertyValue[0] == '\0', SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(channel != SpeechConfig_ServicePropertyChannel_UriQueryParameter, SPXERR_INVALID_ARG);

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

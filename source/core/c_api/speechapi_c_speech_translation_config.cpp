#include "stdafx.h"
#include "speechapi_c_speech_translation_config.h"
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "handle_table.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI speech_config_from_subscription_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* subscription, const char* region, const char* classname);
SPXAPI speech_config_from_authorization_token_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* authToken, const char* region, const char* classname);
SPXAPI speech_config_from_endpoint_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* endpoint, const char* subscription, const char* classname);
SPXAPI speech_config_from_host_internal(SPXSPEECHCONFIGHANDLE* hconfig, const char* host, const char* subscription, const char* classname);

SPXAPI speech_translation_config_from_subscription(SPXSPEECHCONFIGHANDLE* configHandle, const char* subscription, const char* region)
{
    return speech_config_from_subscription_internal(configHandle, subscription, region, "CSpxSpeechTranslationConfig");
}

SPXAPI speech_translation_config_from_authorization_token(SPXSPEECHCONFIGHANDLE* configHandle, const char* authToken, const char* region)
{
    return speech_config_from_authorization_token_internal(configHandle, authToken, region, "CSpxSpeechTranslationConfig");
}

SPXAPI speech_translation_config_from_endpoint(SPXSPEECHCONFIGHANDLE* configHandle, const char* endpoint, const char* subscription)
{
    return speech_config_from_endpoint_internal(configHandle, endpoint, subscription, "CSpxSpeechTranslationConfig");
}

SPXAPI speech_translation_config_from_host(SPXSPEECHCONFIGHANDLE* configHandle, const char* host, const char* subscription)
{
    return speech_config_from_host_internal(configHandle, host, subscription, "CSpxSpeechTranslationConfig");
}

SPXAPI speech_translation_config_add_target_language(SPXSPEECHCONFIGHANDLE configHandle, const char* language)
{
    SPX_IFTRUE_RETURN_HR(language == nullptr, SPXERR_INVALID_ARG);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto configs = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto config = (*configs)[configHandle];

        auto translationConfig = SpxQueryInterface<ISpxSpeechTranslationConfig>(config);
        SPX_IFTRUE_THROW_HR(translationConfig == nullptr, SPXERR_INVALID_ARG);

        translationConfig->AddTargetLanguage(language);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_translation_config_remove_target_language(SPXSPEECHCONFIGHANDLE configHandle, const char* language)
{
    SPX_IFTRUE_RETURN_HR(language == nullptr, SPXERR_INVALID_ARG);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto configs = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto config = (*configs)[configHandle];

        auto translationConfig = SpxQueryInterface<ISpxSpeechTranslationConfig>(config);
        SPX_IFTRUE_THROW_HR(translationConfig == nullptr, SPXERR_INVALID_ARG);

        translationConfig->RemoveTargetLanguage(language);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

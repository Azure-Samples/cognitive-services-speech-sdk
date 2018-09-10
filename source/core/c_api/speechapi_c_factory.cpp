//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_factory.cpp: Definitions for SpeechFactory related C methods
//

#include <string>
#include <vector>

#include "stdafx.h"
#include "service_helpers.h"
#include "site_helpers.h"
#include "handle_helpers.h"
#include "resource_manager.h"
#include "mock_controller.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;

static_assert((int)OutputFormat::Simple == (int)SpeechOutputFormat_Simple, "OutputFormat should match between C and C++ layers");
static_assert((int)OutputFormat::Detailed == (int)SpeechOutputFormat_Detailed, "OutputFormat should match between C and C++ layers");


SPXAPI_(bool) speech_factory_handle_is_valid(SPXFACTORYHANDLE hfactory)
{
    return Handle_IsValid<SPXFACTORYHANDLE, ISpxSpeechApiFactory>(hfactory);
}

SPXAPI speech_factory_handle_close(SPXFACTORYHANDLE hfactory)
{
    return Handle_Close<SPXFACTORYHANDLE, ISpxSpeechApiFactory>(hfactory);
}

std::shared_ptr<ISpxAudioConfig> AudioConfigFromHandleOrEmptyIfInvalid(SPXAUDIOCONFIGHANDLE haudioConfig)
{
    return audio_config_is_handle_valid(haudioConfig)
        ? CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(haudioConfig)
        : nullptr;
}

SPXAPI speech_factory_create_speech_recognizer_from_config(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, SpeechOutputFormat format, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;

        auto factory = CSpxSharedPtrHandleTableManager::GetPtr<ISpxSpeechApiFactory, SPXFACTORYHANDLE>(hfactory);
        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);
        auto recognizer = factory->CreateSpeechRecognizerFromConfig(pszLanguage, (OutputFormat)format, audioInput);

        *phreco = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxRecognizer, SPXRECOHANDLE>(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_factory_create_intent_recognizer_from_config(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, SpeechOutputFormat format, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;

        auto factory = CSpxSharedPtrHandleTableManager::GetPtr<ISpxSpeechApiFactory, SPXFACTORYHANDLE>(hfactory);
        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);
        auto recognizer = factory->CreateIntentRecognizerFromConfig(pszLanguage, (OutputFormat)format, audioInput);

        *phreco = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxRecognizer, SPXRECOHANDLE>(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

inline vector<string> TargetLanguagesVectorFromArray(const char* buffer[], size_t entries)
{
    vector<string> result;
    if (buffer != nullptr)
    {
        for (size_t i = 0; i < entries; i++)
        {
            result.push_back(buffer[i]);
        }
    }

    return result;
}

SPXAPI speech_factory_create_translation_recognizer_from_config(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;

        auto factory = CSpxSharedPtrHandleTableManager::GetPtr<ISpxSpeechApiFactory, SPXFACTORYHANDLE>(hfactory);
        auto toLangs = TargetLanguagesVectorFromArray(targetLanguages, numberOfTargetLanguages);
        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);
        auto recognizer = factory->CreateTranslationRecognizerFromConfig(sourceLanguage, toLangs, voice, audioInput);

        *phreco = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxRecognizer, SPXRECOHANDLE>(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_factory_from_authorization_token(const char* authToken, const char* region, SPXFACTORYHANDLE* phfactory)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phfactory == nullptr);

    if (region == nullptr)
        return SPXERR_INVALID_ARG;

    if (authToken == nullptr)
        return SPXERR_INVALID_ARG;

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceAuthorization_Token), authToken);

        if (region != nullptr && *region != L'\0')
        {
            namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Region), region);
        }
        else
        {
            SPX_IFFAILED_THROW_HR(SPXERR_INVALID_ARG);
        }

        *phfactory = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxSpeechApiFactory, SPXFACTORYHANDLE>(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_factory_from_subscription(const char* subscriptionKey, const char* region, SPXFACTORYHANDLE* phfactory)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phfactory == nullptr);
    if (region == nullptr)
        return SPXERR_INVALID_ARG;

    if (subscriptionKey == nullptr)
        return SPXERR_INVALID_ARG;

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Key), subscriptionKey);

        if (region != nullptr && *region != L'\0')
        {
            namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Region), region);
        }
        else
        {
            SPX_IFFAILED_THROW_HR(SPXERR_INVALID_ARG);
        }

        *phfactory = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxSpeechApiFactory, SPXFACTORYHANDLE>(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_factory_from_endpoint(const char* endpoint, const char* subscription, SPXFACTORYHANDLE* phfactory)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phfactory == nullptr);
    if (endpoint == nullptr)
        return SPXERR_INVALID_ARG;

    if (subscription == nullptr)
        return SPXERR_INVALID_ARG;

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Endpoint), endpoint);

        if (subscription != nullptr && *subscription != L'\0')
        {
            namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Key), subscription);
        }
        else
        {
            SPX_IFFAILED_THROW_HR(SPXERR_INVALID_ARG);
        }

        *phfactory = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxSpeechApiFactory, SPXFACTORYHANDLE>(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speech_factory_get_property_bag(SPXFACTORYHANDLE hfactory, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *hpropbag = SPXHANDLE_INVALID;

        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);

        *hpropbag = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

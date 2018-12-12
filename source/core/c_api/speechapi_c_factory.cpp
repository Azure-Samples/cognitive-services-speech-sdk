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
#include "speechapi_c_speech_config.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;

static_assert((int)OutputFormat::Simple == (int)SpeechOutputFormat_Simple, "OutputFormat should match between C and C++ layers");
static_assert((int)OutputFormat::Detailed == (int)SpeechOutputFormat_Detailed, "OutputFormat should match between C and C++ layers");


std::shared_ptr<ISpxAudioConfig> AudioConfigFromHandleOrEmptyIfInvalid(SPXAUDIOCONFIGHANDLE haudioConfig)
{
    return audio_config_is_handle_valid(haudioConfig)
        ? CSpxSharedPtrHandleTableManager::GetPtr<ISpxAudioConfig, SPXAUDIOCONFIGHANDLE>(haudioConfig)
        : nullptr;
}

SPXAPI recognizer_create_speech_recognizer_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;

        // get the input parameters from the hspeechconfig
        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto speechconfig = (*confighandles)[hspeechconfig];
        auto speechconfig_propertybag = SpxQueryInterface<ISpxNamedProperties>(speechconfig);
        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        //copy the properties from the speech config into the factory
        auto fbag = SpxQueryInterface<ISpxNamedProperties>(factory);
        fbag->Copy(speechconfig_propertybag.get());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(speechconfig);
        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);

        auto recoLanguage = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
        auto outputFormat = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse));
        OutputFormat format = PAL::stricmp(outputFormat.c_str(), PAL::BoolToString(true).c_str()) == 0 ? OutputFormat::Detailed : OutputFormat::Simple;

        auto recognizer = factory->CreateSpeechRecognizerFromConfig(recoLanguage.c_str(), format, audioInput);

        // track the reco handle
        auto recohandles  = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_create_translation_recognizer_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;

        std::shared_ptr<ISpxRecognizer> recognizer;

        // create a factory
        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto speechconfig = (*confighandles)[hspeechconfig];

        //copy the properties from the speech config into the factory
        auto speechconfig_propertybag = SpxQueryInterface<ISpxNamedProperties>(speechconfig);
        auto fbag = SpxQueryInterface<ISpxNamedProperties>(factory);
        fbag->Copy(speechconfig_propertybag.get());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(speechconfig);
        auto source_lang = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
        auto voice = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationVoice));

        // language names are separated by comma
        auto to_langs = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages));
        auto vlangs = PAL::split(to_langs, ",");

        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);
        recognizer = factory->CreateTranslationRecognizerFromConfig(source_lang, vlangs, voice, audioInput);

        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_create_intent_recognizer_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;

        // create a factory
        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto speechconfig = (*confighandles)[hspeechconfig];
        auto speechconfig_propertybag = SpxQueryInterface<ISpxNamedProperties>(speechconfig);
        auto fbag = SpxQueryInterface<ISpxNamedProperties>(factory);
        fbag->Copy(speechconfig_propertybag.get());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(speechconfig);
        auto lang = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
        auto outputFormat = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse));
        OutputFormat format = PAL::stricmp(outputFormat.c_str(), PAL::BoolToString(true).c_str()) == 0 ? OutputFormat::Detailed : OutputFormat::Simple;

        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);
        std::shared_ptr<ISpxRecognizer> recognizer = factory->CreateIntentRecognizerFromConfig(lang.c_str(), format, audioInput);

        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

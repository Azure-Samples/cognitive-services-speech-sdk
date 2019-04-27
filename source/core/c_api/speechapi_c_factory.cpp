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

template<typename FactoryMethod>
auto create_from_config(SPXHANDLE config_handle, SPXHANDLE audio_config_handle, FactoryMethod fm)
{
    auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

    // get the input parameters from the hspeechconfig
    auto config_handles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();

    auto config = (*config_handles)[config_handle];
    auto config_property_bag = SpxQueryInterface<ISpxNamedProperties>(config);
    auto factory_property_bag = SpxQueryInterface<ISpxNamedProperties>(factory);

    //copy the properties from the speech config into the factory
    if (config_property_bag != nullptr)
    {
        factory_property_bag->Copy(config_property_bag.get());
    }

    auto audio_input = AudioConfigFromHandleOrEmptyIfInvalid(audio_config_handle);
    // copy the audio input properties into the factory, if any.
    auto audio_input_properties = SpxQueryInterface<ISpxNamedProperties>(audio_input);
    if (audio_input_properties != nullptr)
    {
        factory_property_bag->Copy(audio_input_properties.get());
    }

    return ((*factory).*fm)(audio_input);
}

SPXAPI recognizer_create_speech_recognizer_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = create_from_config(hspeechconfig, haudioInput, &ISpxSpeechApiFactory::CreateSpeechRecognizerFromConfig);

        // track the reco handle
        auto recohandles  = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_connector_create_speech_bot_connector_from_config(SPXRECOHANDLE* ph_bot_connector, SPXSPEECHCONFIGHANDLE h_bot_config, SPXAUDIOCONFIGHANDLE h_audio_input)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_bot_connector == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(h_bot_config));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_bot_connector = SPXHANDLE_INVALID;
        auto connector = create_from_config(h_bot_config, h_audio_input, &ISpxSpeechApiFactory::CreateSpeechBotConnectorFromConfig);

        // track the handle
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechBotConnector, SPXRECOHANDLE>();
        *ph_bot_connector = handles->TrackHandle(connector);

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

        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);
        // copy the audio input properties into the factory, if any.
        auto audioInput_propertybag = SpxQueryInterface<ISpxNamedProperties>(audioInput);
        if (audioInput_propertybag != nullptr)
        {
            fbag->Copy(audioInput_propertybag.get());
        }
        recognizer = factory->CreateTranslationRecognizerFromConfig(audioInput);

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
        auto recognizer = create_from_config(hspeechconfig, haudioInput, &ISpxSpeechApiFactory::CreateIntentRecognizerFromConfig);

        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_create_speech_synthesizer_from_config(SPXSYNTHHANDLE* phsynth, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioconfig)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phsynth == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phsynth = SPXHANDLE_INVALID;

        // get the speech synthesis related parameters from the hspeechconfig
        auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto speechconfig = (*confighandles)[hspeechconfig];
        auto speechconfig_propertybag = SpxQueryInterface<ISpxNamedProperties>(speechconfig);
        auto factory = SpxCreateObjectWithSite<ISpxSpeechSynthesisApiFactory>("CSpxSpeechSynthesisApiFactory", SpxGetRootSite());

        // copy the properties from the speech config into the factory
        auto fbag = SpxQueryInterface<ISpxNamedProperties>(factory);
        fbag->Copy(speechconfig_propertybag.get());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(speechconfig);
        auto synthLanguage = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthLanguage));
        auto synthVoice = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthVoice));
        auto synthOutputFormat = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthOutputFormat));

        auto audioOutput = AudioConfigFromHandleOrEmptyIfInvalid(haudioconfig);

        auto synthesizer = factory->CreateSpeechSynthesizerFromConfig(synthLanguage.c_str(), synthVoice.c_str(), synthOutputFormat.c_str(), audioOutput);

        // track the synth handle
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        *phsynth = synthhandles->TrackHandle(synthesizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


SPXAPI recognizer_create_conversation_transcriber_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput)
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
        if (speechconfig_propertybag != nullptr)
        {
            fbag->Copy(speechconfig_propertybag.get());
        }

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(speechconfig);
        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioInput);
        // copy the audio input properties into the factory, if any.
        auto audioInput_propertybag = SpxQueryInterface<ISpxNamedProperties>(audioInput);
        if (audioInput_propertybag != nullptr)
        {
            fbag->Copy(audioInput_propertybag.get());
        }
        auto recognizer = factory->CreateConversationTranscriberFromConfig(audioInput);

        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}



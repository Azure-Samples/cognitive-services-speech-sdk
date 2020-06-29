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
#include "speechapi_c_auto_detect_source_lang_config.h"
#include "speechapi_c_source_lang_config.h"
#include "memory_utils.h"
#include "ISpxConversationInterfaces.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;

static_assert((int)OutputFormat::Simple == (int)SpeechOutputFormat_Simple, "OutputFormat should match between C and C++ layers");
static_assert((int)OutputFormat::Detailed == (int)SpeechOutputFormat_Detailed, "OutputFormat should match between C and C++ layers");

/*
 * In the end there will only be once instance of this template as all the functions have
 * the same signature and all the handle types are aliases of SPXHANDLE
 */
template<typename T, typename Function, typename Handle>
std::shared_ptr<T> ObjectOrEmptyIfInvalid(Function fn, Handle handle)
{
    return fn(handle) ? CSpxSharedPtrHandleTableManager::GetPtr<T, Handle>(handle) : nullptr;
}

std::shared_ptr<ISpxSpeechConfig> SpeechConfigFromHandleOrEmptyIfInvalid(SPXSPEECHCONFIGHANDLE hconfig)
{
    return ObjectOrEmptyIfInvalid<ISpxSpeechConfig>(speech_config_is_handle_valid, hconfig);
}

std::shared_ptr<ISpxAudioConfig> AudioConfigFromHandleOrEmptyIfInvalid(SPXAUDIOCONFIGHANDLE haudioConfig)
{
    return ObjectOrEmptyIfInvalid<ISpxAudioConfig>(audio_config_is_handle_valid, haudioConfig);
}

std::shared_ptr<ISpxAutoDetectSourceLangConfig> AutoDetectSourceLangConfigFromHandleOrEmptyIfInvalid(SPXAUTODETECTSOURCELANGCONFIGHANDLE hautoDetectSourceLangConfig)
{
    return ObjectOrEmptyIfInvalid<ISpxAutoDetectSourceLangConfig>(
        auto_detect_source_lang_config_is_handle_valid,
        hautoDetectSourceLangConfig);
}

std::shared_ptr<ISpxSourceLanguageConfig> SourceLangConfigFromHandleOrEmptyIfInvalid(SPXSOURCELANGCONFIGHANDLE hSourceLangConfig)
{
    return ObjectOrEmptyIfInvalid<ISpxSourceLanguageConfig>(
        source_lang_config_is_handle_valid,
        hSourceLangConfig);
}

template<typename FactoryMethod>
auto create_from_config(SPXHANDLE hspeechconfig, SPXHANDLE hautoDetectSourceLangConfig, SPXHANDLE hSourceLangConfig, SPXHANDLE haudioConfig, FactoryMethod fm)
{
    auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());
    SPX_IFTRUE_THROW_HR(factory == nullptr, SPXERR_RUNTIME_ERROR);

    auto factory_property_bag = SpxQueryInterface<ISpxNamedProperties>(factory);

    auto config = SpeechConfigFromHandleOrEmptyIfInvalid(hspeechconfig);
    auto config_property_bag = SpxQueryInterface<ISpxNamedProperties>(config);

    if (config != nullptr)
    {
        Memory::CheckObjectCount(hspeechconfig);

        //copy the properties from the speech config into the factory
        if (config_property_bag != nullptr)
        {
            factory_property_bag->Copy(config_property_bag.get());
        }
    }

    auto audio_input = AudioConfigFromHandleOrEmptyIfInvalid(haudioConfig);
    // copy the audio input properties into the factory, if any.
    auto audio_input_properties = SpxQueryInterface<ISpxNamedProperties>(audio_input);
    if (audio_input_properties != nullptr)
    {
        factory_property_bag->Copy(audio_input_properties.get());
    }

    auto auto_detect_source_lang_config = AutoDetectSourceLangConfigFromHandleOrEmptyIfInvalid(hautoDetectSourceLangConfig);
    // copy the auto detect source language config properties into the factory, if any.
    auto auto_detect_source_lang_config_properties = SpxQueryInterface<ISpxNamedProperties>(auto_detect_source_lang_config);
    if (auto_detect_source_lang_config_properties != nullptr)
    {
        if (config_property_bag != nullptr && config_property_bag->HasStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId)))
        {
            ThrowInvalidArgumentException("EndpointId on SpeechConfig is unsupported for auto detection source language scenario. "
                "Please set per language endpointId through SourceLanguageConfig and use it to construct AutoDetectSourceLanguageConfig.");
        }

        if (auto_detect_source_lang_config_properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages))
            == g_autoDetectSourceLang_OpenRange)
        {
            ThrowInvalidArgumentException("Recognizer doesn't support auto detection source language from open range. "
                "Please set specific languages using AutoDetectSourceLanguageConfig::FromLanguages() or AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs()");
        }
        factory_property_bag->Copy(auto_detect_source_lang_config_properties.get());
    }

    auto source_lang_config = SourceLangConfigFromHandleOrEmptyIfInvalid(hSourceLangConfig);
    // copy the source language config properties into the factory, if any.
    auto source_lang_config_properties = SpxQueryInterface<ISpxNamedProperties>(source_lang_config);
    if (source_lang_config_properties != nullptr)
    {
        factory_property_bag->Copy(source_lang_config_properties.get());
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
        auto recognizer = create_from_config(hspeechconfig, SPXHANDLE_INVALID, SPXHANDLE_INVALID, haudioInput, &ISpxSpeechApiFactory::CreateSpeechRecognizerFromConfig);

        // track the reco handle
        auto recohandles  = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_create_speech_recognizer_from_auto_detect_source_lang_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUTODETECTSOURCELANGCONFIGHANDLE hautoDetectSourceLangConfig, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !auto_detect_source_lang_config_is_handle_valid(hautoDetectSourceLangConfig));
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = create_from_config(hspeechconfig, hautoDetectSourceLangConfig, SPXHANDLE_INVALID, haudioInput, &ISpxSpeechApiFactory::CreateSpeechRecognizerFromConfig);
        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_create_speech_recognizer_from_source_lang_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXSOURCELANGCONFIGHANDLE hSourceLangConfig, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !source_lang_config_is_handle_valid(hSourceLangConfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = create_from_config(hspeechconfig, SPXHANDLE_INVALID, hSourceLangConfig, haudioInput, &ISpxSpeechApiFactory::CreateSpeechRecognizerFromConfig);
        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_connector_create_dialog_service_connector_from_config(SPXRECOHANDLE* ph_dialog_service_connector, SPXSPEECHCONFIGHANDLE h_dialog_service_config, SPXAUDIOCONFIGHANDLE h_audio_input)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_dialog_service_connector == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(h_dialog_service_config));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_dialog_service_connector = SPXHANDLE_INVALID;

        Memory::CheckObjectCount(h_dialog_service_config);

        // Enable keyword verification for dialog service connector by default
        auto config_handles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
        auto config = (*config_handles)[h_dialog_service_config];
        auto config_property_bag = SpxQueryInterface<ISpxNamedProperties>(config);
        auto enableKeywordVerification = config_property_bag->GetStringValue(KeywordConfig_EnableKeywordVerification, "true");
        config_property_bag->SetStringValue(KeywordConfig_EnableKeywordVerification, enableKeywordVerification.c_str());

        auto connector = create_from_config(h_dialog_service_config, SPXHANDLE_INVALID, SPXHANDLE_INVALID, h_audio_input, &ISpxSpeechApiFactory::CreateDialogServiceConnectorFromConfig);

        // track the handle
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxDialogServiceConnector, SPXRECOHANDLE>();
        *ph_dialog_service_connector = handles->TrackHandle(connector);

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

        Memory::CheckObjectCount(hspeechconfig);

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
        Memory::CheckObjectCount(hspeechconfig);

        *phreco = SPXHANDLE_INVALID;
        auto recognizer = create_from_config(hspeechconfig, SPXHANDLE_INVALID, SPXHANDLE_INVALID, haudioInput, &ISpxSpeechApiFactory::CreateIntentRecognizerFromConfig);

        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_create_keyword_recognizer_from_audio_config(SPXRECOHANDLE* phreco, SPXAUDIOCONFIGHANDLE haudio)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = create_from_config(SPXHANDLE_INVALID, SPXHANDLE_INVALID, SPXHANDLE_INVALID, haudio, &ISpxSpeechApiFactory::CreateSpeechRecognizerFromConfig);
        auto properties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        properties->SetStringValue(g_keyword_KeywordOnly, "true");
        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename FactoryMethod>
auto create_synthesizer_from_config(SPXHANDLE hspeechconfig, SPXHANDLE hautoDetectSourceLangConfig, SPXHANDLE haudioConfig, FactoryMethod fm)
{
    const auto factory = SpxCreateObjectWithSite<ISpxSpeechSynthesisApiFactory>("CSpxSpeechSynthesisApiFactory", SpxGetRootSite());
    SPX_IFTRUE_THROW_HR(factory == nullptr, SPXERR_RUNTIME_ERROR);

    auto factory_property_bag = SpxQueryInterface<ISpxNamedProperties>(factory);

    const auto config = SpeechConfigFromHandleOrEmptyIfInvalid(hspeechconfig);
    const auto config_property_bag = SpxQueryInterface<ISpxNamedProperties>(config);

    if (config != nullptr)
    {
        Memory::CheckObjectCount(hspeechconfig);

        //copy the properties from the speech config into the factory
        if (config_property_bag != nullptr)
        {
            factory_property_bag->Copy(config_property_bag.get());
        }
    }

    auto audio_output = AudioConfigFromHandleOrEmptyIfInvalid(haudioConfig);
    // copy the audio output properties into the factory, if any.
    const auto audio_output_properties = SpxQueryInterface<ISpxNamedProperties>(audio_output);
    if (audio_output_properties != nullptr)
    {
        factory_property_bag->Copy(audio_output_properties.get());
    }

    const auto auto_detect_source_lang_config = AutoDetectSourceLangConfigFromHandleOrEmptyIfInvalid(hautoDetectSourceLangConfig);
    // copy the auto detect source language config properties into the factory, if any.
    const auto auto_detect_source_lang_config_properties = SpxQueryInterface<ISpxNamedProperties>(auto_detect_source_lang_config);
    if (auto_detect_source_lang_config_properties != nullptr)
    {
        if (auto_detect_source_lang_config_properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages), "") != g_autoDetectSourceLang_OpenRange)
        {
            ThrowInvalidArgumentException("Auto detection source languages in SpeechSynthesizer doesn't support language range specification. "
                "Please use FromOpenRange to construct AutoDetectSourceLanguageConfig.");
        }
        factory_property_bag->Copy(auto_detect_source_lang_config_properties.get());
    }

    return ((*factory).*fm)(audio_output);
}

SPXAPI synthesizer_create_speech_synthesizer_from_config(SPXSYNTHHANDLE* phsynth, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioOutput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phsynth == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phsynth = SPXHANDLE_INVALID;
        const auto synthesizer = create_synthesizer_from_config(hspeechconfig, SPXHANDLE_INVALID, haudioOutput, &ISpxSpeechSynthesisApiFactory::CreateSpeechSynthesizerFromConfig);
        // track the synth handle
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        *phsynth = synthhandles->TrackHandle(synthesizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_create_speech_synthesizer_from_auto_detect_source_lang_config(SPXSYNTHHANDLE* phsynth, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUTODETECTSOURCELANGCONFIGHANDLE hautoDetectSourceLangConfig, SPXAUDIOCONFIGHANDLE haudioOutput)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phsynth == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !auto_detect_source_lang_config_is_handle_valid(hautoDetectSourceLangConfig));

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phsynth = SPXHANDLE_INVALID;
        const auto synthesizer = create_synthesizer_from_config(hspeechconfig, hautoDetectSourceLangConfig, haudioOutput, &ISpxSpeechSynthesisApiFactory::CreateSpeechSynthesizerFromConfig);
        // track the synth handle
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        *phsynth = synthhandles->TrackHandle(synthesizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

std::shared_ptr<ISpxSpeechApiFactory> create_factory_from_speech_config(SPXSPEECHCONFIGHANDLE hspeechconfig)
{
    if (!speech_config_is_handle_valid(hspeechconfig))
    {
        throw std::runtime_error("Invalid speechconfig handle.");
    }

    Memory::CheckObjectCount(hspeechconfig);

    // get the input parameters from the hspeechconfig
    auto confighandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();
    auto speechconfig = (*confighandles)[hspeechconfig];
    auto speechconfig_propertybag = SpxQueryInterface<ISpxNamedProperties>(speechconfig);
    auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());
    SPX_IFTRUE_THROW_HR(factory == nullptr, SPXERR_RUNTIME_ERROR);

    //copy the properties from the speech config into the factory
    auto fbag = SpxQueryInterface<ISpxNamedProperties>(factory);
    if (speechconfig_propertybag != nullptr)
    {
        fbag->Copy(speechconfig_propertybag.get());
    }

    return factory;
}

SPXAPI conversation_create_from_config(SPXCONVERSATIONHANDLE* pconversation, SPXSPEECHCONFIGHANDLE hspeechconfig, const char* id)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pconversation == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));
    // id is optional
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, id == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *pconversation = SPXHANDLE_INVALID;

        auto factory = create_factory_from_speech_config(hspeechconfig);

        auto conversation = factory->CreateConversationFromConfig(id);

        // track the conversation handle
        auto coversationhandles = CSpxSharedPtrHandleTableManager::Get<ISpxConversation, SPXCONVERSATIONHANDLE>();
        *pconversation = coversationhandles->TrackHandle(conversation);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_create_conversation_transcriber_from_config(SPXRECOHANDLE* phreco, SPXAUDIOCONFIGHANDLE haudioinput)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;

        auto conversation_transcriber = SpxCreateObject<ISpxRecognizer>("CSpxConversationTranscriber", SpxGetRootSite());

        // copy the audio input properties into the conversation transcriber
        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioinput);
        auto audioInput_propertybag = SpxQueryInterface<ISpxNamedProperties>(audioInput);
        auto propertybag_cts = SpxQueryInterface<ISpxNamedProperties>(conversation_transcriber);
        if (audioInput_propertybag != nullptr)
        {
            propertybag_cts->Copy(audioInput_propertybag.get());
        }

        auto conversation_transcriber_init = SpxQueryInterface<ISpxObjectWithAudioConfig>(conversation_transcriber);
        SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, conversation_transcriber_init == nullptr);
        conversation_transcriber_init->SetAudioConfig(audioInput);

        // track the conversation transcriber handle
        auto transcribers = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = transcribers->TrackHandle(conversation_transcriber);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_join_conversation(SPXCONVERSATIONHANDLE hconv, SPXRECOHANDLE hreco)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconv == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto convhandles = CSpxSharedPtrHandleTableManager::Get<ISpxConversation, SPXCONVERSATIONHANDLE>();
        auto conversation = (*convhandles)[hconv];

        auto transcribers = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto conversation_transcriber = (*transcribers)[hreco];

        auto factory = SpxQueryService<ISpxSpeechApiFactory>(conversation);
        SPX_IFTRUE_THROW_HR(factory == nullptr, SPXERR_RUNTIME_ERROR);

        auto session = SpxQueryService<ISpxSession>(conversation);
        SPX_IFTRUE_THROW_HR(session == nullptr, SPXERR_RUNTIME_ERROR);

        auto session_as_site = SpxQueryInterface<ISpxGenericSite>(session);

        auto conversation_transcriber_set_site = SpxQueryInterface<ISpxObjectWithSite>(conversation_transcriber);
        conversation_transcriber_set_site->SetSite(session_as_site);

        // hook audio input to session
        auto audio_input = SpxQueryInterface<ISpxObjectWithAudioConfig>(conversation_transcriber);
        factory->InitSessionFromAudioInputConfig(SpxQueryInterface<ISpxAudioStreamSessionInit>(session), audio_input->GetAudioConfig());

        // hook conversation to conversation transcriber, so that I can get the participant list later
        auto transcriber_ptr = SpxQueryInterface<ISpxConversationTranscriber>(conversation_transcriber);
        transcriber_ptr->JoinConversation(conversation);

        // hook the transcriber to session
        session->AddRecognizer(conversation_transcriber);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_leave_conversation(SPXRECOHANDLE hreco)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hreco == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto transcribers = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto conversation_transcriber = (*transcribers)[hreco];

        auto cts = SpxQueryInterface<ISpxConversationTranscriber>(conversation_transcriber);
        // leave conversation, set site to null
        cts->LeaveConversation();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_create_from_config(SPXCONVERSATIONTRANSLATORHANDLE * phandle, SPXAUDIOCONFIGHANDLE haudioinput)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phandle == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phandle = SPXHANDLE_INVALID;

        auto conversation_translator = SpxCreateObject<ConversationTranslation::ISpxConversationTranslator>(
            "CSpxConversationTranslator", SpxGetRootSite());

        SPX_RETURN_HR_IF(SPXERR_EXTENSION_LIBRARY_NOT_FOUND, conversation_translator == nullptr);

        // copy the audio input properties into the conversation transcriber
        auto audioInput = AudioConfigFromHandleOrEmptyIfInvalid(haudioinput);
        auto audioInput_propertybag = SpxQueryInterface<ISpxNamedProperties>(audioInput);
        auto propertybag_cts = SpxQueryInterface<ISpxNamedProperties>(conversation_translator);
        if (audioInput_propertybag != nullptr)
        {
            propertybag_cts->Copy(audioInput_propertybag.get());
        }

        auto has_audio_config = SpxQueryInterface<ISpxObjectWithAudioConfig>(conversation_translator);
        SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, has_audio_config == nullptr);
        has_audio_config->SetAudioConfig(audioInput);

        // NOTE:
        // Don't initialize the conversation translator here. We need the site from the conversation
        // object (the audio stream instance) which will be passed as part of the join call.

        // track the conversation translator handle
        auto translators = CSpxSharedPtrHandleTableManager::Get<ConversationTranslation::ISpxConversationTranslator, SPXCONVERSATIONTRANSLATORHANDLE>();
        *phandle = translators->TrackHandle(conversation_translator);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI create_voice_profile_client_from_config(SPXVOICEPROFILECLIENTHANDLE* pclient, SPXSPEECHCONFIGHANDLE hspeechconfig)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pclient == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));

    SPXAPI_INIT_HR_TRY(hr)
    {
        *pclient = SPXHANDLE_INVALID;

        auto factory = create_factory_from_speech_config(hspeechconfig);

        auto client = factory->CreateVoiceProfileClientFromConfig();

        // track the conversation handle
        auto clients = CSpxSharedPtrHandleTableManager::Get<ISpxVoiceProfileClient, SPXVOICEPROFILECLIENTHANDLE>();
        *pclient = clients->TrackHandle(client);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI recognizer_create_speaker_recognizer_from_config(SPXSPEAKERIDHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phreco == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !speech_config_is_handle_valid(hspeechconfig));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !audio_config_is_handle_valid(haudioInput));

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = create_from_config(hspeechconfig, SPXHANDLE_INVALID, SPXHANDLE_INVALID, haudioInput, &ISpxSpeechApiFactory::CreateSpeakerRecognizerFromConfig);

        // track the reco handle
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxVoiceProfileClient, SPXSPEAKERIDHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

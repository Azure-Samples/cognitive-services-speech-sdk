//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_api_factory.cpp: Implementation definitions for CSpxSpeechApiFactory C++ class
//

#include "stdafx.h"
#include "spxcore_common.h"
#include "create_object_helpers.h"
#include "speech_api_factory.h"
#include "site_helpers.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerFromConfig(const char* pszLanguage, OutputFormat format, std::shared_ptr<ISpxAudioConfig> audioInput)
{
    return CreateRecognizerFromConfigInternal("CSpxAudioStreamSession", "CSpxRecognizer", pszLanguage, format, audioInput);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizerFromConfig(const char* pszLanguage, OutputFormat format, std::shared_ptr<ISpxAudioConfig> audioInput)
{
    format = OutputFormat::Detailed;
    return CreateRecognizerFromConfigInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer", pszLanguage, format, audioInput);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, const std::string& voice, std::shared_ptr<ISpxAudioConfig> audioInput)
{
    return CreateTranslationRecognizerFromConfigInternal(sourceLanguage, targetLanguages, voice, audioInput);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateRecognizerFromConfigInternal(
    const char* sessionClassName,
    const char* recognizerClassName,
    const char* language,
    OutputFormat format,
    std::shared_ptr<ISpxAudioConfig> audioInput)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>(sessionClassName, factoryAsSite);

    try
    {
        // Initialize the session
        InitSessionFromAudioInputConfig(session, audioInput);

        // Create the recognizer
        auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
        auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>(recognizerClassName, sessionAsSite);

        // Set the recognizer properties
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(session);
        SetRecognizerProperties(namedProperties, language, format);

        // Add the recognizer to the session
        session->AddRecognizer(recognizer);

        // We're done!
        return recognizer;
    }
    catch (...)
    {
        SpxTermAndClearNothrow(session);

        throw;
    }
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerFromConfigInternal(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, const std::string& voice, std::shared_ptr<ISpxAudioConfig> audioInput)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);

    try
    {
        // Initialize the session
        InitSessionFromAudioInputConfig(session, audioInput);

        // Create the translation recognizer
        auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
        auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>("CSpxTranslationRecognizer", sessionAsSite);

        // Set the translation properties
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(session);
        SetTranslationProperties(namedProperties, sourceLanguage, targetLanguages, voice);

        // Add the recognizer to the session
        session->AddRecognizer(recognizer);

        // We're done!
        return recognizer;
    }
    catch (...)
    {
        SpxTermAndClearNothrow(session);

        throw;
    }
}

void CSpxSpeechApiFactory::InitSessionFromAudioInputConfig(std::shared_ptr<ISpxSession> session, std::shared_ptr<ISpxAudioConfig> audioInput)
{
    SPX_THROW_HR_IF(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE, session == nullptr);
    auto sessionInit = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);

    // If we don't already have the config object, make it now, and set it to use the default input device
    if (audioInput == nullptr)
    {
        audioInput = SpxCreateObjectWithSite<ISpxAudioConfig>("CSpxAudioConfig", SpxSiteFromThis(this));
        audioInput->InitFromDefaultDevice();
    }

    // See if we have a file, a stream, or neither, so we can initialize the session correctly...
    auto fileName = audioInput->GetFileName();
    auto stream = audioInput->GetStream();

    if (stream != nullptr)
    {
        sessionInit->InitFromStream(stream);
    }
    else if (fileName.length() > 0)
    {
        sessionInit->InitFromFile(fileName.c_str());
    }
    else
    {
        sessionInit->InitFromMicrophone();
    }
}

void CSpxSpeechApiFactory::SetRecognizerProperties(const std::shared_ptr<ISpxNamedProperties>& namedProperties, const char* language, OutputFormat format)
{
    // Set the recognition language...
    if (language != nullptr)
    {
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), language);
    }

    namedProperties->SetStringValue(
        GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse),
        PAL::BoolToString(format == OutputFormat::Detailed).c_str());
}

void CSpxSpeechApiFactory::SetTranslationProperties(const std::shared_ptr<ISpxNamedProperties>& namedProperties, const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, const std::string& voice)
{
    if (sourceLanguage.empty() && !namedProperties->HasStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId)))
    {
        SPX_DBG_TRACE_ERROR("%s: neither source langauge nor endpointId is set.", __FUNCTION__);
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    if (!sourceLanguage.empty())
    {
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), sourceLanguage.c_str());
    }

    std::string plainStr;
    // The target languages are in BCP-47 format, and should not contain the character ','.
    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, targetLanguages.size() == 0);
    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, targetLanguages[0].empty());
    plainStr = targetLanguages.at(0);
    for (auto lang = targetLanguages.begin() + 1; lang != targetLanguages.end(); ++lang)
    {
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, lang->empty());
        plainStr += "," + *lang;
    }
    if (plainStr.empty())
    {
        SPX_DBG_TRACE_ERROR("%s: target language is empty.", __FUNCTION__);
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }
    namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), plainStr.c_str());
    namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationVoice), voice.c_str());

    // Set mode to conversation for translation
    namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode), g_recoModeConversation);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

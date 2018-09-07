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


std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizer()
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer") ;
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizer(const std::wstring& language)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", nullptr, language.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizer(const std::wstring& language, OutputFormat format)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", nullptr, language.c_str(), format);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", fileName.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", fileName.c_str(), language.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language, OutputFormat format)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", fileName.c_str(), language.c_str(), format);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream)
{
    return CreateSpeechRecognizerWithStream(audioInputStream, L"", OutputFormat::Simple);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream, const std::wstring& language)
{
    return CreateSpeechRecognizerWithStream(audioInputStream, language, OutputFormat::Simple);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream, const std::wstring& language, OutputFormat format)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);

    // Initialize the session
    auto sessionInit = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromStream(audioInputStream);

    // Create the recognizer
    auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>("CSpxRecognizer", sessionAsSite);

    // Set language if we have one. Default will be set to en-US
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(sessionAsSite);
    if (!language.empty())
    {
        namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoLanguage), PAL::ToString(language).c_str());
    }

    namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceResponse_OutputFormat),
        format == OutputFormat::Simple ? GetPropertyName(SpeechPropertyId::SpeechServiceResponse_OutputFormat_Simple): GetPropertyName(SpeechPropertyId::SpeechServiceResponse_OutputFormat_Detailed));

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizer()
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer");
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizer(const std::wstring& language)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer",nullptr, language.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer", fileName.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer", fileName.c_str(), language.c_str());
} 

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizerWithStream(AudioInputStream* audioInputStream)
{
    return CreateIntentRecognizerWithStream(audioInputStream, L"");
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizerWithStream(AudioInputStream* audioInputStream, const std::wstring& language)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);

    // Initialize the session
    auto sessionInit = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromStream(audioInputStream);

    // Create the recognizer
    auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>("CSpxIntentRecognizer", sessionAsSite);

    // Set language if we have one. Default will be set to en-US
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(sessionAsSite);
    if (!language.empty())
    {
        namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoLanguage), PAL::ToString(language).c_str());
    }

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice)
{
    return CreateTranslationRecognizerInternal(nullptr, sourceLanguage, targetLanguages, voice);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice)
{
    return CreateTranslationRecognizerInternal(fileName.c_str(), sourceLanguage, targetLanguages, voice);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerInternal(wchar_t const* fileNameStr, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);

    // Initialize the session
    SPX_THROW_HR_IF(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE, session == nullptr);
    auto sessionInit = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
    if (fileNameStr != nullptr)
    {
        sessionInit->InitFromFile(fileNameStr);
    }
    else
    {
        sessionInit->InitFromMicrophone();
    }

    // Create the recognizer
    auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>("CSpxTranslationRecognizer", sessionAsSite);

    // Set translation parameters.
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(sessionAsSite);
    SetTranslationParameter(namedProperties, sourceLanguage, targetLanguages, voice);

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    //// We're done!
    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);

    // Initialize the session
    SPX_THROW_HR_IF(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE, session == nullptr);
    auto sessionInit = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromStream(stream);

    // Create the recognizer
    auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>("CSpxTranslationRecognizer", sessionAsSite);

    // Set translation parameters.
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(sessionAsSite);
    SetTranslationParameter(namedProperties, sourceLanguage, targetLanguages, voice);

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}

void CSpxSpeechApiFactory::SetTranslationParameter(const std::shared_ptr<ISpxNamedProperties>& namedProperties, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice)
{
    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, sourceLanguage.empty());

    namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_TranslationFromLanguage), PAL::ToString(sourceLanguage).c_str());
    std::wstring plainStr;
    // The target languages are in BCP-47 format, and should not contain the character ','.
    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, targetLanguages.size() == 0);
    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, targetLanguages[0].empty());
    plainStr = targetLanguages.at(0);
    for (auto lang = targetLanguages.begin() + 1; lang != targetLanguages.end(); ++lang)
    {
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, lang->empty());
        plainStr += L"," + *lang;
    }
    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, plainStr.empty());
    namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_TranslationToLanguages), PAL::ToString(plainStr).c_str());
    namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_TranslationVoice), PAL::ToString(voice).c_str());

    // Set mode to conversation for translation
    namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode), GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode_Conversation));
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateRecognizerInternal(const char* sessionClassName,
    const char* recognizerClassName,
    const wchar_t* fileName,
    const wchar_t* language,
    OutputFormat format)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>(sessionClassName, factoryAsSite);

    // Initialize the session
    auto sessionInit = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
    if (fileName != nullptr)
    {
        sessionInit->InitFromFile(fileName);
    }
    else
    {
        sessionInit->InitFromMicrophone();
    }

    // Create the recognizer
    auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>(recognizerClassName, sessionAsSite);

    // Set language if we have one. Default will be set to en-US
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(sessionAsSite);
    if (language != nullptr)
    {
        namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoLanguage), PAL::ToString(language).c_str());
    }

    namedProperties->SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceResponse_OutputFormat),
        format == OutputFormat::Simple ? GetPropertyName(SpeechPropertyId::SpeechServiceResponse_OutputFormat_Simple) : GetPropertyName((SpeechPropertyId::SpeechServiceResponse_OutputFormat_Detailed)));

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

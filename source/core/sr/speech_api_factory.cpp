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
#include "named_properties_constants.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizer() 
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer");
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizer(const std::wstring& language)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", nullptr, language.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", fileName.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer", fileName.c_str(), language.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizer()
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer");
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer", fileName.c_str());
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream)
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

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
{
    return CreateTranslationRecognizerInternal(nullptr, sourceLanguage, targetLanguage);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
{
    return CreateTranslationRecognizerInternal(fileName.c_str(), sourceLanguage, targetLanguage);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerInternal(wchar_t const* fileNameStr, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
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

    // Todo handle source and target lanugage settings.
    // Set language if we have one. Default will be set to en-US
    // if (language != nullptr)
    // {
    //    auto namedProperties = SpxQueryService<ISpxNamedProperties>(sessionAsSite);
    //    namedProperties->SetStringValue(g_SPEECH_RecoLanguage, language);
    //}

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    UNUSED(sourceLanguage);
    UNUSED(targetLanguage);

    //// We're done!
    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
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

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // Todo: set languages.
    UNUSED(sourceLanguage);
    UNUSED(targetLanguage);
    // We're done!
    return recognizer;
}


std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateRecognizerInternal(const char* sessionClassName, 
    const char* recognizerClassName, 
    wchar_t const* fileName,
    wchar_t const* language)
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
    if (language != nullptr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(sessionAsSite);
        namedProperties->SetStringValue(g_SPEECH_RecoLanguage, language);
    }

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

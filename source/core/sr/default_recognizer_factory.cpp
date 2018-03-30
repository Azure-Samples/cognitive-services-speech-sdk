//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// default_recognizer_factory.cpp: Implementation definitions for CSpxDefaultRecognizerFactory C++ class
//

#include "stdafx.h"
#include "spxcore_common.h"
#include "create_object_helpers.h"
#include "default_recognizer_factory.h"
#include "site_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


std::shared_ptr<ISpxRecognizer> CSpxDefaultRecognizerFactory::CreateSpeechRecognizer() 
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxRecognizer");
}

std::shared_ptr<ISpxRecognizer> CSpxDefaultRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
{
    return CreateRecognizerWithFileInputInternal(fileName, "CSpxAudioStreamSession", "CSpxRecognizer");
}

std::shared_ptr<ISpxRecognizer> CSpxDefaultRecognizerFactory::CreateIntentRecognizer()
{
    return CreateRecognizerInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer");
}

std::shared_ptr<ISpxRecognizer> CSpxDefaultRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
{
    return CreateRecognizerWithFileInputInternal(fileName, "CSpxAudioStreamSession", "CSpxIntentRecognizer");
}

std::shared_ptr<ISpxRecognizer> CSpxDefaultRecognizerFactory::CreateRecognizerInternal(const char* sessionClassName, const char* recognizerClassName)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>(sessionClassName, factoryAsSite);

    // Initialize the session
    auto sessionInit = std::dynamic_pointer_cast<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromMicrophone();

    // Create the recognizer
    auto sessionAsSite = std::dynamic_pointer_cast<ISpxSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>(recognizerClassName, sessionAsSite);

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxDefaultRecognizerFactory::CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);

    // Initialize the session
    auto sessionInit = std::dynamic_pointer_cast<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromStream(audioInputStream);

    // Create the recognizer
    auto sessionAsSite = std::dynamic_pointer_cast<ISpxSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>("CSpxRecognizer", sessionAsSite);

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxDefaultRecognizerFactory::CreateRecognizerWithFileInputInternal(const std::wstring& fileName, const char* sessionClassName, const char* recognizerClassName)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>(sessionClassName, factoryAsSite);

    // Initialize the session
    auto sessionInit = std::dynamic_pointer_cast<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromFile(fileName.c_str());

    // Create the recognizer
    auto sessionAsSite = std::dynamic_pointer_cast<ISpxSite>(session);
    auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>(recognizerClassName, sessionAsSite);

    // Add the recognizer to the session
    session->AddRecognizer(recognizer);

    // We're done!
    return recognizer;
}


} // CARBON_IMPL_NAMESPACE

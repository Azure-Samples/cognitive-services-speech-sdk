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


std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateSpeechRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput)
{
    return CreateRecognizerFromConfigInternal("CSpxAudioStreamSession", "CSpxRecognizer", audioInput);
}

std::shared_ptr<ISpxDialogServiceConnector> CSpxSpeechApiFactory::CreateDialogServiceConnectorFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput)
{
    return SpxQueryInterface<ISpxDialogServiceConnector>(CreateRecognizerFromConfigInternal("CSpxAudioStreamSession", "CSpxDialogServiceConnector", audioInput));
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateIntentRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput)
{
    return CreateRecognizerFromConfigInternal("CSpxAudioStreamSession", "CSpxIntentRecognizer", audioInput);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput)
{
    return CreateTranslationRecognizerFromConfigInternal(audioInput);
}


std::shared_ptr<ISpxConversation> CSpxSpeechApiFactory::CreateConversationFromConfig(const char* id)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);
    try
    {
        // create conversation
        auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
        auto conversation = SpxCreateObjectWithSite<ISpxConversation>("CSpxConversation", sessionAsSite);

        conversation->SetConversationId(id);
        conversation->CreateConversation();

        session->SetConversation(conversation);

        return conversation;
    }
    catch (...)
    {
        SpxTermAndClearNothrow(session);

        throw;
    }
}

std::shared_ptr<ISpxVoiceProfileClient> CSpxSpeechApiFactory::CreateVoiceProfileClientFromConfig()
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSpeakerRecognition>("CSpxHttpAudioStreamSession", factoryAsSite);

    try
    {
        // create conversation
        auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
        auto client = SpxCreateObjectWithSite<ISpxVoiceProfileClient>("CSpxVoiceProfileClient", sessionAsSite);
        return client;
    }
    catch (...)
    {
        SpxTermAndClearNothrow(session);

        throw;
    }
}

std::shared_ptr<ISpxVoiceProfileClient>  CSpxSpeechApiFactory::CreateSpeakerRecognizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioInput)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSpeakerRecognition>("CSpxHttpAudioStreamSession", factoryAsSite);

    try
    {
        auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
        auto client = SpxCreateObjectWithSite<ISpxVoiceProfileClient>("CSpxVoiceProfileClient", sessionAsSite);
        auto audioInit = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
        InitSessionFromAudioInputConfig(audioInit, audioInput);

        return client;
    }
    catch (...)
    {
        SpxTermAndClearNothrow(session);

        throw;
    }

}
std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateRecognizerFromConfigInternal(
    const char* sessionClassName,
    const char* recognizerClassName,
    std::shared_ptr<ISpxAudioConfig> audioInput)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>(sessionClassName, factoryAsSite);

    return CreateRecogizer(session, audioInput, recognizerClassName);
}

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateRecogizer(std::shared_ptr<ISpxSession> session, std::shared_ptr<ISpxAudioConfig> audioInput, const char * recognizerClassName)
{
    try
    {
        // Initialize the session
        InitSessionFromAudioInputConfig(SpxQueryInterface<ISpxAudioStreamSessionInit>(session), audioInput);

        // Create the recognizer
        auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
        auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>(recognizerClassName, sessionAsSite);

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

std::shared_ptr<ISpxRecognizer> CSpxSpeechApiFactory::CreateTranslationRecognizerFromConfigInternal(std::shared_ptr<ISpxAudioConfig> audioInput)
{
    // Create the session
    auto factoryAsSite = SpxSiteFromThis(this);
    auto session = SpxCreateObjectWithSite<ISpxSession>("CSpxAudioStreamSession", factoryAsSite);

    try
    {
        // Initialize the session
        auto session_init = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
        InitSessionFromAudioInputConfig(session_init, audioInput);

        // Create the translation recognizer
        auto sessionAsSite = SpxQueryInterface<ISpxGenericSite>(session);
        auto recognizer = SpxCreateObjectWithSite<ISpxRecognizer>("CSpxTranslationRecognizer", sessionAsSite);

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

void CSpxSpeechApiFactory::InitSessionFromAudioInputConfig(std::shared_ptr<ISpxAudioStreamSessionInit> session, std::shared_ptr<ISpxAudioConfig> audioInput)
{
    SPX_THROW_HR_IF(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE, session == nullptr);

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
        session->InitFromStream(stream);
    }
    else if (fileName.length() > 0)
    {
        session->InitFromFile(fileName.c_str());
    }
    else
    {
        session->InitFromMicrophone();
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

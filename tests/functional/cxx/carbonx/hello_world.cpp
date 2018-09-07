// Copyright (c) Microsoft. All rights reserved.
//
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// hello_world.cpp: Sample #1 - Hello World!!
//

#include "stdafx.h"
#include "carbon_test_console.h"
#include "speechapi_cxx_language_understanding_model.h"
#include "speechapi_cxx_session.h"
#include "speechapi_cxx_intent_recognizer.h"


void CarbonTestConsole::Sample_HelloWorld()
{
    auto factory = SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);
    auto recognizer = factory->CreateSpeechRecognizer();

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_WithEvents()
{
    auto factory = SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);
    auto recognizer = factory->CreateSpeechRecognizer();

    recognizer->IntermediateResult += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine("IntermediateResult: text=%s", e.Result.Text.c_str());
    };

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_PickEngine(const char* pszEngine) // L"Usp", L"Unidec", or L"Mock"
{
    auto factory = SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);
    auto recognizer = factory->CreateSpeechRecognizer();
    auto session = Session::FromRecognizer(recognizer);

    std::string propertyName = std::string("__use") + std::string(pszEngine) + std::string("RecoEngine");
    session->Parameters.SetProperty(propertyName, "true");

    recognizer->IntermediateResult += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine("IntermediateResult: text=%s", e.Result.Text.c_str());
    };

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Intent()
{
    constexpr auto luisRegion = R"(westus2)";
    Sample_HelloWorld_Intent(m_subscriptionKey.c_str(), m_intentAppId.c_str(), luisRegion);
}

void CarbonTestConsole::Sample_HelloWorld_Intent(const char* subscriptionKey, const char* appId, const char* region)
{
    auto factory = SpeechFactory::FromSubscription(subscriptionKey, region);

    // Create an intent recognizer using microphone as audio input.
    auto recognizer = factory->CreateIntentRecognizer();

    // Create a LanguageUnderstandingModel associated with your LU application
    auto luisSubscriptionKey = "YourLuisSubscriptionKey"; luisSubscriptionKey = subscriptionKey;
    auto luisAppId = "YourLuisAppId"; luisAppId = appId;
    auto luisRegion = "YourLuisEndpoint"; luisRegion = region;
    auto model = LanguageUnderstandingModel::FromSubscription(luisSubscriptionKey, luisAppId, luisRegion);

    // Add each intent you wish to recognize to the intent recognizer
    auto intentName1 = "IntentNameFromLuisPortal"; intentName1 = "Calendar.Add";
    auto intentName2 = "IntentNameFromLuisPortal"; intentName2 = "Communication.SendEmail";

    recognizer->AddIntent("1", model, intentName1);
    recognizer->AddIntent("some other id", model, intentName2);

    // Prompt the user to speak
    // wcout << L"Say something...\n";
    ConsoleWriteLine("Say something...");

    // Start recognition; will return the first result recognized
    auto result = recognizer->RecognizeAsync().get();

    // Check the reason returned
    switch (result->Reason)
    {
    case Reason::Recognized:
        ConsoleWriteLine("We recognized: %s", result->Text.c_str());
        ConsoleWriteLine("IntentId='%s'", result->IntentId.c_str());
        ConsoleWriteLine("json='%s'", result->Properties.GetProperty(SpeechPropertyId::SpeechServiceResponse_JsonResult).c_str());
        break;
    case Reason::InitialSilenceTimeout:
        ConsoleWriteLine("We only heard silence in the audio stream.");
        break;
    case Reason::InitialBabbleTimeout:
        ConsoleWriteLine("We only heard noise in the audio stream.");
        break; 
    case Reason::NoMatch:
        ConsoleWriteLine("We detected speech in the audio stream, but could not recognize any words from the target language. This could be caused by wrong language setting or wrong audio format.");
        break;
    case Reason::Canceled:
        ConsoleWriteLine("There was an error, reason=%d - %s", int(result->Reason), result->Text.c_str());
        break;
    default:
        break;
    }
}

void CarbonTestConsole::Sample_HelloWorld_Subscription()
{
    auto factory = SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);
    auto recognizer = factory->CreateSpeechRecognizer();

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Subscription_With_CRIS()
{
    auto factory = SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);
    auto recognizer = factory->CreateSpeechRecognizer();

    recognizer->SetDeploymentId(m_customSpeechModelId);

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Language(const char* language)
{
    auto factory = SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);
    auto recognizer = factory->CreateSpeechRecognizer(language);

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

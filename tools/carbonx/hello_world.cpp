// Copyright (c) Microsoft. All rights reserved.
//
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// hello_world.cpp: Sample #1 - Hello World!!
//

#include "stdafx.h"
#include "carbon_test_console.h"
#include "speechapi_cxx_session.h"
#include "speechapi_cxx_intent_recognizer.h"


constexpr auto g_speechSubscriptionKey = LR"(093bc1e6d0004dff88677a4f0bba3da1)";
constexpr auto g_customSpeechSubscriptionKey = LR"(82f1f909b993459d88384a53891f98d3)";
constexpr auto g_customSpeechModelId = LR"(eb29f6e4-e97b-4157-8d3c-9d64a7b21a58)";


void CarbonTestConsole::Sample_HelloWorld()
{
    auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer();
    
    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_WithEvents()
{
    auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer();

    recognizer->IntermediateResult += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine(L"IntermediateResult: text=%ls", e.Result.Text.c_str());
    };

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_PickEngine(const wchar_t* pszEngine) // L"Usp", L"Unidec", or L"Mock"
{
    auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer();
    auto session = Session::FromRecognizer(recognizer);

    std::wstring propertyName = std::wstring(L"__use") + std::wstring(pszEngine) + std::wstring(L"RecoEngine");
    session->Parameters[propertyName.c_str()] = true;

    recognizer->IntermediateResult += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine(L"IntermediateResult: text=%ls", e.Result.Text.c_str());
    };

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Intent()
{
    constexpr auto hostName = LR"(westus.api.cognitive.microsoft.com)";
    constexpr auto subscriptionKey = LR"(0415470b4d0d4ed7b736a7ccac71fa5d)";
    constexpr auto appId = LR"(6ad2c77d-180b-45a2-88aa-8c442538c090)";
    
    Sample_HelloWorld_Intent(hostName, subscriptionKey, appId);
}

void CarbonTestConsole::Sample_HelloWorld_Intent(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId)
{
    auto model = LuisModel::From(hostName, subscriptionKey, appId);

    auto recognizer = DefaultRecognizerFactory::CreateIntentRecognizer();
    recognizer->AddIntent(L"GoBack", L"go back");
    recognizer->AddIntent(L"add to calendar", IntentTrigger::From(model, L"Calendar.Add"));
    recognizer->AddIntent(L"send email", IntentTrigger::From(model, L"Communication.SendEmail"));

    auto result = recognizer->RecognizeAsync().get();
    auto text = result->Text;

    auto intentId = result->IntentId;
    auto luisJson = result->Properties[ResultProperty::LuisJson].GetString();

    ConsoleWriteLine(L"text = '%ls'; intentId = '%ls'; json='%ls'", text.c_str(), intentId.c_str(), luisJson.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Subscription()
{
    DefaultRecognizerFactory::SetSubscriptionKey(g_speechSubscriptionKey);
    auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer();

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Subscription_With_CRIS()
{
    if (0)
    {
        RecognizerFactory::GetDefault()->SetSubscriptionKey(g_customSpeechSubscriptionKey);

        auto recognizer = RecognizerFactory::GetDefault()->CreateSpeechRecognizer();
        recognizer->SetCustomSpeechModel(g_customSpeechModelId);

        ConsoleWriteLine(L"Say something...");
        auto result = recognizer->RecognizeAsync().get();

        ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
    }
    else if (1)
    {
        DefaultRecognizerFactory::SetSubscriptionKey(g_customSpeechSubscriptionKey);

        auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer();
        recognizer->SetCustomSpeechModel(g_customSpeechModelId);

        ConsoleWriteLine(L"Say something...");
        auto result = recognizer->RecognizeAsync().get();

        ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
    }
}

void CarbonTestConsole::Sample_HelloWorld_Language(const wchar_t* language)
{
    auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer(language);

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

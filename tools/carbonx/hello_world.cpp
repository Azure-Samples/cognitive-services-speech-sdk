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


void CarbonTestConsole::Sample_HelloWorld()
{
    auto factory = new RecognizerFactory();
    auto recognizer = factory->CreateSpeechRecognizer();

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_WithEvents()
{
    auto factory = new RecognizerFactory();
    auto recognizer = factory->CreateSpeechRecognizer();

    recognizer->IntermediateResult += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine(L"IntermediateResult: text=%ls", e.Result.Text.c_str());
    };

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_PickEngine(const wchar_t* pszEngine) // L"Usp", L"Unidec", or L"Mock"
{
    auto factory = new RecognizerFactory();
    auto recognizer = factory->CreateSpeechRecognizer();
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

    auto factory = new RecognizerFactory();
    auto recognizer = factory->CreateIntentRecognizer();
    recognizer->AddIntent(L"GoBack", L"go back");
    recognizer->AddIntent(L"add to calendar", IntentTrigger::From(model, L"Calendar.Add"));
    recognizer->AddIntent(L"Bar", IntentTrigger::From(model, L"bar"));
    
    auto result = recognizer->RecognizeAsync().get();
    ConsoleWriteLine(L"text = '%ls'; intentId = '%ls'; json='%ls'", result->Text.c_str(), result->IntentId.c_str(), result->Values[L"json"].GetString().c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Subscription()
{
    RecognizerFactory::Parameters::SetString(L"SPEECH-SubscriptionKey", g_speechSubscriptionKey);
    auto factory = new RecognizerFactory();
    auto recognizer = factory->CreateSpeechRecognizer();

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}


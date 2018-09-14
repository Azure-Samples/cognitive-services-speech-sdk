// Copyright (c) Microsoft. All rights reserved.
//
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// hello_world.cpp: Sample #1 - Hello World!!
//

#include "stdafx.h"
#include "file_utils.h"
#include "carbon_test_console.h"
#include "speechapi_cxx_language_understanding_model.h"
#include "speechapi_cxx_session.h"
#include "speechapi_cxx_intent_recognizer.h"


void CarbonTestConsole::Sample_HelloWorld()
{
    // Create the recognizer "with microphone input"
    auto recognizer = SpeechRecognizer::FromConfig(SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId), nullptr);

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    // Show the result
    ConsoleWriteLine("You said '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_WithReasonInfo()
{
    // Create the recognizer "with microphone input"
    auto recognizer = SpeechRecognizer::FromConfig(SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId), nullptr);

    // Prompt and recognize
    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    // Show the result
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        ConsoleWriteLine("RECOGNIZED: Reason=%d", result->Reason);
        ConsoleWriteLine("RECOGNIZED: '%s'", result->Text.c_str());
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        ConsoleWriteLine("NOMATCH: Reason=%d", result->Reason);
        auto noMatch = NoMatchDetails::FromResult(result);
        switch (noMatch->Reason)
        {
        case NoMatchReason::NotRecognized:
            ConsoleWriteLine("NOMATCH: Speech was detected, but not recognized.");
            break;
        case NoMatchReason::InitialSilenceTimeout:
            ConsoleWriteLine("NOMATCH: The start of the audio stream contains only silence, and the service timed out waiting for speech.");
            break;
        case NoMatchReason::InitialBabbleTimeout:
            ConsoleWriteLine("NOMATCH: The start of the audio stream contains only noise, and the service timed out waiting for speech.");
            break;
        }
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        ConsoleWriteLine("CANCELED: Reason=%d", cancellation->Reason);

        if (cancellation->Reason == CancellationReason::Error)
        {
            ConsoleWriteLine("CANCELED: ErrorDetails=%s", cancellation->ErrorDetails.c_str());
            ConsoleWriteLine("CANCELED: Did you update the subscription info?");
        }
    }
}

void CarbonTestConsole::Sample_HelloWorld_Microphone()
{
    // Create the recognizer "with microphone input"
    auto recognizer = SpeechRecognizer::FromConfig(
        SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId),
        AudioConfig::FromDefaultMicrophoneInput());

    // Prompt and recognize
    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    // Show the result
    ConsoleWriteLine("You said '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_File()
{
    // Create the recognizer "with file input"
    auto recognizer = SpeechRecognizer::FromConfig(
        SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId),
        AudioConfig::FromWavFileInput("one_one_one.wav"));

    // Prompt and recognize
    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    // Show the result
    ConsoleWriteLine("You said '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_PushStream()
{
    // Prepare to use the "Push stream" by reading the data from the file
    FILE* hfile = nullptr;
    PAL::fopen_s(&hfile, "one_one_one.wav", "rb");
    const auto maxSize = 1000000; auto buffer = new uint8_t[maxSize];
    auto size = (int)fread(buffer, 1, maxSize, hfile);

    // Create the recognizer "with stream input" with a "push stream"
    auto config = SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId);
    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioConfig = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

    // "Push" all the audio data into the stream
    pushStream->Write(buffer, size);
    pushStream->Close();
    delete [] buffer;
    fclose(hfile);

    // Prompt and recognize
    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    // Show the result
    ConsoleWriteLine("You said '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_PullStream()
{
    // Prepare for the stream to be "Pulled"
    FILE* hfile = nullptr;
    PAL::fopen_s(&hfile, "one_one_one.wav", "rb");
    fseek(hfile, 44, SEEK_CUR);

    // Create the "pull stream" object with C++ lambda callbacks
    auto pullStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1),
        [=](uint8_t* buffer, uint32_t size) -> int { return (int)fread(buffer, 1, size, hfile); },
        [=]() { fclose(hfile); });

    // Create the recognizer "with stream input" using the "pull stream"
    auto config = SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId);
    auto audioConfig = AudioConfig::FromStreamInput(pullStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

    // Prompt and recognize
    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    // Show the result
    ConsoleWriteLine("You said '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_WithEvents()
{
    auto recognizer = SpeechRecognizer::FromConfig(
        SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId),
        nullptr);

    recognizer->Recognizing += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine("RECOGNIZING: Text=%s", e.Result->Text.c_str());
    };

    recognizer->Recognized.Connect([&] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            ConsoleWriteLine("RECOGNIZED: Text=%s", e.Result->Text.c_str());
            ConsoleWriteLine("  Offset=%lu", e.Result->Offset());
            ConsoleWriteLine("  Duration=%lu", e.Result->Duration());
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            auto noMatch = NoMatchDetails::FromResult(e.Result);
            switch (noMatch->Reason)
            {
            case NoMatchReason::NotRecognized:
                ConsoleWriteLine("NOMATCH: Speech was detected, but not recognized.");
                break;
            case NoMatchReason::InitialSilenceTimeout:
                ConsoleWriteLine("NOMATCH: The start of the audio stream contains only silence, and the service timed out waiting for speech.");
                break;
            case NoMatchReason::InitialBabbleTimeout:
                ConsoleWriteLine("NOMATCH: The start of the audio stream contains only noise, and the service timed out waiting for speech.");
                break;
            }
        }
    });

    recognizer->Canceled.Connect([&](const SpeechRecognitionCanceledEventArgs& e)
    {
        ConsoleWriteLine("CANCELED: Reason=%d", e.Reason);

        if (e.Reason == CancellationReason::Error)
        {
            ConsoleWriteLine("CANCELED: ErrorDetails=%s", e.ErrorDetails.c_str());
            ConsoleWriteLine("CANCELED: Did you update the subscription info?");
        }
    });

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();
}

void CarbonTestConsole::Sample_HelloWorld_PickEngine(const char* pszEngine) // L"Usp", L"Unidec", or L"Mock"
{
    auto recognizer = SpeechRecognizer::FromConfig(
        SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId),
        nullptr);
    auto session = Session::FromRecognizer(recognizer);

    std::string propertyName = std::string("__use") + std::string(pszEngine) + std::string("RecoEngine");
    session->Properties.SetProperty(propertyName, "true");

    recognizer->Recognizing += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine("Recognizing: text=%s", e.Result->Text.c_str());
    };

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Intent()
{
    constexpr auto luisRegion = R"(westus2)";
    Sample_HelloWorld_Intent(m_subscriptionKey.c_str(), m_intentAppId.c_str(), luisRegion);
}

void CarbonTestConsole::Sample_HelloWorld_Intent(const char* subscriptionKey, const char* appId, const char* region)
{
    // Create an intent recognizer using microphone as audio input.
    auto recognizer = IntentRecognizer::FromConfig(
        SpeechConfig::FromSubscription(subscriptionKey, region),
        nullptr);

    // Create a LanguageUnderstandingModel associated with your LU application
    auto luisSubscriptionKey = "YourLuisSubscriptionKey"; luisSubscriptionKey = subscriptionKey;
    auto luisAppId = "YourLuisAppId"; luisAppId = appId;
    auto luisRegion = "YourLuisEndpoint"; luisRegion = region;
    auto model = LanguageUnderstandingModel::FromSubscription(luisSubscriptionKey, luisAppId, luisRegion);

    // Add each intent you wish to recognize to the intent recognizer
    auto intentName1 = "IntentNameFromLuisPortal"; intentName1 = "Calendar.Add";
    auto intentName2 = "IntentNameFromLuisPortal"; intentName2 = "Communication.SendEmail";

    recognizer->AddIntent(model, intentName1, "1");
    recognizer->AddIntent(model, intentName2, "some other id");

    // Prompt the user to speak
    ConsoleWriteLine("Say something...");

    // Start recognition; will return the first result recognized
    auto result = recognizer->RecognizeOnceAsync().get();

    // Check the reason returned
    switch (result->Reason)
    {
    case ResultReason::RecognizedSpeech:
    case ResultReason::RecognizedIntent:
        ConsoleWriteLine("RECOGNIZED: Text=%s", result->Text.c_str());
        ConsoleWriteLine("  Intent Id:'%s'", result->IntentId.c_str());
        ConsoleWriteLine("  Intent Service JSON: '%s'", result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult).c_str());
        break;
    case ResultReason::NoMatch:
        {
            auto noMatch = NoMatchDetails::FromResult(result);
            switch (noMatch->Reason)
            {
            case NoMatchReason::NotRecognized:
                ConsoleWriteLine("NOMATCH: Speech was detected, but not recognized.");
                break;
            case NoMatchReason::InitialSilenceTimeout:
                ConsoleWriteLine("NOMATCH: The start of the audio stream contains only silence, and the service timed out waiting for speech.");
                break;
            case NoMatchReason::InitialBabbleTimeout:
                ConsoleWriteLine("NOMATCH: The start of the audio stream contains only noise, and the service timed out waiting for speech.");
                break;
            }
            break;
        }
    case ResultReason::Canceled:
        {
            auto cancellation = CancellationDetails::FromResult(result);
            ConsoleWriteLine("CANCELED: Reason=%d", cancellation->Reason);

            if (cancellation->Reason == CancellationReason::Error)
            {
                ConsoleWriteLine("CANCELED: ErrorDetails=%s", cancellation->ErrorDetails.c_str());
                ConsoleWriteLine("CANCELED: Did you update the subscription info?");
            }
        }
    default:
        break;
    }
}

void CarbonTestConsole::Sample_HelloWorld_Subscription()
{
    auto recognizer = SpeechRecognizer::FromConfig(
        SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId),
        nullptr);

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Subscription_With_CRIS()
{
    auto config = SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId);
    config->SetEndpointId(m_customSpeechModelId);

    auto recognizer = SpeechRecognizer::FromConfig(config);

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_Language(const char* language)
{
    auto config = SpeechConfig::FromSubscription(m_subscriptionKey, m_regionId);
    config->SetSpeechRecognitionLanguage(language);
    auto recognizer = SpeechRecognizer::FromConfig(config, nullptr);

    ConsoleWriteLine("Say something...");
    auto result = recognizer->RecognizeOnceAsync().get();

    ConsoleWriteLine("You said:\n\n    '%s'", result->Text.c_str());
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Intent;
// </toplevel>

// Intent recognition using microphone.
void IntentRecognitionWithMicrophone()
{
    // <IntentRecognitionWithMicrophone>
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourLuisSubscriptionKey", L"YourLuisServiceRegion");

    // Creates an intent recognizer using microphone as audio input. The default language is "en-us".
    auto recognizer = factory->CreateIntentRecognizer();

    // Creates a language understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId(L"YourLuisAppId");
    recognizer->AddIntent(L"id1", model, L"YourLuisIntentName1");
    recognizer->AddIntent(L"id2", model, L"YourLuisIntentName2");
    recognizer->AddIntent(L"any-IntentId-here", model, L"YourLuisIntentName3");

    wcout << L"Say something...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"Recognition Status: " << int(result->Reason) << L". ";
        if (result->Reason == Reason::Canceled)
        {
            wcout << L"There was an error, reason: " << result->ErrorDetails << std::endl;
        }
        else
        {
            wcout << L"No speech could be recognized.\n";
        }
    }
    else
    {
        wcout << L"We recognized: " << result->Text << std::endl;
        wcout << L"    Intent Id: " << result->IntentId << std::endl;
        wcout << L"    Intent response in Json: " << result->Properties[ResultProperty::LanguageUnderstandingJson].GetString() << std::endl;
    }
    // </IntentRecognitionWithMicrophone>
}

// Intent recognition in the specified language, using microphone.
void IntentRecognitionWithLanguage()
{
    // <IntentRecognitionWithLanguage>
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourLuisSubscriptionKey", L"YourLuisServiceRegion");

    // Creates an intent recognizer in the specified language using microphone as audio input.
    auto lang = L"de-de";
    auto recognizer = factory->CreateIntentRecognizer(lang);

    // Creates a language understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId(L"YourLuisAppId");
    recognizer->AddIntent(L"id1", model, L"YourLuisIntentName1");
    recognizer->AddIntent(L"id2", model, L"YourLuisIntentName2");
    recognizer->AddIntent(L"any-IntentId-here", model, L"YourLuisIntentName3");

    wcout << L"Say something in " << lang << L"...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"Recognition Status:" << int(result->Reason);
        if (result->Reason == Reason::Canceled)
        {
            wcout << L"There was an error, reason: " << result->ErrorDetails << std::endl;
        }
        else
        {
            wcout << L"No speech could be recognized.\n";
        }
    }
    else
    {
        wcout << L"We recognized: " << result->Text << std::endl;
        wcout << L"    Intent Id: " << result->IntentId << std::endl;
        wcout << L"    Intent response in Json: " << result->Properties[ResultProperty::LanguageUnderstandingJson].GetString() << std::endl;
    }
    // </IntentRecognitionWithLanguage>
}


// <IntentContinuousRecognitionWithFile>
static std::mutex g_stopIntentRecognitionMutex;
static std::condition_variable g_stopIntentRecognitionConditionVariable;
static bool g_stopIntentRecognition;

static void NotifyToStopRecognition()
{
    std::unique_lock<std::mutex> locker(g_stopIntentRecognitionMutex);
    g_stopIntentRecognition = true;
    g_stopIntentRecognitionConditionVariable.notify_all();
}

// Defines event handlers for different events.
static void OnPartialResult(const IntentRecognitionEventArgs& e)
{
    wcout << L"IntermediateResult:" << e.Result.Text << std::endl;
}

static void OnFinalResult(const IntentRecognitionEventArgs& e)
{
    wcout << L"FinalResult: status:" << (int)e.Result.Reason << L". Text: " << e.Result.Text << std::endl;
    wcout << L"    Intent Id: " << e.Result.IntentId << std::endl;
    wcout << L"    LUIS Json: " << e.Result.Properties[ResultProperty::LanguageUnderstandingJson].GetString() << std::endl;
}

static void OnCanceled(const IntentRecognitionEventArgs& e)
{
    wcout << L"Canceled:" << (int)e.Result.Reason << L"- " << e.Result.ErrorDetails << std::endl;

    // Notify to stop recognition.
    NotifyToStopRecognition();
}

static void OnSessionStoppedEvent(const SessionEventArgs& e)
{
    wcout << L"Session stopped.";

    // Notify to stop recognition.
    NotifyToStopRecognition();
}

// Continuous intent recognition.
void IntentContinuousRecognitionWithFile()
{
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourLuisSubscriptionKey", L"YourLuisServiceRegion");

    // Creates a intent recognizer using file as audio input.
    // Replace with your own audio file name.
    auto recognizer = factory->CreateIntentRecognizerWithFileInput(L"YourAudioFile.wav");

    // Creates a language understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId(L"YourLuisAppId");
    recognizer->AddIntent(L"id1", model, L"YourLuisIntentName1");
    recognizer->AddIntent(L"id2", model, L"YourLuisIntentName2");
    recognizer->AddIntent(L"any-IntentId-here", model, L"YourLuisIntentName3");

    // Subscribes to events.
    recognizer->IntermediateResult.Connect(&OnPartialResult);
    recognizer->FinalResult.Connect(&OnFinalResult);
    recognizer->Canceled.Connect(&OnCanceled);
    recognizer->SessionStopped.Connect(&OnSessionStoppedEvent);

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    {
        std::unique_lock<std::mutex> lock(g_stopIntentRecognitionMutex);
        g_stopIntentRecognitionConditionVariable.wait(lock, [] { return g_stopIntentRecognition; });
    }

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();

    // Unsubscribes from events.
    recognizer->IntermediateResult.Disconnect(&OnPartialResult);
    recognizer->FinalResult.Disconnect(&OnFinalResult);
    recognizer->Canceled.Disconnect(&OnCanceled);
    recognizer->SessionStopped.Disconnect(&OnSessionStoppedEvent);
}
// </IntentContinuousRecognitionWithFile>

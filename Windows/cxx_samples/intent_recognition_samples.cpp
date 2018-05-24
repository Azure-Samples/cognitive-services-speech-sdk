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
// <IntentRecognitionWithMicrophone>
void IntentRecognitionWithMicrophone()
{
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

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << (int)result->Reason << L"-" << result->ErrorDetails << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
        wcout << L"    Intent Id: " << result->IntentId << '\n';
        wcout << L"    LUIS Json: " << result->Properties[ResultProperty::LanguageUnderstandingJson].GetString() << '\n';
    }
}
// </IntentRecognitionWithMicrophone>

// Intent recognition in the specified language, using microphone.
// <IntentRecognitionWithLanguage>
void IntentRecognitionWithLanguage()
{
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

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << (int)result->Reason << L"-" << result->ErrorDetails << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
        wcout << L"    Intent Id: " << result->IntentId << '\n';
        wcout << L"    LUIS Json: " << result->Properties[ResultProperty::LanguageUnderstandingJson].GetString() << '\n';
    }
}
// </IntentRecognitionWithLanguage>


// Intent recognition using file input
// <IntentRecognitionWithFile>
void IntentRecognitionWithFile()
{
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourLuisSubscriptionKey", L"YourLuisServiceRegion");

    // Creates an intent recognizer using file as audio input.
    // Replace with your own audio file name.
    auto recognizer = factory->CreateIntentRecognizerWithFileInput(L"YourAudioFile.wav");

    // Creates a language understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId(L"YourLuisAppId");
    recognizer->AddIntent(L"id1", model, L"YourLuisIntentName1");
    recognizer->AddIntent(L"id2", model, L"YourLuisIntentName2");
    recognizer->AddIntent(L"any-IntentId-here", model, L"YourLuisIntentName3");

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << (int)result->Reason << L"-" << result->ErrorDetails << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
        wcout << L"    Intent Id: " << result->IntentId << '\n';
        wcout << L"    LUIS Json: " << result->Properties[ResultProperty::LanguageUnderstandingJson].GetString() << '\n';
    }
}
// </IntentRecognitionWithFile>

// Defines event handlers for different events.
// <IntentContinuousRecognitionUsingEvents>
static void OnPartialResult(const IntentRecognitionEventArgs& e)
{
    wcout << L"IntermediateResult:" << e.Result.Text << '\n';
}

static void OnFinalResult(const IntentRecognitionEventArgs& e)
{
    wcout << L"FinalResult: status:" << (int)e.Result.Reason << L". Text: " << e.Result.Text << '\n';
    wcout << L"    Intent Id: " << e.Result.IntentId << '\n';
    wcout << L"    LUIS Json: " << e.Result.Properties[ResultProperty::LanguageUnderstandingJson].GetString() << '\n';
}

static void OnNoMatch(const IntentRecognitionEventArgs& e)
{
    wcout << L"NoMatch:" << (int)e.Result.Reason << '\n';
}

static void OnCanceled(const IntentRecognitionEventArgs& e)
{
    wcout << L"Canceled:" << (int)e.Result.Reason << L"- " << e.Result.Text << '\n';
}

// Continuous intent recognition.
void IntentContinuousRecognitionUsingEvents()
{
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourLuisSubscriptionKey", L"YourLuisServiceRegion");

    // Creates a intent recognizer using microphone as audio input.
    auto recognizer = factory->CreateIntentRecognizer();

    // Creates a language understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId(L"YourLuisAppId");
    recognizer->AddIntent(L"id1", model, L"YourLuisIntentName1");
    recognizer->AddIntent(L"id2", model, L"YourLuisIntentName2");
    recognizer->AddIntent(L"any-IntentId-here", model, L"YourLuisIntentName3");

    // Subscribes to events.
    recognizer->IntermediateResult.Connect(&OnPartialResult);
    recognizer->FinalResult.Connect(&OnFinalResult);
    recognizer->Canceled.Connect(&OnCanceled);
    recognizer->NoMatch.Connect(&OnNoMatch);

    wcout << L"Say something...\n";

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    wcout << L"Press any key to stop\n";
    string s;
    getline(cin, s);

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();

    // Unsubscribes from events.
    recognizer->IntermediateResult.Disconnect(&OnPartialResult);
    recognizer->FinalResult.Disconnect(&OnFinalResult);
    recognizer->Canceled.Disconnect(&OnCanceled);
    recognizer->NoMatch.Disconnect(&OnNoMatch);
}
// </IntentContinuousRecognitionUsingEvents>

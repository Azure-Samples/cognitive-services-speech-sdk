//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <string>
#include <vector>

#include <speechapi_cxx.h>


using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Recognition;
using namespace Microsoft::CognitiveServices::Speech::Recognition::Translation;


// Translation with microphone input.
// <TranslationWithMicrophone>
void TranslationWithMicrophone()
{
    // Gets recognizer factory.
    auto factory = RecognizerFactory::GetDefault();

    // Sets subscription key
    // Replaces with your own subscription key.
    factory->SetSubscriptionKey(L"YourSubscriptionKey");

    // Sets source and target languages
    auto fromLanguage = L"en-US";
    vector<wstring> toLanguages { L"de-DE", L"fr-FR" };

    // Creates a translation recognizer using microphone as audio input.
    auto recognizer = factory->CreateTranslationRecognizer(fromLanguage, toLanguages);
    wcout << L"Say something...\n";

    // Starts translation. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error in speech recognition, reason " << int(result->Reason) << L"-" << result->Text << '\n';
    }
    else if (result->TranslationStatus != TranslationStatus::Success)
    {
        wcout << L"There was an error in translation, status: " << int(result->TranslationStatus) << '\n';
    }
    else
    {
        wcout << "We recognized in " << fromLanguage << ":" << result->Text << '\n';
        for (const auto& it : result->Translations)
        {
            wcout << L"    Translated into " << it.first.c_str() << ":" << it.second.c_str();
        }
    }
}
// </TranslationWithMicrophone>

// Translation with file input.
// <TranslationWithFile>
void TranslationWithFile()
{
    // Gets recognizer factory.
    auto factory = RecognizerFactory::GetDefault();

    // Sets subscription key
    // Replaces with your own subscription key.
    factory->SetSubscriptionKey(L"YourSubscriptionKey");

    // Sets source and target languages
    auto fromLanguage = L"en-US";
    vector<wstring> toLanguages{ L"de-DE", L"fr-FR" };

    // Creates a translation recognizer using file as audio input.
    // Replaces with your own audio file name.
    auto recognizer = factory->CreateTranslationRecognizerWithFileInput(L"YourAudioFileName", fromLanguage, toLanguages);

    // Starts translation. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error in speech recognition, reason " << int(result->Reason) << L"-" << result->Text << '\n';
    }
    else if (result->TranslationStatus != TranslationStatus::Success)
    {
        wcout << L"There was an error in translation, status: " << int(result->TranslationStatus) << '\n';
    }
    else
    {
        wcout << "We recognized in " << fromLanguage << ":" << result->Text << '\n';
        for (const auto& it : result->Translations)
        {
            wcout << L"    Translated into " << it.first.c_str() << ":" << it.second.c_str();
        }
    }
}
// </TranslationWithFile>

// Defines event handlers for different events.
// <TranslationContinuousRecognitionUsingEvents>
static void OnPartialResult(const TranslationTextResultEventArgs& e)
{
    wcout << L"IntermediateResult: Recognized text:" << e.Result.Text << '\n';
    for (const auto& it : e.Result.Translations)
    {
        wcout << L"    Translated into " << it.first.c_str() << ":" << it.second.c_str();
    }
}

static void OnFinalResult(const TranslationTextResultEventArgs& e)
{
    wcout << L"FinalResult: status:" << (int)e.Result.TranslationStatus << L". Recognized Text: " << e.Result.Text << '\n';
    for (const auto& it : e.Result.Translations)
    {
        wcout << L"    Translated into " << it.first.c_str() << ":" << it.second.c_str();
    }
}

static void OnSynthesisResult(const TranslationSynthesisResultEventArgs& e)
{
    wcout << L"Translation syntheis result: size of audio data: " << e.Result.Audio.size();
}

static void OnNoMatch(const TranslationTextResultEventArgs& e)
{
    wcout << L"NoMatch:" << (int)e.Result.Reason << '\n';
}

static void OnCanceled(const TranslationTextResultEventArgs& e)
{
    wcout << L"Canceled:" << (int)e.Result.Reason << L"- " << e.Result.Text << '\n';
}

// Continuous translation.
void TranslationContinuousRecognitionUsingEvents()
{
    // Gets recognizer factory.
    auto factory = RecognizerFactory::GetDefault();

    // Sets subscription key
    // Replaces with your own subscription key.
    factory->SetSubscriptionKey(L"YourSubscriptionKey");

    // Sets source and target languages
    auto fromLanguage = L"en-US";
    vector<wstring> toLanguages{ L"de-DE", L"fr-FR" };

    // Creates a translation recognizer using microphone as audio input.
    auto recognizer = factory->CreateTranslationRecognizer(fromLanguage, toLanguages);

    // Subscribes to events.
    recognizer->IntermediateResult.Connect(&OnPartialResult);
    recognizer->FinalResult.Connect(&OnFinalResult);
    recognizer->Canceled.Connect(&OnCanceled);
    recognizer->NoMatch.Connect(&OnNoMatch);
    recognizer->TranslationSynthesisResultEvent.Connect(&OnSynthesisResult);

    wcout << L"Say something...\n";

    // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    wcout << L"Press any key to stop\n";
    string s;
    getline(cin, s);

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();

    // Unsubscribes to events.
    recognizer->IntermediateResult.Disconnect(&OnPartialResult);
    recognizer->FinalResult.Disconnect(&OnFinalResult);
    recognizer->Canceled.Disconnect(&OnCanceled);
    recognizer->NoMatch.Disconnect(&OnNoMatch);
    recognizer->TranslationSynthesisResultEvent.Disconnect(&OnSynthesisResult);
}
// </TranslationContinuousRecognitionUsingEvents>

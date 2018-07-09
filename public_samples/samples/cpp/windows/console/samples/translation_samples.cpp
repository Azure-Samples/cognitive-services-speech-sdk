//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <string>
#include <vector>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Translation;
// </toplevel>


// Translation with microphone input.
void TranslationWithMicrophone()
{
    // <TranslationWithMicrophone>
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

    // Sets source and target languages
    // Replace with the languages of your choice.
    auto fromLanguage = L"en-US";
    vector<wstring> toLanguages { L"de", L"fr" };

    // Creates a translation recognizer using microphone as audio input.
    auto recognizer = factory->CreateTranslationRecognizer(fromLanguage, toLanguages);
    wcout << L"Say something...\n";

    // Starts translation.
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
    else if (result->TranslationStatus != TranslationStatusCode::Success)
    {
        wcout << L"There was an error in translation, status: " << int(result->TranslationStatus) << std::endl;
    }
    else
    {
        wcout << "We recognized in " << fromLanguage << ": " << result->Text << std::endl;
        for (const auto& it : result->Translations)
        {
            wcout << L"    Translated into " << it.first.c_str() << ": " << it.second.c_str() << std::endl;
        }
    }
    // </TranslationWithMicrophone>
}

// Continuous translation.
void TranslationContinuousRecognition()
{
    // <TranslationContinuousRecognition>
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

    // Sets source and target languages
    auto fromLanguage = L"en-US";
    vector<wstring> toLanguages{ L"de", L"fr" };

    // Creates a translation recognizer using microphone as audio input.
    auto recognizer = factory->CreateTranslationRecognizer(fromLanguage, toLanguages);

    // Subscribes to events.
    recognizer->IntermediateResult.Connect([](const TranslationTextResultEventArgs& e)
    {
        wcout << L"IntermediateResult: Recognized text:" << e.Result.Text << std::endl;
        for (const auto& it : e.Result.Translations)
        {
            wcout << L"    Translated into " << it.first.c_str() << ": " << it.second.c_str() << std::endl;
        }
    });

    recognizer->FinalResult.Connect([](const TranslationTextResultEventArgs& e)
    {
        wcout << L"FinalResult: status:" << (int)e.Result.TranslationStatus << L". Recognized Text: " << e.Result.Text << std::endl;
        for (const auto& it : e.Result.Translations)
        {
            wcout << L"    Translated into " << it.first.c_str() << ": " << it.second.c_str() << std::endl;
        }
    });

    recognizer->Canceled.Connect([](const TranslationTextResultEventArgs& e)
    {
        wcout << L"Canceled:" << (int)e.Result.Reason << L"- " << e.Result.Text << std::endl;
    });

    recognizer->TranslationSynthesisResultEvent.Connect([](const TranslationSynthesisResultEventArgs& e)
    {
        if (e.Result.SynthesisStatus == SynthesisStatusCode::Success)
        {
            wcout << L"Translation synthesis result: size of audio data: " << e.Result.Audio.size();
        }
        else if (e.Result.SynthesisStatus == SynthesisStatusCode::Error)
        {
            wcout << L"Translation synthesis error: " << e.Result.FailureReason;
        }
    });

    wcout << L"Say something...\n";

    // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    wcout << L"Press any key to stop\n";
    string s;
    getline(cin, s);

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();
    // </TranslationContinuousRecognition>
}
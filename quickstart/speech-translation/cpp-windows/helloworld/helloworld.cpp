//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#include "pch.h"
#include <iostream>
#include <vector>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Translation;

// Translation with microphone input.
void TranslationWithMicrophone()
{
    // Creates an instance of a speech translation config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechTranslationConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Sets source and target languages
    // Replace with the languages of your choice.
    auto fromLanguage = "en-US";
    config->SetSpeechRecognitionLanguage(fromLanguage);
    config->AddTargetLanguage("de");
    config->AddTargetLanguage("fr");

    // Creates a translation recognizer using microphone as audio input.
    auto recognizer = TranslationRecognizer::FromConfig(config);
    cout << "Say something...\n";

    // Starts translation. RecognizeOnceAsync() returns when the first utterance has been recognized,
    // so it is suitable only for single shot recognition like command or query. For long-running
    // recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::TranslatedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl
        << "    Language=" << fromLanguage << std::endl;

        for (const auto& it : result->Translations)
        {
            cout << "TRANSLATED into '" << it.first.c_str() << "': " << it.second.c_str() << std::endl;
        }
    }
    else if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << " (text could not be translated)" << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
}

int wmain()
{
    TranslationWithMicrophone();
    cout << "Please press a key to continue.\n";
    cin.get();
    return 0;
}
// </code>

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <string>
#include <vector>
#ifdef _WIN32
    #include <windows.h>
#endif
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Translation;
// </toplevel>


// Translation with microphone input.
void TranslationWithMicrophone()
{
    // <TranslationWithMicrophone>
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

    // Starts translation, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
    // seconds of audio is processed. The task returns the recognized text as well as the translation.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::TranslatedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl
             << "  Language=" << fromLanguage << std::endl;

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
    // </TranslationWithMicrophone>
}

// Continuous translation.
void TranslationContinuousRecognition()
{
    // Creates an instance of a speech translation config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechTranslationConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Sets source and target languages
    auto fromLanguage = "en-US";
    config->SetSpeechRecognitionLanguage(fromLanguage);
    config->AddTargetLanguage("de");
    config->AddTargetLanguage("fr");

    // Creates a translation recognizer using microphone as audio input.
    auto recognizer = TranslationRecognizer::FromConfig(config);

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const TranslationRecognitionEventArgs& e)
    {
        cout << "Recognizing:" << e.Result->Text << std::endl;
        for (const auto& it : e.Result->Translations)
        {
            cout << "  Translated into '" << it.first.c_str() << "': " << it.second.c_str() << std::endl;
        }
    });

    recognizer->Recognized.Connect([](const TranslationRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::TranslatedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << " (text could not be translated)" << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            cout << "NOMATCH: Speech could not be recognized." << std::endl;
        }

        for (const auto& it : e.Result->Translations)
        {
            cout << "  Translated into '" << it.first.c_str() << "': " << it.second.c_str() << std::endl;
        }
    });

    recognizer->Canceled.Connect([](const TranslationRecognitionCanceledEventArgs& e)
    {
        cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;
        if (e.Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    });

    recognizer->Synthesizing.Connect([](const TranslationSynthesisEventArgs& e)
    {
        auto size = e.Result->Audio.size();
        cout << "Translation synthesis result: size of audio data: " << size
             << (size == 0 ? "(END)" : "");
    });

    cout << "Say something...\n";

    // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().get();

    cout << "Press any key to stop\n";
    string s;
    getline(cin, s);

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
}

#pragma region Language Detection related samples

// Translation with microphone input, single utterance
void TranslationAndLanguageIdWithMicrophone()
{
    // <TranslationAndLanguageIdWithMicrophone>

    // When you use Language ID with speech translation, you must set a v2 endpoint and use
    // SpeechTranslationConfig::FromEndpoint() to create the SpeechTranslationConfig object.
    // This will be fixed in a future version of Speech SDK.

    // Replace "YourServiceRegion" with your service region (e.g. "westus"):
    string speechv2Endpoint = "wss://YourServiceRegion.stt.speech.microsoft.com/speech/universal/v2";

    // Replace "YourSubscriptionKey" with your speech subscription key:
    auto config = SpeechTranslationConfig::FromEndpoint(speechv2Endpoint, "YourSubscriptionKey");

    // Define the set of possible input (source) spoken languages for language detection
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "de-DE" });

    // Define the set of target (translation) languages
    config->AddTargetLanguage("es");
    config->AddTargetLanguage("fr");

    // Creates a translation recognizer using microphone as audio input.
    auto recognizer = TranslationRecognizer::FromConfig(config, autoDetectSourceLanguageConfig);
    cout << "Say something in English or German...\n";

    // Starts translation, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
    // seconds of audio is processed. The task returns the recognized text as well as the translation.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::TranslatedSpeech)
    {
        auto detectedLanguage = result->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult);
        cout << "RECOGNIZED in " << detectedLanguage << ": Text = " << result->Text << std::endl;

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
    // </TranslationAndLanguageIdWithMicrophone>
}

void ContinuousTranslationAndLanguageIdWithMultiLingualFile()
{
    // When you use Language ID with speech translation, you must set a v2 endpoint and use
    // SpeechTranslationConfig::FromEndpoint() to create the SpeechTranslationConfig object.
    // This will be fixed in a future version of Speech SDK.

    // Replace "YourServiceRegion" with your service region (e.g. "westus"):
    string speechv2Endpoint = "wss://YourServiceRegion.stt.speech.microsoft.com/speech/universal/v2";

    // Replace "YourSubscriptionKey" with your speech subscription key:
    auto config = SpeechTranslationConfig::FromEndpoint(speechv2Endpoint, "YourSubscriptionKey");

    // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
    // Please refer to the documentation of Language ID for more information.
    // https://aka.ms/speech/lid?pivots=programming-language-cpp
    config->SetProperty(PropertyId::SpeechServiceConnection_LanguageIdMode, "Continuous");

    // Define the set of possible input (source) spoken languages for language detection
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "zh-CN" });

    // Define the set of target (translation) languages
    config->AddTargetLanguage("de");
    config->AddTargetLanguage("fr");

    // Creates a translation recognizer using microphone as audio input.
    auto audioInput = AudioConfig::FromWavFileInput("en-us_zh-cn.wav");
    auto recognizer = TranslationRecognizer::FromConfig(config, autoDetectSourceLanguageConfig, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const TranslationRecognitionEventArgs& e)
        {
            std::string lidResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult);

            cout << "Recognizing in Language = "<< lidResult << ": Text=" << e.Result->Text << std::endl;
            for (const auto& it : e.Result->Translations)
            {
                cout << "  Translated into '" << it.first.c_str() << "': " << it.second.c_str() << std::endl;
            }
        });

    recognizer->Recognized.Connect([](const TranslationRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::TranslatedSpeech)
            {
                std::string lidResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult);
                cout << "RECOGNIZED in Language = " << lidResult << ": Text=" << e.Result->Text << std::endl;
            }
            else if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "RECOGNIZED: Text=" << e.Result->Text << " (text could not be translated)" << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }

            for (const auto& it : e.Result->Translations)
            {
                cout << "  Translated into '" << it.first.c_str() << "': " << it.second.c_str() << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const TranslationRecognitionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;
            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;

                recognitionEnd.set_value(); // Notify to stop recognition.
            }
        });

    recognizer->Synthesizing.Connect([](const TranslationSynthesisEventArgs& e)
        {
            auto size = e.Result->Audio.size();
            cout << "Translation synthesis result: size of audio data: " << size
                << (size == 0 ? "(END)" : "");
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().get();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
}

#pragma endregion

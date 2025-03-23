//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <speechapi_cxx.h>
#include <fstream>
#include "wav_file_reader.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
// </toplevel>

// Standalone language detection (one utterance) using microphone.
void StandaloneLanguageDetectionWithMicrophone()
{
    // <StandaloneLanguageDetectionWithMicrophone>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Define the set of spoken languages that will need to be identified
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "de-DE", "zh-CN", "en-US"});

    // Creates a speech recognizer using microphone as audio input
    auto recognizer = SourceLanguageRecognizer::FromConfig(config, autoDetectSourceLanguageConfig);
    cout << "Say something in German, Chinese or English...\n";

    // Starts Standalone language detection, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        auto lidResult = AutoDetectSourceLanguageResult::FromResult(result);
        cout << "RECOGNIZED in "<< lidResult->Language << std::endl;
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
    // </StandaloneLanguageDetectionWithMicrophone>
}

// Standalone language detection (one utterance) using WAV file input.
void StandaloneLanguageDetectionWithFileInput()
{
    // <StandaloneLanguageDetectionWithFileInput>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Request detailed output format.
    config->SetOutputFormat(OutputFormat::Detailed);

    // Define the set of spoken languages that will need to be identified
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "de-DE" });

    auto audioInput = AudioConfig::FromWavFileInput("whatstheweatherlike.wav");

    // Creates a speech recognizer
    auto recognizer = SourceLanguageRecognizer::FromConfig(config, autoDetectSourceLanguageConfig, audioInput);

    // Starts Standalone language detection, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        auto lidResult = AutoDetectSourceLanguageResult::FromResult(result);
        cout << "RECOGNIZED in " << lidResult->Language << std::endl
             << "  Speech Service JSON: " << result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult)
             << std::endl;
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
    // </StandaloneLanguageDetectionWithFileInput>
}


// Standalone language detection of continuous audio (multiple utterances), from a WAV file
void StandaloneLanguageDetectionInContinuousModeWithMultiLingualFileInput()
{
    // <StandaloneLanguageDetectionInContinuousModeWithMultiLingualFileInput>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
    // Please refer to the documentation of Language ID for more information.
    config->SetProperty(PropertyId::SpeechServiceConnection_LanguageIdMode, "Continuous");

    // Define the set of languages to detect
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "zh-CN" });

    // Creates a speech recognizer using microphone as audio input.
    auto audioInput = AudioConfig::FromWavFileInput("en-us_zh-cn.wav");
    auto recognizer = SourceLanguageRecognizer::FromConfig(config, autoDetectSourceLanguageConfig, audioInput);

    // Promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognized.Connect([](const SpeechRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                auto lidResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
                cout << "RECOGNIZED in " << lidResult->Language << "\n"
                    << "  Offset=" << e.Result->Offset() << "\n"
                    << "  Duration=" << e.Result->Duration() << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << "\n"
                    << "CANCELED: ErrorDetails=" << e.ErrorDetails << "\n"
                    << "CANCELED: Did you update the subscription info?" << std::endl;

                recognitionEnd.set_value(); // Notify to stop recognition.
            }
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().get();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
    // </StandaloneLanguageDetectionInContinuousModeWithMultiLingualFileInput>
}

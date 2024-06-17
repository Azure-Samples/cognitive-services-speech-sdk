//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <thread>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Translation;

extern shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig();
extern shared_ptr<HybridSpeechConfig> CreateHybridSpeechConfig();


// Lists available embedded speech translation models.
void ListEmbeddedSpeechTranslationModels()
{
    // Creates an instance of an embedded speech config.
    auto speechConfig = CreateEmbeddedSpeechConfig();
    if (!speechConfig)
    {
        return;
    }

    // Gets a list of models.
    auto models = speechConfig->GetSpeechTranslationModels();

    if (!models.empty())
    {
        cout << "Models found [" << models.size() << "]:" << endl;
        for (const auto& model : models)
        {
            cout << endl;
            cout << model->Name << endl;
            cout << " Source language(s) [" << model->SourceLanguages.size() << "]: ";
            for (const auto& sourceLang : model->SourceLanguages)
            {
                cout << sourceLang << " ";
            }
            cout << endl;
            cout << " Target language(s) [" << model->TargetLanguages.size() << "]: ";
            for (const auto& targetLang : model->TargetLanguages)
            {
                cout << targetLang << " ";
            }
            cout << endl;
            cout << " Path: " << model->Path << endl;
        }
    }
    else
    {
        cerr << "No models found. Either the path is not valid or the format of model(s) is unknown." << endl;
    }
}


void TranslateSpeech(shared_ptr<TranslationRecognizer> recognizer)
{
    // Subscribes to events.
    recognizer->Recognizing += [](const TranslationRecognitionEventArgs& e)
    {
        // Intermediate result (hypothesis).
        // Note that embedded "many-to-1" translation models support only one
        // target language (the model native output language). For example, a
        // "Many-to-English" model generates only output in English.
        // At the moment embedded translation cannot provide transcription of
        // the source language.
        if (e.Result->Reason == ResultReason::TranslatingSpeech)
        {
            // Source (input) language identification is enabled when TranslationRecognizer
            // is created with an AutoDetectSourceLanguageConfig argument.
            // In case the model does not support this functionality or the language cannot
            // be identified, the result Language is "Unknown".
            auto sourceLangResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
            const auto& sourceLang = sourceLangResult->Language;

            for (const auto& translation : e.Result->Translations)
            {
                auto targetLang = translation.first;
                auto outputText = translation.second;
                cout << "Translating [" << sourceLang << " -> " << targetLang << "]: " << outputText << endl;
            }
        }
    };

    recognizer->Recognized += [](const TranslationRecognitionEventArgs& e)
    {
        // Final result. May differ from the last intermediate result.
        if (e.Result->Reason == ResultReason::TranslatedSpeech)
        {
            auto sourceLangResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
            const auto& sourceLang = sourceLangResult->Language;

            for (const auto& translation : e.Result->Translations)
            {
                auto targetLang = translation.first;
                auto outputText = translation.second;
                cout << "TRANSLATED [" << sourceLang << " -> " << targetLang << "]: " << outputText << endl;
            }
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            // NoMatch occurs when no speech phrase was recognized.
            auto reason = NoMatchDetails::FromResult(e.Result)->Reason;
            cout << "NO MATCH: Reason=";
            switch (reason)
            {
            case NoMatchReason::NotRecognized:
                // Input audio was not silent but contained no recognizable speech.
                cout << "NotRecognized" << endl;
                break;
            case NoMatchReason::InitialSilenceTimeout:
                // Input audio was silent and the (initial) silence timeout expired.
                // In continuous recognition this can happen multiple times during
                // a session, not just at the very beginning.
                cout << "InitialSilenceTimeout" << endl;
                break;
            default:
                // Other reasons are not supported in embedded speech at the moment.
                cout << int(reason) << endl;
                break;
            }
        }
    };

    recognizer->Canceled += [](const TranslationRecognitionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            // Input stream was closed or the end of an input file was reached.
            cout << "CANCELED: EndOfStream" << endl;
            break;

        case CancellationReason::Error:
            // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
            cerr << "CANCELED: ErrorCode=" << int(e.ErrorCode) << endl;
            cerr << "CANCELED: ErrorDetails=\"" << e.ErrorDetails << "\"" << endl;
            break;

        default:
            cout << "CANCELED: Reason=" << int(e.Reason) << endl;
            break;
        }
    };

    recognizer->SessionStarted += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        cout << "Session started." << endl;
    };

    recognizer->SessionStopped += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        cout << "Session stopped." << endl;
    };

    // The following lines run continuous recognition that listens for speech
    // in input audio and generates results until stopped. To run recognition
    // only once, replace this code block with
    //
    // auto result = recognizer->RecognizeOnceAsync().get();
    //
    // and optionally check this returned result for the outcome instead of
    // doing it in event handlers.

    // Starts continuous recognition.
    recognizer->StartContinuousRecognitionAsync().get();

    cout << "Say something (press Enter to stop)...\n";
    cin.get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
}


// Translates speech using embedded speech config and the system default microphone device.
void EmbeddedSpeechTranslationFromMicrophone()
{
    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto sourceLangConfig = AutoDetectSourceLanguageConfig::FromOpenRange(); // optional, for input language identification
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();

    auto recognizer = TranslationRecognizer::FromConfig(speechConfig, sourceLangConfig, audioConfig);
    TranslateSpeech(recognizer);
}

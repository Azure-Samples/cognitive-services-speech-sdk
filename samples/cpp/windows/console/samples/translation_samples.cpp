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
#include "wav_file_reader.h"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <thread>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Translation;
// </toplevel>


// Translation with microphone input.
void TranslationWithMicrophone()
{
    // <TranslationWithMicrophone>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechTranslationConfig::FromEndpoint("YourServiceEndpoint", "YourSubscriptionKey");

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
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
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


// Speech translation with an inline commit on a push stream.
void TranslationWithInlineCommit()
{
    // <TranslationWithInlineCommit>
    const char* endpoint = getenv("SPEECH_ENDPOINT");
    const char* subscriptionKey = getenv("SPEECH_RESOURCE_KEY");
    if (endpoint == nullptr || subscriptionKey == nullptr || !*endpoint || !*subscriptionKey)
    {
        throw invalid_argument("Set SPEECH_ENDPOINT and SPEECH_RESOURCE_KEY before running this sample.");
    }
    auto config = SpeechTranslationConfig::FromEndpoint(endpoint, subscriptionKey);
    config->SetSpeechRecognitionLanguage("en-US");
    config->AddTargetLanguage("de");
    config->AddTargetLanguage("fr");
    // Enable inline commit on a service deployment that supports the feature.
    config->SetServiceProperty("setfeature", "forcecommit", ServicePropertyChannel::UriQueryParameter);

    WavFileReader reader("whatstheweatherlike.wav");
    mutex resultMutex;
    condition_variable resultReceived;
    bool recognitionDone = false;
    uint32_t acknowledgedToken = 0;
    string cancellationError;

    // The input must be 16 kHz, 16-bit, mono PCM audio.
    auto pushStream = AudioInputStream::CreatePushStream();
    auto recognizer = TranslationRecognizer::FromConfig(config, AudioConfig::FromStreamInput(pushStream));

    recognizer->Recognizing.Connect([](const TranslationRecognitionEventArgs& e)
    {
        cout << "RECOGNIZING: Text=" << e.Result->Text << endl;
    });
    recognizer->Recognized.Connect([&](const TranslationRecognitionEventArgs& e)
    {
        lock_guard<mutex> lock(resultMutex);
        cout << "RECOGNIZED: Reason=" << (int)e.Result->Reason << " Text=" << e.Result->Text;
        // NoMatch can acknowledge a commit; naturally segmented results have token 0.
        if (e.Result->CommitToken() != 0)
        {
            acknowledgedToken = e.Result->CommitToken();
            cout << " commit_token=" << acknowledgedToken;
        }
        cout << endl;
        for (const auto& translation : e.Result->Translations)
        {
            cout << "  " << translation.first << ": " << translation.second << endl;
        }
        resultReceived.notify_all();
    });
    recognizer->Canceled.Connect([&](const TranslationRecognitionCanceledEventArgs& e)
    {
        lock_guard<mutex> lock(resultMutex);
        cout << "CANCELED: Reason=" << (int)e.Reason << " Details=" << e.ErrorDetails << endl;
        if (e.Reason == CancellationReason::Error)
        {
            cancellationError = e.ErrorDetails.empty() ? "Recognition canceled." : e.ErrorDetails;
        }
        recognitionDone = true;
        resultReceived.notify_all();
    });
    recognizer->SessionStopped.Connect([&](const SessionEventArgs& e)
    {
        lock_guard<mutex> lock(resultMutex);
        cout << "SESSION STOPPED: SessionId=" << e.SessionId << endl;
        recognitionDone = true;
        resultReceived.notify_all();
    });

    // Send the next durationMilliseconds of audio; omit the duration to send the rest.
    // This helper keeps the file position and reuses the same push stream.
    auto streamAudio = [&](uint32_t durationMilliseconds = 0)
    {
        const uint32_t bytesPerSecond = 16000 * 2; // 16 kHz, 16-bit, mono PCM.
        uint32_t bytesRemaining = bytesPerSecond * durationMilliseconds / 1000;
        vector<uint8_t> buffer(3200); // 100 ms of audio per write.
        while (durationMilliseconds == 0 || bytesRemaining > 0)
        {
            {
                lock_guard<mutex> lock(resultMutex);
                if (recognitionDone)
                {
                    throw runtime_error("Recognition ended before all audio was written. " + cancellationError);
                }
            }
            uint32_t bytesToRead = static_cast<uint32_t>(buffer.size());
            if (durationMilliseconds != 0)
            {
                bytesToRead = (std::min)(bytesToRead, bytesRemaining);
            }
            int bytesRead = reader.Read(buffer.data(), bytesToRead);
            if (bytesRead == 0)
            {
                if (durationMilliseconds != 0)
                {
                    throw runtime_error("The WAV file ended before the requested duration.");
                }
                break;
            }
            pushStream->Write(buffer.data(), static_cast<uint32_t>(bytesRead));
            if (durationMilliseconds != 0)
            {
                bytesRemaining -= static_cast<uint32_t>(bytesRead);
            }
            // Send at approximately the same pace as live audio.
            this_thread::sleep_for(chrono::duration<double>(static_cast<double>(bytesRead) / bytesPerSecond));
        }
    };

    bool streamClosed = false;
    try
    {
        recognizer->StartContinuousRecognitionAsync().get();
        // 1. Stream the first 590 ms: approximately "what's the" in this recording.
        cout << "Writing audio segment 1" << endl;
        streamAudio(590);

        // 2. Commit the first part without closing the stream or stopping recognition.
        uint32_t token = pushStream->Commit();
        cout << "COMMIT: token=" << token << endl;
        if (token == 0)
        {
            throw runtime_error("The SDK rejected the commit request.");
        }
        {
            // Keep the callback's token even if the acknowledgment arrives before Commit() returns.
            unique_lock<mutex> lock(resultMutex);
            resultReceived.wait_for(lock, chrono::seconds(15), [&]
            {
                return acknowledgedToken == token || recognitionDone;
            });
            if (acknowledgedToken != token)
            {
                throw runtime_error("No acknowledgment before timeout or session end. Check inline commit support. "
                    + cancellationError);
            }
            cout << "COMMIT ACKNOWLEDGED: token=" << token << endl;
        }

        // 3. Stream the rest ("weather like") using the same stream and recognizer.
        cout << "Writing audio segment 2" << endl;
        streamAudio();

        // Closing the stream lets the service finalize the remaining audio normally.
        pushStream->Close();
        streamClosed = true;
        unique_lock<mutex> lock(resultMutex);
        if (!resultReceived.wait_for(lock, chrono::seconds(15), [&] { return recognitionDone; }))
        {
            throw runtime_error("Timed out waiting for recognition to finish after the end of audio.");
        }
        if (!cancellationError.empty())
        {
            throw runtime_error(cancellationError);
        }
    }
    catch (...)
    {
        if (!streamClosed)
        {
            pushStream->Close();
        }
        recognizer->StopContinuousRecognitionAsync().get();
        throw;
    }
    recognizer->StopContinuousRecognitionAsync().get();
    // </TranslationWithInlineCommit>
}

// Continuous translation.
void TranslationContinuousRecognition()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechTranslationConfig::FromEndpoint("YourServiceEndpoint", "YourSubscriptionKey");

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

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
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
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
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

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().get();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
}

#pragma endregion

#pragma region Multilingual Translation with language identification
void ContinuousMultilingualTranslationWithLanguageIdentification()
{
    // When you use Multilingual Translation with language identification, 
    // you don't need to define any candidate languages to detect, but you must set a v2 endpoint and use
    // SpeechTranslationConfig::FromEndpoint() to create the SpeechTranslationConfig object.
    // This will be fixed in a future version of Speech SDK.

    // Replace "YourServiceRegion" with your service region (e.g. "westus"):
    string speechv2Endpoint = "wss://YourServiceRegion.stt.speech.microsoft.com/speech/universal/v2";

    // Replace "YourSubscriptionKey" with your speech subscription key:
    auto config = SpeechTranslationConfig::FromEndpoint(speechv2Endpoint, "YourSubscriptionKey");

    // You don't need to define any candidate languages to detect.
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromOpenRange();

    // Define the set of target (translation) languages
    config->AddTargetLanguage("de");
    config->AddTargetLanguage("fr");

    // Creates a translation recognizer using audio file as audio input.
    auto audioInput = AudioConfig::FromWavFileInput("en-us_zh-cn.wav");
    auto recognizer = TranslationRecognizer::FromConfig(config, autoDetectSourceLanguageConfig, audioInput);

    // Promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const TranslationRecognitionEventArgs& e)
        {
            std::string lidResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult);

            cout << "Recognizing in Language = "<< lidResult << ": Text=" << e.Result->Text << std::endl;
            if (e.Result->Reason == ResultReason::TranslatingSpeech) {
                for (const auto& it : e.Result->Translations)
                {
                    cout << "  Translated into '" << it.first.c_str() << "': " << it.second.c_str() << std::endl;
                }
            }
        });

    recognizer->Recognized.Connect([](const TranslationRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::TranslatedSpeech)
            {
                std::string lidResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult);
                cout << "RECOGNIZED in Language = " << lidResult << ": Text=" << e.Result->Text << std::endl;
                
                for (const auto& it : e.Result->Translations)
                {
                    cout << "  Translated into '" << it.first.c_str() << "': " << it.second.c_str() << std::endl;
                }
            }
            else if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "RECOGNIZED: Text=" << e.Result->Text << " (text could not be translated)" << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
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
}

#pragma endregion

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <speechapi_cxx.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "wav_file_reader.h"
#include <vector>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

// Pronunciation assessment.
void PronunciationAssessmentWithMicrophone()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // The pronunciation assessment service has a longer default end silence timeout (5 seconds) than normal STT
    // as the pronunciation assessment is widely used in education scenario where kids have longer break in reading.
    // You can adjust the end silence timeout based on your real scenario.
    config->SetProperty(PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs, "3000");

    std::string referenceText = "";
    // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    auto pronunciationConfig = PronunciationAssessmentConfig::Create(referenceText,
        PronunciationAssessmentGradingSystem::HundredMark,
        PronunciationAssessmentGranularity::Phoneme, true);

    pronunciationConfig->EnableProsodyAssessment();

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = SpeechRecognizer::FromConfig(config, "en-US");

    recognizer->SessionStarted.Connect([](const SessionEventArgs& e) {
        std::cout << "SESSION ID: " << e.SessionId << std::endl;
        });

    while (true)
    {
        // Receives reference text from console input.
        cout << "Enter reference text that you want to assess, or enter empty text to exit." << std::endl;
        cout << "> ";
        getline(cin, referenceText);
        if (referenceText.empty())
        {
            break;
        }

        pronunciationConfig->SetReferenceText(referenceText);
        cout << "Read out \"" << referenceText << "\" for pronunciation assessment ..." << endl;

        pronunciationConfig->ApplyTo(recognizer);

        // Starts speech recognition, and returns after a single utterance is recognized.
        // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
        auto result = recognizer->RecognizeOnceAsync().get();

        // Checks result.
        if (result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << result->Text << std::endl
                 <<"  PRONUNCIATION ASSESSMENT RESULTS:";

            auto pronunciationResult = PronunciationAssessmentResult::FromResult(result);

            cout << "    Accuracy score: " << pronunciationResult->AccuracyScore << ", Prosody Score: " << pronunciationResult->ProsodyScore << ", Pronunciation score: "
                 << pronunciationResult->PronunciationScore << ", Completeness score: " << pronunciationResult->CompletenessScore
                 << ", FluencyScore: " << pronunciationResult->FluencyScore << endl;
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
}

void PronunciationAssessmentWithStreamInternalAsync(shared_ptr<SpeechConfig> speechConfig, std::string referenceText, std::vector<uint8_t> audioData, std::promise<int> resultReceived, std::vector<std::string>& resultContainer)
{
    auto audioFormat = AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1); // This need be set based on the format of the given audio data
    auto audioInputStream = AudioInputStream::CreatePushStream(audioFormat);
    auto audioConfig = AudioConfig::FromStreamInput(audioInputStream);
    // Specify the language used for Pronunciation Assessment
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, "en-US", audioConfig);

    // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    auto pronAssessmentConfig = PronunciationAssessmentConfig::Create(referenceText, PronunciationAssessmentGradingSystem::HundredMark, PronunciationAssessmentGranularity::Phoneme, false);

    pronAssessmentConfig->EnableProsodyAssessment();

    speechRecognizer->SessionStarted.Connect([](const SessionEventArgs& e) {
        std::cout << "SESSION ID: " << e.SessionId << std::endl;
        });

    pronAssessmentConfig->ApplyTo(speechRecognizer);

    audioInputStream->Write(audioData.data(), static_cast<uint32_t>(audioData.size()));
    audioInputStream->Write(nullptr, 0);// send a zero-size chunk to signal the end of stream

    auto result = speechRecognizer->RecognizeOnceAsync().get();
    if (result->Reason == ResultReason::Canceled)
    {
        auto cancellationDetails = CancellationDetails::FromResult(result);
        std::cout << cancellationDetails->ErrorDetails << std::endl;
    }
    else
    {
        auto responsePA = PronunciationAssessmentResult::FromResult(result);
        std::string responseResult = "PRONUNCIATION ASSESSMENT RESULTS : \n";
        responseResult = responseResult + "  Accuracy score: " + std::to_string(responsePA->AccuracyScore) + "  Prosody score: " + std::to_string(responsePA->ProsodyScore) + ", Pronunciation score: " + std::to_string(responsePA->PronunciationScore) + ", Completeness score : " + std::to_string(responsePA->CompletenessScore) + ", FluencyScore: " + std::to_string(responsePA->FluencyScore);

        resultContainer.push_back(responseResult);
    }

    resultReceived.set_value(1);
}

// Pronunciation assessment with audio stream input.
// See more information at https://aka.ms/csspeech/pa
void PronunciationAssessmentWithStream()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Read audio data from file. In real scenario this can be from memory or network
    std::ifstream file("whatstheweatherlike.wav", std::ios::binary | std::ios::ate);
    auto audioDataWithHeaderSize = file.tellg();
    file.seekg(46);
    auto audioData = std::vector<uint8_t>(static_cast<size_t>(audioDataWithHeaderSize) - static_cast<size_t>(46));

    file.read((char*)audioData.data(), audioData.size());

    std::promise<int> resultReceived;
    std::future<int> futureResult = resultReceived.get_future();
    std::vector<std::string> resultContainer;

    auto startTime = std::chrono::high_resolution_clock::now();

    auto task = std::async(std::launch::async, PronunciationAssessmentWithStreamInternalAsync, config, "what's the weather like", audioData, std::move(resultReceived), std::ref(resultContainer));
    
    int result = futureResult.get();
    const auto& resultPA = resultContainer[0];

    auto endTime = std::chrono::high_resolution_clock::now();

    std::cout << resultPA << std::endl;

    auto timeCost = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "Time cost: " << timeCost << "ms" << std::endl;
}

// Pronunciation assessment configured with json
void PronunciationAssessmentConfiguredWithJson()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech recognizer from an audio file
    auto audioConfig = AudioConfig::FromWavFileInput("whatstheweatherlike.wav");

    std::string referenceText = "what's the weather like";

    // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    std::string json_config = "{\"GradingSystem\":\"HundredMark\",\"Granularity\":\"Phoneme\",\"EnableMiscue\":true, \"ScenarioId\":\"[scenario ID will be assigned by product team]\"}";
    auto pronunciationConfig = PronunciationAssessmentConfig::CreateFromJson(json_config);
    pronunciationConfig->SetReferenceText(referenceText);

    pronunciationConfig->EnableProsodyAssessment();

    // Creates a speech recognizer.
    auto recognizer = SpeechRecognizer::FromConfig(config, "en-US", audioConfig);

    recognizer->SessionStarted.Connect([](const SessionEventArgs& e) {
        std::cout << "SESSION ID: " << e.SessionId << std::endl;
        });

    pronunciationConfig->ApplyTo(recognizer);

    // Starts speech recognition, and returns after a single utterance is recognized.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl
            << "  PRONUNCIATION ASSESSMENT RESULTS:";

        auto pronunciationResult = PronunciationAssessmentResult::FromResult(result);

        cout << "    Accuracy score: " << pronunciationResult->AccuracyScore << ", Prosody Score: " << pronunciationResult->ProsodyScore << ", Pronunciation score: "
            << pronunciationResult->PronunciationScore << ", Completeness score : " << pronunciationResult->CompletenessScore
            << ", FluencyScore: " << pronunciationResult->FluencyScore << endl;
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

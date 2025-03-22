//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <speechapi_cxx.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "wav_file_reader.h"
#include <vector>
#include <future>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
// </toplevel>

// Speech recognition using microphone.
void SpeechRecognitionWithMicrophone()
{
    // <SpeechRecognitionWithMicrophone>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
    auto recognizer = SpeechRecognizer::FromConfig(config);
    cout << "Say something...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;
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
    // </SpeechRecognitionWithMicrophone>
}


// Speech recognition in the specified language, using microphone, and requesting detailed output format.
void SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Request for detailed recognition result
    config->SetOutputFormat(OutputFormat::Detailed);

    // If you also want word-level timing in the detailed recognition results, set the following.
    // Note that if you set the following, you can omit the previous line
    //   "config->SetOutputFormat(OutputFormat::Detailed)",
    // since word-level timing implies detailed recognition results.
    config->RequestWordLevelTimestamps();

    // Creates a speech recognizer in the specified language using microphone as audio input.
    // Replace the language with your language in BCP-47 format, e.g. en-US.
    auto lang = "de-DE";
    auto recognizer = SpeechRecognizer::FromConfig(config, lang);
    cout << "Say something in " << lang << "...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;

        // Time units are in hundreds of nanoseconds (HNS), where 10000 HNS equals 1 millisecond
        cout << "Offset: " << result->Offset() << std::endl
             << "Duration: " << result->Duration() << std::endl;

        // Get access to the JSON string that includes detailed speech recognition results
        string jsonString = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        // cout << "  Speech Service JSON: " << jsonString << std::endl;

        // Extract the "NBest" array of recognition results from the JSON.
        // Note that the first cell in the NBest array corresponds to the recognition results 
        // (NOT the cell with the highest confidence number!)
        auto jsonResult = nlohmann::json::parse(jsonString);
        auto nbestArray = jsonResult["NBest"];
        for (size_t i = 0; i < nbestArray.size(); i++)
        {
            auto nbestItem = nbestArray[i];
            cout << "\tConfidence: " << nbestItem["Confidence"] << std::endl;
            cout << "\tLexical: " << nbestItem["Lexical"] << std::endl;
            cout << "\tITN: " <<  nbestItem["ITN"] << std::endl; // ITN stands for Inverse Text Normalization
            cout << "\tMaskedITN: " << nbestItem["MaskedITN"] << std::endl;
            cout << "\tDisplay: " << nbestItem["Display"] << std::endl;

            // Word-level timing
            auto wordsArray = nbestItem["Words"];
            cout << "\t\tWord | Offset | Duration" << std::endl;
            for (size_t j = 0; j < wordsArray.size(); j++)
            {
                auto wordItem = wordsArray[j];
                cout << "\t\t" << wordItem["Word"] << " " << wordItem["Offset"] << " " << wordItem["Duration"] << std::endl;
            }
        }
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

void SpeechContinuousRecognitionWithFile()
{
    // <SpeechContinuousRecognitionWithFile>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("whatstheweatherlike.wav");
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing: Text=" << e.Result->Text << std::endl;
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << "\n"
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

    recognizer->SessionStarted.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session started." << std::endl;
    });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped." << std::endl;
        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().get();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
    // </SpeechContinuousRecognitionWithFile>
}

// Speech recognition using a customized model.
void SpeechRecognitionUsingCustomizedModel()
{
    // <SpeechRecognitionUsingCustomizedModel>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");
    // Set the endpoint ID of your customized model
    // Replace with your own CRIS endpoint ID.
    config->SetEndpointId("YourEndpointId");

    config->SetSpeechRecognitionLanguage("en-US");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = SpeechRecognizer::FromConfig(config);

    cout << "Say something...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;
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
    // </SpeechRecognitionUsingCustomizedModel>
}

void SpeechContinuousRecognitionWithPullStream()
{
    // First, define your own pull audio input stream callback class that implements the
    // PullAudioInputStreamCallback interface. The sample here illustrates how to define such
    // a callback that reads audio data from a wav file.
    // AudioInputFromFileCallback implements PullAudioInputStreamCallback interface, and uses a wav file as source
    class AudioInputFromFileCallback final : public PullAudioInputStreamCallback
    {
    public:
        // Constructor that creates an input stream from a file.
        AudioInputFromFileCallback(const string& audioFileName)
            : m_reader(audioFileName)
        {
        }

        // Implements AudioInputStream::Read() which is called to get data from the audio stream.
        // It copies data available in the stream to 'dataBuffer', but no more than 'size' bytes.
        // If the data available is less than 'size' bytes, it is allowed to just return the amount of data that is currently available.
        // If there is no data, this function must wait until data is available.
        // It returns the number of bytes that have been copied in 'dataBuffer'.
        // It returns 0 to indicate that the stream reaches end or is closed.
        int Read(uint8_t* dataBuffer, uint32_t size) override
        {
            return m_reader.Read(dataBuffer, size);
        }
        // Implements AudioInputStream::Close() which is called when the stream needs to be closed.
        void Close() override
        {
            m_reader.Close();
        }

    private:
        WavFileReader m_reader;
    };

    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a callback that will read audio data from a WAV file.
    // Currently, the only supported WAV format is mono(single channel), 16 kHZ sample rate, 16 bits per sample.
    // Replace with your own audio file name.
    auto callback = make_shared<AudioInputFromFileCallback>("whatstheweatherlike.wav");
    auto pullStream = AudioInputStream::CreatePullStream(callback);

    // Creates a speech recognizer from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pullStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing: Text=" << e.Result->Text << std::endl;
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl
                 << "  Offset=" << e.Result->Offset() << std::endl
                 << "  Duration=" << e.Result->Duration() << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            cout << "NOMATCH: Speech could not be recognized." << std::endl;
        }
    });

    recognizer->Canceled.Connect([&recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            cout << "CANCELED: Reach the end of the file." << std::endl;
            break;

        case CancellationReason::Error:
            cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            recognitionEnd.set_value();
            break;

        default:
            cout << "unknown reason ?!" << std::endl;
        }
    });

    recognizer->SessionStarted.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session started." << std::endl;
    });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped." << std::endl;
        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    // Waits for recognition end.
    recognitionEnd.get_future().wait();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();
}

void SpeechContinuousRecognitionWithPushStream()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a push stream
    auto pushStream = AudioInputStream::CreatePushStream();

    // Creates a speech recognizer from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing: Text=" << e.Result->Text << std::endl;
    });

    recognizer->Recognized.Connect([](const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl
                << "  Offset=" << e.Result->Offset() << std::endl
                << "  Duration=" << e.Result->Duration() << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            cout << "NOMATCH: Speech could not be recognized." << std::endl;
        }
    });

    recognizer->Canceled.Connect([&recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            cout << "CANCELED: Reach the end of the file." << std::endl;
            break;

        case CancellationReason::Error:
            cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            recognitionEnd.set_value();
            break;

        default:
            cout << "CANCELED: received unknown reason." << std::endl;
        }

    });

    recognizer->SessionStarted.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session started." << std::endl;
    });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped." << std::endl;
        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    WavFileReader reader("whatstheweatherlike.wav");

    vector<uint8_t> buffer(1000);

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    // Read data and push them into the stream
    int readSamples = 0;
    while((readSamples = reader.Read(buffer.data(), (uint32_t)buffer.size())) != 0)
    {
        // Push a buffer into the stream
        pushStream->Write(buffer.data(), readSamples);
    }

    // Close the push stream.
    pushStream->Close();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
}

// Keyword-triggered speech recognition using microphone.
void KeywordTriggeredSpeechRecognitionWithMicrophone()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
    auto recognizer = SpeechRecognizer::FromConfig(config);

    // Promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            cout << "RECOGNIZING: Text=" << e.Result->Text << std::endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizingKeyword)
        {
            cout << "RECOGNIZING KEYWORD: Text=" << e.Result->Text << std::endl;
        }
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedKeyword)
        {
            cout << "RECOGNIZED KEYWORD: Text=" << e.Result->Text << std::endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl;
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
        }
    });

    recognizer->SessionStarted.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "SESSIONSTARTED: SessionId=" << e.SessionId << std::endl;
    });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "SESSIONSTOPPED: SessionId=" << e.SessionId << std::endl;

        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    // Creates an instance of a keyword recognition model. Update this to
    // point to the location of your keyword recognition model.
    auto model = KeywordRecognitionModel::FromFile("YourKeywordRecognitionModelFile.table");

    // The phrase your keyword recognition model triggers on.
    auto keyword = "YourKeyword";

    // Starts continuous recognition. Use StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartKeywordRecognitionAsync(model).get();

    cout << "Say something starting with '" << keyword
         << "' followed by whatever you want..." << std::endl;

    // Waits for a single successful keyword-triggered speech recognition (or error).
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopKeywordRecognitionAsync().get();
}

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

// Callback function for writing response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string GetChatCompletion(const std::string& oaiResourceName, const std::string& oaiDeploymentName, const std::string& oaiApiVersion, const std::string& oaiApiKey, const std::string& sendText) {
    using json = nlohmann::json;

    CURL* curl;
    CURLcode res;
    std::string sampleSentence1 = "OK the movie i like to talk about is the cove it is very say phenomenal sensational documentary about adopting hunting practices in japan i think the director is called well i think the name escapes me anyway but well let's talk about the movie basically it's about dolphin hunting practices in japan there's a small village where where villagers fisherman Q almost twenty thousand dolphins on a yearly basis which is brutal and just explain massacre this book has influenced me a lot i still remember the first time i saw this movie i think it was in middle school one of my teachers showed it to all the class or the class and i remember we were going through some really boring topics like animal protection at that time it was really boring to me but right before everyone was going to just sleep in the class the teacher decided to put the textbook down and show us a clear from this document documentary we were shocked speechless to see the has of the dolphins chopped off and left on the beach and the C turning bloody red with their blood which is i felt sick i couldn't need fish for a whole week and it was lasting impression if not scarring impression and i think this movie is still very meaningful and it despite me a lot especially on wildlife protection dolphins or search beautiful intelligent animals of the sea and why do villagers and fishermen in japan killed it i assume there was a great benefit to its skin or some scientific research but the ironic thing is that they only kill them for the meat because the meat taste great that sickens me for awhile and i think the book inspired me to do a lot of different to do a lot of things about well i protection i follow news like";
    std::string sampleSentence2 = "yes i can speak how to this movie is it is worth young wolf young man this is this movie from korea it's a crime movies the movies on the movies speaker speaker or words of young man love hello a cow are you saying they end so i have to go to the go to the america or ha ha ha lots of years a go on the woman the woman is very old he talk to korea he carpool i want to go to the this way this whole home house this house is a is hey so what's your man and at the end the girl cause so there's a woman open open hum finally finds other wolf so what's your young man so the young man don't so yeah man the young man remember he said here's a woman also so am i it's very it's very very sad she is she is a crack credit thank you ";
    std::string sampleSentence3 = "yes i want i want to talk about the TV series are enjoying watching a discount name is a friends and it's uh accommodate in the third decades decades an it come out the third decades and its main characters about a six friends live in the NYC but i watched it a long time ago i can't remember the name of them and the story is about what they are happening in their in their life and there are many things treating them and how the friendship are hard friendship and how the french how the strong strongly friendship they obtain them and they always have some funny things happen they only have happened something funny things and is a comedy so that was uh so many and i like this be cause of first adult cause it has a funding it has a farming serious and it can improve my english english words and on the other hand it can i can know about a lot of cultures about the united states and i i first hear about death TV series it's come out of a website and i took into and i watch it after my after my finish my studies and when i was a bad mood when i when i'm in a bad mood or i ";
    std::string topic = "the season of the fall";
    // URL formation
    std::string url = "https://" + oaiResourceName + ".openai.azure.com/openai/deployments/" + oaiDeploymentName + "/chat/completions?api-version=" + oaiApiVersion;

    // JSON data to send in the POST request
    json requestData = {
        {"messages", {
            {{"role", "system"}, {"content", "You are an English teacher and please help to grade a student's essay from vocabulary and grammar and topic relevance on how well the essay aligns with the title, and output format as: {\"vocabulary\": *.*(0-100), \"grammar\": *.*(0-100), \"topic\": *.*(0-100)}."}},
            {{"role", "user"}, {"content", "Example1: this essay: \"" + sampleSentence1 + "\" has vocabulary and grammar scores of 8 and 8, respectively. Example2: this essay : \"" + sampleSentence2 + "\" has vocabulary and grammar scores of 4 and 4.3, respectively. Example3: this essay: \"" + sampleSentence3 + "\" has vocabulary and grammar scores of 5 and 5, respectively. The essay for you to score is " + sendText + ", and the title is \"" + topic + "\". The script is from speech recognition so that please first add punctuations when needed, remove duplicates and unnecessary un uh from oral speech, then find all the misuse of words and grammar errors in this essay, find advanced words and grammar usages, and finally give scores based on this information. Please only respond as this format {\"vocabulary\": *.*(0-100), \"grammar\": *.*(0-100)}, \"topic\": *.*(0-100)}."}}
        }},
        {"temperature", 0},
        {"top_p", 1}
    };

    std::string jsonData = requestData.dump();

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::string responseContent;
    if (curl) {
        struct curl_slist* headers = nullptr;

        // Set the API key header
        headers = curl_slist_append(headers, ("api-key: " + oaiApiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Set options for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.length());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Set the write function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseContent);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }

        // Clean up curl
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    // Global cleanup
    curl_global_cleanup();

    // Parse the JSON response
    auto jsonResponse = json::parse(responseContent);
    return jsonResponse["choices"][0]["message"]["content"].get<std::string>();
}

// Pronunciation assessment with content assessment
void PronunciationAssessmentWithContentAssessment()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");
    using json = nlohmann::json;
    std::string oaiResourceName = "YourAoaiResourceName";
    std::string oaiDeploymentName = "YourAoaiDeploymentName";
    std::string oaiApiVersion = "YourAoaiApiVersion";
    std::string oaiApiKey = "YourAoaiApiKey";

    // Creates a speech recognizer from an audio file
    auto audioConfig = AudioConfig::FromWavFileInput("pronunciation_assessment_fall.wav");

    // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    auto pronunciationConfig = PronunciationAssessmentConfig::Create("", PronunciationAssessmentGradingSystem::HundredMark, PronunciationAssessmentGranularity::Phoneme, false);

    pronunciationConfig->EnableProsodyAssessment();

    // Creates a speech recognizer.
    auto recognizer = SpeechRecognizer::FromConfig(config, "en-US", audioConfig);

    recognizer->SessionStarted.Connect([](const SessionEventArgs& e) {
        std::cout << "SESSION ID: " << e.SessionId << std::endl;
        });

    pronunciationConfig->ApplyTo(recognizer);

    vector<string> recognizedTexts;
    std::shared_ptr<PronunciationContentAssessmentResult> contentResult;
    promise<void> recognitionEnd;

    recognizer->SessionStopped += [&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped." << endl;
        recognitionEnd.set_value();
    };

    recognizer->Canceled += [&recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            cout << "CANCELED: Reach the end of the file." << std::endl;
            break;

        case CancellationReason::Error:
            cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            recognitionEnd.set_value();
            break;

        default:
            cout << "CANCELED: received unknown reason." << std::endl;
        }
    };

    recognizer->Recognized += [&recognizedTexts, &contentResult](const SpeechRecognitionEventArgs& e)
    {
        string text = e.Result->Text;
        if (!text.empty() && text != ".")
            recognizedTexts.push_back(text);
    };

    recognizer->StartContinuousRecognitionAsync().wait();

    recognitionEnd.get_future().get(); // Waits for recognition end.

    recognizer->StopContinuousRecognitionAsync().get();

    std::ostringstream connectedText;

    for (size_t i = 0; i < recognizedTexts.size(); ++i) {
        connectedText << recognizedTexts[i];
        if (i != recognizedTexts.size() - 1) {
            connectedText << " ";
        }
    }

    cout << "Content assessment for: " << endl;
    cout << connectedText.str() << endl;

    std::string result = GetChatCompletion(oaiResourceName, oaiDeploymentName, oaiApiVersion, oaiApiKey, connectedText.str());

    try {
        // Parse JSON response
        json contentResult = json::parse(result);

        if (!contentResult.is_null()) {
            // Check and print vocabulary score
            if (contentResult.contains("vocabulary")) {
                cout << "Vocabulary score: " << fixed << std::setprecision(1) << contentResult["vocabulary"].get<double>() << endl;
            }
            else {
                cout << "Vocabulary key not found." << endl;
            }

            // Check and print grammar score
            if (contentResult.contains("grammar")) {
                cout << "Grammar score: " << fixed << std::setprecision(1) << contentResult["grammar"].get<double>() << endl;
            }
            else {
                cout << "Grammar key not found." << endl;
            }

            // Check and print topic score
            if (contentResult.contains("topic")) {
                cout << "Topic score: " << fixed << std::setprecision(1) << contentResult["topic"].get<double>() << endl;
            }
            else {
                cout << "Topic key not found." << endl;
            }
        }
        else {
            cout << "Deserialization failed." << endl;
        }
    }
    catch (exception& ex) {
        cerr << "An error occurred: " << ex.what() << endl;
    }
}

#pragma region Language Detection related samples

void SpeechRecognitionAndLanguageIdWithMicrophone()
{
    // <SpeechRecognitionAndLanguageIdWithMicrophone>
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto speechConfig = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Define the set of spoken languages that will need to be identified.
    // Replace the languages with your languages in BCP-47 format, e.g. "fr-FR".
    // Please see https://learn.microsoft.com/azure/cognitive-services/speech-service/language-support?tabs=language-identification
    // for the full list of supported languages
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "de-DE" });

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig);
    cout << "Say something in English or German...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
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
        cout << "RECOGNIZED in " << lidResult->Language << ": Text=" << result->Text << std::endl;
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
    // </SpeechRecognitionAndLanguageIdWithMicrophone>
}


void SpeechRecognitionAndLanguageIdWithCustomModelsWithMicrophone()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto speechConfig = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    std::vector<std::shared_ptr<SourceLanguageConfig>> sourceLanguageConfigs;

    // Replace the languages with your languages in BCP-47 format, e.g. zh-CN.
    sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("en-US"));

    // Replace the languages with your languages in BCP-47 format, e.g. zh-CN.
    // Please see https://learn.microsoft.com/azure/cognitive-services/speech-service/language-support?tabs=language-identification for all supported languages.
    // Set the endpoint ID of your customized mode that will be used for fr-FR,  Replace with your own CRIS endpoint ID.
    sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("fr-FR", "The Endpoint Id for custom model of fr-FR"));

    // Construct AutoDetectSourceLanguageConfig with the 2 source language configurations
    // Currently this feature only supports 2 different language candidates
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(sourceLanguageConfigs);

    // Creates a speech recognizer using the auto detect source language config
    // The recognizer uses microphone,  to use file or stream as audio input, just construct the audioInput and pass to FromConfig API as the 3rd parameter.
    // Ex: auto recognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioInput);
    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig);
    cout << "Say something in either English or French...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    auto autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult::FromResult(result);
    auto language = autoDetectSourceLanguageResult->Language;
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;
        cout << "RECOGNIZED: Language=" << language << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        if (language.empty())
        {
            // serivce cannot detect the source language
            cout << "NOMATCH: Service cannot detect the source language." << std::endl;
        }
        else
        {
            // serivce can detect the source language but cannot recongize the speech content
            cout << "NOMATCH: Service can recognize the speech." << std::endl;
        }
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


void SpeechContinuousRecognitionAndLanguageIdWithMultiLingualFile()
{
    // <SpeechContinuousRecognitionAndLanguageIdWithMultiLingualFile>

    // Creates an instance of a speech config with specified subscription key and service region.
    // Note: For multi-lingual speech recognition with language id, it only works with speech v2 endpoint,
    // you must use FromEndpoint api in order to use the speech v2 endpoint.

    // Replace YourServiceRegion with your region, for example "westus", and
    // replace YourSubscriptionKey with your own speech key.
    string speechv2Endpoint = "wss://YourServiceRegion.stt.speech.microsoft.com/speech/universal/v2";
    auto speechConfig = SpeechConfig::FromEndpoint(speechv2Endpoint, "YourSubscriptionKey");
    
    // Set the mode of input language detection to either "AtStart" (the default) or "Continuous".
    // Please refer to the documentation of Language ID for more information.
    // https://aka.ms/speech/lid?pivots=programming-language-cpp
    speechConfig->SetProperty(PropertyId::SpeechServiceConnection_LanguageIdMode, "Continuous");

    // Define the set of languages to detect
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "zh-CN" });

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("en-us_zh-cn.wav");
    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const SpeechRecognitionEventArgs& e)
        {
            auto lidResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
            cout << "Recognizing in " << lidResult->Language << ": Text =" << e.Result->Text << std::endl;
        });

    recognizer->Recognized.Connect([](const SpeechRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                auto lidResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
                cout << "RECOGNIZED in " << lidResult->Language << ": Text=" << e.Result->Text << "\n"
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
    // </SpeechContinuousRecognitionAndLanguageIdWithMultiLingualFile>
}

#pragma endregion

// Speech recognition from default microphone with Microsoft Audio Stack enabled.
void SpeechContinuousRecognitionFromDefaultMicrophoneWithMASEnabled()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates an instance of audio config using default microphone as audio input and with audio processing options specified.
    // All default enhancements from Microsoft Audio Stack are enabled.
    // Only works when input is from a microphone array.
    // On Windows, microphone array geometry is obtained from the driver. On other operating systems, a single channel (mono)
    // microphone is assumed.
    auto audioProcessingOptions = AudioProcessingOptions::Create(AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT);
    auto audioInput = AudioConfig::FromDefaultMicrophoneInput(audioProcessingOptions);

    // Creates a speech recognizer.
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing:" << e.Result->Text << std::endl;
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << "\n"
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
}

// Speech recognition from a microphone with Microsoft Audio Stack enabled and pre-defined microphone array geometry specified.
void SpeechRecognitionFromMicrophoneWithMASEnabledAndPresetGeometrySpecified()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates an instance of audio config using a microphone as audio input and with audio processing options specified.
    // All default enhancements from Microsoft Audio Stack are enabled and preset microphone array geometry is specified
    // in audio processing options.
    auto audioProcessingOptions = AudioProcessingOptions::Create(AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT, PresetMicrophoneArrayGeometry::Linear2);
    auto audioInput = AudioConfig::FromMicrophoneInput("<device id>", audioProcessingOptions);

    // Creates a speech recognizer.
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
    cout << "Say something..." << std::endl;

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;
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

// Speech recognition from multi-channel file with Microsoft Audio Stack enabled and custom microphone array geometry specified.
void SpeechContinuousRecognitionFromMultiChannelFileWithMASEnabledAndCustomGeometrySpecified()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates an instance of audio config using multi-channel WAV file as audio input and with audio processing options specified.
    // All default enhancements from Microsoft Audio Stack are enabled and custom microphone array geometry is provided.
    MicrophoneArrayGeometry microphoneArrayGeometry
    {
        MicrophoneArrayType::Planar,
        // Approximate coordinates for a microphone array with one microphone in the center and six microphones evenly spaced
        // in a circle with radius approximately equal to 42.5 mm.
        { { 0, 0, 0 }, { 40, 0, 0 }, { 20, -35, 0 }, { -20, -35, 0 }, { -40, 0, 0 }, { -20, 35, 0 }, { 20, 35, 0 } }
    };
    auto audioProcessingOptions = AudioProcessingOptions::Create(AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT,
                                                                microphoneArrayGeometry,
                                                                SpeakerReferenceChannel::LastChannel);
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("katiesteve.wav", audioProcessingOptions);

    // Creates a speech recognizer.
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing:" << e.Result->Text << std::endl;
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << "\n"
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
}

// Speech recognition from pull stream with custom set of enhancements from Microsoft Audio Stack enabled.
void SpeechRecognitionFromPullStreamWithSelectMASEnhancementsEnabled()
{
    // First, define your own pull audio input stream callback class that implements the
    // PullAudioInputStreamCallback interface. The sample here illustrates how to define such
    // a callback that reads audio data from a wav file.
    // AudioInputFromFileCallback implements PullAudioInputStreamCallback interface, and uses a wav file as source
    class AudioInputFromFileCallback final : public PullAudioInputStreamCallback
    {
    public:
        // Constructor that creates an input stream from a file.
        AudioInputFromFileCallback(const string& audioFileName)
            : m_reader(audioFileName)
        {
        }

        // Implements AudioInputStream::Read() which is called to get data from the audio stream.
        // It copies data available in the stream to 'dataBuffer', but no more than 'size' bytes.
        // If the data available is less than 'size' bytes, it is allowed to just return the amount of data that is currently available.
        // If there is no data, this function must wait until data is available.
        // It returns the number of bytes that have been copied in 'dataBuffer'.
        // It returns 0 to indicate that the stream reaches end or is closed.
        int Read(uint8_t* dataBuffer, uint32_t size) override
        {
            return m_reader.Read(dataBuffer, size);
        }
        // Implements AudioInputStream::Close() which is called when the stream needs to be closed.
        void Close() override
        {
            m_reader.Close();
        }

    private:
        WavFileReader m_reader;
    };

    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a callback that will read audio data from a WAV file.
    // Microsoft Audio Stack supports sample rates that are integral multiples of 16 KHz. Additionally, the following
    // formats are supported: 32-bit IEEE little endian float, 32-bit little endian signed int, 24-bit little endian signed int,
    // 16-bit little endian signed int, and 8-bit signed int.
    // Replace with your own audio file name.
    auto callback = make_shared<AudioInputFromFileCallback>("whatstheweatherlike.wav");
    auto pullStream = AudioInputStream::CreatePullStream(callback);

    // Creates an instance of audio config with pull stream as audio input and with audio processing options specified.
    // All default enhancements from Microsoft Audio Stack are enabled except acoustic echo cancellation and preset
    // microphone array geometry is specified in audio processing options.
    auto audioProcessingOptions = AudioProcessingOptions::Create(AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT | AUDIO_INPUT_PROCESSING_DISABLE_ECHO_CANCELLATION,
                                                                PresetMicrophoneArrayGeometry::Mono);
    auto audioInput = AudioConfig::FromStreamInput(pullStream, audioProcessingOptions);

    // Creates a speech recognizer.
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;
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

// Speech recognition from push stream with Microsoft Audio Stack enabled and beamforming angles specified.
void SpeechContinuousRecognitionFromPushStreamWithMASEnabledAndBeamformingAnglesSpecified()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a push stream.
    auto pushStream = AudioInputStream::CreatePushStream(AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8));

    // Creates an instance of audio config using push stream as audio input and with audio processing options specified.
    // All default enhancements from Microsoft Audio Stack are enabled and custom microphone array geometry with beamforming
    // angles is specified.
    MicrophoneArrayGeometry microphoneArrayGeometry
    {
        MicrophoneArrayType::Planar,
        70U,
        110U,
        // Approximate coordinates for a microphone array with one microphone in the center and six microphones evenly spaced
        // in a circle with radius approximately equal to 42.5 mm.
        { { 0, 0, 0 }, { 40, 0, 0 }, { 20, -35, 0 }, { -20, -35, 0 }, { -40, 0, 0 }, { -20, 35, 0 }, { 20, 35, 0 } }
    };
    auto audioProcessingOptions = AudioProcessingOptions::Create(AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT,
                                                                microphoneArrayGeometry,
                                                                SpeakerReferenceChannel::LastChannel);
    auto audioInput = AudioConfig::FromStreamInput(pushStream, audioProcessingOptions);

    // Creates a speech recognizer.
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing:" << e.Result->Text << std::endl;
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << "\n"
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

    WavFileReader reader("katiesteve.wav");

    vector<uint8_t> buffer(1024);

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    // Read data and push them into the stream
    int readSamples = 0;
    while((readSamples = reader.Read(buffer.data(), (uint32_t)buffer.size())) != 0)
    {
        // Push a buffer into the stream
        pushStream->Write(buffer.data(), readSamples);
    }

    // Close the push stream.
    pushStream->Close();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
}

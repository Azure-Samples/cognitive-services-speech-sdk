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

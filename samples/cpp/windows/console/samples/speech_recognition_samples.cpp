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

// Speech recognition using microphone.
void SpeechRecognitionWithMicrophone()
{
    // <SpeechRecognitionWithMicrophone>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
    auto recognizer = SpeechRecognizer::FromConfig(config);
    cout << "Say something...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
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
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Request detailed output format.
    config->SetOutputFormat(OutputFormat::Detailed);

    // Creates a speech recognizer in the specified language using microphone as audio input.
    // Replace the language with your language in BCP-47 format, e.g. en-US.
    auto lang = "de-DE";
    auto recognizer = SpeechRecognizer::FromConfig(config, lang);
    cout << "Say something in " << lang << "...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl
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
}

void SpeechContinuousRecognitionWithFile()
{
    // <SpeechContinuousRecognitionWithFile>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("whatstheweatherlike.wav");
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
    // </SpeechContinuousRecognitionWithFile>
}

// Speech recognition using a customized model.
void SpeechRecognitionUsingCustomizedModel()
{
    // <SpeechRecognitionUsingCustomizedModel>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    // Set the endpoint ID of your customized model
    // Replace with your own CRIS endpoint ID.
    config->SetEndpointId("YourEndpointId");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = SpeechRecognizer::FromConfig(config);

    cout << "Say something...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
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

    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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
        cout << "Recognizing:" << e.Result->Text << std::endl;
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

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped.";
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
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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
        cout << "Recognizing:" << e.Result->Text << std::endl;
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

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped.";
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
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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

// Speech recognition with auto detection for source language
void SpeechRecognitionWithSourceLanguageAutoDetection()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto speechConfig = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

     // Currently this feature only supports 2 different language candidates
     // Replace the languages with your languages in BCP-47 format, e.g. fr-FR.
     // Please see https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support for all supported languages
     auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "de-DE" });

     // The recognizer uses microphone,  to use file or stream as audio input, just construct the audioInput and pass to FromConfig API as the 3rd parameter.
     // Ex: auto recognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioInput);
     auto recognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig);
     cout << "Say something in either English or German...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
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

// Speech recognition with auto detection for source language and using customized model
void SpeechRecognitionWithSourceLanguageAutoDetectionUsingCustomizedModel()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    std::vector<std::shared_ptr<SourceLanguageConfig>> sourceLanguageConfigs;
    // Replace the languages with your languages in BCP-47 format, e.g. zh-CN.
    sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("en-US"));
    // Replace the languages with your languages in BCP-47 format, e.g. zh-CN.
    // Please see https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support for all supported languages
    // Set the endpoint ID of your customized mode that will be used for fr-FR,  Replace with your own CRIS endpoint ID.
    sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("fr-FR", "The Endpoint Id for custom model of fr-FR"));
    // Construct AutoDetectSourceLanguageConfig with the 2 source language configurations
    // Currently this feature only supports 2 different language candidates
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(sourceLanguageConfigs);

    // Creates a speech recognizer using the auto detect source language config
    // The recognizer uses microphone,  to use file or stream as audio input, just construct the audioInput and pass to FromConfig API as the 3rd parameter.
    // Ex: auto recognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioInput);
    auto recognizer = SpeechRecognizer::FromConfig(config, autoDetectSourceLanguageConfig);
    cout << "Say something in either English or French...\n";

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
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

// Pronunciation assessment.
void PronunciationAssessmentWithMicrophone()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    // Note: The pronunciation assessment feature is currently only available on westus, eastasia and centralindia regions.
    // And this feature is currently only available on en-US language.
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    std::string referenceText = "";
    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    auto pronunciationConfig = PronunciationAssessmentConfig::Create(referenceText,
        PronunciationAssessmentGradingSystem::HundredMark,
        PronunciationAssessmentGranularity::Phoneme, true);

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = SpeechRecognizer::FromConfig(config);

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

            cout << "    Accuracy score: " << pronunciationResult->AccuracyScore << ", Pronunciation score: "
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
}

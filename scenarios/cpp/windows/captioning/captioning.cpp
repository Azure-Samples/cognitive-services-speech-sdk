//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// NOTES:
// - Install the NuGet package Microsoft.CognitiveServices.Speech.
// - The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
// https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk
// - In Windows, place the following files from the Speech SDK in the same folder as your compiled .exe:
//     - Microsoft.CognitiveServices.Speech.core.dll
//     - Microsoft.CognitiveServices.Speech.extension.audio.sys.dll

#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <speechapi_cxx.h>
#include "helper.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Speaker;

class Captioning
{
private:

    std::shared_ptr<UserConfig> m_userConfig = NULL;
    std::shared_ptr<AudioStreamFormat> m_format = NULL;
    std::shared_ptr<BinaryFileReader> m_callback = NULL;
    std::shared_ptr<PullAudioInputStream> m_stream = NULL;

    std::string TimestampFromSpeechRecognitionResult(std::shared_ptr<SpeechRecognitionResult> result)
    {
        std::ostringstream strTimestamp;
        auto timestamp = TimestampFromTicks(result->Offset(), result->Offset() + result->Duration());    
        
        std::ostringstream startSeconds_1, endSeconds_1;
        // setw value is 2 for seconds + 1 for floating point + 3 for decimal places.
        startSeconds_1 << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << timestamp.startSeconds;
        endSeconds_1 << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << timestamp.endSeconds;    
        
        if (m_userConfig->useSubRipTextCaptionFormat)
        {
            // SRT format requires ',' as decimal separator rather than '.'.
            std::string startSeconds_2(startSeconds_1.str());
            std::string endSeconds_2(endSeconds_1.str());
            replace(startSeconds_2.begin(), startSeconds_2.end(), '.', ',');
            replace(endSeconds_2.begin(), endSeconds_2.end(), '.', ',');
            strTimestamp << std::setfill('0') << std::setw(2) << timestamp.startHours << ":"
                << std::setfill('0') << std::setw(2) << timestamp.startMinutes << ":" << startSeconds_2 << " --> "
                << std::setfill('0') << std::setw(2) << timestamp.endHours << ":"
                << std::setfill('0') << std::setw(2) << timestamp.endMinutes << ":" << endSeconds_2;
        }
        else
        {
            strTimestamp << std::setfill('0') << std::setw(2) << timestamp.startHours << ":"
                << std::setfill('0') << std::setw(2) << timestamp.startMinutes << ":" << startSeconds_1.str() << " --> "
                << std::setfill('0') << std::setw(2) << timestamp.endHours << ":"
                << std::setfill('0') << std::setw(2) << timestamp.endMinutes << ":" << endSeconds_1.str();
        }
        
        return strTimestamp.str ();
    }

    std::string LanguageFromSpeechRecognitionResult(std::shared_ptr<SpeechRecognitionResult> result)
    {
        if (m_userConfig->languageIDLanguages.has_value())
        {
            auto languageIDResult = AutoDetectSourceLanguageResult::FromResult(result);
            return "[" + languageIDResult->Language + "] ";
        }
        else
        {
            return "";
        }
    }

    std::string CaptionFromSpeechRecognitionResult(int sequenceNumber, std::shared_ptr<SpeechRecognitionResult> result)
    {
        std::string caption;
        if (!m_userConfig->showRecognizingResults && m_userConfig->useSubRipTextCaptionFormat)
        {
            caption += sequenceNumber + "\n";
        }
        caption += TimestampFromSpeechRecognitionResult(result) + "\n"
            + LanguageFromSpeechRecognitionResult(result)
            + result->Text + "\n\n";
        return caption;
    }

    void WriteToConsole(std::string text)
    {
        if (!m_userConfig->suppressConsoleOutput)
        {
            std::cout << text << std::flush;
        }
    }

    void WriteToConsoleOrFile(std::string text)
    {
        WriteToConsole(text);
        if (m_userConfig->outputFile.has_value())
        {
            std::ofstream outputStream;
            outputStream.open(m_userConfig->outputFile.value(), std::ios_base::app);
            outputStream << text;
            outputStream.close();
        }
    }

    std::shared_ptr<Audio::AudioConfig> AudioConfigFromUserConfig()
    {
        if (m_userConfig->inputFile.has_value())
        {
            if (EndsWith(m_userConfig->inputFile.value(), ".wav"))
            {
                auto reader = std::make_shared<WavFileReader>(m_userConfig->inputFile.value());
                m_format = AudioStreamFormat::GetWaveFormatPCM(reader->GetFormat().SamplesPerSec, reader->GetFormat().BitsPerSample, reader->GetFormat().Channels);
                reader->Close();
            }
            else
            {
                m_format = AudioStreamFormat::GetCompressedFormat(m_userConfig->compressedAudioFormat);
            }
            m_callback = std::make_shared<BinaryFileReader>(m_userConfig->inputFile.value());
            m_stream = AudioInputStream::CreatePullStream(m_format, m_callback);
            return AudioConfig::FromStreamInput(m_stream);
        }
        else
        {
            return Audio::AudioConfig::FromDefaultMicrophoneInput();
        }
    }

    std::shared_ptr<SpeechConfig> SpeechConfigFromUserConfig()
    {
        std::shared_ptr<SpeechConfig> speechConfig;
        if (m_userConfig->languageIDLanguages.has_value())
        {
            auto endpoint = V2EndpointFromRegion(m_userConfig->region);
            speechConfig = SpeechConfig::FromEndpoint(endpoint, m_userConfig->subscriptionKey);
        }
        else
        {
            speechConfig = SpeechConfig::FromSubscription(m_userConfig->subscriptionKey, m_userConfig->region);
        }

        speechConfig->SetProfanity(m_userConfig->profanityOption);

        if (m_userConfig->stablePartialResultThreshold.has_value())
        {
            // Note: To get default value:
            // std::cout << speechConfig->GetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold) << std::endl;
            speechConfig->SetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold, m_userConfig->stablePartialResultThreshold.value());
        }
        
        speechConfig->SetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption, "TrueText");
        
        return speechConfig;
    }

public:
    Captioning(std::shared_ptr<UserConfig> userConfig)
        : m_userConfig(userConfig)
    {
        if (m_userConfig->outputFile.has_value())
        {
            std::filesystem::path outputFile { m_userConfig->outputFile.value() };
            // If the output file exists, truncate it.
            if (std::filesystem::exists(outputFile))
            {
                std::ofstream outputStream;
                outputStream.open(m_userConfig->outputFile.value());
                outputStream.close();            
            }
        }
        if (!m_userConfig->useSubRipTextCaptionFormat && !m_userConfig->showRecognizingResults)
        {
            WriteToConsoleOrFile("WEBVTT\n\n");
        }
    }

    std::shared_ptr<SpeechRecognizer> SpeechRecognizerFromUserConfig()
    {
        auto audioConfig = AudioConfigFromUserConfig();
        auto speechConfig = SpeechConfigFromUserConfig();
        std::shared_ptr<SpeechRecognizer> speechRecognizer;
        
        if (m_userConfig->languageIDLanguages.has_value())
        {
            auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages(m_userConfig->languageIDLanguages.value());
            speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioConfig);
        }
        else
        {
            speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        }

        if (m_userConfig->phraseList.has_value())
        {
            auto grammar = PhraseListGrammar::FromRecognizer(speechRecognizer);
            grammar->AddPhrase(m_userConfig->phraseList.value());
        }
        
        return speechRecognizer;
    }

    std::optional<std::string> RecognizeContinuous(std::shared_ptr<SpeechRecognizer> speechRecognizer)
    {
        std::promise<std::optional<std::string>> recognitionEnd;
        auto sequenceNumber = 0;

        if (m_userConfig->showRecognizingResults) {
            // Capture variables we will need inside the lambda.
            // See:
            // https://www.cppstories.com/2020/08/lambda-capturing.html/
            speechRecognizer->Recognizing.Connect([this](const SpeechRecognitionEventArgs& e)
                {
                    if (ResultReason::RecognizingSpeech == e.Result->Reason && e.Result->Text.length() > 0)
                    {
                        // We don't show sequence numbers for partial results.
                        WriteToConsole(CaptionFromSpeechRecognitionResult(0, e.Result));
                    }
                    else if (ResultReason::NoMatch == e.Result->Reason)
                    {
                        WriteToConsole("NOMATCH: Speech could not be recognized.\n");
                    }
                });
        }

        speechRecognizer->Recognized.Connect([this, &sequenceNumber](const SpeechRecognitionEventArgs& e)
            {
                if (ResultReason::RecognizedSpeech == e.Result->Reason && e.Result->Text.length() > 0)
                {
                    sequenceNumber++;
                    WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber, e.Result));
                }
                else if (ResultReason::NoMatch == e.Result->Reason)
                {
                    WriteToConsole("NOMATCH: Speech could not be recognized.\n");
                }
            });

        speechRecognizer->Canceled.Connect([this, &recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
            {
                if (CancellationReason::EndOfStream == e.Reason)
                {
                    WriteToConsole("End of stream reached.\n");
                    recognitionEnd.set_value(std::nullopt); // Notify to stop recognition.
                }
                else if (CancellationReason::CancelledByUser == e.Reason)
                {
                    WriteToConsole("User canceled request.\n");
                    recognitionEnd.set_value(std::nullopt); // Notify to stop recognition.
                }
                else if (CancellationReason::Error == e.Reason)
                {
                    std::ostringstream error;
                    error << "Encountered error.\n"
                        << "ErrorCode: " << (int)e.ErrorCode << "\n"
                        << "ErrorDetails: " << e.ErrorDetails << std::endl;
                    recognitionEnd.set_value(std::optional<std::string>{ error.str() }); // Notify to stop recognition.
                }
                else
                {
                    std::ostringstream error;
                    error << "Request was cancelled for an unrecognized reason: " << (int)e.Reason << std::endl;
                    recognitionEnd.set_value(std::optional<std::string>{ error.str() }); // Notify to stop recognition.
                }
            });

        speechRecognizer->SessionStopped.Connect([this, &recognitionEnd](const SessionEventArgs& e)
            {
                WriteToConsole("Session stopped.\n");
                recognitionEnd.set_value(std::nullopt); // Notify to stop recognition.
            });

        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
        speechRecognizer->StartContinuousRecognitionAsync().get();

        // Waits for recognition end.
        auto result = recognitionEnd.get_future().get();

        // Stops recognition.
        speechRecognizer->StopContinuousRecognitionAsync().get();
        
        return result;
    }
};

int main(int argc, char* argv[])
{
    const std::string usage = "Usage: caption.exe [...]\n\n"
"  CONNECTION\n"
"    --key KEY                     Your Azure Speech service subscription key.\n"
"    --region REGION               Your Azure Speech service region.\n"
"                                  Examples: westus, eastus\n\n"
"  LANGUAGE\n"
"    --languages LANG1,LANG2       Enable language identification for specified languages.\n"
"                                  Example: en-US,ja-JP\n\n"
"  INPUT\n"
"    --input FILE                  Input audio from file (default input is the microphone.)\n"
"    --url URL                     Input audio from URL (default input is the microphone.)\n"
"    --format FORMAT               Use compressed audio format.\n"
"                                  Valid only with --file or --url.\n"
"                                  If this is not specified, uncompressed format (wav) is assumed.\n"
"                                  Valid values: alaw, any, flac, mp3, mulaw, ogg_opus\n"
"                                  Default value: any\n\n"
"  RECOGNITION\n"
"    --recognizing                 Output Recognizing results (default output is Recognized results only.)\n"
"                                  These are always written to the console, never to an output file.\n"
"                                  --quiet overrides this.\n\n"
"  ACCURACY\n"
"    --phrases PHRASE1;PHRASE2     Example: Constoso;Jessie;Rehaan\n\n"
"  OUTPUT\n"
"    --help                        Show this help and stop.\n"
"    --output FILE                 Output captions to file.\n"
"    --srt                         Output captions in SubRip Text format (default format is WebVTT.)\n"
"    --quiet                       Suppress console output, except errors.\n"
"    --profanity OPTION            Valid values: raw, remove, mask\n"
"    --threshold NUMBER            Set stable partial result threshold.\n"
"                                  Default value: 3\n";

    try
    {
        if (CmdOptionExists(argv, argv + argc, "--help"))
        {
            std::cout << usage << std::endl;
        }
        else
        {
            auto userConfig = UserConfigFromArgs(argc, argv, usage);
            auto captioning = std::make_shared<Captioning>(userConfig);
            auto speechRecognizer = captioning->SpeechRecognizerFromUserConfig();
            auto error = captioning->RecognizeContinuous(speechRecognizer);
            if (error.has_value())
            {
                std::cout << error.value() << std::endl;
            }
        }
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/* NOTES:
- In Windows, place the following files from the Speech SDK in the same folder as your compiled .exe:
    - Microsoft.CognitiveServices.Speech.core.dll
    - Microsoft.CognitiveServices.Speech.extension.audio.sys.dll
*/

#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>

/* Notes:
- Install the NuGet package Microsoft.CognitiveServices.Speech.
- The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=windows%2Cubuntu%2Cios-xcode%2Cmac-xcode%2Candroid-studio
*/
#include <speechapi_cxx.h>

#include "helper.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Speaker;

class TooFewArguments : public std::exception
{
    public: const char* what() const throw()
    {
        return "Too few arguments.\n";
    }
};

/* Simple command line argument parsing functions. See
https://stackoverflow.com/a/868894
The value parameter to std::find must be std::string. Otherwise std::find will compare the pointer values rather than the string contents.
*/
std::optional<std::string> GetCmdOption(char** begin, char** end, const std::string& option)
{
    char** result = std::find(begin, end, option);
    if (result != end)
    {
        // We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
        char** value = ++result;
        // std::string(nullptr) is undefined.        
        if (value != end && nullptr != *value)
        {
            return std::optional<std::string>{ std::string(*value) };
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        return std::nullopt;
    }
}

inline bool CmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while(std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// See:
// https://stackoverflow.com/a/42844629
static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static std::string V2EndpointFromRegion(std::string region)
{
    // Note: Continuous language identification is supported only with v2 endpoints.
    return std::string("wss://" + region + ".stt.speech.microsoft.com/speech/universal/v2");
}

static Timestamp TimestampFromTicks(uint64_t startTicks, uint64_t endTicks)
{
    const float ticksPerSecond = 10000000.0;

    const float startSeconds_1 = startTicks / ticksPerSecond;
    const float endSeconds_1 = endTicks / ticksPerSecond;
    
    const int startMinutes_1 = startSeconds_1 / 60;
    const int endMinutes_1 = endSeconds_1 / 60;    
    
    const int startHours = startMinutes_1 / 60;
    const int endHours = endMinutes_1 / 60;
    
    const float startSeconds_2 = fmod(startSeconds_1, 60.0);
    const float endSeconds_2 = fmod(endSeconds_1, 60.0);
    
    const float startMinutes_2 = fmod(startMinutes_1, 60.0);
    const float endMinutes_2 = fmod(endMinutes_1, 60.0);
    
    return Timestamp(startHours, endHours, startMinutes_2, endMinutes_2, startSeconds_2, endSeconds_2);
}

static UserConfig UserConfigFromArgs(int argc, char* argv[])
{
    // Verify argc >= 3 (caption.exe, subscriptionKey, region)
    if (argc < 3)
    {
        throw TooFewArguments();
    }

    std::optional<std::vector<std::string>> languageIDLanguages = std::nullopt;
    std::optional<std::string> languageIDLanguagesResult = GetCmdOption(argv, argv + argc, "-l");
    if (languageIDLanguagesResult.has_value())
    {
        languageIDLanguages = std::optional<std::vector<std::string>>{ split(languageIDLanguagesResult.value(), ',') };
    }

    return UserConfig(
        CmdOptionExists(argv, argv + argc, "-f"),
        CmdOptionExists(argv, argv + argc, "-m"),
        languageIDLanguages,
        GetCmdOption(argv, argv + argc, "-i"),
        GetCmdOption(argv, argv + argc, "-o"),
        GetCmdOption(argv, argv + argc, "-p"),
        CmdOptionExists(argv, argv + argc, "-q"),
        CmdOptionExists(argv, argv + argc, "-u"),
        GetCmdOption(argv, argv + argc, "-t"),
        CmdOptionExists(argv, argv + argc, "-s"),
        argv[argc - 2],
        argv[argc - 1]
    );
}

class Captioning
{
private:

    UserConfig m_userConfig;
    std::shared_ptr<AudioStreamFormat> m_format = NULL;
    std::shared_ptr<AudioInputFromFileCallback> m_callback = NULL;
    std::shared_ptr<PullAudioInputStream> m_stream = NULL;

    std::string TimestampFromSpeechRecognitionResult(std::shared_ptr<SpeechRecognitionResult> result)
    {
        std::ostringstream strTimestamp;
        Timestamp timestamp = TimestampFromTicks(result->Offset(), result->Offset() + result->Duration());    
        
        std::ostringstream startSeconds_1, endSeconds_1;
        // setw value is 2 for seconds + 1 for floating point + 3 for decimal places.
        startSeconds_1 << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << timestamp.startSeconds;
        endSeconds_1 << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << timestamp.endSeconds;    
        
        if (m_userConfig.srtEnabled)
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
        if (m_userConfig.languageIDLanguages.has_value())
        {
            std::shared_ptr<AutoDetectSourceLanguageResult> languageIDResult = AutoDetectSourceLanguageResult::FromResult(result);
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
        if (!m_userConfig.partialResultsEnabled && m_userConfig.srtEnabled)
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
        if (!m_userConfig.suppressOutputEnabled)
        {
            std::cout << text << std::flush;
        }
    }

    void WriteToConsoleOrFile(std::string text)
    {
        WriteToConsole(text);
        if (m_userConfig.outputFile.has_value())
        {
            std::ofstream outputStream;
            outputStream.open(m_userConfig.outputFile.value(), std::ios_base::app);
            outputStream << text;
            outputStream.close();
        }
    }

    std::shared_ptr<Audio::AudioConfig> AudioConfigFromUserConfig()
    {
        if (m_userConfig.inputFile.has_value())
        {
            if (endsWith(m_userConfig.inputFile.value(), ".wav"))
            {
                auto reader = std::make_shared<WavFileReader>(m_userConfig.inputFile.value());
                m_format = AudioStreamFormat::GetWaveFormatPCM(reader->m_formatHeader.SamplesPerSec, reader->m_formatHeader.BitsPerSample, reader->m_formatHeader.Channels);
                reader->Close();
            }
            else
            {
// TODO1
//                m_format = AudioStreamFormat::GetCompressedFormat(m_userConfig.compressedAudioFormat);
                m_format = AudioStreamFormat::GetCompressedFormat(AudioStreamContainerFormat::ANY);
            }
            m_callback = std::make_shared<AudioInputFromFileCallback>(m_userConfig.inputFile.value());
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
        if (m_userConfig.languageIDLanguages.has_value())
        {
            std::string endpoint = V2EndpointFromRegion(m_userConfig.region);
            speechConfig = SpeechConfig::FromEndpoint(endpoint, m_userConfig.subscriptionKey);
        }
        else
        {
            speechConfig = SpeechConfig::FromSubscription(m_userConfig.subscriptionKey, m_userConfig.region);
        }

        if (m_userConfig.profanityFilterRemoveEnabled)
        {
            speechConfig->SetProfanity(ProfanityOption::Removed);
        }
        else if (m_userConfig.profanityFilterMaskEnabled)
        {
            speechConfig->SetProfanity(ProfanityOption::Masked);
        }
        
        if (m_userConfig.stablePartialResultThreshold.has_value())
        {
            // Note: To get default value:
            // std::cout << speechConfig->GetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold) << std::endl;
            speechConfig->SetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold, m_userConfig.stablePartialResultThreshold.value());
        }
        
        speechConfig->SetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption, "TrueText");
        
        return speechConfig;
    }

public:
    Captioning(UserConfig userConfig)
        : m_userConfig(userConfig)
    {
        if (m_userConfig.outputFile.has_value())
        {
            std::filesystem::path outputFile { m_userConfig.outputFile.value() };
            // If the output file exists, truncate it.
            if (std::filesystem::exists(outputFile))
            {
                std::ofstream outputStream;
                outputStream.open(m_userConfig.outputFile.value());
                outputStream.close();            
            }
        }
        if (!m_userConfig.srtEnabled && !m_userConfig.partialResultsEnabled)
        {
            WriteToConsoleOrFile("WEBVTT\n\n");
        }
    }

    std::shared_ptr<SpeechRecognizer> SpeechRecognizerFromUserConfig()
    {
        std::shared_ptr<Audio::AudioConfig> audioConfig = AudioConfigFromUserConfig();
        std::shared_ptr<SpeechConfig> speechConfig = SpeechConfigFromUserConfig();
        std::shared_ptr<SpeechRecognizer> speechRecognizer;
        
        if (m_userConfig.languageIDLanguages.has_value())
        {
            std::shared_ptr<AutoDetectSourceLanguageConfig> autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages(m_userConfig.languageIDLanguages.value());
            speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioConfig);
        }
        else
        {
            speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        }

        if (m_userConfig.phraseList.has_value())
        {
            std::shared_ptr<PhraseListGrammar> grammar = PhraseListGrammar::FromRecognizer(speechRecognizer);
            grammar->AddPhrase(m_userConfig.phraseList.value());
        }
        
        return speechRecognizer;
    }

    std::optional<std::string> RecognizeContinuous(std::shared_ptr<SpeechRecognizer> speechRecognizer)
    {
        std::promise<std::optional<std::string>> recognitionEnd;
        int sequenceNumber = 0;

        if (m_userConfig.partialResultsEnabled) {
            /* Capture variables we will need inside the lambda.
            See:
            https://www.cppstories.com/2020/08/lambda-capturing.html/
            */
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
        std::optional<std::string> result = recognitionEnd.get_future().get();

        // Stops recognition.
        speechRecognizer->StopContinuousRecognitionAsync().get();
        
        return result;
    }
};

int main(int argc, char* argv[])
{
    const std::string usage = "Usage: caption.exe [-f] [-h] [-i file] [-l languages] [-m] [-o file] [-p phrases] [-q] [-s] [-t number] [-u] <subscriptionKey> <region>\n"
    "              -f: Enable profanity filter (remove profanity). Overrides -m.\n"
    "              -h: Show this help and stop.\n"
    "              -i: Input audio file *file* (default input is from the microphone.)\n"
    "    -l languages: Enable language identification for specified *languages*.\n"
    "                  Example: en-US,ja-JP\n"
    "              -m: Enable profanity filter (mask profanity). -f overrides this.\n"
    "         -o file: Output to *file*.\n"
    "      -p phrases: Add specified *phrases*.\n"
    "                  Example: Constoso;Jessie;Rehaan\n"
    "              -q: Suppress console output (except errors).\n"
    "              -s: Output captions in SRT format (default is WebVTT format.)\n"
    "       -t number: Set stable partial result threshold to *number*.\n"
    "                  Example: 3\n"
    "              -u: Output partial results. These are always written to the console, never to an output file. -q overrides this.\n";

    try
    {
        if (CmdOptionExists(argv, argv + argc, "-h"))
        {
            std::cout << usage << std::endl;
        }
        else
        {
            auto userConfig = UserConfigFromArgs(argc, argv);
            auto captioning = std::make_shared<Captioning>(userConfig);
            std::shared_ptr<SpeechRecognizer> speechRecognizer = captioning->SpeechRecognizerFromUserConfig();
            std::optional<std::string> error = captioning->RecognizeContinuous(speechRecognizer);
            if (error.has_value())
            {
                std::cout << error.value() << std::endl;
            }
        }
    }
    catch (TooFewArguments e)
    {
        std::cout << e.what() << "\n"
            << usage << std::endl;
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}

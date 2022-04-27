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

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Speaker;

class TooFewArguments : public exception
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
optional<string> GetCmdOption(char** begin, char** end, const string& option)
{
    char** result = find(begin, end, option);
    if (result != end)
    {
        // We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
        char** value = ++result;
        // std::string(nullptr) is undefined.        
        if (value != end && nullptr != *value)
        {
            return optional<string>{ string(*value) };
        }
        else
        {
            return nullopt;
        }
    }
    else
    {
        return nullopt;
    }
}

inline bool CmdOptionExists(char** begin, char** end, const string& option)
{
    return find(begin, end, option) != end;
}

vector<string> split(const string& s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while(getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// See:
// https://stackoverflow.com/a/42844629
static bool endsWith(const string& str, const string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static string V2EndpointFromRegion(string region)
{
    // Note: Continuous language identification is supported only with v2 endpoints.
    return string("wss://" + region + ".stt.speech.microsoft.com/speech/universal/v2");
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

    optional<vector<string>> languageIDLanguages = nullopt;
    optional<string> languageIDLanguagesResult = GetCmdOption(argv, argv + argc, "-l");
    if (languageIDLanguagesResult.has_value())
    {
        languageIDLanguages = optional<vector<string>>{ split(languageIDLanguagesResult.value(), ',') };
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
    shared_ptr<AudioStreamFormat> m_format = NULL;
    shared_ptr<AudioInputFromFileCallback> m_callback = NULL;
    shared_ptr<PullAudioInputStream> m_stream = NULL;

    string TimestampFromSpeechRecognitionResult(shared_ptr<SpeechRecognitionResult> result)
    {
        ostringstream strTimestamp;
        Timestamp timestamp = TimestampFromTicks(result->Offset(), result->Offset() + result->Duration());    
        
        ostringstream startSeconds_1, endSeconds_1;
        // setw value is 2 for seconds + 1 for floating point + 3 for decimal places.
        startSeconds_1 << setfill('0') << setw(6) << fixed << setprecision(3) << timestamp.startSeconds;
        endSeconds_1 << setfill('0') << setw(6) << fixed << setprecision(3) << timestamp.endSeconds;    
        
        if (m_userConfig.srtEnabled)
        {
            // SRT format requires ',' as decimal separator rather than '.'.
            string startSeconds_2(startSeconds_1.str());
            string endSeconds_2(endSeconds_1.str());
            replace(startSeconds_2.begin(), startSeconds_2.end(), '.', ',');
            replace(endSeconds_2.begin(), endSeconds_2.end(), '.', ',');
            strTimestamp << setfill('0') << setw(2) << timestamp.startHours << ":"
                << setfill('0') << setw(2) << timestamp.startMinutes << ":" << startSeconds_2 << " --> "
                << setfill('0') << setw(2) << timestamp.endHours << ":"
                << setfill('0') << setw(2) << timestamp.endMinutes << ":" << endSeconds_2;
        }
        else
        {
            strTimestamp << setfill('0') << setw(2) << timestamp.startHours << ":"
                << setfill('0') << setw(2) << timestamp.startMinutes << ":" << startSeconds_1.str() << " --> "
                << setfill('0') << setw(2) << timestamp.endHours << ":"
                << setfill('0') << setw(2) << timestamp.endMinutes << ":" << endSeconds_1.str();
        }
        
        return strTimestamp.str ();
    }

    string LanguageFromSpeechRecognitionResult(shared_ptr<SpeechRecognitionResult> result)
    {
        if (m_userConfig.languageIDLanguages.has_value())
        {
            shared_ptr<AutoDetectSourceLanguageResult> languageIDResult = AutoDetectSourceLanguageResult::FromResult(result);
            return "[" + languageIDResult->Language + "] ";
        }
        else
        {
            return "";
        }
    }

    string CaptionFromSpeechRecognitionResult(int sequenceNumber, shared_ptr<SpeechRecognitionResult> result)
    {
        string caption;
        if (!m_userConfig.partialResultsEnabled && m_userConfig.srtEnabled)
        {
            caption += sequenceNumber + "\n";
        }
        caption += TimestampFromSpeechRecognitionResult(result) + "\n"
            + LanguageFromSpeechRecognitionResult(result)
            + result->Text + "\n\n";
        return caption;
    }

    void WriteToConsole(string text)
    {
        if (!m_userConfig.suppressOutputEnabled)
        {
            cout << text << flush;
        }
    }

    void WriteToConsoleOrFile(string text)
    {
        WriteToConsole(text);
        if (m_userConfig.outputFile.has_value())
        {
            ofstream outputStream;
            outputStream.open(m_userConfig.outputFile.value(), ios_base::app);
            outputStream << text;
            outputStream.close();
        }
    }

    shared_ptr<Audio::AudioConfig> AudioConfigFromUserConfig()
    {
        if (m_userConfig.inputFile.has_value())
        {
            if (endsWith(m_userConfig.inputFile.value(), ".wav"))
            {
                auto reader = make_shared<WavFileReader>(m_userConfig.inputFile.value());
                m_format = AudioStreamFormat::GetWaveFormatPCM(reader->m_formatHeader.SamplesPerSec, reader->m_formatHeader.BitsPerSample, reader->m_formatHeader.Channels);
                reader->Close();
            }
            else
            {
// TODO1
//                m_format = AudioStreamFormat::GetCompressedFormat(m_userConfig.compressedAudioFormat);
                m_format = AudioStreamFormat::GetCompressedFormat(AudioStreamContainerFormat::ANY);
            }
            m_callback = make_shared<AudioInputFromFileCallback>(m_userConfig.inputFile.value());
            m_stream = AudioInputStream::CreatePullStream(m_format, m_callback);
            return AudioConfig::FromStreamInput(m_stream);
        }
        else
        {
            return Audio::AudioConfig::FromDefaultMicrophoneInput();
        }
    }

    shared_ptr<SpeechConfig> SpeechConfigFromUserConfig()
    {
        shared_ptr<SpeechConfig> speechConfig;
        if (m_userConfig.languageIDLanguages.has_value())
        {
            string endpoint = V2EndpointFromRegion(m_userConfig.region);
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
            filesystem::path outputFile { m_userConfig.outputFile.value() };
            // If the output file exists, truncate it.
            if (filesystem::exists(outputFile))
            {
                ofstream outputStream;
                outputStream.open(m_userConfig.outputFile.value());
                outputStream.close();            
            }
        }
        if (!m_userConfig.srtEnabled && !m_userConfig.partialResultsEnabled)
        {
            WriteToConsoleOrFile("WEBVTT\n\n");
        }
    }

    shared_ptr<SpeechRecognizer> SpeechRecognizerFromUserConfig()
    {
        shared_ptr<Audio::AudioConfig> audioConfig = AudioConfigFromUserConfig();
        shared_ptr<SpeechConfig> speechConfig = SpeechConfigFromUserConfig();
        shared_ptr<SpeechRecognizer> speechRecognizer;
        
        if (m_userConfig.languageIDLanguages.has_value())
        {
            shared_ptr<AutoDetectSourceLanguageConfig> autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages(m_userConfig.languageIDLanguages.value());
            speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioConfig);
        }
        else
        {
            speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        }

        if (m_userConfig.phraseList.has_value())
        {
            shared_ptr<PhraseListGrammar> grammar = PhraseListGrammar::FromRecognizer(speechRecognizer);
            grammar->AddPhrase(m_userConfig.phraseList.value());
        }
        
        return speechRecognizer;
    }

    optional<string> RecognizeContinuous(shared_ptr<SpeechRecognizer> speechRecognizer)
    {
        promise<optional<string>> recognitionEnd;
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
                    recognitionEnd.set_value(nullopt); // Notify to stop recognition.
                }
                else if (CancellationReason::CancelledByUser == e.Reason)
                {
                    WriteToConsole("User canceled request.\n");
                    recognitionEnd.set_value(nullopt); // Notify to stop recognition.
                }
                else if (CancellationReason::Error == e.Reason)
                {
                    ostringstream error;
                    error << "Encountered error.\n"
                        << "ErrorCode: " << (int)e.ErrorCode << "\n"
                        << "ErrorDetails: " << e.ErrorDetails << endl;
                    recognitionEnd.set_value(optional<string>{ error.str() }); // Notify to stop recognition.
                }
                else
                {
                    ostringstream error;
                    error << "Request was cancelled for an unrecognized reason: " << (int)e.Reason << endl;
                    recognitionEnd.set_value(optional<string>{ error.str() }); // Notify to stop recognition.
                }
            });

        speechRecognizer->SessionStopped.Connect([this, &recognitionEnd](const SessionEventArgs& e)
            {
                WriteToConsole("Session stopped.\n");
                recognitionEnd.set_value(nullopt); // Notify to stop recognition.
            });

        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
        speechRecognizer->StartContinuousRecognitionAsync().get();

        // Waits for recognition end.
        optional<string> result = recognitionEnd.get_future().get();

        // Stops recognition.
        speechRecognizer->StopContinuousRecognitionAsync().get();
        
        return result;
    }
};

int main(int argc, char* argv[])
{
    const string usage = "Usage: caption.exe [-f] [-h] [-i file] [-l languages] [-m] [-o file] [-p phrases] [-q] [-s] [-t number] [-u] <subscriptionKey> <region>\n"
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
            cout << usage << endl;
        }
        else
        {
            auto userConfig = UserConfigFromArgs(argc, argv);
            auto captioning = make_shared<Captioning>(userConfig);
            shared_ptr<SpeechRecognizer> speechRecognizer = captioning->SpeechRecognizerFromUserConfig();
            optional<string> error = captioning->RecognizeContinuous(speechRecognizer);
            if (error.has_value())
            {
                cout << error.value() << endl;
            }
        }
    }
    catch (TooFewArguments e)
    {
        cout << e.what() << "\n"
            << usage << endl;
    }
    catch (exception e)
    {
        cout << e.what() << endl;
    }
}

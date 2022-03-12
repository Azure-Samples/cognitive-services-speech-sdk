/* NOTES:
- In Windows, place the following files from the Speech SDK in the same folder as your compiled .exe:
    - Microsoft.CognitiveServices.Speech.core.dll
    - Microsoft.CognitiveServices.Speech.extension.audio.sys.dll
*/

/*
Dependencies
*/

#include <exception> // exception
#include <filesystem> // filesystem
#include <fstream> // ofstream
#include <iomanip> // fixed, setfill, setprecision, setw
#include <iostream> // cout
#include <optional> // optional
#include <sstream> // istringstream, ostringstream

/* Notes:
- Install the NuGet package Microsoft.CognitiveServices.Speech.
- The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=windows%2Cubuntu%2Cios-xcode%2Cmac-xcode%2Candroid-studio
*/
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Speaker;

/*
Types
*/

struct UserConfig
{
    const bool profanityFilterRemoveEnabled = false;
    const bool profanityFilterMaskEnabled = false;
    const std::optional<std::vector<std::string>> languageIDLanguages = std::nullopt;
    const std::optional<std::string> inputFile = std::nullopt;
    const std::optional<std::string> outputFile = std::nullopt;
    const std::optional<std::string> phraseList;
    const bool suppressOutputEnabled = false;
    const bool partialResultsEnabled = false;
    const std::optional<std::string> stablePartialResultThreshold = std::nullopt;
    const bool srtEnabled = false;
    const bool trueTextEnabled = false;
    const std::string subscriptionKey;
    const std::string region;
    
    UserConfig(
        bool profanityFilterRemoveEnabled,
        bool profanityFilterMaskEnabled,
        std::optional<std::vector<std::string>> languageIDLanguages,
        std::optional<std::string> inputFile,
        std::optional<std::string> outputFile,
        std::optional<std::string> phraseList,
        bool suppressOutputEnabled,
        bool partialResultsEnabled,
        std::optional<std::string> stablePartialResultThreshold,
        bool srtEnabled,
        bool trueTextEnabled,
        std::string subscriptionKey,
        std::string region
        ) :
        profanityFilterRemoveEnabled(profanityFilterRemoveEnabled),
        profanityFilterMaskEnabled(profanityFilterMaskEnabled),
        languageIDLanguages(languageIDLanguages),
        inputFile(inputFile),
        outputFile(outputFile),
        phraseList(phraseList),
        suppressOutputEnabled(suppressOutputEnabled),
        partialResultsEnabled(partialResultsEnabled),
        stablePartialResultThreshold(stablePartialResultThreshold),
        srtEnabled(srtEnabled),
        trueTextEnabled(trueTextEnabled),
        subscriptionKey(subscriptionKey),
        region(region)
        {}
};

struct Timestamp
{
    const int startHours;
    const int endHours;
    const int startMinutes;
    const int endMinutes;
    const float startSeconds;
    const float endSeconds;

    Timestamp(
        int startHours,
        int endHours,
        int startMinutes,
        int endMinutes,
        float startSeconds,
        float endSeconds
    ) :
    startHours(startHours),
    endHours(endHours),
    startMinutes(startMinutes),
    endMinutes(endMinutes),
    startSeconds(startSeconds),
    endSeconds(endSeconds)
    {}
};

class TooFewArguments : public std::exception
{
    public: const char* what() const throw()
    {
        return "Too few arguments.\n";
    }
};

/*
Helper functions
*/

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

std::string V2EndpointFromRegion(std::string region)
{
// Note: Continuous language identification is supported only with v2 endpoints.
    return std::string("wss://" + region + ".stt.speech.microsoft.com/speech/universal/v2");
}

Timestamp TimestampFromTicks(uint64_t startTicks, uint64_t endTicks)
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

std::string TimestampFromSpeechRecognitionResult(std::shared_ptr<SpeechRecognitionResult> result, UserConfig userConfig)
{
    std::ostringstream strTimestamp;
    Timestamp timestamp = TimestampFromTicks(result->Offset(), result->Offset() + result->Duration());    
    
    std::ostringstream startSeconds_1, endSeconds_1;
// setw value is 2 for seconds + 1 for floating point + 3 for decimal places.
    startSeconds_1 << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << timestamp.startSeconds;
    endSeconds_1 << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << timestamp.endSeconds;    
    
    if (userConfig.srtEnabled)
    {
// SRT format requires ',' as decimal separator rather than '.'.
        std::string startSeconds_2(startSeconds_1.str());
        std::string endSeconds_2(endSeconds_1.str());
        std::replace(startSeconds_2.begin(), startSeconds_2.end(), '.', ',');
        std::replace(endSeconds_2.begin(), endSeconds_2.end(), '.', ',');
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

std::string LanguageFromSpeechRecognitionResult(std::shared_ptr<SpeechRecognitionResult> result, UserConfig userConfig)
{
    if (userConfig.languageIDLanguages.has_value())
    {
        std::shared_ptr<AutoDetectSourceLanguageResult> languageIDResult = AutoDetectSourceLanguageResult::FromResult(result);
        return "[" + languageIDResult->Language + "] ";
    }
    else
    {
        return "";
    }
}

std::string CaptionFromSpeechRecognitionResult(int sequenceNumber, std::shared_ptr<SpeechRecognitionResult> result, UserConfig userConfig)
{
    std::string caption;
    if (!userConfig.partialResultsEnabled && userConfig.srtEnabled)
    {
        caption += sequenceNumber + "\n";
    }
    caption += TimestampFromSpeechRecognitionResult(result, userConfig) + "\n"
        + LanguageFromSpeechRecognitionResult(result, userConfig)
        + result->Text + "\n\n";
    return caption;
}

void WriteToConsole(std::string text, UserConfig userConfig)
{
    if (!userConfig.suppressOutputEnabled)
    {
        std::cout << text << std::flush;
    }
}

void WriteToConsoleOrFile(std::string text, UserConfig userConfig)
{
    WriteToConsole(text, userConfig);
    if (userConfig.outputFile.has_value())
    {
        std::ofstream outputStream;
        outputStream.open(userConfig.outputFile.value(), std::ios_base::app);
        outputStream << text;
        outputStream.close();
    }
}

void Initialize(UserConfig userConfig)
{
    if (userConfig.outputFile.has_value())
    {
        std::filesystem::path outputFile { userConfig.outputFile.value() };
// If the output file exists, truncate it.
        if (std::filesystem::exists(outputFile))
        {
            std::ofstream outputStream;
            outputStream.open(userConfig.outputFile.value());
            outputStream.close();            
        }
    }
    if (!userConfig.srtEnabled && !userConfig.partialResultsEnabled)
    {
        WriteToConsoleOrFile("WEBVTT\n\n", userConfig);
    }
}

/*
Main functions
*/

UserConfig UserConfigFromArgs(int argc, char* argv[])
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
        GetCmdOption(argv, argv + argc, "-r"),
        CmdOptionExists(argv, argv + argc, "-s"),
        CmdOptionExists(argv, argv + argc, "-t"),
        argv[argc - 2],
        argv[argc - 1]
    );
}

std::shared_ptr<Audio::AudioConfig> AudioConfigFromUserConfig(UserConfig userConfig)
{
    if (userConfig.inputFile.has_value())
    {
        return Audio::AudioConfig::FromWavFileInput(userConfig.inputFile.value());
    }
    else
    {
        return Audio::AudioConfig::FromDefaultMicrophoneInput();
    }
}

std::shared_ptr<SpeechConfig> SpeechConfigFromUserConfig(UserConfig userConfig)
{
    std::shared_ptr<SpeechConfig> speechConfig;
    if (userConfig.languageIDLanguages.has_value())
    {
        std::string endpoint = V2EndpointFromRegion(userConfig.region);
        speechConfig = SpeechConfig::FromEndpoint(endpoint, userConfig.subscriptionKey);
    }
    else
    {
        speechConfig = SpeechConfig::FromSubscription(userConfig.subscriptionKey, userConfig.region);
    }

    if (userConfig.profanityFilterRemoveEnabled)
    {
        speechConfig->SetProfanity(ProfanityOption::Removed);
    }
    else if (userConfig.profanityFilterMaskEnabled)
    {
        speechConfig->SetProfanity(ProfanityOption::Masked);
    }
    
    if (userConfig.stablePartialResultThreshold.has_value())
    {
// Note: To get default value:
// std::cout << speechConfig->GetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold) << std::endl;
        speechConfig->SetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold, userConfig.stablePartialResultThreshold.value());
    }
    
    if (userConfig.trueTextEnabled)
    {
        speechConfig->SetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption, "TrueText");
    }
    
    return speechConfig;
}

std::shared_ptr<SpeechRecognizer> SpeechRecognizerFromSpeechConfig(std::shared_ptr<SpeechConfig> speechConfig, std::shared_ptr<Audio::AudioConfig> audioConfig, UserConfig userConfig)
{
    std::shared_ptr<SpeechRecognizer> speechRecognizer;
    if (userConfig.languageIDLanguages.has_value())
    {
        std::shared_ptr<AutoDetectSourceLanguageConfig> detectLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages(userConfig.languageIDLanguages.value());
        speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, detectLanguageConfig, audioConfig);
    }
    else
    {
        speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    }
    
    if (userConfig.phraseList.has_value())
    {
        std::shared_ptr<PhraseListGrammar> grammar = PhraseListGrammar::FromRecognizer(speechRecognizer);
        grammar->AddPhrase(userConfig.phraseList.value());
    }
    
    return speechRecognizer;
}

std::shared_ptr<SpeechRecognizer> SpeechRecognizerFromUserConfig(UserConfig userConfig)
{
    std::shared_ptr<Audio::AudioConfig> audioConfig = AudioConfigFromUserConfig(userConfig);
    std::shared_ptr<SpeechConfig> speechConfig = SpeechConfigFromUserConfig(userConfig);
    return SpeechRecognizerFromSpeechConfig(speechConfig, audioConfig, userConfig);
}

std::optional<std::string> RecognizeContinuous(std::shared_ptr<SpeechRecognizer> speechRecognizer, UserConfig userConfig)
{
    std::promise<std::optional<std::string>> recognitionEnd;
    int sequenceNumber = 0;

    if (userConfig.partialResultsEnabled) {
/* Capture variables we will need inside the lambda.
See:
https://www.cppstories.com/2020/08/lambda-capturing.html/
*/
        speechRecognizer->Recognizing.Connect([&userConfig](const SpeechRecognitionEventArgs& e)
            {
                if (ResultReason::RecognizingSpeech == e.Result->Reason && e.Result->Text.length() > 0)
                {
// We don't show sequence numbers for partial results.
                    WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(0, e.Result, userConfig), userConfig);
                }
                else if (ResultReason::NoMatch == e.Result->Reason)
                {
                    WriteToConsole("NOMATCH: Speech could not be recognized.\n", userConfig);
                }
            });
    }
    else
    {
        speechRecognizer->Recognized.Connect([&userConfig, &sequenceNumber](const SpeechRecognitionEventArgs& e)
            {
                if (ResultReason::RecognizedSpeech == e.Result->Reason && e.Result->Text.length() > 0)
                {
                    sequenceNumber++;
                    WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber, e.Result, userConfig), userConfig);
                }
                else if (ResultReason::NoMatch == e.Result->Reason)
                {
                    WriteToConsole("NOMATCH: Speech could not be recognized.\n", userConfig);
                }
            });
    }

    speechRecognizer->Canceled.Connect([&userConfig, &recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
        {
            if (CancellationReason::EndOfStream == e.Reason)
            {
                WriteToConsole("End of stream reached.\n", userConfig);
                recognitionEnd.set_value(std::nullopt); // Notify to stop recognition.
            }
            else if (CancellationReason::CancelledByUser == e.Reason)
            {
                WriteToConsole("User canceled request.\n", userConfig);
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

    speechRecognizer->SessionStopped.Connect([&userConfig, &recognitionEnd](const SessionEventArgs& e)
        {
            WriteToConsole("Session stopped.\n", userConfig);
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

int main(int argc, char* argv[])
{
    const std::string usage = "Usage: caption.exe [-f] [-h] [-i file] [-l languages] [-m] [-o file] [-p phrases] [-q] [-r number] [-s] [-t] [-u] <subscriptionKey> <region>\n"
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
    "       -r number: Set stable partial result threshold to *number*.\n"
    "                  Example: 3\n"
    "              -s: Emit SRT (default is WebVTT.)\n"
    "              -t: Enable TrueText.\n"
    "              -u: Emit partial results instead of finalized results.\n";

    try
    {
        if (CmdOptionExists(argv, argv + argc, "-h"))
        {
            std::cout << usage << std::endl;
        }
        else
        {
            UserConfig userConfig = UserConfigFromArgs(argc, argv);
            Initialize(userConfig);
            std::shared_ptr<SpeechRecognizer> speechRecognizer = SpeechRecognizerFromUserConfig(userConfig);
            std::optional<std::string> error = RecognizeContinuous(speechRecognizer, userConfig);
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

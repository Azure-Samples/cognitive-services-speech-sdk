/* NOTES:
- In Windows, place the following files from the Speech SDK in the same folder as your compiled .exe:
	- Microsoft.CognitiveServices.Speech.core.dll
	- Microsoft.CognitiveServices.Speech.extension.audio.sys.dll
*/

/*
Dependencies
*/

// exception
#include <exception>
// filesystem
#include <filesystem>
// ofstream
#include <fstream>
// fixed, setfill, setprecision, setw
#include <iomanip>
// cout
#include <iostream>
// optional
#include <optional>
// istringstream, ostringstream
#include <sstream>

/* Notes:
- Install the NuGet package Microsoft.CognitiveServices.Speech.
- The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=windows%2Cubuntu%2Cios-xcode%2Cmac-xcode%2Candroid-studio
*/
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Speaker;

/*
Global constants
*/

// Note: Continuous language identification is supported only with v2 endpoints.
const std::string v2EndpointStart ("wss://");
const std::string v2EndpointEnd (".stt.speech.microsoft.com/speech/universal/v2");

const float ticksPerSecond = 10000000.0;

const std::string usage = "Usage: caption.exe [-d] [-f] [-h] [-i file] [-l] [-o file] [-p phrases] [-q] [-r number] [-s] [-t] [-u] <subscriptionKey> <region>\n"
"              -d: Enable dictation mode.\n"
"              -f: Enable profanity filter.\n"
"              -h: Show this help and stop.\n"
"              -i: Input audio file *file* (default input is from the microphone.)\n"
"    -l languages: Enable language identification for specified *languages*.\n"
"                  Example: en-US,ja-JP\n"
"         -o file: Output to *file*.\n"
"      -p phrases: Add specified *phrases*.\n"
"                  Example: Constoso;Jessie;Rehaan\n"
"              -q: Suppress console output (except errors).\n"
"       -r number: Set stable partial result threshold to *number*.\n"
"                  Example: 3\n"
"              -s: Emit SRT (default is WebVTT.)\n"
"              -t: Enable TrueText.\n"
"              -u: Emit partial results instead of finalized results.\n";

/*
Types
*/

struct UserConfig
{
	const bool prioritizeAccuracyEnabled = false;
	const bool dictationEnabled = false;
	const bool profanityFilterEnabled = false;
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
	
	UserConfig (
		bool prioritizeAccuracyEnabled,
		bool dictationEnabled,
		bool profanityFilterEnabled,
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
		prioritizeAccuracyEnabled (prioritizeAccuracyEnabled),
		dictationEnabled (dictationEnabled),
		profanityFilterEnabled (profanityFilterEnabled),
		languageIDLanguages (languageIDLanguages),
		inputFile (inputFile),
		outputFile (outputFile),
		phraseList (phraseList),
		suppressOutputEnabled (suppressOutputEnabled),
		partialResultsEnabled (partialResultsEnabled),
		stablePartialResultThreshold (stablePartialResultThreshold),
		srtEnabled (srtEnabled),
		trueTextEnabled (trueTextEnabled),
		subscriptionKey (subscriptionKey),
		region (region)
		{}
};

struct CaptionTime
{
	const int startHours;
	const int endHours;
	const int startMinutes;
	const int endMinutes;
	const float startSeconds;
	const float endSeconds;

	CaptionTime (
		int startHours,
		int endHours,
		int startMinutes,
		int endMinutes,
		float startSeconds,
		float endSeconds
	) :
	startHours (startHours),
	endHours (endHours),
	startMinutes (startMinutes),
	endMinutes (endMinutes),
	startSeconds (startSeconds),
	endSeconds (endSeconds)
	{}
};

class TooFewArguments : public std::exception
{
	public: const char* what () const throw ()
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
std::optional<std::string> GetCmdOption (char** begin, char** end, const std::string& option)
{
	char** result = std::find (begin, end, option);
	if (result != end)
	{
// We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
		char** value = ++result;
// std::string (nullptr) is undefined.		
		if (value != end && nullptr != *value)
		{
			return std::optional<std::string>{ std::string (*value) };
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

inline bool CmdOptionExists (char** begin, char** end, const std::string& option)
{
	return std::find (begin, end, option) != end;
}

std::vector<std::string> split (const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream (s);
	while (std::getline (tokenStream, token, delimiter))
	{
		tokens.push_back (token);
	}
	return tokens;
}

std::string V2EndpointFromRegion (std::string region)
{
	return std::string (v2EndpointStart + region + v2EndpointEnd);
}

CaptionTime CaptionTimeFromTicks (uint64_t startTicks, uint64_t endTicks)
{
	const float startSeconds_1 = startTicks / ticksPerSecond;
	const float endSeconds_1 = endTicks / ticksPerSecond;
	
	const int startMinutes_1 = startSeconds_1 / 60;
	const int endMinutes_1 = endSeconds_1 / 60;	
	
	const int startHours = startMinutes_1 / 60;
	const int endHours = endMinutes_1 / 60;
	
	const float startSeconds_2 = fmod (startSeconds_1, 60.0);
	const float endSeconds_2 = fmod (endSeconds_1, 60.0);
	
	const float startMinutes_2 = fmod (startMinutes_1, 60.0);
	const float endMinutes_2 = fmod (endMinutes_1, 60.0);
	
	return CaptionTime (startHours, endHours, startMinutes_2, endMinutes_2, startSeconds_2, endSeconds_2);
}

std::string AddLanguageToResult (std::shared_ptr<SpeechRecognitionResult> result, UserConfig userConfig)
{
	std::ostringstream text;
	if (true == userConfig.languageIDLanguages.has_value ())
	{
		auto languageIDResult = AutoDetectSourceLanguageResult::FromResult (result);
		text << "[" << languageIDResult->Language << "] " << result->Text << std::endl;
	}
	else
	{
		text << result->Text << std::endl;
	}
	return text.str ();
}

std::string CaptionFromSpeechRecognitionResult (bool srt, int sequenceNumber, std::shared_ptr<SpeechRecognitionResult> result, UserConfig userConfig)
{
	std::ostringstream caption;
	CaptionTime captionTime = CaptionTimeFromTicks (result->Offset (), result->Offset () + result->Duration ());	
	
	std::ostringstream startSeconds_1, endSeconds_1;
// setw value is 2 for seconds + 1 for floating point + 3 for decimal places.
	startSeconds_1 << std::setfill ('0') << std::setw (6) << std::fixed << std::setprecision (3) << captionTime.startSeconds;
	endSeconds_1 << std::setfill ('0') << std::setw (6) << std::fixed << std::setprecision (3) << captionTime.endSeconds;	
	
	if (true == srt)
	{
// SRT format requires ',' as decimal separator rather than '.'.
		std::string startSeconds_2 (startSeconds_1.str ());
		std::string endSeconds_2 (endSeconds_1.str ());
		std::replace (startSeconds_2.begin (), startSeconds_2.end (), '.', ',');
		std::replace (endSeconds_2.begin (), endSeconds_2.end (), '.', ',');
		caption << sequenceNumber << "\n"
			<< std::setfill ('0') << std::setw (2) << captionTime.startHours << ":"
			<< std::setfill ('0') << std::setw (2) << captionTime.startMinutes << ":" << startSeconds_2 << " --> "
			<< std::setfill ('0') << std::setw (2) << captionTime.endHours << ":"
			<< std::setfill ('0') << std::setw (2) << captionTime.endMinutes << ":" << endSeconds_2 << std::endl;
	}
	else
	{
		caption << std::setfill ('0') << std::setw (2) << captionTime.startHours << ":"
			<< std::setfill ('0') << std::setw (2) << captionTime.startMinutes << ":" << startSeconds_1.str () << " --> "
			<< std::setfill ('0') << std::setw (2) << captionTime.endHours << ":"
			<< std::setfill ('0') << std::setw (2) << captionTime.endMinutes << ":" << endSeconds_1.str () << std::endl;
	}
	caption << AddLanguageToResult (result, userConfig) << std::endl;
	return caption.str ();
}

void WriteToConsole (std::string text, UserConfig userConfig)
{
	if (false == userConfig.suppressOutputEnabled)
	{
		std::cout << text;
	}
}

void WriteToConsoleOrFile (std::string text, UserConfig userConfig)
{
	if (false == userConfig.suppressOutputEnabled)
	{
		std::cout << text;
	}
	if (true == userConfig.outputFile.has_value ())
	{
		std::ofstream outputStream;
		outputStream.open (userConfig.outputFile.value (), std::ios_base::app);
		outputStream << text;
		outputStream.close ();
	}
}

void Initialize (UserConfig userConfig)
{
	if (true == userConfig.outputFile.has_value ())
	{
		std::filesystem::path outputFile { userConfig.outputFile.value () };
// If the output file exists, truncate it.
		if (std::filesystem::exists (outputFile))
		{
			std::ofstream outputStream;
			outputStream.open (userConfig.outputFile.value ());
			outputStream.close ();			
		}
	}

	if (false == userConfig.srtEnabled && false == userConfig.partialResultsEnabled)
	{
		WriteToConsoleOrFile ("WEBVTT\n\n", userConfig);
	}
}

/*
Main functions
*/

UserConfig UserConfigFromArgs (int argc, char* argv[])
{
	bool prioritizeAccuracyEnabled = false;
	bool dictationEnabled = false;
	bool profanityFilterEnabled = false;
	std::optional<std::vector<std::string>> languageIDLanguages;
	std::optional<std::string> inputFile;
	std::optional<std::string> outputFile;
	std::optional<std::string> phraseList;
	bool suppressOutputEnabled = false;
	bool partialResultsEnabled = false;
	std::optional<std::string> stablePartialResultThreshold;
	bool srtEnabled = false;
	bool trueTextEnabled = false;
	std::string subscriptionKey;
	std::string region;

// Verify argc >= 3 (caption.exe, subscriptionKey, region)
	if (argc < 3)
	{
		throw TooFewArguments ();
	}

	subscriptionKey = argv[argc - 2];
	region = argv[argc - 1];

	prioritizeAccuracyEnabled = CmdOptionExists (argv, argv + argc, "-a");
	dictationEnabled = CmdOptionExists (argv, argv + argc, "-d");
	profanityFilterEnabled = CmdOptionExists (argv, argv + argc, "-f");
	std::optional<std::string> languageIDLanguagesResult = GetCmdOption (argv, argv + argc, "-l");
	if (true == languageIDLanguagesResult.has_value ())
	{
		languageIDLanguages = std::optional<std::vector<std::string>>{ split (languageIDLanguagesResult.value (), ',') };
	}
	inputFile = GetCmdOption (argv, argv + argc, "-i");
	outputFile = GetCmdOption (argv, argv + argc, "-o");
	phraseList = GetCmdOption (argv, argv + argc, "-p");
	suppressOutputEnabled = CmdOptionExists (argv, argv + argc, "-q");
	partialResultsEnabled = CmdOptionExists (argv, argv + argc, "-u");
	stablePartialResultThreshold = GetCmdOption (argv, argv + argc, "-r");
	srtEnabled = CmdOptionExists (argv, argv + argc, "-s");
	trueTextEnabled = CmdOptionExists (argv, argv + argc, "-t");
	
	UserConfig userConfig = UserConfig (
		prioritizeAccuracyEnabled,
		dictationEnabled,
		profanityFilterEnabled,
		languageIDLanguages,
		inputFile,
		outputFile,
		phraseList,
		suppressOutputEnabled,
		partialResultsEnabled,
		stablePartialResultThreshold,
		srtEnabled,
		trueTextEnabled,
		subscriptionKey,
		region
	);
	
	return userConfig;
}

std::shared_ptr<Audio::AudioConfig> AudioConfigFromUserConfig (UserConfig userConfig)
{
	std::shared_ptr<Audio::AudioConfig> audioConfig;
	if (true == userConfig.inputFile.has_value ())
	{
		audioConfig = Audio::AudioConfig::FromWavFileInput (userConfig.inputFile.value ());
	}
	else
	{
		audioConfig = Audio::AudioConfig::FromDefaultMicrophoneInput ();
	}
	return audioConfig;
}

std::shared_ptr<SpeechConfig> SpeechConfigFromUserConfig (UserConfig userConfig)
{
	std::shared_ptr<SpeechConfig> speechConfig;
	if (userConfig.languageIDLanguages.has_value ()) {
		std::string endpoint = V2EndpointFromRegion (userConfig.region);
		speechConfig = SpeechConfig::FromEndpoint (endpoint, userConfig.subscriptionKey);
	}
	else
	{
		speechConfig = SpeechConfig::FromSubscription (userConfig.subscriptionKey, userConfig.region);
	}

/* Note: Dictation mode is only supported for continuous recognition.
See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-to-text?tabs=windowsinstall&pivots=programming-language-cpp#dictation-mode
*/
	if (true == userConfig.dictationEnabled)
	{
		speechConfig->EnableDictation ();
	}

	if (true == userConfig.profanityFilterEnabled)
	{
		speechConfig->SetProfanity (ProfanityOption::Removed);
	}
	
	if (true == userConfig.stablePartialResultThreshold.has_value ())
	{
// Note: To get default value:
// std::cout << speechConfig->GetProperty (PropertyId::SpeechServiceResponse_StablePartialResultThreshold) << std::endl;
		speechConfig->SetProperty (PropertyId::SpeechServiceResponse_StablePartialResultThreshold, userConfig.stablePartialResultThreshold.value ());
	}
	
	if (true == userConfig.trueTextEnabled)
	{
		speechConfig->SetProperty (PropertyId::SpeechServiceResponse_PostProcessingOption, "TrueText");
	}
	
	return speechConfig;
}

std::shared_ptr<SpeechRecognizer> SpeechRecognizerFromSpeechConfig (std::shared_ptr<SpeechConfig> speechConfig, std::shared_ptr<Audio::AudioConfig> audioConfig, UserConfig userConfig)
{
	std::shared_ptr<SpeechRecognizer> speechRecognizer;
	if (true == userConfig.languageIDLanguages.has_value ())
	{
		std::shared_ptr<AutoDetectSourceLanguageConfig> detectLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages (userConfig.languageIDLanguages.value ());
		speechRecognizer = SpeechRecognizer::FromConfig (speechConfig, detectLanguageConfig, audioConfig);
	}
	else
	{
		speechRecognizer = SpeechRecognizer::FromConfig (speechConfig, audioConfig);
	}
	
	if (true == userConfig.phraseList.has_value ())
	{
		std::shared_ptr<PhraseListGrammar> grammar = PhraseListGrammar::FromRecognizer (speechRecognizer);
		grammar->AddPhrase (userConfig.phraseList.value ());
	}
	
	return speechRecognizer;
}

std::shared_ptr<SpeechRecognizer> SpeechRecognizerFromUserConfig (UserConfig userConfig)
{
	std::shared_ptr<Audio::AudioConfig> audioConfig = AudioConfigFromUserConfig (userConfig);
	std::shared_ptr<SpeechConfig> speechConfig = SpeechConfigFromUserConfig (userConfig);
	return SpeechRecognizerFromSpeechConfig (speechConfig, audioConfig, userConfig);
}

std::optional<std::string> RecognizeContinuous (std::shared_ptr<SpeechRecognizer> speechRecognizer, UserConfig userConfig)
{
	std::promise<std::optional<std::string>> recognitionEnd;
	int sequenceNumber = 0;

	if (true == userConfig.partialResultsEnabled) {
/* Capture variables we will need inside the lambda.
See:
https://www.cppstories.com/2020/08/lambda-capturing.html/
*/
		speechRecognizer->Recognizing.Connect ([&userConfig](const SpeechRecognitionEventArgs& e)
			{
				if (ResultReason::RecognizingSpeech == e.Result->Reason && e.Result->Text.length () > 0)
				{
					WriteToConsoleOrFile (AddLanguageToResult (e.Result, userConfig), userConfig);
				}
				else if (ResultReason::NoMatch == e.Result->Reason)
				{
					WriteToConsole ("NOMATCH: Speech could not be recognized.\n", userConfig);
				}
			});
	}
	else
	{
		speechRecognizer->Recognized.Connect ([&userConfig, &sequenceNumber](const SpeechRecognitionEventArgs& e)
			{
				if (ResultReason::RecognizedSpeech == e.Result->Reason && e.Result->Text.length () > 0)
				{
					sequenceNumber++;
					WriteToConsoleOrFile (CaptionFromSpeechRecognitionResult (userConfig.srtEnabled, sequenceNumber, e.Result, userConfig), userConfig);
				}
				else if (ResultReason::NoMatch == e.Result->Reason)
				{
					WriteToConsole ("NOMATCH: Speech could not be recognized.\n", userConfig);
				}
			});
	}

	speechRecognizer->Canceled.Connect ([&userConfig, &recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
		{
			if (CancellationReason::EndOfStream == e.Reason)
			{
				WriteToConsole ("End of stream reached.\n", userConfig);
				recognitionEnd.set_value (std::nullopt); // Notify to stop recognition.
			}
			else if (CancellationReason::CancelledByUser == e.Reason)
			{
				WriteToConsole ("User canceled request.\n", userConfig);
				recognitionEnd.set_value (std::nullopt); // Notify to stop recognition.
			}
			else if (CancellationReason::Error == e.Reason)
			{
				std::ostringstream error;
				error << "Encountered error.\n"
					<< "ErrorCode: " << (int)e.ErrorCode << "\n"
					<< "ErrorDetails: " << e.ErrorDetails << std::endl;
				recognitionEnd.set_value (std::optional<std::string>{ error.str () }); // Notify to stop recognition.
			}
			else
			{
				std::ostringstream error;
				error << "Request was cancelled for an unrecognized reason: " << (int)e.Reason << std::endl;
				recognitionEnd.set_value (std::optional<std::string>{ error.str () }); // Notify to stop recognition.
			}
		});

	speechRecognizer->SessionStopped.Connect ([&userConfig, &recognitionEnd](const SessionEventArgs& e)
		{
			WriteToConsole ("Session stopped.\n", userConfig);
			recognitionEnd.set_value (std::nullopt); // Notify to stop recognition.
		});

	// Starts continuous recognition. Uses StopContinuousRecognitionAsync () to stop recognition.
	speechRecognizer->StartContinuousRecognitionAsync ().get ();

	// Waits for recognition end.
	std::optional<std::string> result = recognitionEnd.get_future ().get ();

	// Stops recognition.
	speechRecognizer->StopContinuousRecognitionAsync ().get ();
	
	return result;
}

int main (int argc, char* argv[])
{
	try
	{
		if (true == CmdOptionExists (argv, argv + argc, "-h"))
		{
			std::cout << usage << std::endl;
		}
		else
		{
			UserConfig userConfig = UserConfigFromArgs (argc, argv);
			std::shared_ptr<SpeechRecognizer> speechRecognizer = SpeechRecognizerFromUserConfig (userConfig);
			Initialize (userConfig);
			std::optional<std::string> error = RecognizeContinuous (speechRecognizer, userConfig);
			if (true == error.has_value ())
			{
				std::cout << error.value () << std::endl;
			}
		}
	}
	catch (TooFewArguments e)
	{
		std::cout << e.what () << "\n"
			<< usage << std::endl;
	}
	catch (std::exception e)
	{
		std::cout << e.what () << std::endl;
	}
}

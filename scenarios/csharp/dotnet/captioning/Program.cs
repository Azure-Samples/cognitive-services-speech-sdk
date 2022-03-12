/*
Dependencies
*/

using System; // File
using System.Linq; // Enumerable.Contains
using System.Text; // StringBuilder
using System.Threading.Tasks; // Task

/* Notes:
- Install the Speech NuGet package:
dotnet add package Microsoft.CognitiveServices.Speech
- The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=windows%2Cubuntu%2Cios-xcode%2Cmac-xcode%2Candroid-studio
*/
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace Caption
{
/*
Types
*/

    public struct UserConfig
    {
        readonly public bool profanityFilterRemoveEnabled = false;
        readonly public bool profanityFilterMaskEnabled = false;
        readonly public string[]? languageIDLanguages;
        readonly public string? inputFile;
        readonly public string? outputFile;
        readonly public string? phraseList;
        readonly public bool suppressOutputEnabled = false;
        readonly public bool partialResultsEnabled = false;
        readonly public string? stablePartialResultThreshold;
        readonly public bool srtEnabled = false;
        readonly public bool trueTextEnabled = false;
        readonly public string subscriptionKey;
        readonly public string region;
        
        public UserConfig (
            bool profanityFilterRemoveEnabled,
            bool profanityFilterMaskEnabled,
            string[]? languageIDLanguages,
            string? inputFile,
            string? outputFile,
            string? phraseList,
            bool suppressOutputEnabled,
            bool partialResultsEnabled,
            string? stablePartialResultThreshold,
            bool srtEnabled,
            bool trueTextEnabled,
            string subscriptionKey,
            string region
            )
        {
            this.profanityFilterRemoveEnabled = profanityFilterRemoveEnabled;
            this.profanityFilterMaskEnabled = profanityFilterMaskEnabled;
            this.languageIDLanguages = languageIDLanguages;
            this.inputFile = inputFile;
            this.outputFile = outputFile;
            this.phraseList = phraseList;
            this.suppressOutputEnabled = suppressOutputEnabled;
            this.partialResultsEnabled = partialResultsEnabled;
            this.stablePartialResultThreshold = stablePartialResultThreshold;
            this.srtEnabled = srtEnabled;
            this.trueTextEnabled = trueTextEnabled;
            this.subscriptionKey = subscriptionKey;
            this.region = region;
        }
    }

    public class TooFewArguments : Exception
    {
        public TooFewArguments() : base() { }
    }

    class Program
    {

/*
Helper functions
*/

        static string? GetCmdOption(string[] args, string option)
        {
            int index = Array.IndexOf(args, option);
            if (index > -1 && index < args.Length - 2)
            {
// We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
                return args[index + 1];
            }
            else {
                return null;
            }
        }

        static bool CmdOptionExists(string[] args, string option)
        {
            return args.Contains (option);
        }

        static string V2EndpointFromRegion(string region)
        {
// Note: Continuous language identification is supported only with v2 endpoints.
            return "wss://" + region + ".stt.speech.microsoft.com/speech/universal/v2";
        }

        static string TimestampFromSpeechRecognitionResult(SpeechRecognitionResult result, UserConfig userConfig)
        {
            DateTime startTime = new DateTime(result.OffsetInTicks);
            DateTime endTime = startTime.Add(result.Duration);
            string format;

            if(userConfig.srtEnabled)
            {
// SRT format requires ',' as decimal separator rather than '.'.
                format = "HH:mm:ss,fff";
            }
            else
            {
                format = "HH:mm:ss.fff";
            }

            return String.Format("{0} --> {1}", startTime.ToString(format), endTime.ToString(format));
        }

        static string LanguageFromSpeechRecognitionResult(SpeechRecognitionResult result, UserConfig userConfig)
        {
            if (userConfig.languageIDLanguages is not null)
            {
                AutoDetectSourceLanguageResult languageIDResult = AutoDetectSourceLanguageResult.FromResult(result);
                return "[" + languageIDResult.Language + "] ";
            }
            else
            {
                return "";
            }
        }

        static string CaptionFromSpeechRecognitionResult(int sequenceNumber, SpeechRecognitionResult result, UserConfig userConfig)
        {
            StringBuilder caption = new StringBuilder();
            if (!userConfig.partialResultsEnabled && userConfig.srtEnabled)
            {
                caption.AppendFormat("{0}{1}", sequenceNumber, Environment.NewLine);
            }
            caption.AppendFormat("{0}{1}", TimestampFromSpeechRecognitionResult(result, userConfig), Environment.NewLine);
            caption.Append(LanguageFromSpeechRecognitionResult(result, userConfig));
            caption.AppendFormat("{0}{1}{1}", result.Text, Environment.NewLine);
            return caption.ToString();
        }

        static void WriteToConsole(string text, UserConfig userConfig)
        {
            if (!userConfig.suppressOutputEnabled)
            {
                Console.Write(text);
            }
        }

        static void WriteToConsoleOrFile(string text, UserConfig userConfig)
        {
            WriteToConsole(text, userConfig);
            if (userConfig.outputFile is string outputFileValue)
            {
                File.AppendAllText(outputFileValue, text);
            }
        }

        static void Initialize(UserConfig userConfig)
        {
            if (userConfig.outputFile is string outputFileValue && File.Exists(outputFileValue))
            {
                File.Delete(outputFileValue);
            }
            if (!userConfig.srtEnabled)
            {
                WriteToConsoleOrFile(String.Format("WEBVTT{0}{0}", Environment.NewLine), userConfig);
            }
        }

/*
Main functions
*/

        static UserConfig UserConfigFromArgs(string[] args)
        {
// Verify argc >= 3 (caption.exe, subscriptionKey, region)
            if (args.Length < 3)
            {
                throw new TooFewArguments();
            }

            string[]? languageIDLanguages = null;
            if (GetCmdOption(args, "-l") is string languageIDLanguagesResult)
            {
                languageIDLanguages = languageIDLanguagesResult.Split(',');
            }
            
            return new UserConfig(
                CmdOptionExists(args, "-f"),
                CmdOptionExists(args, "-m"),
                languageIDLanguages,
                GetCmdOption(args, "-i"),
                GetCmdOption(args, "-o"),
                GetCmdOption(args, "-p"),
                CmdOptionExists(args, "-q"),
                CmdOptionExists(args, "-u"),
                GetCmdOption(args, "-r"),
                CmdOptionExists(args, "-s"),
                CmdOptionExists(args, "-t"),
                args[args.Length - 2],
                args[args.Length - 1]
            );
        }

        static AudioConfig AudioConfigFromUserConfig(UserConfig userConfig)
        {
            if (userConfig.inputFile is string inputFileValue)
            {
                return AudioConfig.FromWavFileInput(inputFileValue);
            }
            else
            {
                return AudioConfig.FromDefaultMicrophoneInput();
            }
        }

        static SpeechConfig SpeechConfigFromUserConfig(UserConfig userConfig)
        {
            SpeechConfig speechConfig;
            if (userConfig.languageIDLanguages is not null)
            {
                speechConfig = SpeechConfig.FromEndpoint(new Uri(V2EndpointFromRegion(userConfig.region)), userConfig.subscriptionKey);
            }
            else
            {
                speechConfig = SpeechConfig.FromSubscription(userConfig.subscriptionKey, userConfig.region);
            }

            if (userConfig.profanityFilterRemoveEnabled)
            {
                speechConfig.SetProfanity(ProfanityOption.Removed);
            }
            else if (userConfig.profanityFilterMaskEnabled)
            {
                speechConfig.SetProfanity(ProfanityOption.Masked);
            }
            
            if (userConfig.stablePartialResultThreshold is string stablePartialResultThresholdValue)
            {
                speechConfig.SetProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, stablePartialResultThresholdValue);
            }
            
            if (userConfig.trueTextEnabled)
            {
                speechConfig.SetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");
            }
            
            return speechConfig;
        }

        static SpeechRecognizer SpeechRecognizerFromSpeechConfig(SpeechConfig speechConfig, AudioConfig audioConfig, UserConfig userConfig)
        {
            SpeechRecognizer speechRecognizer;
            if (userConfig.languageIDLanguages is string[] languageIDLanguagesValue)
            {
                AutoDetectSourceLanguageConfig detectLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(languageIDLanguagesValue);
                speechRecognizer = new SpeechRecognizer(speechConfig, detectLanguageConfig, audioConfig);
            }
            else
            {
                speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);
            }
            
            if (userConfig.phraseList is string phraseListValue)
            {
                PhraseListGrammar grammar = PhraseListGrammar.FromRecognizer(speechRecognizer);
                grammar.AddPhrase(phraseListValue);
            }
            
            return speechRecognizer;
        }

        static SpeechRecognizer SpeechRecognizerFromUserConfig(UserConfig userConfig)
        {
            AudioConfig audioConfig = AudioConfigFromUserConfig(userConfig);
            SpeechConfig speechConfig = SpeechConfigFromUserConfig(userConfig);
            return SpeechRecognizerFromSpeechConfig(speechConfig, audioConfig, userConfig);
        }

        static async Task<string?> RecognizeContinuous(SpeechRecognizer speechRecognizer, UserConfig userConfig)
        {
            var recognitionEnd = new TaskCompletionSource<string?>();
            int sequenceNumber = 0;

            if (userConfig.partialResultsEnabled)
            {
                speechRecognizer.Recognizing += (object? sender, SpeechRecognitionEventArgs e) =>
                    {
                        if (ResultReason.RecognizingSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                        {
    // We don't show sequence numbers for partial results.
                            WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(0, e.Result, userConfig), userConfig);
                        }
                        else if (ResultReason.NoMatch == e.Result.Reason)
                        {
                            WriteToConsole("NOMATCH: Speech could not be recognized.\n", userConfig);
                        }
                    };
            }
            else
            {
                speechRecognizer.Recognized += (object? sender, SpeechRecognitionEventArgs e) =>
                    {
                        if (ResultReason.RecognizedSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                        {
                            sequenceNumber++;
                            WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(0, e.Result, userConfig), userConfig);
                        }
                        else if (ResultReason.NoMatch == e.Result.Reason)
                        {
                            WriteToConsole("NOMATCH: Speech could not be recognized.\n", userConfig);
                        }
                    };
            }

            speechRecognizer.Canceled += (object? sender, SpeechRecognitionCanceledEventArgs e) =>
                {
                    if (CancellationReason.EndOfStream == e.Reason)
                    {
                        WriteToConsole(String.Format("End of stream reached.{0}", Environment.NewLine), userConfig);
                        recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.CancelledByUser == e.Reason)
                    {
                        WriteToConsole(String.Format("User canceled request.{0}", Environment.NewLine), userConfig);
                        recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.Error == e.Reason)
                    {
                        var error = String.Format("Encountered error.{0}Error code: {1}{0}Error details: {2}{0}", Environment.NewLine, (int)e.ErrorCode, e.ErrorDetails);
                        recognitionEnd.TrySetResult(error); // Notify to stop recognition.
                    }
                    else
                    {
                        var error = String.Format("Request was cancelled for an unrecognized reason: {0}.{1}", (int)e.Reason, Environment.NewLine);
                        recognitionEnd.TrySetResult(error); // Notify to stop recognition.
                    }
                };

            speechRecognizer.SessionStopped += (object? sender, SessionEventArgs e) =>
                {
                    WriteToConsole(String.Format("Session stopped.{0}", Environment.NewLine), userConfig);
                    recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                };

// Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            await speechRecognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

// Waits for recognition end.
            Task.WaitAll(new[] { recognitionEnd.Task });

// Stops recognition.
            await speechRecognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            
            return recognitionEnd.Task.Result;
        }

/* Note: To pass command-line arguments, run:
dotnet run -- [args]
For example:
dotnet run -- -h
*/
        static void Main(string[] args)
        {
            const string usage = @"Usage: caption.exe [-f] [-h] [-i file] [-l languages] [-m] [-o file] [-p phrases] [-q] [-r number] [-s] [-t] [-u] <subscriptionKey> <region>
              -f: Enable profanity filter (remove profanity). Overrides -m.
              -h: Show this help and stop.
              -i: Input audio file *file* (default input is from the microphone.)
    -l languages: Enable language identification for specified *languages*.
                  Example: en-US,ja-JP
              -m: Enable profanity filter (mask profanity). -f overrides this.
         -o file: Output to *file*.
      -p phrases: Add specified *phrases*.
                  Example: Constoso;Jessie;Rehaan
              -q: Suppress console output (except errors).
       -r number: Set stable partial result threshold to *number*.
                  Example: 3
              -s: Emit SRT (default is WebVTT.)
              -t: Enable TrueText.
              -u: Emit partial results instead of finalized results.";
            
            try
            {
                if (args.Contains("-h"))
                {
                    Console.WriteLine(usage);
                }
                else
                {
                    UserConfig userConfig = UserConfigFromArgs(args);
                    Initialize(userConfig);
                    SpeechRecognizer speechRecognizer = SpeechRecognizerFromUserConfig(userConfig);                    
                    if (RecognizeContinuous(speechRecognizer, userConfig).Result is string error)
                    {
                        Console.WriteLine(error);
                    }
                }
            }
            catch(TooFewArguments)
            {
                Console.WriteLine("Too few arguments.");
                Console.WriteLine(usage);
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}

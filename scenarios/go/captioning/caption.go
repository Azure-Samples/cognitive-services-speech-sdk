package main

/*
Dependencies
*/

import(
    "bufio"
    "errors"
    "fmt"
/* Notes:
- Add Speech SDK dependency to your .mod file:
go get -d github.com/Microsoft/cognitive-services-speech-sdk-go
See also:
https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?tabs=dotnet%2Cmacos%2Cjre%2Cbrowser&pivots=programming-language-go#configure-the-go-environment
- The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=windows%2Cubuntu%2Cios-xcode%2Cmac-xcode%2Candroid-studio
- In Windows, place the following files from the Speech SDK in the same folder as your .go file (if you are using `go run`) or compiled .exe:
    - Microsoft.CognitiveServices.Speech.core.dll
    - Microsoft.CognitiveServices.Speech.extension.audio.sys.dll
*/
    "github.com/Microsoft/cognitive-services-speech-sdk-go/common"
    "github.com/Microsoft/cognitive-services-speech-sdk-go/audio"
    "github.com/Microsoft/cognitive-services-speech-sdk-go/speech"    
    "log"
    "os"
    "strings"
    "time"
)

/*
Types
*/

type UserConfig struct {  
    profanityFilterRemoveEnabled bool
    profanityFilterMaskEnabled bool
    inputFile *string
    outputFile *string
    suppressOutputEnabled bool
    partialResultsEnabled bool
    stablePartialResultThreshold *string
    srtEnabled bool
    trueTextEnabled bool
    subscriptionKey string
    region string
}

type Timestamp struct {
    startHours int
    endHours int
    startMinutes int
    endMinutes int
    startSeconds int
    endSeconds int
    startMilliseconds int
    endMilliseconds int
}

/*
Global constants
*/

var newline = "\n"

/*
Helper functions
*/

// Type parameters require Go version 1.18 beta 1 or later.
func IndexOf(xs []string, x string)(bool, int) {
    for i, j := range xs {
        if j == x {
            return true, i
        }
    }
    return false, -1
}

func GetCmdOption(args []string, option string) *string {
    var result, index = IndexOf(args, option)
    if result && index < len(args) - 2 {
// We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
        return &args[index + 1]
    } else {
        return nil
    }
}

func CmdOptionExists(args []string, option string) bool {
    var result, _ = IndexOf(args, option)
    return result
}

func TimestampFromSpeechRecognitionResult(result speech.SpeechRecognitionResult) Timestamp {
    var startMilliseconds_1 = time.Duration(result.Offset).Milliseconds()
    var endMilliseconds_1 = time.Duration(result.Offset + result.Duration).Milliseconds()

    var startSeconds_1 = int(startMilliseconds_1) / 1000
    var endSeconds_1 = int(endMilliseconds_1) / 1000
    
    var startMilliseconds_2 = int(startMilliseconds_1) % 1000
    var endMilliseconds_2 = int(endMilliseconds_1) % 1000
    
    var startMinutes_1 = startSeconds_1 / 60
    var endMinutes_1 = endSeconds_1 / 60
    
    var startHours = startMinutes_1 / 60
    var endHours = endMinutes_1 / 60
    
    var startSeconds_2 = startSeconds_1 % 60
    var endSeconds_2 = endSeconds_1 % 60
    
    var startMinutes_2 = startMinutes_1 % 60
    var endMinutes_2 = endMinutes_1 % 60
    
    return Timestamp {
        startMilliseconds : startMilliseconds_2,
        endMilliseconds : endMilliseconds_2,
        startSeconds : startSeconds_2,
        endSeconds : endSeconds_2,
        startMinutes : startMinutes_2,
        endMinutes : endMinutes_2,
        startHours : startHours,
        endHours : endHours,
    }
}

func TimestampStringFromSpeechRecognitionResult(result speech.SpeechRecognitionResult, userConfig UserConfig) string {
    var time_format string
    var timestamp = TimestampFromSpeechRecognitionResult(result)
    if userConfig.srtEnabled {
// SRT format requires ',' as decimal separator rather than '.'.
        time_format = "%02d:%02d:%02d,%03d"
    } else {
        time_format = "%02d:%02d:%02d.%03d"
    }
    return fmt.Sprintf(time_format + " --> " + time_format, timestamp.startHours, timestamp.startMinutes, timestamp.startSeconds, timestamp.startMilliseconds, timestamp.endHours, timestamp.endMinutes, timestamp.endSeconds, timestamp.endMilliseconds)
}

func CaptionFromSpeechRecognitionResult(sequenceNumber int, result speech.SpeechRecognitionResult, userConfig UserConfig) string {
    var caption strings.Builder
    if !userConfig.partialResultsEnabled && userConfig.srtEnabled {
        caption.WriteString(fmt.Sprintf("%d%s", sequenceNumber, newline));
    }
    caption.WriteString(fmt.Sprintf("%s%s", TimestampStringFromSpeechRecognitionResult(result, userConfig), newline));
    caption.WriteString(fmt.Sprintf("%s%s%s", result.Text, newline, newline));
    return caption.String()
}

func WriteToConsole(text string, userConfig UserConfig) {
    if !userConfig.suppressOutputEnabled {
        fmt.Print(text)
    }
}

func WriteToConsoleOrFile(text string, userConfig UserConfig) {
    WriteToConsole(text, userConfig)
    if nil != userConfig.outputFile {
        file, err := os.OpenFile(*userConfig.outputFile, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
        if nil != err {
            log.Fatal(err)
        }
        defer file.Close()
        _, err = file.WriteString(text)
        if nil != err {
            log.Fatal(err)
        }        
    }
}

func Initialize(userConfig UserConfig) {
    if nil != userConfig.outputFile {
        _, err := os.Stat(*userConfig.outputFile)
        if nil == err {
            os.Remove(*userConfig.outputFile)
        } else if !errors.Is(err, os.ErrNotExist) {
            log.Fatal(err)
        }
    }

    if !userConfig.srtEnabled {
        WriteToConsoleOrFile(fmt.Sprintf("WEBVTT%s%s", newline, newline), userConfig)
    }
}

/*
Main functions
*/

func UserConfigFromArgs(args []string) UserConfig {
    var userConfig = UserConfig {
        profanityFilterRemoveEnabled : false,
        profanityFilterMaskEnabled : false,
        inputFile : nil,
        outputFile : nil,
        suppressOutputEnabled : false,
        partialResultsEnabled : false,
        stablePartialResultThreshold : nil,
        srtEnabled : false,
        trueTextEnabled : false,
        subscriptionKey : "",
        region : "",
    }
    
// Verify argc >= 3 (caption.go, subscriptionKey, region)
    if len(args) < 3 {
        log.Fatal("Too few arguments.")
    }

    userConfig.subscriptionKey = args[len(args) - 2]
    userConfig.region = args[len(args) - 1]

    userConfig.profanityFilterRemoveEnabled = CmdOptionExists(args, "-f")
    userConfig.profanityFilterMaskEnabled = CmdOptionExists(args, "-m")
    userConfig.inputFile = GetCmdOption(args, "-i")
    userConfig.outputFile = GetCmdOption(args, "-o")
    userConfig.partialResultsEnabled = CmdOptionExists(args, "-u")
    userConfig.suppressOutputEnabled = CmdOptionExists(args, "-q");
    userConfig.stablePartialResultThreshold = GetCmdOption(args, "-r")
    userConfig.srtEnabled = CmdOptionExists(args, "-s")
    userConfig.trueTextEnabled = CmdOptionExists(args, "-t")
    
    return userConfig
}

func AudioConfigFromUserConfig(userConfig UserConfig) *audio.AudioConfig {
    if nil != userConfig.inputFile {
        result, err := audio.NewAudioConfigFromWavFileInput(*userConfig.inputFile)
        if nil != err {
            log.Fatal(err)
        }
        return result
    } else {
        result, err := audio.NewAudioConfigFromDefaultMicrophoneInput()
        if nil != err {
            log.Fatal(err)
        }
        return result
    }
}

func SpeechConfigFromUserConfig(userConfig UserConfig) *speech.SpeechConfig {
    var speechConfig, err = speech.NewSpeechConfigFromSubscription(userConfig.subscriptionKey, userConfig.region)
    if nil != err {
        log.Fatal(err)
    }

    if userConfig.profanityFilterRemoveEnabled {
        speechConfig.SetProfanity(common.Removed)
    } else if userConfig.profanityFilterMaskEnabled {
        speechConfig.SetProfanity(common.Masked)
    }
    
    if nil != userConfig.stablePartialResultThreshold {
        speechConfig.SetProperty(common.SpeechServiceResponseStablePartialResultThreshold, *userConfig.stablePartialResultThreshold)
    }
    
    if userConfig.trueTextEnabled {
        speechConfig.SetProperty(common.SpeechServiceResponsePostProcessingOption, "TrueText")
    }

    return speechConfig
}

func SpeechRecognizerFromUserConfig(userConfig UserConfig) *speech.SpeechRecognizer {
    var audioConfig = AudioConfigFromUserConfig(userConfig)
    defer audioConfig.Close()
    var speechConfig = SpeechConfigFromUserConfig(userConfig)
    defer speechConfig.Close()
    var speechRecognizer, err = speech.NewSpeechRecognizerFromConfig(speechConfig, audioConfig)
    if nil != err {
        log.Fatal(err)
    }
    
    return speechRecognizer
}

func GetRecognizingHandler(userConfig UserConfig) func(speech.SpeechRecognitionEventArgs) {
    return func(e speech.SpeechRecognitionEventArgs) {
        defer e.Close()
        if common.RecognizingSpeech == e.Result.Reason && len(e.Result.Text) > 0 {
// We don't show sequence numbers for partial results.
            WriteToConsole(CaptionFromSpeechRecognitionResult(0, e.Result, userConfig), userConfig);
        } else if common.NoMatch == e.Result.Reason {
            WriteToConsole("NOMATCH: Speech could not be recognized.\n", userConfig);
        }
    }
}

func GetRecognizedHandler(userConfig UserConfig) func(speech.SpeechRecognitionEventArgs) {
    return func(e speech.SpeechRecognitionEventArgs) {
        defer e.Close()
        var sequenceNumber int = 0
        if common.RecognizedSpeech == e.Result.Reason && len(e.Result.Text) > 0 {
            sequenceNumber++
            WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber, e.Result, userConfig), userConfig);
        } else if common.NoMatch == e.Result.Reason {
            WriteToConsole(fmt.Sprintf("NOMATCH: Speech could not be recognized.%s", newline), userConfig)
        }
    }
}

func GetSessionStoppedHandler(userConfig UserConfig) func(speech.SessionEventArgs) {
    return func(e speech.SessionEventArgs) {
        defer e.Close()
        WriteToConsole(fmt.Sprintf("Session stopped.%s", newline), userConfig)
    }
}

func GetCanceledHandler(userConfig UserConfig) func(speech.SpeechRecognitionCanceledEventArgs) {
    return func(e speech.SpeechRecognitionCanceledEventArgs) {
        defer e.Close()
        if common.EndOfStream == e.Reason {
            WriteToConsole(fmt.Sprintf("End of stream reached.%s", newline), userConfig)
        } else if common.CancelledByUser == e.Reason {
            WriteToConsole(fmt.Sprintf("User canceled request.%s", newline), userConfig)
        } else if common.Error == e.Reason {
            WriteToConsole(fmt.Sprintf("Encountered error.%sError code: %d%sError details: %s%s", newline, int(e.ErrorCode), newline, e.ErrorDetails, newline), userConfig)
        } else {
            WriteToConsole(fmt.Sprintf("Request was cancelled for an unrecognized reason: %d.%s", int(e.Reason), newline), userConfig)
        }
    }
}

func recognizeContinuous(speechRecognizer *speech.SpeechRecognizer, userConfig UserConfig) {
    if userConfig.partialResultsEnabled {
        speechRecognizer.Recognizing(GetRecognizingHandler(userConfig))
    }
    speechRecognizer.Recognized(GetRecognizedHandler(userConfig))
    speechRecognizer.SessionStopped(GetSessionStoppedHandler(userConfig))
    speechRecognizer.Canceled(GetCanceledHandler(userConfig))
    
    speechRecognizer.StartContinuousRecognitionAsync()
    defer speechRecognizer.StopContinuousRecognitionAsync()
// Keep function from exiting before we are done.
    bufio.NewReader(os.Stdin).ReadBytes('\n')
}

func main() {
    var usage string = `Usage: go run caption.go [-f] [-h] [-i file] [-m] [-o file] [-q] [-r number] [-s] [-t] [-u] <subscriptionKey> <region>
            -f: Enable profanity filter (remove profanity). Overrides -m.
            -h: Show this help and stop.
            -i: Input audio file *file* (default input is from the microphone.)
            -m: Enable profanity filter (mask profanity). -f overrides this.
       -o file: Output to *file*.
            -q: Suppress console output (except errors).
     -r number: Set stable partial result threshold to *number*.
                Example: 3
            -s: Output captions in SRT format (default is WebVTT format.)
            -t: Enable TrueText.
            -u: Output partial results. These are always written to the console, never to an output file. -q overrides this.`

    if CmdOptionExists(os.Args, "-h") {
        fmt.Println(usage);
    } else {
        var userConfig = UserConfigFromArgs(os.Args)
        Initialize(userConfig)
        var speechRecognizer = SpeechRecognizerFromUserConfig(userConfig)
        defer speechRecognizer.Close()
        recognizeContinuous(speechRecognizer, userConfig)
    }
}

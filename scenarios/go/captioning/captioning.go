//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package main

// Notes:
// - Add Speech SDK dependency to your .mod file:
// go get -d github.com/Microsoft/cognitive-services-speech-sdk-go
// See also:
// https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform
// - The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
// https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk
// - Install gstreamer:
// https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams
// - In Windows, place the following files from the Speech SDK in the same folder as your .go file (if you are using `go run`) or compiled .exe:
//     - Microsoft.CognitiveServices.Speech.core.dll
//     - Microsoft.CognitiveServices.Speech.extension.audio.sys.dll

import(
    "bufio"
    "errors"
    "fmt"
    "github.com/Microsoft/cognitive-services-speech-sdk-go/common"
    "github.com/Microsoft/cognitive-services-speech-sdk-go/audio"
    "github.com/Microsoft/cognitive-services-speech-sdk-go/speech"    
    "log"
    "os"
    "strings"
    "time"
)

func GetCompressedAudioFormat(args []string) audio.AudioStreamContainerFormat {
    var value = GetCmdOption(args, "--format");
    if nil == value {
        return audio.ANY
    } else {
        switch strings.ToLower(*value) {
            case "alaw" : return audio.ALAW
            case "flac" : return audio.FLAC
            case "mp3" : return audio.MP3
            case "mulaw" : return audio.MULAW
            case "ogg_opus" : return audio.OGGOPUS
            default : return audio.ANY
        }
    }
}

func GetProfanityOption(args []string) common.ProfanityOption {
    var value = GetCmdOption(args, "--profanity");
    if nil == value {
        return common.Masked
    } else {
        switch strings.ToLower(*value) {
            case "raw" : return common.Raw
            case "remove" : return common.Removed
            default : return common.Masked
        }
    }
}

func TimestampFromSpeechRecognitionResult(result speech.SpeechRecognitionResult) Timestamp {
    var startMilliseconds = time.Duration(result.Offset).Milliseconds()
    var endMilliseconds = time.Duration(result.Offset + result.Duration).Milliseconds()

    var startSeconds = int(startMilliseconds) / 1000
    var endSeconds = int(endMilliseconds) / 1000
   
    var startMinutes = startSeconds / 60
    var endMinutes = endSeconds / 60
    
    return Timestamp {
        startMilliseconds : int(startMilliseconds) % 1000,
        endMilliseconds : int(endMilliseconds) % 1000,
        startSeconds : startSeconds % 60,
        endSeconds : endSeconds % 60,
        startMinutes : startMinutes % 60,
        endMinutes : endMinutes % 60,
        startHours : startMinutes / 60,
        endHours : endMinutes / 60,
    }
}

func TimestampStringFromSpeechRecognitionResult(result speech.SpeechRecognitionResult, userConfig UserConfig) string {
    var time_format string
    var timestamp = TimestampFromSpeechRecognitionResult(result)
    if userConfig.useSubRipTextCaptionFormat {
        // SRT format requires ',' as decimal separator rather than '.'.
        time_format = "%02d:%02d:%02d,%03d"
    } else {
        time_format = "%02d:%02d:%02d.%03d"
    }
    return fmt.Sprintf(time_format + " --> " + time_format, timestamp.startHours, timestamp.startMinutes, timestamp.startSeconds, timestamp.startMilliseconds, timestamp.endHours, timestamp.endMinutes, timestamp.endSeconds, timestamp.endMilliseconds)
}

func CaptionFromSpeechRecognitionResult(sequenceNumber int, result speech.SpeechRecognitionResult, userConfig UserConfig) string {
    var caption strings.Builder
    if !userConfig.showRecognizingResults && userConfig.useSubRipTextCaptionFormat {
        caption.WriteString(fmt.Sprintf("%d%s", sequenceNumber, newline));
    }
    caption.WriteString(fmt.Sprintf("%s%s", TimestampStringFromSpeechRecognitionResult(result, userConfig), newline));
    caption.WriteString(fmt.Sprintf("%s%s%s", result.Text, newline, newline));
    return caption.String()
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

    if !userConfig.useSubRipTextCaptionFormat {
        WriteToConsoleOrFile(fmt.Sprintf("WEBVTT%s%s", newline, newline), userConfig)
    }
}

func UserConfigFromArgs(args []string, usage string) UserConfig {
    var userConfig = UserConfig {
        useCompressedAudio : false,
        compressedAudioFormat : audio.ANY,
        profanityOption : common.Masked,
        inputFile : nil,
        outputFile : nil,
        suppressConsoleOutput : false,
        showRecognizingResults : false,
        stablePartialResultThreshold : nil,
        useSubRipTextCaptionFormat : false,
        subscriptionKey : "",
        region : "",
    }
    
    var key = GetCmdOption(args, "--key")
    if nil == key {
        log.Fatal(fmt.Sprintf("Missing subscription key.%s%s", newline, usage))
    }
    var region = GetCmdOption(args, "--region")    
    if nil == region {
        log.Fatal(fmt.Sprintf("Missing region.%s%s", newline, usage))
    }
    
    userConfig.useCompressedAudio = CmdOptionExists(args, "--format")
    userConfig.compressedAudioFormat = GetCompressedAudioFormat(args)
    userConfig.profanityOption = GetProfanityOption(args)
    userConfig.inputFile = GetCmdOption(args, "--input")
    userConfig.outputFile = GetCmdOption(args, "--output")
    userConfig.showRecognizingResults = CmdOptionExists(args, "--recognizing")
    userConfig.suppressConsoleOutput = CmdOptionExists(args, "--quiet");
    userConfig.stablePartialResultThreshold = GetCmdOption(args, "--threshold")
    userConfig.useSubRipTextCaptionFormat = CmdOptionExists(args, "--srt")
    userConfig.subscriptionKey = *key
    userConfig.region = *region
    
    return userConfig
}

func AudioConfigFromUserConfig(userConfig UserConfig) *audio.AudioConfig {
    if nil != userConfig.inputFile {
        var format *audio.AudioStreamFormat
        var err error
        if strings.HasSuffix(*userConfig.inputFile, ".wav") {
            framerate, bitsPerSample, nChannels := ReadWavFileHeader(*userConfig.inputFile)
            format, err = audio.GetWaveFormatPCM(framerate, bitsPerSample, nChannels)
            if nil != err {
                log.Fatal(err)
            }
        } else {
            format, err = audio.GetCompressedFormat(userConfig.compressedAudioFormat)
            if nil != err {
                log.Fatal(err)
            }
            defer format.Close()
        }
        callback := GetBinaryFileReaderCallback(*userConfig.inputFile)
        stream, err := audio.CreatePullStreamFromFormat(callback, format)
        if nil != err {
            log.Fatal(err)
        }
        defer stream.Close()
        audioConfig, err := audio.NewAudioConfigFromStreamInput(stream)
        if nil != err {
            log.Fatal(err)
        }
        return audioConfig
    } else {
        audioConfig, err := audio.NewAudioConfigFromDefaultMicrophoneInput()
        if nil != err {
            log.Fatal(err)
        }
        return audioConfig
    }
}

func SpeechConfigFromUserConfig(userConfig UserConfig) *speech.SpeechConfig {
    var speechConfig, err = speech.NewSpeechConfigFromSubscription(userConfig.subscriptionKey, userConfig.region)
    if nil != err {
        log.Fatal(err)
    }

    speechConfig.SetProfanity(userConfig.profanityOption)
    
    if nil != userConfig.stablePartialResultThreshold {
        speechConfig.SetProperty(common.SpeechServiceResponseStablePartialResultThreshold, *userConfig.stablePartialResultThreshold)
    }
    
    speechConfig.SetProperty(common.SpeechServiceResponsePostProcessingOption, "TrueText")

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
    if userConfig.showRecognizingResults {
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
    var usage string = `Usage: go run captioning.go [...]

  HELP
    --help                        Show this help and stop.

  CONNECTION
    --key KEY                     Your Azure Speech service subscription key.
    --region REGION               Your Azure Speech service region.
                                  Examples: westus, eastus

  INPUT
    --input FILE                  Input audio from file (default input is the microphone.)
    --url URL                     Input audio from URL (default input is the microphone.)
    --format FORMAT               Use compressed audio format.
                                  If this is not present, uncompressed format (wav) is assumed.
                                  Valid only with --file or --url.
                                  Valid values: alaw, any, flac, mp3, mulaw, ogg_opus

  RECOGNITION
    --recognizing                 Output Recognizing results (default output is Recognized results only.)
                                  These are always written to the console, never to an output file.
                                  --quiet overrides this.

  ACCURACY
    --phrases PHRASE1;PHRASE2     Example: Constoso;Jessie;Rehaan

  OUTPUT
    --output FILE                 Output captions to text file.
    --srt                         Output captions in SubRip Text format (default format is WebVTT.)
    --quiet                       Suppress console output, except errors.
    --profanity OPTION            Valid values: raw, remove, mask
    --threshold NUMBER            Set stable partial result threshold.
                                  Default value: 3
`

    if CmdOptionExists(os.Args, "--help") {
        fmt.Println(usage);
    } else {
        var userConfig = UserConfigFromArgs(os.Args, usage)
        Initialize(userConfig)
        var speechRecognizer = SpeechRecognizerFromUserConfig(userConfig)
        defer speechRecognizer.Close()
        recognizeContinuous(speechRecognizer, userConfig)
    }
}

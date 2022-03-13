"use strict";

/*
Dependencies
*/

/* Notes:
- To install dependencies for this sample:
npm install microsoft-cognitiveservices-speech-sdk
- The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=windows%2Cubuntu%2Cios-xcode%2Cmac-xcode%2Candroid-studio
*/
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as fs from "fs";

/*
Types
*/

interface UserConfig {
    readonly profanityFilterRemoveEnabled : boolean;
    readonly profanityFilterMaskEnabled : boolean;
    readonly inputFile : string;
    readonly outputFile : string;
    readonly phraseList : string;
    readonly suppressOutputEnabled : boolean;
    readonly partialResultsEnabled : boolean;
    readonly stablePartialResultThreshold : string;
    readonly srtEnabled : boolean;
    readonly trueTextEnabled : boolean;
    readonly subscriptionKey : string;
    readonly region : string;
}

/*
Global constants
*/

const newline = "\n";

/*
Helper functions
*/

function GetCmdOption(args : string[], option : string) : string
{
    const index = args.indexOf(option);
    if (index > -1 && index < args.length - 2)
    {
// We found the option(for example, "-o"), so advance from that to the value(for example, "filename").
        return args[index + 1];
    }
    else {
        return null;
    }
}

function CmdOptionExists(args : string[], option : string) : boolean
{
    return args.includes(option);
}

function TimestampFromSpeechRecognitionResult(result : sdk.SpeechRecognitionResult, userConfig : UserConfig) : string
{
/* Offset and duration are measured in 100-nanosecond increments. The Date constructor takes a value measured in milliseconds.
100 nanoseconds is equal to a tick. There are 10,000 ticks in a millisecond.
See:
https://docs.microsoft.com/en-us/dotnet/api/system.timespan.ticks
https://docs.microsoft.com/en-us/javascript/api/microsoft-cognitiveservices-speech-sdk/speechrecognitionresult?view=azure-node-latest#microsoft-cognitiveservices-speech-sdk-speechrecognitionresult-offset
https://docs.microsoft.com/en-us/javascript/api/microsoft-cognitiveservices-speech-sdk/speechrecognitionresult?view=azure-node-latest#microsoft-cognitiveservices-speech-sdk-speechrecognitionresult-duration
*/
    const ticksPerMillisecond = 10000;
    const startTime = new Date(result.offset/ticksPerMillisecond);
    const endTime = new Date(result.offset/ticksPerMillisecond + result.duration/ticksPerMillisecond);
// Note We must use getUTC* methods, or the results are adjusted for our local time zone, which we don't want.
    const start_hours = startTime.getUTCHours().toString().padStart(2, '0');
    const start_minutes = startTime.getUTCMinutes().toString().padStart(2, '0');
    const start_seconds = startTime.getUTCSeconds().toString().padStart(2, '0');
    const start_milliseconds = startTime.getUTCMilliseconds().toString().padStart(3, '0');
    const end_hours = endTime.getUTCHours().toString().padStart(2, '0');
    const end_minutes = endTime.getUTCMinutes().toString().padStart(2, '0');
    const end_seconds = endTime.getUTCSeconds().toString().padStart(2, '0');
    const end_milliseconds = endTime.getUTCMilliseconds().toString().padStart(3, '0');    
    
    if (userConfig.srtEnabled)
    {
// SRT format requires ',' as decimal separator rather than '.'.
        return `${start_hours}:${start_minutes}:${start_seconds},${start_milliseconds} --> ${end_hours}:${end_minutes}:${end_seconds},${end_milliseconds}`;
    }
    else
    {
        return `${start_hours}:${start_minutes}:${start_seconds}.${start_milliseconds} --> ${end_hours}:${end_minutes}:${end_seconds}.${end_milliseconds}`;
    }
}

function CaptionFromSpeechRecognitionResult(sequenceNumber : number, result : sdk.SpeechRecognitionResult, userConfig : UserConfig) : string
{
    var caption = "";
    if (!userConfig.partialResultsEnabled && userConfig.srtEnabled)
    {
        caption += `${sequenceNumber}${newline}`;
    }
    caption += `${TimestampFromSpeechRecognitionResult(result, userConfig)}${newline}`;
    caption += `${result.text}${newline}${newline}`
    return caption;
}

function WriteToConsole(text : string, userConfig : UserConfig) : void
{
    if (!userConfig.suppressOutputEnabled)
    {
        process.stdout.write(text);
    }
}

function WriteToConsoleOrFile(text : string, userConfig : UserConfig) : void
{
    WriteToConsole(text, userConfig);
    if (null !== userConfig.outputFile)
    {
        fs.appendFileSync(userConfig.outputFile, text);
    }
}

function Initialize(userConfig : UserConfig) : void
{
    if (null !== userConfig.outputFile && fs.existsSync(userConfig.outputFile))
    {
        fs.unlinkSync(userConfig.outputFile);
    }

    if (!userConfig.srtEnabled)
    {
        WriteToConsoleOrFile(`WEBVTT${newline}${newline}`, userConfig);
    }
}

/*
Main functions
*/

// Note: We verified args.length >= 3 in the caller, main().
function UserConfigFromArgs(args : string[]) : UserConfig
{
    return {
        profanityFilterRemoveEnabled : CmdOptionExists(args, "-f"),
        profanityFilterMaskEnabled : CmdOptionExists(args, "-m"),
        inputFile : GetCmdOption(args, "-i"),
        outputFile : GetCmdOption(args, "-o"),
        phraseList : GetCmdOption(args, "-p"),
        suppressOutputEnabled : CmdOptionExists(args, "-q"),
        partialResultsEnabled : CmdOptionExists(args, "-u"),
        stablePartialResultThreshold : GetCmdOption(args, "-r"),
        srtEnabled : CmdOptionExists(args, "-s"),
        trueTextEnabled : CmdOptionExists(args, "-t"),
        subscriptionKey : args[args.length - 2],
        region : args[args.length - 1],
    };
}

function AudioConfigFromUserConfig(userConfig : UserConfig) : sdk.AudioConfig
{
    if (null !== userConfig.inputFile)
    {
        return sdk.AudioConfig.fromWavFileInput(fs.readFileSync(userConfig.inputFile));
    }
    else
    {
        return sdk.AudioConfig.fromDefaultMicrophoneInput();
    }
}

function SpeechConfigFromUserConfig(userConfig : UserConfig) : sdk.SpeechConfig
{
    const speechConfig = sdk.SpeechConfig.fromSubscription(userConfig.subscriptionKey, userConfig.region);

    if (userConfig.profanityFilterRemoveEnabled)
    {
        speechConfig.setProfanity(sdk.ProfanityOption.Removed);
    }
    else if (userConfig.profanityFilterMaskEnabled)
    {
        speechConfig.setProfanity(sdk.ProfanityOption.Masked);
    }
    
    if (null !== userConfig.stablePartialResultThreshold)
    {
        speechConfig.setProperty("SpeechServiceResponse_StablePartialResultThreshold", userConfig.stablePartialResultThreshold);
    }
    
    if (userConfig.trueTextEnabled)
    {
        speechConfig.setProperty("SpeechServiceResponse_PostProcessingOption", "TrueText");
    }
    
    return speechConfig;
}

function SpeechRecognizerFromSpeechConfig(speechConfig : sdk.SpeechConfig, audio_config : sdk.AudioConfig, userConfig : UserConfig) : sdk.SpeechRecognizer
{
    var speechRecognizer = new sdk.SpeechRecognizer(speechConfig, audio_config);
    
    if (null !== userConfig.phraseList)
    {
        const grammar = sdk.PhraseListGrammar.fromRecognizer(speechRecognizer);
        grammar.addPhrase(userConfig.phraseList);
    }
    
    return speechRecognizer;
}

function SpeechRecognizerFromUserConfig(userConfig : UserConfig) : sdk.SpeechRecognizer
{
    const audioConfig = AudioConfigFromUserConfig(userConfig);
    const speechConfig = SpeechConfigFromUserConfig(userConfig);
    return SpeechRecognizerFromSpeechConfig(speechConfig, audioConfig, userConfig);
}

/* See:
https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/how-to-recognize-speech?tabs=windowsinstall&pivots=programming-language-javascript#use-continuous-recognition
*/
function RecognizeContinuous(speechRecognizer : sdk.SpeechRecognizer, userConfig : UserConfig) : void
{
    var sequenceNumber = 0;
    
    if (userConfig.partialResultsEnabled)
    {
        speechRecognizer.recognizing = function(s, e) {
            if (sdk.ResultReason.RecognizingSpeech == e.result.reason && e.result.text.length > 0)
            {
// We don't show sequence numbers for partial results.
                WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(0, e.result, userConfig), userConfig);
            }
            else if (sdk.ResultReason.NoMatch == e.result.reason)
            {
                WriteToConsole("NOMATCH: Speech could not be recognized.${newline}", userConfig);
            }
        };
    }
    else
    {
        speechRecognizer.recognized = function(s, e) {
            if (sdk.ResultReason.RecognizedSpeech == e.result.reason && e.result.text.length > 0)
            {
                sequenceNumber++;
                WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber, e.result, userConfig), userConfig);
            }
            else if (sdk.ResultReason.NoMatch == e.result.reason)
            {
                WriteToConsole("NOMATCH: Speech could not be recognized.${newline}", userConfig);
            }
        };
    }

    speechRecognizer.canceled =(s, e) => {
        if (sdk.CancellationReason.EndOfStream == e.reason)
        {
            WriteToConsole("End of stream reached.${newline}", userConfig);
        }
        else if (sdk.CancellationReason.Error == e.reason)
        {
            var error = "Encountered error.${newline}Error code: ${e.errorCode}${newline}Error details: ${e.errorDetails}${newline}";
        }
        else
        {
            var error = "Request was cancelled for an unrecognized reason: ${e.reason}.${newline}";
        }

        speechRecognizer.stopContinuousRecognitionAsync();
    };

    speechRecognizer.sessionStopped =(s, e) => {
        WriteToConsole("Session stopped.${newline}", userConfig);

        speechRecognizer.stopContinuousRecognitionAsync();
    };

    speechRecognizer.startContinuousRecognitionAsync();
}

function main(args : string[]) : void
{
    const usage = `Usage: node caption.js [-f] [-h] [-i file] [-m] [-o file] [-p phrases] [-q] [-r number] [-s] [-t] [-u] <subscriptionKey> <region>
            -f: Enable profanity filter (remove profanity). Overrides -m.
            -h: Show this help and stop.
            -i: Input audio file *file* (default input is from the microphone.)
            -m: Enable profanity filter (mask profanity). -f overrides this.
       -o file: Output to *file*.
    -p phrases: Add specified *phrases*.
                Example: Constoso;Jessie;Rehaan
            -q: Suppress console output (except errors).
     -r number: Set stable partial result threshold to *number*.
                Example: 3
            -s: Emit SRT(default is WebVTT.)
            -t: Enable TrueText.
            -u: Emit partial results instead of finalized results.`;

// Verify argc >= 3 (caption.exe, subscriptionKey, region)
    if (args.length < 3)
    {
        console.log("Too few arguments.");
        console.log(usage);
        return;
    }

    if (CmdOptionExists(args, "-h"))
    {
        console.log(usage);
    }
    else
    {
        const userConfig = UserConfigFromArgs(args);    
        Initialize(userConfig);    
        const audio_config = AudioConfigFromUserConfig(userConfig);
        const speechRecognizer = SpeechRecognizerFromUserConfig(userConfig);
        RecognizeContinuous(speechRecognizer, userConfig);
    }
}

main(process.argv);

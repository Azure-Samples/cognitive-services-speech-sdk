//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

"use strict";

/* Notes:
- To install dependencies for this sample:
npm install microsoft-cognitiveservices-speech-sdk
- The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=windows%2Cubuntu%2Cios-xcode%2Cmac-xcode%2Candroid-studio
*/

import { Buffer } from 'buffer';
import * as fs from "fs";
import * as sdk from "microsoft-cognitiveservices-speech-sdk";

// For definition of PullAudioInputStreamCallback interface, see:
// https://github.com/microsoft/cognitive-services-speech-sdk-js/blob/master/src/sdk/Audio/PullAudioInputStreamCallback.ts
class BinaryFileReader implements sdk.PullAudioInputStreamCallback {
    m_fd : number;

    constructor(audioFileName: string) {
        this.m_fd = fs.openSync(audioFileName, 'r');
// TODO1 If err, throw.
    }

    // See:
    // https://javascript.info/arraybuffer-binary-arrays
    read(dataBuffer: ArrayBuffer): number {
        var view = new Uint16Array(dataBuffer);
        var bytesRead = fs.readSync(this.m_fd, view);
        return bytesRead;
    }
    
    close(): void {
        fs.closeSync(this.m_fd);
    }
}

interface WavFileHeaderInfo {
    framerate : number;
    bitsPerSample : number;
    nChannels : number;
}

function ReadInt32(fd : number) : number {
    var buffer = Buffer.alloc(4);
    var bytesRead = fs.readSync(fd, buffer);
    // TODO1 Throw if bytesRead wrong.
    return buffer.readInt32LE();
}

function ReadUInt16(fd : number) : number {
    var buffer = Buffer.alloc(2);
    var bytesRead = fs.readSync(fd, buffer);
    // TODO1 Throw if bytesRead wrong.
    return buffer.readUInt16LE();
}

function ReadUInt32(fd : number) : number {
    var buffer = Buffer.alloc(4);
    var bytesRead = fs.readSync(fd, buffer);
    // TODO1 Throw if bytesRead wrong.
    return buffer.readUInt32LE();
}

function ReadString(fd : number, length : number) : string {
    var buffer = Buffer.alloc(length);
    var bytesRead = fs.readSync(fd, buffer);
    // TODO1 Throw if bytesRead wrong.
    return buffer.toString();
}

function ReadWavFileHeader(audioFileName : string) : WavFileHeaderInfo {
    var fd = fs.openSync(audioFileName, 'r');

    if(ReadString(fd, 4) != "RIFF") {
        throw "Invalid .wav file header.";
    }
    // File length
    ReadInt32(fd);
    if(ReadString(fd, 4) != "WAVE") {
        throw "Invalid .wav file header.";
    }
    if(ReadString(fd, 4) != "fmt ") {
        throw "Invalid .wav file header.";
    }
    // Format size
    // TODO1 Throw if >= 16
    ReadInt32(fd);
    // Format tag
    ReadUInt16(fd);
    var nChannels = ReadUInt16(fd);
    var framerate = ReadUInt32(fd);
    // Average bytes per second
    ReadUInt32(fd);
    // Block align
    ReadUInt16(fd);
    var bitsPerSample = ReadUInt16(fd);

    fs.closeSync(fd);

    return { framerate : framerate, bitsPerSample : bitsPerSample, nChannels : nChannels };
}

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

const newline = "\n";

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
    https://docs.microsoft.com/dotnet/api/system.timespan.ticks
    https://docs.microsoft.com/javascript/api/microsoft-cognitiveservices-speech-sdk/speechrecognitionresult
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
        var header = ReadWavFileHeader(userConfig.inputFile);
        var format = sdk.AudioStreamFormat.getWaveFormatPCM(header.framerate, header.bitsPerSample, header.nChannels);
        var callback = new BinaryFileReader(userConfig.inputFile);
        var stream = sdk.AudioInputStream.createPullStream(callback, format);
        return sdk.AudioConfig.fromStreamInput(stream);
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
    
    speechConfig.setProperty("SpeechServiceResponse_PostProcessingOption", "TrueText");
    
    return speechConfig;
}

function SpeechRecognizerFromUserConfig(userConfig : UserConfig) : sdk.SpeechRecognizer
{
    const audioConfig = AudioConfigFromUserConfig(userConfig);
    const speechConfig = SpeechConfigFromUserConfig(userConfig);
    var speechRecognizer = new sdk.SpeechRecognizer(speechConfig, audioConfig);
    
    if (null !== userConfig.phraseList)
    {
        const grammar = sdk.PhraseListGrammar.fromRecognizer(speechRecognizer);
        grammar.addPhrase(userConfig.phraseList);
    }
    
    return speechRecognizer;
}

/* See:
https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-recognize-speech
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
                WriteToConsole(CaptionFromSpeechRecognitionResult(0, e.result, userConfig), userConfig);
            }
            else if (sdk.ResultReason.NoMatch == e.result.reason)
            {
                WriteToConsole("NOMATCH: Speech could not be recognized.${newline}", userConfig);
            }
        };
    }

    speechRecognizer.recognized = function(s, e) {
        if (sdk.ResultReason.RecognizedSpeech == e.result.reason && e.result.text.length > 0)
        {
            sequenceNumber++;
            WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber, e.result, userConfig), userConfig);
        }
        else if (sdk.ResultReason.NoMatch == e.result.reason)
        {
            // TODO1 String interpolation isn't working!
            WriteToConsole("NOMATCH: Speech could not be recognized.${newline}", userConfig);
        }
    };

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
            -s: Output captions in SRT format (default is WebVTT format.)
            -t: Enable TrueText.
            -u: Output partial results. These are always written to the console, never to an output file. -q overrides this.`;

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

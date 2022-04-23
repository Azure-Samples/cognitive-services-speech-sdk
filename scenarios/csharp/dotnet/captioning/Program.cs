//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// Notes:
// - Install the Speech NuGet package:
// dotnet add package Microsoft.CognitiveServices.Speech
// - The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
// https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk
// - Install gstreamer:
// https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Captioning
{
    class Program
    {
        private UserConfig userConfig;
        private const string usage = @"Usage: caption.exe [-c ALAW|ANY|FLAC|MP3|MULAW|OGG_OPUS] [-f] [-h] [-i path] [-l languages] [-m] [-o path] [-p phrases] [-q] [-s] [-t number] [-u] <subscriptionKey> <region>
       -c format: Use compressed audio format.
                  Valid values: ALAW, ANY, FLAC, MP3, MULAW, OGG_OPUS.
                  Default value: ANY.
              -f: Remove profanity (default behavior is to mask profanity). Overrides -m.
              -h: Show this help and stop.
         -i path: Input audio file *path* (default input is from the microphone.)
    -l languages: Enable language identification for specified *languages*.
                  Example: en-US,ja-JP
              -m: Disable masking profanity (default behavior). -f overrides this.
         -o path: Output to file *path*.
      -p phrases: Add specified *phrases*.
                  Example: Constoso;Jessie;Rehaan
              -q: Suppress console output (except errors).
              -s: Output captions in SubRip Text format (default is WebVTT format.)
       -t number: Set stable partial result threshold to *number*.
                  Example: 3
              -u: Output Recognizing results (default is Recognized results only). These are always written to the console, never to an output file. -q overrides this.";
        
        private static string? GetCmdOption(string[] args, string option)
        {
            var index = Array.IndexOf(args, option);
            if (index > -1 && index < args.Length - 2)
            {
                // We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
                return args[index + 1];
            }
            else {
                return null;
            }
        }

        private static bool CmdOptionExists(string[] args, string option)
        {
            return args.Contains (option);
        }

        private static AudioStreamContainerFormat GetCompressedAudioFormat(string[] args)
        {
            var value = GetCmdOption(args, "-c");
            if (null == value)
            {
                return AudioStreamContainerFormat.ANY;
            }
            else
            {
                switch (value.ToLower())
                {
                    case "alaw" : return AudioStreamContainerFormat.ALAW;
                    case "flac" : return AudioStreamContainerFormat.FLAC;
                    case "mp3" : return AudioStreamContainerFormat.MP3;
                    case "mulaw" : return AudioStreamContainerFormat.MULAW;
                    case "ogg_opus" : return AudioStreamContainerFormat.OGG_OPUS;
                    default : return AudioStreamContainerFormat.ANY;
                }
            }
        }

        private static string V2EndpointFromRegion(string region)
        {
            // Note: Continuous language identification is supported only with v2 endpoints.
            return $"wss://{region}.stt.speech.microsoft.com/speech/universal/v2";
        }

        private string TimestampFromSpeechRecognitionResult(SpeechRecognitionResult result)
        {
            var startTime = new DateTime(result.OffsetInTicks);
            var endTime = startTime.Add(result.Duration);

            // SRT format requires ',' as decimal separator rather than '.'.
            return this.userConfig.useSubRipTextCaptionFormat
                ? $"{startTime:HH:mm:ss,fff} --> {endTime:HH:mm:ss,fff}"
                : $"{startTime:HH:mm:ss.fff} --> {endTime:HH:mm:ss.fff}";            
        }

        private string LanguageFromSpeechRecognitionResult(SpeechRecognitionResult result)
        {
            if (this.userConfig.languageIDLanguages is not null)
            {
                var languageIDResult = AutoDetectSourceLanguageResult.FromResult(result);
                return $"[{languageIDResult.Language}]";
            }
            else
            {
                return "";
            }
        }

        private string CaptionFromSpeechRecognitionResult(int sequenceNumber, SpeechRecognitionResult result)
        {
            var caption = new StringBuilder();
            if (!this.userConfig.showRecognizingResults && this.userConfig.useSubRipTextCaptionFormat)
            {
                caption.AppendFormat($"{sequenceNumber}{Environment.NewLine}");
            }
            caption.AppendFormat($"{TimestampFromSpeechRecognitionResult(result)}{Environment.NewLine}");
            caption.Append(LanguageFromSpeechRecognitionResult(result));
            caption.AppendFormat($"{result.Text}{Environment.NewLine}{Environment.NewLine}");
            return caption.ToString();
        }

        private void WriteToConsole(string text)
        {
            if (!this.userConfig.suppressConsoleOutput)
            {
                Console.Write(text);
            }
        }

        private void WriteToConsoleOrFile(string text)
        {
            WriteToConsole(text);
            if (this.userConfig.outputFilePath is string outputFilePathValue)
            {
                File.AppendAllText(outputFilePathValue, text);
            }
        }

        //
        // Constructor
        //
        public Program(string[] args)
        {
            // Verify argc >= 3 (caption.exe, subscriptionKey, region)
            if (args.Length < 3)
            {
                throw new ArgumentException($"Too few arguments.{Environment.NewLine}Usage: {usage}");
            }

            string[]? languageIDLanguages = null;
            if (GetCmdOption(args, "-l") is string languageIDLanguagesResult)
            {
                languageIDLanguages = languageIDLanguagesResult.Split(',');
            }
            
            this.userConfig = new UserConfig(
                GetCompressedAudioFormat(args),
                CmdOptionExists(args, "-f"),
                CmdOptionExists(args, "-m"),
                languageIDLanguages,
                GetCmdOption(args, "-i"),
                GetCmdOption(args, "-o"),
                GetCmdOption(args, "-p"),
                CmdOptionExists(args, "-q"),
                CmdOptionExists(args, "-u"),
                CmdOptionExists(args, "-s"),
                GetCmdOption(args, "-t"),
                args[args.Length - 2],
                args[args.Length - 1]
            );
        }

        private void Initialize()
        {
            if (this.userConfig.outputFilePath is string outputFilePathValue && File.Exists(outputFilePathValue))
            {
                File.Delete(outputFilePathValue);
            }
            if (!this.userConfig.useSubRipTextCaptionFormat)
            {
                WriteToConsoleOrFile($"WEBVTT{Environment.NewLine}");
            }
        }

        //
        // Create AudioConfig
        //
        private AudioConfig AudioConfigFromUserConfig()
        {
            if (this.userConfig.inputFilePath is string inputFilePathValue)
            {
                if (inputFilePathValue.EndsWith(".wav"))
                {
                    return Helper.OpenWavFile(inputFilePathValue, AudioProcessingOptions.Create(0));
                }
                else
                {
                    var reader = new BinaryReader(File.OpenRead(inputFilePathValue));
                    var format = AudioStreamFormat.GetCompressedFormat(userConfig.compressedAudioFormat);
                    var stream = new PullAudioInputStream(new BinaryAudioStreamReader(reader), format);
                    return AudioConfig.FromStreamInput(stream);
                }
            }
            else
            {
                return AudioConfig.FromDefaultMicrophoneInput();
            }
        }

        //
        // Create SpeechConfig
        //
        private SpeechConfig SpeechConfigFromUserConfig()
        {
            SpeechConfig speechConfig;
            // Language identification requires V2 endpoint.
            if (this.userConfig.languageIDLanguages is not null)
            {
                speechConfig = SpeechConfig.FromEndpoint(new Uri(V2EndpointFromRegion(this.userConfig.region)), this.userConfig.subscriptionKey);
            }
            else
            {
                speechConfig = SpeechConfig.FromSubscription(this.userConfig.subscriptionKey, this.userConfig.region);
            }

            if (this.userConfig.removeProfanity)
            {
                speechConfig.SetProfanity(ProfanityOption.Removed);
            }
            else if (this.userConfig.disableMaskingProfanity)
            {
                speechConfig.SetProfanity(ProfanityOption.Raw);
            }
            
            if (this.userConfig.stablePartialResultThreshold is string stablePartialResultThresholdValue)
            {
                speechConfig.SetProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, stablePartialResultThresholdValue);
            }
            
            speechConfig.SetProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");
            
            return speechConfig;
        }

        //
        // Create SpeechRecognizer
        //
        private SpeechRecognizer SpeechRecognizerFromUserConfig()
        {
            var audioConfig = AudioConfigFromUserConfig();
            var speechConfig = SpeechConfigFromUserConfig();
            SpeechRecognizer speechRecognizer;
            
            if (userConfig.languageIDLanguages is string[] languageIDLanguagesValue)
            {
                var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(languageIDLanguagesValue);
                speechRecognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, audioConfig);
            }
            else
            {
                speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);
            }
            
            if (this.userConfig.phraseList is string phraseListValue)
            {
                var grammar = PhraseListGrammar.FromRecognizer(speechRecognizer);
                grammar.AddPhrase(phraseListValue);
            }
            
            return speechRecognizer;
        }

        //
        // Convert audio to captions
        //
        private async Task<string?> RecognizeContinuous(SpeechRecognizer speechRecognizer)
        {
            var recognitionEnd = new TaskCompletionSource<string?>();
            int sequenceNumber = 0;

            if (this.userConfig.showRecognizingResults)
            {
                speechRecognizer.Recognizing += (object? sender, SpeechRecognitionEventArgs e) =>
                    {
                        if (ResultReason.RecognizingSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                        {
                            // We don't show sequence numbers for partial results.
                            WriteToConsole(CaptionFromSpeechRecognitionResult(0, e.Result));
                        }
                        else if (ResultReason.NoMatch == e.Result.Reason)
                        {
                            WriteToConsole($"NOMATCH: Speech could not be recognized.{Environment.NewLine}");
                        }
                    };
            }

            speechRecognizer.Recognized += (object? sender, SpeechRecognitionEventArgs e) =>
                {
                    if (ResultReason.RecognizedSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                    {
                        sequenceNumber++;
                        WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber, e.Result));
                    }
                    else if (ResultReason.NoMatch == e.Result.Reason)
                    {
                        WriteToConsole($"NOMATCH: Speech could not be recognized.{Environment.NewLine}");
                    }
                };

            speechRecognizer.Canceled += (object? sender, SpeechRecognitionCanceledEventArgs e) =>
                {
                    if (CancellationReason.EndOfStream == e.Reason)
                    {
                        WriteToConsole($"End of stream reached.{Environment.NewLine}");
                        recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.CancelledByUser == e.Reason)
                    {
                        WriteToConsole($"User canceled request.{Environment.NewLine}");
                        recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.Error == e.Reason)
                    {
                        var error = $"Encountered error.{Environment.NewLine}Error code: {(int)e.ErrorCode}{Environment.NewLine}Error details: {e.ErrorDetails}{Environment.NewLine}";
                        recognitionEnd.TrySetResult(error); // Notify to stop recognition.
                    }
                    else
                    {
                        var error = $"Request was cancelled for an unrecognized reason: {(int)e.Reason}.{Environment.NewLine}";
                        recognitionEnd.TrySetResult(error); // Notify to stop recognition.
                    }
                };

            speechRecognizer.SessionStopped += (object? sender, SessionEventArgs e) =>
                {
                    WriteToConsole($"Session stopped.{Environment.NewLine}");
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

        // Note: To pass command-line arguments, run:
        // dotnet run -- [args]
        // For example:
        // dotnet run -- -h
        public static void Main(string[] args)
        {
            try
            {
                if (args.Contains("-h"))
                {
                    Console.WriteLine(usage);
                }
                else
                {
                    Program program = new Program(args);
                    program.Initialize();
                    var speechRecognizer = program.SpeechRecognizerFromUserConfig();
                    if (program.RecognizeContinuous(speechRecognizer).Result is string error)
                    {
                        Console.WriteLine(error);
                    }
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}

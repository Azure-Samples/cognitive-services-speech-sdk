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

using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace Captioning
{
    class Program
    {
        private UserConfig userConfig;
        private const string usage = @"USAGE: dotnet run -- [...]

  HELP
    --help                        Show this help and stop.

  CONNECTION
    --key KEY                     Your Azure Speech service subscription key.
    --region REGION               Your Azure Speech service region.
                                  Examples: westus, eastus

  LANGUAGE
    --languages LANG1;LANG2       Enable language identification for specified languages.
                                  Example: en-US;ja-JP

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
";
        
        private static string? GetCmdOption(string[] args, string option)
        {
            int index = Array.IndexOf(args, option);
            if (index > -1 && index < args.Length - 1)
            {
                // We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
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

        private static string[]? GetLanguageIDLanguages(string[] args)
        {
            string[]? languageIDLanguages = null;
            if (GetCmdOption(args, "--languages") is string languageIDLanguagesResult)
            {
                languageIDLanguages = languageIDLanguagesResult.Split(';');
            }
            return languageIDLanguages;
        }

        private static AudioStreamContainerFormat GetCompressedAudioFormat(string[] args)
        {
            string? value = GetCmdOption(args, "--format");
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

        private static ProfanityOption GetProfanityOption(string[] args)
        {
            string? value = GetCmdOption(args, "--profanity");
            if (value is null)
            {
                return ProfanityOption.Masked;
            }
            else
            {
                switch (value.ToLower())
                {
                    case "raw" : return ProfanityOption.Raw;
                    case "remove" : return ProfanityOption.Removed;
                    default : return ProfanityOption.Masked;
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
            DateTime endTime = startTime.Add(result.Duration);

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
            string? key = GetCmdOption(args, "--key");
            if (key is null)
            {
                throw new ArgumentException($"Missing subscription key.{Environment.NewLine}Usage: {usage}");
            }
            string? region = GetCmdOption(args, "--region");
            if (region is null)
            {
                throw new ArgumentException($"Missing region.{Environment.NewLine}Usage: {usage}");
            }
            
            this.userConfig = new UserConfig(
                CmdOptionExists(args, "--format"),
                GetCompressedAudioFormat(args),
                GetProfanityOption(args),
                GetLanguageIDLanguages(args),
                GetCmdOption(args, "--input"),
                GetCmdOption(args, "--output"),
                GetCmdOption(args, "--phrases"),
                CmdOptionExists(args, "--quiet"),
                CmdOptionExists(args, "--recognizing"),
                CmdOptionExists(args, "--srt"),
                GetCmdOption(args, "--threshold"),
                key,
                region
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
                if (!this.userConfig.useCompressedAudio)
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

            speechConfig.SetProfanity(this.userConfig.profanityOption);
            
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
            AudioConfig audioConfig = AudioConfigFromUserConfig();
            SpeechConfig speechConfig = SpeechConfigFromUserConfig();
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
        // dotnet run -- --help
        public static void Main(string[] args)
        {
            try
            {
                if (args.Contains("--help"))
                {
                    Console.WriteLine(usage);
                }
                else
                {
                    Program program = new Program(args);
                    program.Initialize();
                    SpeechRecognizer speechRecognizer = program.SpeechRecognizerFromUserConfig();
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

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
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Azure.AI.Details.Common.CLI;

namespace Captioning
{
    class Program
    {
        private UserConfig _userConfig;
        private TaskCompletionSource<string?> _recognitionEnd = new TaskCompletionSource<string?>();
        private int _srtSequenceNumber = 0;
        private CaptionHelper _captionHelper;
        private int? _currentCaptionsLength = null;
        private int? _currentCaptionSize = null;
        private long? _previousCaptionEndTimeTicks = null;
        
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
    --format FORMAT               Use compressed audio format.
                                  If this is not present, uncompressed format (wav) is assumed.
                                  Valid only with --file.
                                  Valid values: alaw, any, flac, mp3, mulaw, ogg_opus

  MODE
    --offline                     Output offline results.
                                  Overrides --realTime.
    --realTime                    Output real-time results.
                                  Default output mode is offline.
    --realTimeDelay               Simulated real-time caption delay.
                                  Valid only with --realTime.
                                  Minimum is 0. Default is 0.
                                  TODO1 Not implemented.

  ACCURACY
    --phrases PHRASE1;PHRASE2     Example: ""Constoso;Jessie;Rehaan""

  OUTPUT
    --output FILE                 Output captions to text file.
    --srt                         Output captions in SubRip Text format (default format is WebVTT.)
    --maxCaptionLength LENGTH     Set the maximum number of characters per line for a caption to LENGTH.
                                  Minimum is 20. Default is no limit.
    --maxCaptionLines LINES       Set the maximum number of lines for a caption to LINES.
                                  Valid only with --maxCaptionLength.
                                  Minimum is 1. Default is 3.
    --quiet                       Suppress console output, except errors.
    --profanity OPTION            Valid values: raw, remove, mask
    --threshold NUMBER            Set stable partial result threshold.
                                  Default value: 3
";
        
        private static string? GetCmdOption(string[] args, string option)
        {
            int index = Array.FindIndex(args, x => x.Equals(option, StringComparison.OrdinalIgnoreCase));
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
            return args.Contains (option, StringComparer.OrdinalIgnoreCase);
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

        private string GetTimestamp(DateTime startTime, DateTime endTime)
        {
            // SRT format requires ',' as decimal separator rather than '.'.
            return this._userConfig.useSubRipTextCaptionFormat
                ? $"{startTime:HH:mm:ss,fff} --> {endTime:HH:mm:ss,fff}"
                : $"{startTime:HH:mm:ss.fff} --> {endTime:HH:mm:ss.fff}";
        }

        private string? LanguageFromSpeechRecognitionResult(SpeechRecognitionResult result)
        {
            if (null != this._userConfig.languageIDLanguages)
            {
                var languageIDResult = AutoDetectSourceLanguageResult.FromResult(result);
                return languageIDResult.Language;
            }
            else
            {
                return null;
            }
        }

        private string CaptionFromTextAndTimes(string? language, string text, DateTime startTime, DateTime endTime)
        {
            var caption = new StringBuilder();
            
            if (this._userConfig.useSubRipTextCaptionFormat)
            {
                caption.AppendFormat($"{this._srtSequenceNumber}{Environment.NewLine}");
                this._srtSequenceNumber++;
            }
            caption.AppendFormat($"{GetTimestamp(startTime, endTime)}{Environment.NewLine}");
            if (null != language)
            {
                caption.Append($"[{language}] ");
            }
            caption.AppendFormat($"{text}{Environment.NewLine}{Environment.NewLine}");
            return caption.ToString();
        }

// JW 20220819 new code
        private string AdjustRealTimeCaption(string? language, Caption caption, bool isRecognizedResult)
        {
            string retval = null;
            
            // If the caption has fewer lines than maxCaptionLines, pad it with empty lines.
            // That way, captions start at (maxCaptionLines) from the bottom of the screen,
            // and progress toward the bottom of the screen, rather than start at the
            // bottom and be pushed upward.
            var captionLines = Regex.Matches(caption.Text, "\\n").Count + 1;
            for (int i = 0; i < this._userConfig.maxCaptionLines - captionLines; i++)
            {
                caption.Text += "\n&nbsp;";
            }
            
            // If the current caption has decreased in size, due to a change in a previous caption,
            // drop this caption and do not update the previous ending timestamp.
            // Always show Recognized results.
            if (_currentCaptionSize is null || caption.Text.Length >= _currentCaptionSize.Value || isRecognizedResult)
            {
                // By default, leave the starting timestamp unchanged.
                var beginTimeTicks = caption.Begin.Ticks;
                if (_previousCaptionEndTimeTicks is long previousCaptionEndTimeTicksValue)
                {
                    // If the starting timestamp is already later than the previous ending timestamp, leave it unchanged.
                    // Otherwise, move the starting timestamp forward to the previous ending timestamp.
                    beginTimeTicks = beginTimeTicks > previousCaptionEndTimeTicksValue ? beginTimeTicks : previousCaptionEndTimeTicksValue;
                }

                // If the starting timestamp is later than the ending timestamp,
                // drop this caption and do not update the previous ending timestamp or current caption size.
                if (beginTimeTicks < caption.End.Ticks)
                {
                    retval = CaptionFromTextAndTimes(language, caption.Text, new DateTime(beginTimeTicks), new DateTime(caption.End.Ticks));
                    // Record the ending timestamp so we can ensure the next caption starts after this one ends.
                    _previousCaptionEndTimeTicks = caption.End.Ticks;
                    // Record the current caption size so we can drop any subsequent current captions with smaller sizes.
                    _currentCaptionSize = caption.Text.Length;
                }
                // Always show Recognized results.
                else if (isRecognizedResult)
                {
                    var beginTime = new DateTime(beginTimeTicks);
                    // Set the ending timestamp to one second after the starting timestamp.
                    var endTime = beginTime.Add(TimeSpan.FromSeconds(1));
                    retval = CaptionFromTextAndTimes(language, caption.Text, beginTime, endTime);
                    // Record the ending timestamp and current caption size.
                    _previousCaptionEndTimeTicks = endTime.Ticks;
                    _currentCaptionSize = caption.Text.Length;
                }
            }
            
            return retval;
        }

        private List<string> CaptionFromSpeechRecognitionResult(SpeechRecognitionResult result, bool isRecognizedResult)
        {
            var retval = new List<string>();
            string? language = LanguageFromSpeechRecognitionResult(result);
            
            if (this._userConfig.maxCaptionLength is null)
            {
                var startTime = new DateTime(result.OffsetInTicks);
                DateTime endTime = startTime.Add(result.Duration);
                retval.Add(CaptionFromTextAndTimes(language, result.Text, startTime, endTime));
            }
            else
            {
                // Split the caption into multiple captions based on maxCaptionLength and maxCaptionLines.
                List<Caption> captions = this._captionHelper.GetCaptions(result, result.Text);
                
// TODO1 Debugging code to see all results from CaptionHelper.GetCaptions().
/*
                Console.WriteLine("*** DEBUG ***");
                Console.WriteLine(captions.Aggregate("", (result, caption) => result + CaptionFromTextAndTimes(language, caption.Text, new DateTime(caption.Begin.Ticks), new DateTime(caption.End.Ticks))));
                Console.WriteLine("*** END DEBUG ***");
                Console.WriteLine();
*/

                // Make sure CaptionHelper returned at least one caption.
                if (captions.Count > 0)
                {
                    // If we have a current number of captions...
                    if (_currentCaptionsLength is int currentCaptionsLengthValue)
                    {
                        // If the number of captions returned by CaptionHelper has decreased...
                        if (captions.Count < _currentCaptionsLength)
                        {
                            // Reset the current caption size.
                            _currentCaptionSize = null;
                        }
                        // If the number of captions returned by CaptionHelper has increased...
                        else if (captions.Count > _currentCaptionsLength && captions.Count > 1)
                        {
                            // Get the previous current caption.
                            var oldCaption = captions[captions.Count - 2];
                            // Show the previous current caption for an extra second, since we are about to replace it.
                            oldCaption.End = oldCaption.End.Add(TimeSpan.FromSeconds(1));
                            // Emit the previous current caption.
                            retval.Add(AdjustRealTimeCaption(language, oldCaption, isRecognizedResult));
                            
                            // Reset the current caption size.
                            _currentCaptionSize = null;
                        }
                    }

                    // Set the current number of captions.
                    _currentCaptionsLength = captions.Count;

                    // Get the current caption.
                    var caption = captions[captions.Count - 1];

                    if (isRecognizedResult)
                    {
                        // Show the current caption for an extra second, since it is a final (Recognized) result.
                        // TODO1 This should be configurable, but we don't want it confused with real-time delay.
                        caption.End = caption.End.Add(TimeSpan.FromSeconds(1));
                        retval.Add(AdjustRealTimeCaption(language, caption, isRecognizedResult));
                        // Reset the current caption size.
                        _currentCaptionSize = null;                        
                    }
                    else
                    {
                        retval.Add(AdjustRealTimeCaption(language, caption, isRecognizedResult));
                    }
                }
            }
            
            return retval;
        }
// JW 20220819 end new code

        private void WriteToConsole(string text)
        {
            if (!this._userConfig.suppressConsoleOutput)
            {
                Console.Write(text);
            }
        }

        private void WriteToConsoleOrFile(string text)
        {
            WriteToConsole(text);
            if (this._userConfig.outputFilePath is string outputFilePathValue)
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
            
            CaptioningMode captioningMode = CmdOptionExists(args, "--realTime") && !CmdOptionExists(args, "--offline") ? CaptioningMode.RealTime : CaptioningMode.Offline;
            
            string? strRealTimeDelay = GetCmdOption(args, "--realTimeDelay");
            int intRealTimeDelay = 0;
            if (null != strRealTimeDelay)
            {
                intRealTimeDelay = Int32.Parse(strRealTimeDelay);
                if (intRealTimeDelay < 0)
                {
                    intRealTimeDelay = 0;
                }
            }
            
            string? strMaxCaptionLength = GetCmdOption(args, "--maxCaptionLength");
            int? intMaxCaptionLength = null;
            if (null != strMaxCaptionLength)
            {
                intMaxCaptionLength = Int32.Parse(strMaxCaptionLength);
                if (intMaxCaptionLength < 20)
                {
                    intMaxCaptionLength = 20;
                }
            }
            
            string? strMaxCaptionLines = GetCmdOption(args, "--maxCaptionLines");
            int intMaxCaptionLines = 3;
            if (null != strMaxCaptionLines)
            {
                intMaxCaptionLines = Int32.Parse(strMaxCaptionLines);
                if (intMaxCaptionLines < 1)
                {
                    intMaxCaptionLines = 3;
                }
            }
            
            this._userConfig = new UserConfig(
                CmdOptionExists(args, "--format"),
                GetCompressedAudioFormat(args),
                GetProfanityOption(args),
                GetLanguageIDLanguages(args),
                GetCmdOption(args, "--input"),
                GetCmdOption(args, "--output"),
                GetCmdOption(args, "--phrases"),
                CmdOptionExists(args, "--quiet"),
                captioningMode,
                intRealTimeDelay,
                CmdOptionExists(args, "--srt"),
                intMaxCaptionLength,
                intMaxCaptionLines,
                GetCmdOption(args, "--threshold"),
                key,
                region
            );
        }

        private void Initialize()
        {
            if (this._userConfig.outputFilePath is string outputFilePathValue && File.Exists(outputFilePathValue))
            {
                File.Delete(outputFilePathValue);
            }
            if (!this._userConfig.useSubRipTextCaptionFormat)
            {
                WriteToConsoleOrFile($"WEBVTT{Environment.NewLine}{Environment.NewLine}");
            }
            
            if (null != this._userConfig.maxCaptionLength)
            {
                var language = "en";
                if (this._userConfig.languageIDLanguages is string[] languageIDLanguagesValue)
                {
                    // If the user specified languages, for now, we just use the first one.
                    // languageIDLanguages is actually locales (e.g. "en-US"), not languages,
                    // so just take the first two letters.
                    // TODO1 Consider moving language parameter to CaptionHelper.GetCaptions() so we can handle language on a per-caption basis.
                    // Use the result of LanguageFromSpeechRecognitionResult().
                    language = languageIDLanguagesValue[0].Substring(0, 2);
                }
                this._captionHelper = new CaptionHelper(language, this._userConfig.maxCaptionLength.Value, this._userConfig.maxCaptionLines, Enumerable.Empty<object>());
            }
        }

        //
        // Create AudioConfig
        //
        private AudioConfig AudioConfigFromUserConfig()
        {
            if (this._userConfig.inputFilePath is string inputFilePathValue)
            {
                if (!this._userConfig.useCompressedAudio)
                {
                    return Helper.OpenWavFile(inputFilePathValue, AudioProcessingOptions.Create(0));
                }
                else
                {
                    var reader = new BinaryReader(File.OpenRead(inputFilePathValue));
                    var format = AudioStreamFormat.GetCompressedFormat(this._userConfig.compressedAudioFormat);
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
            if (null != this._userConfig.languageIDLanguages)
            {
                speechConfig = SpeechConfig.FromEndpoint(new Uri(V2EndpointFromRegion(this._userConfig.region)), this._userConfig.subscriptionKey);
            }
            else
            {
                speechConfig = SpeechConfig.FromSubscription(this._userConfig.subscriptionKey, this._userConfig.region);
            }

            speechConfig.SetProfanity(this._userConfig.profanityOption);
            
            if (this._userConfig.stablePartialResultThreshold is string stablePartialResultThresholdValue)
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
            
            if (this._userConfig.languageIDLanguages is string[] languageIDLanguagesValue)
            {
                var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(languageIDLanguagesValue);
                speechRecognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, audioConfig);
            }
            else
            {
                speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);
            }
            
            if (this._userConfig.phraseList is string phraseListValue)
            {
                var grammar = PhraseListGrammar.FromRecognizer(speechRecognizer);
                foreach (var phrase in phraseListValue.Split(";")) {
                    grammar.AddPhrase(phrase);
                }
            }
            
            return speechRecognizer;
        }

        //
        // Convert audio to captions
        //
        private async Task<string?> RecognizeContinuous(SpeechRecognizer speechRecognizer)
        {
            if (CaptioningMode.RealTime == this._userConfig.captioningMode)
            {
                speechRecognizer.Recognizing += (object? sender, SpeechRecognitionEventArgs e) =>
                    {
                        if (ResultReason.RecognizingSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                        {
                            List<string> captions = CaptionFromSpeechRecognitionResult(e.Result, false);
                            captions.ForEach(caption => WriteToConsoleOrFile(caption));
                        }
                        else if (ResultReason.NoMatch == e.Result.Reason)
                        {
                            WriteToConsole($"NOMATCH: Speech could not be recognized.{Environment.NewLine}");
                        }
                    };
            }

            // We use Recognized results in both offline and real-time modes.
            speechRecognizer.Recognized += (object? sender, SpeechRecognitionEventArgs e) =>
                {
                    if (ResultReason.RecognizedSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                    {
                        List<string> captions = CaptionFromSpeechRecognitionResult(e.Result, true);
                        captions.ForEach(caption => WriteToConsoleOrFile(caption));
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
                        this._recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.CancelledByUser == e.Reason)
                    {
                        WriteToConsole($"User canceled request.{Environment.NewLine}");
                        this._recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.Error == e.Reason)
                    {
                        var error = $"Encountered error.{Environment.NewLine}Error code: {(int)e.ErrorCode}{Environment.NewLine}Error details: {e.ErrorDetails}{Environment.NewLine}";
                        this._recognitionEnd.TrySetResult(error); // Notify to stop recognition.
                    }
                    else
                    {
                        var error = $"Request was cancelled for an unrecognized reason: {(int)e.Reason}.{Environment.NewLine}";
                        this._recognitionEnd.TrySetResult(error); // Notify to stop recognition.
                    }
                };

            speechRecognizer.SessionStopped += (object? sender, SessionEventArgs e) =>
                {
                    WriteToConsole($"Session stopped.{Environment.NewLine}");
                    this._recognitionEnd.TrySetResult(null); // Notify to stop recognition.
                };

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            await speechRecognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

            // Waits for recognition end.
            Task.WaitAll(new[] { this._recognitionEnd.Task });

            // Stops recognition.
            await speechRecognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            
            return this._recognitionEnd.Task.Result;
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
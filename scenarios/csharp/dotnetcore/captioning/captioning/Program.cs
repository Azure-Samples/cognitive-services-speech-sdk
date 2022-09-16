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
        private CaptionHelper? _captionHelper;
        // TODO1
//        private int? _currentNumberOfCaptions = null;
//        private int? _currentCaptionSize = null;
        private Caption? _previousCaption = null;
        private long? _previousCaptionEndTimeTicks = null;
        // TODO1
        private List<string> _recognizedCaptions = new List<string>();
        private List<string> _recognizingCaptions = new List<string>();
        
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
    --realTimeDelay SECONDS       Extend display of Recognized (final) results by SECONDS.
                                  Valid only with --realTime.
                                  Minimum is 0.0. Default is 1.0.

  ACCURACY
    --phrases PHRASE1;PHRASE2     Example: ""Constoso;Jessie;Rehaan""

  OUTPUT
    --output FILE                 Output captions to text file.
    --srt                         Output captions in SubRip Text format (default format is WebVTT.)
    --maxCaptionLength LENGTH     Set the maximum number of characters per line for a caption to LENGTH.
                                  Minimum is 20. Default is no limit.
    --captionLines LINES          Set the number of lines for a caption to LINES.
                                  Valid only with --maxCaptionLength.
                                  Minimum is 1. Default is 2.
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

        private string GetTimestamp(TimeSpan startTime, TimeSpan endTime)
        {
            // TODO1 Verify we need to escape ,
            // SRT format requires ',' as decimal separator rather than '.'.
            return this._userConfig.useSubRipTextCaptionFormat
                ? $"{startTime:hh\\:mm\\:ss\\,fff} --> {endTime:hh\\:mm\\:ss\\,fff}"
                : $"{startTime:hh\\:mm\\:ss\\.fff} --> {endTime:hh\\:mm\\:ss\\.fff}";
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

// TODO1 Rename
        private string CaptionFromTextAndTimes(string? language, Caption caption)
        {
            var retval = new StringBuilder();
            
            if (this._userConfig.useSubRipTextCaptionFormat)
            {
                retval.AppendFormat($"{this._srtSequenceNumber}{Environment.NewLine}");
                this._srtSequenceNumber++;
            }
            retval.AppendFormat($"{GetTimestamp(caption.Begin, caption.End)}{Environment.NewLine}");
            if (language is string languageValue)
            {
                retval.Append($"[{languageValue}] ");
            }
            retval.AppendFormat($"{caption.Text}{Environment.NewLine}{Environment.NewLine}");
            return retval.ToString();
        }

// JW 20220819 new code

// Change this to return Caption?
        private Caption? AdjustRealTimeCaption(string? language, Caption caption, bool isRecognizedResult)
        {
            Caption? retval = null;
/*            
            // If the current caption has decreased in size, due to a change in a previous caption,
            // drop this caption and do not update the previous ending timestamp.
            // Always show Recognized results.
            if (_currentCaptionSize is null || caption.Text.Length >= _currentCaptionSize.Value || isRecognizedResult)
            {
*/
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
                    //retval = CaptionFromTextAndTimes(language, caption.Text, new DateTime(beginTimeTicks), new DateTime(caption.End.Ticks));
                    retval = new Caption() { Sequence = caption.Sequence, Begin = new TimeSpan(beginTimeTicks), End = caption.End, Text = caption.Text };
                    // Record the ending timestamp so we can ensure the next caption starts after this one ends.
                    _previousCaptionEndTimeTicks = caption.End.Ticks;
                    // Record the current caption size so we can drop any subsequent current captions with smaller sizes.
//                    _currentCaptionSize = caption.Text.Length;
                }
                // Always show Recognized results.
                else if (isRecognizedResult)
                {
                    var beginTime = new TimeSpan(beginTimeTicks);
                    // Set the ending timestamp after the starting timestamp.
                    var endTime = beginTime.Add(TimeSpan.FromSeconds(_userConfig.realTimeDelay));
                    //retval = CaptionFromTextAndTimes(language, caption.Text, beginTime, endTime);
                    retval = new Caption() { Sequence = caption.Sequence, Begin = beginTime, End = endTime, Text = caption.Text };
                    // Record the ending timestamp and current caption size.
                    _previousCaptionEndTimeTicks = endTime.Ticks;
//                    _currentCaptionSize = caption.Text.Length;
                }
/*
            }
*/
            return retval;
        }

// TODO1 Need to check offline or realTime and adjust behavior accordingly.
        private string? CaptionFromSpeechRecognitionResult(SpeechRecognitionResult result, bool isRecognizedResult)
        {
            //var retval = new List<string>();
            string? retval = null;
            string? language = LanguageFromSpeechRecognitionResult(result);
            
            if (this._userConfig.maxCaptionLength is null)
            {
                var startTime = new TimeSpan(result.OffsetInTicks);
                TimeSpan endTime = startTime.Add(result.Duration);
                // TODO1 No idea if sequence number is right.
                retval = CaptionFromTextAndTimes(language, new Caption(){ Sequence = _srtSequenceNumber, Begin = startTime, End = endTime, Text = result.Text });
            }
            else if (this._captionHelper is CaptionHelper captionHelper)
            {
                // Split the caption into multiple captions based on maxCaptionLength and captionLines.
                List<Caption> captions = captionHelper.GetCaptions(result, result.Text);
                
/*
                // TODO1 Won't repeated lines cause problems?
                
                // TODO1 Problem is if captions is < queue length. Maybe need to pad captions themselves out to queue length, then add? But then we might push out stuff we do not want to. I think we need to retain the existing code for determining when new captions arrive, then apply this logic.
                foreach (Caption item in captions)
                {
                    _currentCaptions.Enqueue(item.Text);
                }
                // TODO1 TEMP; use VTT cues instead.
                while (_currentCaptions.Count < _userConfig.captionLines)
                {
                    _currentCaptions.Enqueue("&nbsp;");
                }
                while (_currentCaptions.Count > _userConfig.captionLines)
                {
                    _currentCaptions.Dequeue();
                }

// TODO1 This will throw if count is 0.
// TODO1 Problem, getcaptions returns more than one result. Do we just want the last one?
                Caption caption = captions[captions.Count - 1];
                caption.Text = string.Join("\n", _currentCaptions.ToArray());
// TODO1 caption needs to be, what, _currentCaptions? The text from _currentCaptions?
                if (AdjustRealTimeCaption(language, caption, isRecognizedResult) is string adjustedCaption)
                {
                    retval.Add(adjustedCaption);
                }
*/
                
// TODO1 Debugging code to see all results from CaptionHelper.GetCaptions().
/*
                Console.WriteLine("*** DEBUG ***");
                Console.WriteLine(captions.Aggregate("", (result, caption) => result + CaptionFromTextAndTimes(language, caption.Text, new DateTime(caption.Begin.Ticks), new DateTime(caption.End.Ticks))));
                Console.WriteLine("*** END DEBUG ***");
                Console.WriteLine();
*/

// TODO1 What we are doing here is determining whether this is a new caption, namely if the number of captions from captionHelper has decreased. I think it is in THAT case we want to use the queue.
/*
                // Make sure CaptionHelper returned at least one caption.
                if (captions.Count > 0)
                {
                    // If we have a current number of captions...
                    if (_currentNumberOfCaptions is int currentNumberOfCaptionsValue)
                    {
                        // If the number of captions returned by CaptionHelper has decreased...
                        if (captions.Count < currentNumberOfCaptionsValue)
                        {
                            // Reset the current caption size.
                            _currentCaptionSize = null;
                        }
                        // If the number of captions returned by CaptionHelper has increased...
                        else if (captions.Count > currentNumberOfCaptionsValue && captions.Count > 1)
                        {
                            // Get the previous current caption.
                            var oldCaption = captions[captions.Count - 2];
                            // Show the previous current caption for an extra second, since we are about to replace it.
                            oldCaption.End = oldCaption.End.Add(TimeSpan.FromSeconds(1));
                            // Emit the previous current caption.
                            if (AdjustRealTimeCaption(language, oldCaption, isRecognizedResult) is string adjustedCaption)
                            {
                                retval.Add(adjustedCaption);
                            }
                            // Reset the current caption size.
                            _currentCaptionSize = null;
                        }
                    }

                    // Set the current number of captions.
                    _currentNumberOfCaptions = captions.Count;

                    // Get the current caption.
                    var caption = captions[captions.Count - 1];

// TODO1 Is this identical to captionslength decreasing due to new caption? I.e. can new caption only happen after recognized result?
// TODO1 Why is this not an else case of the above?
                    if (isRecognizedResult)
                    {
                        // Show the current caption for an extra second, since it is a final (Recognized) result.
                        // TODO1 This should be configurable, but we don't want it confused with real-time delay.
                        caption.End = caption.End.Add(TimeSpan.FromSeconds(1));
                        if (AdjustRealTimeCaption(language, caption, isRecognizedResult) is string adjustedCaption)
                        {
                            retval.Add(adjustedCaption);
                        }
                        // Reset the current caption size.
                        _currentCaptionSize = null;                        
                    }
                    else
                    {
                        if (AdjustRealTimeCaption(language, caption, isRecognizedResult) is string adjustedCaption)
                        {
                            retval.Add(adjustedCaption);
                        }
                    }
                }
            }
*/

// TODO1 Need to split each caption into lines, then put those into these lists.
// TODO1 Should do that in caption helper instead.

                // Make sure CaptionHelper returned at least one caption.
                if (captions.Count > 0)
                {
                    List<string> lines = captions.SelectMany(caption => caption.Text.Split("\n")).ToList();
                    _recognizingCaptions.Clear();
                    foreach (string line in lines)
                    {
                        if (isRecognizedResult)
                        {
                            _recognizedCaptions.Add(line);
                        }
                        else
                        {
                            _recognizingCaptions.Add(line);
                        }
                    }

                    List<string> lines_2 = _recognizingCaptions.TakeLast(_userConfig.captionLines).ToList();
                    
                    // TODO1 What did we do beforehand to get the timestamps? Same thing?
                    // TODO1 Note we have already verified captions.Count > 0.
                    var old_caption = captions.Last();
                    var caption_2 = new Caption()
                    {
                        // TODO1 Check this is right.
                        Sequence = old_caption.Sequence,
                        Begin = old_caption.Begin,
// TODO1
//                        End = isRecognizedResult ? old_caption.End.Add(TimeSpan.FromSeconds(_userConfig.realTimeDelay)) : old_caption.End,
// TODO1 Fix?
                        End = old_caption.End,
                        Text = "",
                    };
                    
                    if (AdjustRealTimeCaption(language, caption_2, isRecognizedResult) is Caption caption_3)
                    {
                        string text = lines_2.Aggregate((acc, item) => $"{acc}\n{item}");
                        
                        if (_previousCaption is Caption previousCaptionValue)
                        {
                            TimeSpan end;
                            if (caption_3.Begin.Subtract(previousCaptionValue.End) > TimeSpan.FromSeconds(_userConfig.realTimeDelay))
                            {
                                end = previousCaptionValue.End.Add(TimeSpan.FromSeconds(_userConfig.realTimeDelay));
                                // TODO1 Problem, this can prevent the recognized result from ever appearing in the file. But commenting it out does not work either.
                                _recognizedCaptions.Clear();
                            }
                            else
                            {
                                // TODO1 We could hold off on adding recognizedCaption lines until here, and also hold off on aggregating lines into Text until here? Or maybe after this if/else block.
                                int recognizedCaptionsIndex = _recognizedCaptions.Count - 1;
                                while (lines_2.Count < _userConfig.captionLines && recognizedCaptionsIndex >= 0)
                                {
                                    lines_2.Insert(0, _recognizedCaptions[recognizedCaptionsIndex]);
                                    recognizedCaptionsIndex--;
                                }
                                text = lines_2.Aggregate((acc, item) => $"{acc}\n{item}");    
                                end = caption_3.Begin;
                            }

                            Caption caption_4 = new Caption() { Sequence = previousCaptionValue.Sequence, Begin = previousCaptionValue.Begin, End = end, Text = previousCaptionValue.Text };
//                            if (AdjustRealTimeCaption(language, caption_4, isRecognizedResult) is Caption caption_5)
//                            {
                            retval = CaptionFromTextAndTimes(language, caption_4);
//                            }
                        }
                        
                        Caption caption_5 = new Caption(){ Sequence = caption_3.Sequence, Begin = caption_3.Begin, End = caption_3.End, Text = text };
                        _previousCaption = caption_5;
                    }
                    

                    
// TODO1 Need to handle final caption.
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
            double dblRealTimeDelay = 1.0;
            if (null != strRealTimeDelay)
            {
                dblRealTimeDelay = Double.Parse(strRealTimeDelay);
                if (dblRealTimeDelay < 0.0)
                {
                    dblRealTimeDelay = 1.0;
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
            
            string? strCaptionLines = GetCmdOption(args, "--captionLines");
            int intCaptionLines = 2;
            if (null != strCaptionLines)
            {
                intCaptionLines = Int32.Parse(strCaptionLines);
                if (intCaptionLines < 1)
                {
                    intCaptionLines = 2;
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
                dblRealTimeDelay,
                CmdOptionExists(args, "--srt"),
                intMaxCaptionLength,
                intCaptionLines,
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
                this._captionHelper = new CaptionHelper(language, this._userConfig.maxCaptionLength.Value, this._userConfig.captionLines, Enumerable.Empty<object>());
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
                            if (CaptionFromSpeechRecognitionResult(e.Result, false) is string caption)
                            {
                                WriteToConsoleOrFile(caption);
                            }
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
                        if (CaptionFromSpeechRecognitionResult(e.Result, true) is string caption)
                        {
                            WriteToConsoleOrFile(caption);
                        }
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

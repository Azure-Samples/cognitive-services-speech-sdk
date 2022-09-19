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
        private UserConfig? _userConfig;
        private int _srtSequenceNumber = 0;
        private Caption? _previousCaption;
        private bool _previousResultIsRecognized = false;
        private List<string> _recognizedLines = new List<string>();
        private List<RecognitionResult> _offlineResults = new List<RecognitionResult>();
        
        private static string V2EndpointFromRegion(string region)
        {
            // Note: Continuous language identification is supported only with v2 endpoints.
            return $"wss://{region}.stt.speech.microsoft.com/speech/universal/v2";
        }

        private void WriteToConsole(string text)
        {
            if (!this._userConfig!.suppressConsoleOutput)
            {
                Console.Write(text);
            }
        }

        private void WriteToConsoleOrFile(string text)
        {
            WriteToConsole(text);
            if (this._userConfig!.outputFilePath is string outputFilePathValue)
            {
                File.AppendAllText(outputFilePathValue, text);
            }
        }

        private string GetTimestamp(TimeSpan startTime, TimeSpan endTime)
        {
            // SRT format requires ',' as decimal separator rather than '.'.
            return this._userConfig!.useSubRipTextCaptionFormat
                ? $"{startTime:hh\\:mm\\:ss\\,fff} --> {endTime:hh\\:mm\\:ss\\,fff}"
                : $"{startTime:hh\\:mm\\:ss\\.fff} --> {endTime:hh\\:mm\\:ss\\.fff}";
        }

/* TODO1 This is not currently used.
Passing result to CaptionHelper causes GetTextOrTranslation to return nothing.
GetTextOrTranslation looks for result is TranslationRecognitionResult translated && translated.Translations.Keys.Contains(_language).
That's not happening because we don't specify languages to the speech recognizer. Default languageIDLanguages is empty list, not en-US.

Result looks like this:
ResultId:42da47f298334ab6aa393a9b2e0b50cd Reason:RecognizedSpeech Recognized text:<One more with six seconds.>. Json:{"Id":"42da47f298334ab6aa393a9b2e0b50cd","RecognitionStatus":"Success","DisplayText":"One more with six seconds.","Offset":25800000,"Duration":15900000}
*/
        private string GetLanguageForCaptionHelper()
        {
            var language = "en-US";
            if (this._userConfig!.languageIDLanguages is string[] languageIDLanguagesValue)
            {
                // If the user specified languages, for now, we just use the first one.
                // TODO1 Talk to Rob and Heiko about this. Should CaptionHelper get the language from each result?
                language = languageIDLanguagesValue[0];
            }
            return language;
        }

        private string? LanguageFromSpeechRecognitionResult(SpeechRecognitionResult result)
        {
            string? retval = null;
            
            if (null != this._userConfig!.languageIDLanguages)
            {
                var languageIDResult = AutoDetectSourceLanguageResult.FromResult(result);
                retval = languageIDResult.Language;
            }

            return retval;
        }

        private string StringFromCaption(string? language, Caption caption)
        {
            var retval = new StringBuilder();
            
            if (this._userConfig!.useSubRipTextCaptionFormat)
            {
                retval.AppendFormat($"{caption.Sequence}{Environment.NewLine}");
            }
            retval.AppendFormat($"{GetTimestamp(caption.Begin, caption.End)}{Environment.NewLine}");
            // TODO1 I think Heiko wanted to remove this. I.e., only have language specified for entire transcript, not per result.
            if (language is string languageValue)
            {
                retval.Append($"[{languageValue}] ");
            }
            retval.AppendFormat($"{caption.Text}{Environment.NewLine}{Environment.NewLine}");
            return retval.ToString();
        }

        private string AdjustRealTimeCaptionText(string text, bool isRecognizedResult)
        {
            // Split the caption text into multiple lines based on maxLineLength and lines.
            var captionHelper = new CaptionHelper(null, this._userConfig!.maxLineLength, this._userConfig!.lines, Enumerable.Empty<object>());
            List<string> lines = captionHelper.LinesFromText(text);

            // Recognizing results can change with each new result, so we do not save previous Recognizing results.
            // Recognized results are final, so we save them in a member value.
            var recognizingLines = new List<string>();
            if (isRecognizedResult)
            {
                _recognizedLines = _recognizedLines.Concat(lines).ToList();
            }
            else
            {
                recognizingLines = lines.TakeLast(_userConfig!.lines).ToList();
            }
            
            // If the Recognizing result has fewer lines than _userConfig.lines,
            // we insert lines from our saved Recognized results.
            int recognizedLinesToAdd = Math.Max(0, _userConfig!.lines - recognizingLines.Count());
            recognizingLines = _recognizedLines.TakeLast(recognizedLinesToAdd).Concat(recognizingLines).ToList();
            return string.Join('\n', recognizingLines.ToArray());
        }

        private string? CaptionFromRealTimeResult(SpeechRecognitionResult result, bool isRecognizedResult)
        {
            string? retval = null;
            string? language = LanguageFromSpeechRecognitionResult(result);

            // Convert the SpeechRecognitionResult to a caption.
            var startTime = new TimeSpan(result.OffsetInTicks).Add(this._userConfig!.delay);
            Caption caption = new Caption()
            {
                Sequence = _srtSequenceNumber++,
                Begin = startTime,
                End = startTime.Add(result.Duration),
                // We are not ready to set the text for this caption.
                // First we need to determine whether to clear _recognizedLines.
                Text = ""
            };

            // If we have a previous caption...
            if (_previousCaption is Caption previousCaption)
            {
                // If the previous result was type Recognized...
                if (_previousResultIsRecognized)
                {
                    // Set the end timestamp for the previous caption to the earliest of:
                    // - The end timestamp for the previous caption plus the remain time.
                    // - The start timestamp for the current caption.
                    TimeSpan previousEnd = previousCaption.End.Add(_userConfig!.remainTime);
                    previousCaption.End = previousEnd < caption.Begin ? previousEnd : caption.Begin;
                    // If the gap between the original end timestamp for the previous caption
                    // and the start timestamp for the current caption is larger than remainTime,
                    // clear the cached recognized lines.
                    // Note this needs to be done before we call AdjustRealTimeCaptionText
                    // for the current caption, because it uses _recognizedLines.
                    if (previousEnd < caption.Begin)
                    {
                        _recognizedLines.Clear();
                    }
                }
                // If the previous result was type Recognizing, simply set the start timestamp
                // for the current caption to the end timestamp for the previous caption.
                // Note this presumes there will not be a large gap between Recognizing results,
                // because such a gap would cause the previous Recognizing result to be succeeded
                // by a Recognized result.
                else
                {
                    caption.Begin = previousCaption.End;
                }

                // If the start timestamp is later than the end timestamp, drop the caption.
                // This sometimes happens when we receive a lot of Recognizing results close together.
                if (previousCaption.Begin < previousCaption.End)
                {
                    retval = StringFromCaption(language, previousCaption);
                }
            }

            // Break the caption text into lines if needed.
            caption.Text = AdjustRealTimeCaptionText(result.Text, isRecognizedResult);
            // Save the current caption as the previous caption.
            _previousCaption = caption;
            // Save the result type as the previous result type.
            _previousResultIsRecognized = isRecognizedResult;

            return retval;
        }

        private IEnumerable<Caption> CaptionsFromOfflineResults()
        {
            // Split the caption text into multiple lines based on maxLineLength and lines.
            IEnumerable<Caption> captions = CaptionHelper.GetCaptions(null, this._userConfig!.maxLineLength, this._userConfig!.lines, _offlineResults);
            // Save the last caption.
            Caption lastCaption = captions.Last();
            lastCaption.End.Add(this._userConfig!.remainTime);
            // In offline mode, all captions come from RecognitionResults of type Recognized.
            // Set the end timestamp for each caption to the earliest of:
            // - The end timestamp for this caption plus the remain time.
            // - The start timestamp for the next caption.            
            List<Caption> captions_2 = captions
                .Pairwise()
                .Select(captions =>
                {
                    TimeSpan end = captions.Item1.End.Add(_userConfig!.remainTime);
                    captions.Item1.End = end < captions.Item2.Begin ? end : captions.Item2.Begin;
                    return captions.Item1;
                })
                .ToList();
            // Re-add the last caption.
            captions_2.Add(lastCaption);
            return captions_2;
        }

        private void Finish()
        {
            if (CaptioningMode.Offline == this._userConfig!.captioningMode)
            {
                foreach (Caption caption in CaptionsFromOfflineResults())
                {
                    // TODO1 Fix language. We might be able to remove lang parameter from StringFromCaption.
                    WriteToConsoleOrFile(StringFromCaption(null, caption));
                }
            }
            else if (CaptioningMode.RealTime == this._userConfig!.captioningMode)
            {
                // Show the last "previous" caption, which is actually the last caption.
                if (_previousCaption is Caption previousCaptionValue)
                {
                    previousCaptionValue.End.Add(this._userConfig!.remainTime);
                    // TODO1 Fix language. We might be able to remove lang parameter from StringFromCaption.
                    WriteToConsoleOrFile(StringFromCaption(null, previousCaptionValue));
                }
            }
        }

        //
        // Create UserConfig from command-line arguments.
        // Clear output file if needed.
        // Write WebVTT header if needed.
        //
        private void Initialize(string[] args, string usage)
        {
            this._userConfig = UserConfig.UserConfigFromArgs(args, usage);

            if (this._userConfig!.outputFilePath is string outputFilePathValue && File.Exists(outputFilePathValue))
            {
                File.Delete(outputFilePathValue);
            }
            if (!this._userConfig!.useSubRipTextCaptionFormat)
            {
                WriteToConsoleOrFile($"WEBVTT{Environment.NewLine}{Environment.NewLine}");
            }
        }

        //
        // Create AudioConfig
        //
        private AudioConfig AudioConfigFromUserConfig()
        {
            if (this._userConfig!.inputFilePath is string inputFilePathValue)
            {
                if (!this._userConfig!.useCompressedAudio)
                {
                    return Helper.OpenWavFile(inputFilePathValue, AudioProcessingOptions.Create(0));
                }
                else
                {
                    var reader = new BinaryReader(File.OpenRead(inputFilePathValue));
                    var format = AudioStreamFormat.GetCompressedFormat(this._userConfig!.compressedAudioFormat);
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
            if (null != this._userConfig!.languageIDLanguages)
            {
                speechConfig = SpeechConfig.FromEndpoint(new Uri(V2EndpointFromRegion(this._userConfig!.region)), this._userConfig!.subscriptionKey);
            }
            else
            {
                speechConfig = SpeechConfig.FromSubscription(this._userConfig!.subscriptionKey, this._userConfig!.region);
            }

            speechConfig.SetProfanity(this._userConfig!.profanityOption);
            
            if (this._userConfig!.stablePartialResultThreshold is string stablePartialResultThresholdValue)
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
            
            if (this._userConfig!.languageIDLanguages is string[] languageIDLanguagesValue)
            {
                var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(languageIDLanguagesValue);
                speechRecognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, audioConfig);
            }
            else
            {
                speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);
            }
            
            if (this._userConfig!.phraseList is string phraseListValue)
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
            var stopRecognition = new TaskCompletionSource<string?>();
            
            // We only use Recognizing results in real-time mode.
            if (CaptioningMode.RealTime == this._userConfig!.captioningMode)
            {
                speechRecognizer.Recognizing += (object? sender, SpeechRecognitionEventArgs e) =>
                    {
                        if (ResultReason.RecognizingSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                        {
                            if (CaptionFromRealTimeResult(e.Result, false) is string caption)
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
                        if (CaptioningMode.Offline == this._userConfig!.captioningMode)
                        {
                            _offlineResults.Add(e.Result);
                        }
                        else if (CaptionFromRealTimeResult(e.Result, true) is string caption)
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
                        stopRecognition.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.CancelledByUser == e.Reason)
                    {
                        WriteToConsole($"User canceled request.{Environment.NewLine}");
                        stopRecognition.TrySetResult(null); // Notify to stop recognition.
                    }
                    else if (CancellationReason.Error == e.Reason)
                    {
                        var error = $"Encountered error.{Environment.NewLine}Error code: {(int)e.ErrorCode}{Environment.NewLine}Error details: {e.ErrorDetails}{Environment.NewLine}";
                        stopRecognition.TrySetResult(error); // Notify to stop recognition.
                    }
                    else
                    {
                        var error = $"Request was cancelled for an unrecognized reason: {(int)e.Reason}.{Environment.NewLine}";
                        stopRecognition.TrySetResult(error); // Notify to stop recognition.
                    }
                };

            speechRecognizer.SessionStopped += (object? sender, SessionEventArgs e) =>
                {
                    WriteToConsole($"Session stopped.{Environment.NewLine}");
                    stopRecognition.TrySetResult(null); // Notify to stop recognition.
                };

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            await speechRecognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

            // Waits for completion.
            // Use Task.WaitAny to keep the task rooted.
            Task.WaitAny(new[] { stopRecognition.Task });

            // Stops recognition.
            await speechRecognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            
            return stopRecognition.Task.Result;
        }

        // Note: To pass command-line arguments, run:
        // dotnet run -- [args]
        // For example:
        // dotnet run -- --help
        public static void Main(string[] args)
        {
            const string usage = @"USAGE: dotnet run -- [...]

  HELP
    --help                           Show this help and stop.

  CONNECTION
    --key KEY                        Your Azure Speech service subscription key.
    --region REGION                  Your Azure Speech service region.
                                     Examples: westus, eastus

  LANGUAGE
    --languages LANG1;LANG2          Enable language identification for specified languages.
                                     Example: en-US;ja-JP

  INPUT
    --input FILE                     Input audio from file (default input is the microphone.)
    --format FORMAT                  Use compressed audio format.
                                     If this is not present, uncompressed format (wav) is assumed.
                                     Valid only with --file.
                                     Valid values: alaw, any, flac, mp3, mulaw, ogg_opus

  MODE
    --offline                        Output offline results.
                                     Overrides --realTime.
    --realTime                       Output real-time results.
                                     Default output mode is offline.

  ACCURACY
    --phrases ""PHRASE1;PHRASE2""    Example: ""Constoso;Jessie;Rehaan""

  OUTPUT
    --output FILE                    Output captions to FILE.
    --srt                            Output captions in SubRip Text format (default format is WebVTT.)
    --maxLineLength LENGTH           Set the maximum number of characters per line for a caption to LENGTH.
                                     Minimum is 20. Default is 37.
    --lines LINES                    Set the number of lines for a caption to LINES.
                                     Minimum is 1. Default is 2.
    --delay SECONDS                  How many SECONDS to delay the appearance of each caption.
                                     Minimum is 0.0. Default is 1.0.
    --remainTime SECONDS             How many SECONDS a caption should remain on screen if it is not replaced by another.
                                     Minimum is 0.0. Default is 1.0.
    --quiet                          Suppress console output, except errors.
    --profanity OPTION               Valid values: raw, remove, mask
    --threshold NUMBER               Set stable partial result threshold.
                                     Default is 3.
";

            if (args.Contains("--help"))
            {
                Console.WriteLine(usage);
            }
            else
            {
                Program program = new Program();
                program.Initialize(args, usage);
                SpeechRecognizer speechRecognizer = program.SpeechRecognizerFromUserConfig();
                if (program.RecognizeContinuous(speechRecognizer).Result is string error)
                {
                    Console.WriteLine(error);
                }
                else
                {
                    program.Finish();
                }
            }
        }
    }
}

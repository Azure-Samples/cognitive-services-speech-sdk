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
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace Captioning
{
    struct CaptionChunk
    {
        public string? language;
        public int startIndex;
        public string text;
        public DateTime startTime;
        public DateTime endTime;
        
        public CaptionChunk(string? language, int startIndex, string text, DateTime startTime, DateTime endTime) {
            this.language = language;
            this.startIndex = startIndex;
            this.text = text;
            this.startTime = startTime;
            this.endTime = endTime;
        }
    }
    
    class Program
    {
        private UserConfig userConfig;
        private TaskCompletionSource<string?> recognitionEnd = new TaskCompletionSource<string?>();
        private int srtSequenceNumber = 0;
        // If the user specifies --recognizing and --maxCaptionLength, we need to break Recognizing results into chunks.
        private CaptionChunk? currentCaptionChunk;
        
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

  RECOGNITION
    --recognizing                 Output Recognizing results (default output is Recognized results only.)
    --recognized                  Output Recognized results.
                                  Default is true, unless --recognizing is true, then default is false.

  ACCURACY
    --phrases PHRASE1;PHRASE2     Example: Constoso;Jessie;Rehaan

  OUTPUT
    --output FILE                 Output captions to text file.
    --srt                         Output captions in SubRip Text format (default format is WebVTT.)
    --maxCaptionLength LENGTH     Break up captions longer than LENGTH.
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

        private string GetTimestamp(DateTime startTime, DateTime endTime)
        {
            // SRT format requires ',' as decimal separator rather than '.'.
            return this.userConfig.useSubRipTextCaptionFormat
                ? $"{startTime:HH:mm:ss,fff} --> {endTime:HH:mm:ss,fff}"
                : $"{startTime:HH:mm:ss.fff} --> {endTime:HH:mm:ss.fff}";
        }

        private string? LanguageFromSpeechRecognitionResult(SpeechRecognitionResult result)
        {
            if (null != this.userConfig.languageIDLanguages)
            {
                var languageIDResult = AutoDetectSourceLanguageResult.FromResult(result);
                return languageIDResult.Language;
            }
            else
            {
                return null;
            }
        }

        private List<List<WordLevelTimingResult>> ChunksFromWordLevelTimingResults(List<WordLevelTimingResult> results)
        {
            var chunks = new List<List<WordLevelTimingResult>>();
            var currentChunk = new List<WordLevelTimingResult>();
            var currentChunkLength = 0;
            foreach(var result in results)
            {
                // If the current word, preceded by a space, makes the current chunk too long...
                if (currentChunkLength + result.Word.Length + 1 > userConfig.maxCaptionLength)
                {
                    // Add the current chunk to the results.
                    chunks.Add(currentChunk);
                    // Start a new chunk and add the current word.
                    currentChunk = new List<WordLevelTimingResult>();
                    currentChunk.Add(result);
                    currentChunkLength = result.Word.Length;
// TODO1 If we get a word whose length alone > userConfig.maxCaptionLength, throw? Tell user they set limit too low. Or maybe just let the word through?
                }
                else
                {
                    // Add the current word to the current chunk. Account for the preceding space.
                    currentChunk.Add(result);
                    currentChunkLength += result.Word.Length + 1;
                }
            }
            // Add the last chunk to the results.
            chunks.Add(currentChunk);
            return chunks;
        }

        private (string text, DateTime startTime, DateTime endTime) TextAndTimesFromChunk(List<WordLevelTimingResult> chunk)
        {
            var text = new StringBuilder();
            var endTime = chunk[0].Offset;
            foreach (var word in chunk)
            {
                text.AppendFormat($"{word.Word} ");
                endTime += word.Duration;
            }
            return (text.ToString(), new DateTime(chunk[0].Offset), new DateTime(endTime));
        }

        private string CaptionFromTextAndTimes(string? language, string text, DateTime startTime, DateTime endTime)
        {
            var caption = new StringBuilder();
            
            if (this.userConfig.useSubRipTextCaptionFormat)
            {
                caption.AppendFormat($"{srtSequenceNumber}{Environment.NewLine}");
                srtSequenceNumber++;
            }
            caption.AppendFormat($"{GetTimestamp(startTime, endTime)}{Environment.NewLine}");
            if (null != language)
            {
                caption.Append($"[{language}] ");
            }
            caption.AppendFormat($"{text}{Environment.NewLine}{Environment.NewLine}");
            return caption.ToString();
        }

        private string SplitRecognizedResult(SpeechRecognitionResult result)
        {
            string? language = LanguageFromSpeechRecognitionResult(result);
            
            var detailedResults = result.Best();
            // Notes:
            // You must set speechConfig.RequestWordLevelTimestamps() to get word-level timestamps.
            // Word-level timestamps are only available for Recognized results.
            if(detailedResults != null && detailedResults.Any())
            {
                // The first item in detailedResults corresponds to the recognized text.
                // This is not necessarily the item with the highest confidence number.
                var words = detailedResults.ToList()[0].Words.ToList();
                
                List<List<WordLevelTimingResult>> chunks = ChunksFromWordLevelTimingResults(words);
                var caption = new StringBuilder();
                foreach(var chunk in chunks)
                {
                    var (text, startTime, endTime) = TextAndTimesFromChunk(chunk);
                    caption.Append(CaptionFromTextAndTimes(language, text, startTime, endTime));
                }
                return caption.ToString();
            }
            else
            {
                recognitionEnd.TrySetResult("Error: SpeechRecognitionResult missing detailed results. To get detailed results, you must call SpeechConfig.RequestWordLevelTimestamps()."); // Notify to stop recognition.
                return "";
            }
        }

        private string CaptionFromSpeechRecognitionResult(SpeechRecognitionResult result)
        {
            string? language = LanguageFromSpeechRecognitionResult(result);
            var startTime = new DateTime(result.OffsetInTicks);
            DateTime endTime = startTime.Add(result.Duration);
            return CaptionFromTextAndTimes(language, result.Text, startTime, endTime);
        }

        private string CaptionFromRecognizedSpeechEventResult(SpeechRecognitionResult result)
        {
            if (null != userConfig.maxCaptionLength && result.Text.Length > userConfig.maxCaptionLength)
            {
                return SplitRecognizedResult(result);
            }
            else
            {
                return CaptionFromSpeechRecognitionResult(result);
            }
        }

        private string SplitRecognizingResult(SpeechRecognitionResult result)
        {
            // If the entire text of the Recognizing result is within the max caption length, simply create or update the current caption chunk.
            if (result.Text.Length <= this.userConfig.maxCaptionLength)
            {
                currentCaptionChunk = new CaptionChunk (LanguageFromSpeechRecognitionResult(result), 0, result.Text, new DateTime(result.OffsetInTicks), new DateTime(result.OffsetInTicks).Add(result.Duration));
                return null;
            }
            else
            {
                // If the entire text of the Recognizing result exceeds the max caption length, and we have not yet created a current caption chunk,
                // then there was never a Recognizing result that was within the max caption length, which means the max caption length is too short.
                if (this.currentCaptionChunk is null)
                {
                    recognitionEnd.TrySetResult("Error: You may have set --maxCaptionLength to too short a length."); // Notify to stop recognition.
                    return null;
                }
                else
                {
                    // Get the text for what will become the current caption chunk.
                    var text = result.Text.Substring(this.currentCaptionChunk.Value.startIndex);
                
                    // Does the current caption chunk now exceed the max caption length?
                    if (text.Length > this.userConfig.maxCaptionLength)
                    {
                        // Show the current caption chunk.
                        var caption = CaptionFromTextAndTimes(this.currentCaptionChunk.Value.language, this.currentCaptionChunk.Value.text, this.currentCaptionChunk.Value.startTime, this.currentCaptionChunk.Value.endTime);

                        // TODO1 Break on any non-word character, not just space.
                        // Move the starting index for the current caption chunk forward to one character after
                        // the last space in the old caption chunk.
                        var startIndex = text.Substring(0, this.userConfig.maxCaptionLength.Value).LastIndexOf(" ") + 1;
                        // Get the text for the new current caption chunk using the new starting index.
                        text = text.Substring(startIndex);
                        // Update the current caption chunk. Add the new starting index to the old starting index.
                        // Note the starting index applies to the entire Recognizing result, not the current caption chunk.
                        // Set the new starting timestamp to one millisecond after the old ending timestamp.
                        this.currentCaptionChunk = new CaptionChunk (this.currentCaptionChunk.Value.language, this.currentCaptionChunk.Value.startIndex + startIndex, text, this.currentCaptionChunk.Value.endTime.Add(TimeSpan.FromMilliseconds(1)), new DateTime(result.OffsetInTicks).Add(result.Duration));
                        
                        return caption;
                    }
                    else
                    {
                        // If the current caption chunk is still within the max caption length,
                        // simply update the current caption chunk with the new text and ending timestamp.
                        this.currentCaptionChunk = new CaptionChunk (this.currentCaptionChunk.Value.language, this.currentCaptionChunk.Value.startIndex, text, this.currentCaptionChunk.Value.startTime, new DateTime(result.OffsetInTicks).Add(result.Duration));
                        return null;
                    }
                }                
            }
        }

        private string? CaptionFromRecognizingSpeechEventResult(SpeechRecognitionResult result)
        {
            if (this.userConfig.maxCaptionLength is null)
            {
                return CaptionFromSpeechRecognitionResult(result);
            }
            else
            {
                return SplitRecognizingResult(result);
            }
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
            
            string? strMaxCaptionLength = GetCmdOption(args, "--maxCaptionLength");
            int? intMaxCaptionLength = null;
            if (null != strMaxCaptionLength)
            {
                intMaxCaptionLength = Int32.Parse(strMaxCaptionLength);
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
                CmdOptionExists(args, "--recognized"),
                CmdOptionExists(args, "--recognizing"),
                CmdOptionExists(args, "--srt"),
                intMaxCaptionLength,
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
                WriteToConsoleOrFile($"WEBVTT{Environment.NewLine}{Environment.NewLine}");
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
            if (null != this.userConfig.languageIDLanguages)
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
            
            speechConfig.RequestWordLevelTimestamps();
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
            if (this.userConfig.showRecognizingResults)
            {
                speechRecognizer.Recognizing += (object? sender, SpeechRecognitionEventArgs e) =>
                    {
                        if (ResultReason.RecognizingSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                        {
                            var caption = CaptionFromRecognizingSpeechEventResult(e.Result);
                            if (null != caption)
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

            speechRecognizer.Recognized += (object? sender, SpeechRecognitionEventArgs e) =>
                {
                    // If the user specifies --recognizing and --maxCaptionLength, then a Recognized event
                    // means we need to show the current caption chunk, if any, then clear it.
                    if (this.userConfig.showRecognizingResults && null != this.currentCaptionChunk)
                    {
                        WriteToConsoleOrFile(CaptionFromTextAndTimes(this.currentCaptionChunk.Value.language, this.currentCaptionChunk.Value.text, this.currentCaptionChunk.Value.startTime, this.currentCaptionChunk.Value.endTime));
                        currentCaptionChunk = null;
                    }
                    // If the user does not specify --recognizing, then we show Recognized results by default.
                    // If the user does specify --recognizing, then we show Recognized results only if the user also specifies --recognized.
                    if (!this.userConfig.showRecognizingResults || this.userConfig.showRecognizedResults)
                    {
                        if (ResultReason.RecognizedSpeech == e.Result.Reason && e.Result.Text.Length > 0)
                        {
                            WriteToConsoleOrFile(CaptionFromRecognizedSpeechEventResult(e.Result));
                        }
                        else if (ResultReason.NoMatch == e.Result.Reason)
                        {
                            WriteToConsole($"NOMATCH: Speech could not be recognized.{Environment.NewLine}");
                        }
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

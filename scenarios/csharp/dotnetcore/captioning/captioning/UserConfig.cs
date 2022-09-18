//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Linq;

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace Captioning
{
	public enum CaptioningMode
	{
    	Offline,
    	RealTime
	}
	
    public class UserConfig
    {
        /// True to use a compressed audio format; otherwise, use uncompressed (wav).
        readonly public bool useCompressedAudio = false;
        /// Compressed audio format for input audio. Default is Any.
        readonly public AudioStreamContainerFormat compressedAudioFormat = AudioStreamContainerFormat.ANY;
        /// Whether to show, remove, or mask profanity. Default is to mask it.
        readonly public ProfanityOption profanityOption = ProfanityOption.Masked;
        /// Enable language identification for these languages.
        /// Languages must be delimited by commas.
        /// Example: en-US,ja-JP
        readonly public string[]? languageIDLanguages;
        /// Input audio file path. Default input is the microphone.
        readonly public string? inputFilePath;
        /// Output file path. Default output is the console.
        readonly public string? outputFilePath;
        /// Predefined phrases to help the Speech service interpret the provided audio.
        /// Phrases must be delimited by semicolons.
        /// Example: "Constoso;Jessie;Rehaan"
        readonly public string? phraseList;
        /// True to suppress console output (except errors).
        readonly public bool suppressConsoleOutput = false;
        /// The captioning mode. Default is offline.
        readonly public CaptioningMode captioningMode = CaptioningMode.Offline;
        /// How long (in seconds) a caption should remain on screen. Default is 1.0.
        readonly public TimeSpan remainTime;
        /// Output captions in SubRip Text format (default is WebVTT format).
        readonly public bool useSubRipTextCaptionFormat = false;
        /// The maximum number of characters per line for a caption. Default is no limit.
        readonly public int? maxLineLength;
        /// How many lines of captioning to show. Only valid with maxLineLength. Default is 2.
        readonly public int lines;
        /// Set the stable partial result threshold on the Speech service. This setting value must contain an integer.
        /// Example: 3
        readonly public string? stablePartialResultThreshold;
        /// The subscription key for your Speech service subscription.
        readonly public string subscriptionKey;
        /// The region for your Speech service subscription.
        readonly public string region;
        
        public UserConfig (
            bool useCompressedAudio,
            AudioStreamContainerFormat compressedAudioFormat,
            ProfanityOption profanityOption,
            string[]? languageIDLanguages,
            string? inputFilePath,
            string? outputFilePath,
            string? phraseList,
            bool suppressConsoleOutput,
            CaptioningMode captioningMode,
            TimeSpan remainTime,
            bool useSubRipTextCaptionFormat,
            int? maxLineLength,
            int lines,
            string? stablePartialResultThreshold,
            string subscriptionKey,
            string region
            )
        {
            this.useCompressedAudio = useCompressedAudio;
            this.compressedAudioFormat = compressedAudioFormat;
            this.profanityOption = profanityOption;
            this.languageIDLanguages = languageIDLanguages;
            this.inputFilePath = inputFilePath;
            this.outputFilePath = outputFilePath;
            this.phraseList = phraseList;
            this.suppressConsoleOutput = suppressConsoleOutput;
            this.captioningMode = captioningMode;
            this.remainTime = remainTime;
            this.useSubRipTextCaptionFormat = useSubRipTextCaptionFormat;
            this.maxLineLength = maxLineLength;
            this.lines = lines;
            this.stablePartialResultThreshold = stablePartialResultThreshold;
            this.subscriptionKey = subscriptionKey;
            this.region = region;
        }

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
        
        public static UserConfig UserConfigFromArgs(string[] args, string usage)
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
            
            string? strRemainTime = GetCmdOption(args, "--remainTime");
            TimeSpan timeSpanRemainTime = TimeSpan.FromSeconds(1.0);
            if (null != strRemainTime)
            {
                double dblRemainTime = Double.Parse(strRemainTime);
                if (dblRemainTime < 0.0)
                {
                    dblRemainTime = 1.0;
                }
                timeSpanRemainTime = TimeSpan.FromSeconds(dblRemainTime);
            }
            
            string? strMaxLineLength = GetCmdOption(args, "--maxLineLength");
            int? intMaxLineLength = null;
            if (null != strMaxLineLength)
            {
                intMaxLineLength = Int32.Parse(strMaxLineLength);
                if (intMaxLineLength < 20)
                {
                    intMaxLineLength = 20;
                }
            }
            
            string? strLines = GetCmdOption(args, "--lines");
            int intLines = 2;
            if (null != strLines)
            {
                intLines = Int32.Parse(strLines);
                if (intLines < 1)
                {
                    intLines = 2;
                }
            }
            
            return new UserConfig(
                CmdOptionExists(args, "--format"),
                GetCompressedAudioFormat(args),
                GetProfanityOption(args),
                GetLanguageIDLanguages(args),
                GetCmdOption(args, "--input"),
                GetCmdOption(args, "--output"),
                GetCmdOption(args, "--phrases"),
                CmdOptionExists(args, "--quiet"),
                captioningMode,
                timeSpanRemainTime,
                CmdOptionExists(args, "--srt"),
                intMaxLineLength,
                intLines,
                GetCmdOption(args, "--threshold"),
                key,
                region
            );
        }
    }
}

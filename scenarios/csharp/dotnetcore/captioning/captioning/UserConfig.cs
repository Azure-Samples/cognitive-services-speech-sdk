//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
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
        readonly public static int defaultMaxLineLengthSBCS = 37;
        readonly public static int defaultMaxLineLengthMBCS = 30;
        
        // 20220921 This is for debugging out-of-order Recognizing results and can be removed.
        readonly public bool debug = false;
        /// True to use a compressed audio format; otherwise, use uncompressed (wav).
        readonly public bool useCompressedAudio = false;
        /// Compressed audio format for input audio. Default is Any.
        readonly public AudioStreamContainerFormat compressedAudioFormat = AudioStreamContainerFormat.ANY;
        /// Whether to show, remove, or mask profanity. Default is to mask it.
        readonly public ProfanityOption profanityOption = ProfanityOption.Masked;
        /// Examples: en-US, ja-JP
        readonly public string language;
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
        /// How long (in milliseconds) a caption should remain on screen. Default is 1000.
        readonly public TimeSpan remainTime;
        /// How long (in milliseconds) to delay all caption timestamps. Default is 1000.
        readonly public TimeSpan delay;
        /// Output captions in SubRip Text format (default is WebVTT format).
        readonly public bool useSubRipTextCaptionFormat = false;
        /// The maximum number of characters per line for a caption. Default is 37.
        readonly public int maxLineLength;
        /// How many lines of captioning to show. Only valid with maxLineLength. Default is 2.
        readonly public int lines;
        /// Set the stable partial result threshold on the Speech service. This setting value must contain an integer.
        /// Example: 3
        readonly public string? stablePartialResultThreshold;
        /// The resource key for your Speech service subscription.
        readonly public string subscriptionKey;
        /// The region for your Speech service subscription.
        readonly public string region;
        
        public UserConfig (
            bool debug,
            bool useCompressedAudio,
            AudioStreamContainerFormat compressedAudioFormat,
            ProfanityOption profanityOption,
            string language,
            string? inputFilePath,
            string? outputFilePath,
            string? phraseList,
            bool suppressConsoleOutput,
            CaptioningMode captioningMode,
            TimeSpan remainTime,
            TimeSpan delay,
            bool useSubRipTextCaptionFormat,
            int maxLineLength,
            int lines,
            string? stablePartialResultThreshold,
            string subscriptionKey,
            string region
            )
        {
            this.debug = debug;
            this.useCompressedAudio = useCompressedAudio;
            this.compressedAudioFormat = compressedAudioFormat;
            this.profanityOption = profanityOption;
            this.language = language;
            this.inputFilePath = inputFilePath;
            this.outputFilePath = outputFilePath;
            this.phraseList = phraseList;
            this.suppressConsoleOutput = suppressConsoleOutput;
            this.captioningMode = captioningMode;
            this.remainTime = remainTime;
            this.delay = delay;
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

        private static string GetLanguage(string[] args)
        {
            var retval = "en-US";
            if (GetCmdOption(args, "--language") is string language)
            {
                retval = language;
            }
            return retval;
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
            string key;
            Console.WriteLine("key args: " + GetCmdOption(args, "--key"));
            if (GetCmdOption(args, "--key") is string keyOptionValue)
            {
                key = keyOptionValue;
            }
            else if (Environment.GetEnvironmentVariable("SPEECH_KEY") is string keyValue)
            {
                key = keyValue;
            }
            else 
            {
                throw new ArgumentException($"Please set the SPEECH_KEY environment variable or provide a Speech resource key with the --key option.{Environment.NewLine}Usage: {usage}");
            }
            string region;
            if (GetCmdOption(args, "--region") is string regionOptionValue)
            {
                region = regionOptionValue;
            }
            else if (Environment.GetEnvironmentVariable("SPEECH_REGION") is string regionValue)
            {
                region = regionValue;
            }
            else 
            {
                throw new ArgumentException($"Please set the SPEECH_REGION environment variable or provide a Speech region with the --region option.{Environment.NewLine}Usage: {usage}");
            }
            
            CaptioningMode captioningMode = CmdOptionExists(args, "--realTime") && !CmdOptionExists(args, "--offline") ? CaptioningMode.RealTime : CaptioningMode.Offline;
            
            string? strRemainTime = GetCmdOption(args, "--remainTime");
            TimeSpan timeSpanRemainTime = TimeSpan.FromMilliseconds(1000);
            if (null != strRemainTime)
            {
                int intRemainTime = Int32.Parse(strRemainTime);
                if (intRemainTime < 0)
                {
                    intRemainTime = 1000;
                }
                timeSpanRemainTime = TimeSpan.FromMilliseconds(intRemainTime);
            }
            
            string? strDelay = GetCmdOption(args, "--delay");
            TimeSpan timeSpanDelay = TimeSpan.FromMilliseconds(1000);
            if (null != strDelay)
            {
                int intDelay = Int32.Parse(strDelay);
                if (intDelay < 0)
                {
                    intDelay = 1000;
                }
                timeSpanDelay = TimeSpan.FromMilliseconds(intDelay);
            }
            
            string? strMaxLineLength = GetCmdOption(args, "--maxLineLength");
            int intMaxLineLength = defaultMaxLineLengthSBCS;
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
                CmdOptionExists(args, "--debug"),
                CmdOptionExists(args, "--format"),
                GetCompressedAudioFormat(args),
                GetProfanityOption(args),
                GetLanguage(args),
                GetCmdOption(args, "--input"),
                GetCmdOption(args, "--output"),
                GetCmdOption(args, "--phrases"),
                CmdOptionExists(args, "--quiet"),
                captioningMode,
                timeSpanRemainTime,
                timeSpanDelay,
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
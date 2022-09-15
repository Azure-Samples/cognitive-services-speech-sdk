//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

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
        /// Simulated real-time caption delay. Default is 0. Only applies to real-time captioning mode.
        readonly public int realTimeDelay = 0;
        /// Output captions in SubRip Text format (default is WebVTT format).
        readonly public bool useSubRipTextCaptionFormat = false;
        /// The maximum number of characters per line for a caption. Default is no limit.
        readonly public int? maxCaptionLength;
        /// How many lines of captioning to show. TODO1 Only applies when maxCaptionLength is specified. Default is 2.
        readonly public int captionLines;
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
            int realTimeDelay,
            bool useSubRipTextCaptionFormat,
            int? maxCaptionLength,
            int captionLines,
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
            this.realTimeDelay = realTimeDelay;
            this.useSubRipTextCaptionFormat = useSubRipTextCaptionFormat;
            this.maxCaptionLength = maxCaptionLength;
            this.captionLines = captionLines;
            this.stablePartialResultThreshold = stablePartialResultThreshold;
            this.subscriptionKey = subscriptionKey;
            this.region = region;
        }
    }
}

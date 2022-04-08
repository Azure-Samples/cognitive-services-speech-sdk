//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Captioning
{
    public class UserConfig
    {
        /// True to remove profanity. Default is to mask it. If true, this overrides disableMaskingProfanity.
        readonly public bool removeProfanity = false;
        /// True to not mask profanity. Default is to mask it. If removeProfanity is true, it overrides this setting.
        readonly public bool disableMaskingProfanity = false;
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
        /// Example: Constoso;Jessie;Rehaan
        readonly public string? phraseList;
        /// True to suppress console output (except errors). If true, this overrides showRecognizingResults.
        readonly public bool suppressConsoleOutput = false;
        /// True to output Recognizing results. Default is to output Recognized results only.
        /// Recognizing results are always written to the console, never to an output file.
        /// If suppressConsoleOutput is true, it overrides this setting.
        readonly public bool showRecognizingResults = false;
        /// Set the stable partial result threshold on the Speech service. This setting value must contain an integer.
        /// Example: 3
        readonly public string? stablePartialResultThreshold;
        /// Output captions in SubRip Text format (default is WebVTT format).
        readonly public bool useSubRipTextCaptionFormat = false;
        /// Enable the TrueText setting on the Speech service.
        readonly public bool useTrueText = false;
        /// The subscription key for your Speech service subscription.
        readonly public string subscriptionKey;
        /// The region for your Speech service subscription.
        readonly public string region;
        
        public UserConfig (
            bool removeProfanity,
            bool disableMaskingProfanity,
            string[]? languageIDLanguages,
            string? inputFilePath,
            string? outputFilePath,
            string? phraseList,
            bool suppressConsoleOutput,
            bool showRecognizingResults,
            string? stablePartialResultThreshold,
            bool useSubRipTextCaptionFormat,
            bool useTrueText,
            string subscriptionKey,
            string region
            )
        {
            this.removeProfanity = removeProfanity;
            this.disableMaskingProfanity = disableMaskingProfanity;
            this.languageIDLanguages = languageIDLanguages;
            this.inputFilePath = inputFilePath;
            this.outputFilePath = outputFilePath;
            this.phraseList = phraseList;
            this.suppressConsoleOutput = suppressConsoleOutput;
            this.showRecognizingResults = showRecognizingResults;
            this.stablePartialResultThreshold = stablePartialResultThreshold;
            this.useSubRipTextCaptionFormat = useSubRipTextCaptionFormat;
            this.useTrueText = useTrueText;
            this.subscriptionKey = subscriptionKey;
            this.region = region;
        }
    }
}

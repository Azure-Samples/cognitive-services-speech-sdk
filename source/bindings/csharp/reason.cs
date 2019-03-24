//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the possible reasons a recognition result might be generated.
    /// </summary>
    public enum ResultReason
    {
        /// <summary>
        /// Indicates speech could not be recognized. More details can be found in the NoMatchDetails object.
        /// </summary>
        NoMatch = 0,

        /// <summary>
        /// Indicates that the recognition was canceled. More details can be found using the CancellationDetails object.
        /// </summary>
        Canceled = 1,

        /// <summary>
        /// Indicates the speech result contains hypothesis text.
        /// </summary>
        RecognizingSpeech = 2,

        /// <summary>
        /// Indicates the speech result contains final text that has been recognized.
        /// Speech Recognition is now complete for this phrase.
        /// </summary>
        RecognizedSpeech = 3,

        /// <summary>
        /// Indicates the intent result contains hypothesis text and intent.
        /// </summary>
        RecognizingIntent = 4,

        /// <summary>
        /// Indicates the intent result contains final text and intent.
        /// Speech Recognition and Intent determination are now complete for this phrase.
        /// </summary>
        RecognizedIntent = 5,

        /// <summary>
        /// Indicates the translation result contains hypothesis text and its translation(s).
        /// </summary>
        TranslatingSpeech = 6,

        /// <summary>
        /// Indicates the translation result contains final text and corresponding translation(s).
        /// Speech Recognition and Translation are now complete for this phrase.
        /// </summary>
        TranslatedSpeech = 7,

        /// <summary>
        /// Indicates the synthesized audio result contains a non-zero amount of audio data
        /// </summary>
        SynthesizingAudio = 8,

        /// <summary>
        /// Indicates the synthesized audio is now complete for this phrase.
        /// </summary>
        SynthesizingAudioCompleted = 9,

        /// <summary>
        /// Indicates the speech result contains (unverified) keyword text.
        /// Added in version 1.3.0
        /// </summary>
        RecognizingKeyword = 10,

        /// <summary>
        /// Indicates that keyword recognition completed recognizing the given keyword.
        /// Added in version 1.3.0
        /// </summary>
        RecognizedKeyword = 11,

        /// <summary>
        /// Indicates the speech synthesis is now started.
        /// Added in version 1.4.0
        /// </summary>
        SynthesizingAudioStarted = 12
    }
}

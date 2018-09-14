//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines the possible reasons a recognition result might be generated.
 */
export enum ResultReason {
    /**
     * Indicates speech could not be recognized. More details can be found in the NoMatchDetails object.
     */
    NoMatch,

    /**
     * Indicates that the recognition was canceled. More details can be found using the CancellationDetails object.
     */
    Canceled,

    /**
     * Indicates the speech result contains hypothesis text.
     */
    RecognizingSpeech,

    /**
     * Indicates the speech result contains final text that has been recognized.
     * Speech Recognition is now complete for this phrase.
     */
    RecognizedSpeech,

    /**
     * Indicates the intent result contains hypothesis text and intent.
     */
    RecognizingIntent,

    /**
     * Indicates the intent result contains final text and intent.
     * Speech Recognition and Intent determination are now complete for this phrase.
     */
    RecognizedIntent,

    /**
     * Indicates the translation result contains hypothesis text and its translation(s).
     */
    TranslatingSpeech,

    /**
     * Indicates the translation result contains final text and corresponding translation(s).
     * Speech Recognition and Translation are now complete for this phrase.
     */
    TranslatedSpeech,

    /**
     * Indicates the synthesized audio result contains a non-zero amount of audio data
     */
    SynthesizingAudio,

    /**
     * Indicates the synthesized audio is now complete for this phrase.
     */
    SynthesizingAudioComplete,
}

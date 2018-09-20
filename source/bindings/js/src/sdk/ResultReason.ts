//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * Defines the possible reasons a recognition result might be generated.
 * @class ResultReason
 */
export enum ResultReason {
    /**
     * Indicates speech could not be recognized. More details can be found in the NoMatchDetails object.
     * @member ResultReason.NoMatch
     */
    NoMatch,

    /**
     * Indicates that the recognition was canceled. More details can be found using the CancellationDetails object.
     * @member ResultReason.Canceled
     */
    Canceled,

    /**
     * Indicates the speech result contains hypothesis text.
     * @member ResultReason.RecognizedSpeech
     */
    RecognizingSpeech,

    /**
     * Indicates the speech result contains final text that has been recognized.
     * Speech Recognition is now complete for this phrase.
     * @member ResultReason.RecognizedSpeech
     */
    RecognizedSpeech,

    /**
     * Indicates the intent result contains hypothesis text and intent.
     * @member ResultReason.RecognizingIntent
     */
    RecognizingIntent,

    /**
     * Indicates the intent result contains final text and intent.
     * Speech Recognition and Intent determination are now complete for this phrase.
     * @member ResultReason.RecognizedIntent
     */
    RecognizedIntent,

    /**
     * Indicates the translation result contains hypothesis text and its translation(s).
     * @member ResultReason.TranslatingSpeech
     */
    TranslatingSpeech,

    /**
     * Indicates the translation result contains final text and corresponding translation(s).
     * Speech Recognition and Translation are now complete for this phrase.
     * @member ResultReason.TranslatedSpeech
     */
    TranslatedSpeech,

    /**
     * Indicates the synthesized audio result contains a non-zero amount of audio data
     * @member ResultReason.SynthesizingAudio
     */
    SynthesizingAudio,

    /**
     * Indicates the synthesized audio is now complete for this phrase.
     * @member ResultReason.SynthesizingAudioCompleted
     */
    SynthesizingAudioCompleted,
}

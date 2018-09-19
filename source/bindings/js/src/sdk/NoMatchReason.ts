//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * Defines the possible reasons a recognition result might not be recognized.
 */
export enum NoMatchReason {

    /**
     * Indicates that speech was detected, but not recognized.
     */
    NotRecognized,

    /**
     * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
     */
    InitialSilenceTimeout,

    /**
     * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
     */
    InitialBabbleTimeout,
}

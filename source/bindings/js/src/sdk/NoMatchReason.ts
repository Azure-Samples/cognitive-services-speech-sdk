// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

/**
 * Defines the possible reasons a recognition result might not be recognized.
 * @class NoMatchReason
 */
export enum NoMatchReason {

    /**
     * Indicates that speech was detected, but not recognized.
     * @member NoMatchReason.NotRecognized
     */
    NotRecognized,

    /**
     * Indicates that the start of the audio stream contained only silence,
     * and the service timed out waiting for speech.
     * @member NoMatchReason.InitialSilenceTimeout
     */
    InitialSilenceTimeout,

    /**
     * Indicates that the start of the audio stream contained only noise,
     * and the service timed out waiting for speech.
     * @member NoMatchReason.InitialBabbleTimeout
     */
    InitialBabbleTimeout,
}
